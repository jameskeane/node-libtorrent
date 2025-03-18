#include "session.h"
#include "alert.h"
#include "entry.h"

#include <libtorrent/session.hpp>
using namespace lt;

Napi::Value SessionConstructor(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object as the first argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Convert Napi::Object to libtorrent::settings_pack
    Napi::Object obj = info[0].As<Napi::Object>();

    lt::settings_pack settings;
    Napi::Array propertyNames = obj.GetPropertyNames();
    for (uint32_t i = 0; i < propertyNames.Length(); ++i) {
        Napi::Value key = propertyNames[i];
        std::string keyStr = key.As<Napi::String>().Utf8Value();
        int keyEnum = lt::setting_by_name(keyStr.c_str());

        if (keyEnum == -1) {
            Napi::TypeError::New(env, "Unknown settingpack key \"" + keyStr + "\"").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Value value = obj.Get(key);

        if (value.IsNumber()) {
            int valueInt = value.As<Napi::Number>().Int32Value();
            settings.set_int(keyEnum, valueInt);
        } else if (value.IsString()) {
            std::string valueStr = value.As<Napi::String>().Utf8Value();
            settings.set_str(keyEnum, valueStr);
        } else if (value.IsBoolean()) {
            bool valueBool = value.As<Napi::Boolean>().Value();
            settings.set_bool(keyEnum, valueBool);
        } else {
            Napi::TypeError::New(env, "Unsupported type for settingpack key \"" + keyStr + "\"").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    lt::session_params params(std::move(settings));
    lt::session* session = new lt::session(params);
    Napi::External<lt::session> ext = Napi::External<lt::session>::New(env, session);
    return ext;
  }


Napi::FunctionReference Session::constructor;

Napi::Object Session::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Session", {
        InstanceMethod("set_alert_notify", &Session::SetAlertNotify),
        InstanceMethod("pop_alerts", &Session::PopAlerts),
        InstanceMethod("abort", &Session::Abort),
        InstanceMethod("async_add_torrent", &Session::AsyncAddTorrent),
        InstanceMethod("dht_get_immutable_item", &Session::DhtGetImmutableItem),
        InstanceMethod("dht_put_immutable_item", &Session::DhtPutImmutableItem),
        InstanceMethod("dht_get_mutable_item", &Session::DhtGetMutableItem),
        InstanceMethod("dht_put_mutable_item", &Session::DhtPutMutableItem),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Session", func);
    exports.Set("session", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Value sessionExternal = SessionConstructor(info);
        return Session::constructor.New({sessionExternal});
    }));
    
    return exports;
}

Session::Session(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Session>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(env, "Cannot instantiate Session directly. Use session() instead")
            .ThrowAsJavaScriptException();
        return;
    }
    
    session_ = info[0].As<Napi::External<lt::session>>().Data();
}

Session::~Session() {
    // delete _session; // todo why does this segfault?
}

Napi::Value Session::Abort(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    session_->abort();

    // Clean up the alert notify reference if it exists
    if (alertNotifyRef_) {
        alertNotifyRef_.Abort();
        alertNotifyRef_.Release();
    }
    return env.Undefined();
}

Napi::Value Session::SetAlertNotify(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "Expected a function as the first argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the callback function
    Napi::Function callback = info[0].As<Napi::Function>();

    // Store the callback reference as a member variable instead of creating a new one
    if (alertNotifyRef_) {
        // Clear previous callback if it exists
        alertNotifyRef_.Abort();
        alertNotifyRef_.Release();
    }
    // Create a persistent reference to the callback function to keep it alive
    alertNotifyRef_ = Napi::ThreadSafeFunction::New(
        env,                           // Environment
        callback,                      // JavaScript function
        "alert_notify_callback",       // Name for debugging
        0,                             // No max queue size
        1,                             // Initial thread count
        []( Napi::Env ) {              // Finalizer when the thread-safe function is destroyed
            // Cleanup if needed
        }
    );

    // Set the alert notify function on the session
    session_->set_alert_notify([this]() {
        // Execute the callback from the JavaScript side
        alertNotifyRef_.NonBlockingCall();
    });

    return env.Undefined();
}

Napi::Value Session::PopAlerts(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Create a vector to store alerts
    std::vector<lt::alert*> alerts;

    // Call libtorrent's pop_alerts
    session_->pop_alerts(&alerts);

    // Create a JavaScript array to hold the alerts
    Napi::Array result = Napi::Array::New(env, alerts.size());

    // Convert each alert to a JavaScript object
    for (size_t i = 0; i < alerts.size(); ++i) {
        lt::alert* alert = alerts[i];

        // Set this alert in the result array
        result.Set(i, alert_to_js(env, alert));
    }

    return result;
}


Napi::Value Session::AsyncAddTorrent(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object as the first argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the params object from JavaScript
    Napi::Object torrentParams = info[0].As<Napi::Object>();

    // Create a libtorrent add_torrent_params object
    lt::add_torrent_params params;

    // Extract properties from the JavaScript object
    if (torrentParams.Has("save_path") && torrentParams.Get("save_path").IsString()) {
        params.save_path = torrentParams.Get("save_path").As<Napi::String>().Utf8Value();
    }

    // Handle ti (torrent info) if provided
    if (torrentParams.Has("ti") && torrentParams.Get("ti").IsExternal()) {
        // Assuming ti is an external pointer to a torrent_info object
        lt::torrent_info* ti = torrentParams.Get("ti").As<Napi::External<lt::torrent_info>>().Data();
        params.ti = std::shared_ptr<lt::torrent_info>(ti);
    }

    // Handle info hash
    if (torrentParams.Has("info_hashes") && torrentParams.Get("info_hashes").IsBuffer()) {
        Napi::Buffer<char> hashBuffer = torrentParams.Get("info_hashes").As<Napi::Buffer<char>>();
        if (hashBuffer.Length() == 20) { // SHA1 hash is 20 bytes
            lt::sha1_hash hash(hashBuffer.Data());
            params.info_hashes = info_hash_t(hash);
        }
        else if (hashBuffer.Length() == 32) { // SHA256 hash is 32 bytes
            lt::sha256_hash hash(hashBuffer.Data());
            params.info_hashes = info_hash_t(hash);
        }
        else {
            Napi::TypeError::New(env, "Infohash must be either sha1 or sha256").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    // Handle name
    if (torrentParams.Has("name") && torrentParams.Get("name").IsString()) {
        params.name = torrentParams.Get("name").As<Napi::String>().Utf8Value();
    }

    // Handle root_certificate
    if (torrentParams.Has("root_certificate") && torrentParams.Get("root_certificate").IsString()) {
        params.root_certificate = torrentParams.Get("root_certificate").As<Napi::String>().Utf8Value();
    }

    // todo add more parameter mappings

    // Add the torrent asynchronously
    session_->async_add_torrent(params);
    return env.Undefined();
}


Napi::Value Session::DhtGetImmutableItem(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsBuffer()) {
        Napi::TypeError::New(env, "Expected a buffer containing SHA1 hash as first argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the buffer containing the SHA1 hash
    Napi::Buffer<char> hashBuffer = info[0].As<Napi::Buffer<char>>();
    if (hashBuffer.Length() != 20) {  // SHA1 hash is 20 bytes
        Napi::TypeError::New(env, "Hash buffer must be exactly 20 bytes").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Convert to lt::sha1_hash
    lt::sha1_hash target_hash(hashBuffer.Data());

    // Initiate the DHT get operation
    session_->dht_get_item(target_hash);
    return env.Undefined();
}

Napi::Value Session::DhtPutImmutableItem(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected a value as the first argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Convert the JS value to a libtorrent entry
    lt::entry e = ValueToEntry(info[0]);
    if ( e.type() == lt::entry::undefined_t ) {
        Napi::TypeError::New(env, "Failed to convert value to entry").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Put the immutable item to DHT
    lt::sha1_hash hash = session_->dht_put_item(e);

    // Return sha1 hash of the item as a Buffer
    return Napi::Buffer<char>::Copy(env, hash.data(), hash.size());
}


Napi::Value Session::DhtGetMutableItem(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Expected public key buffer and salt as arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the public key buffer
    Napi::Buffer<char> pubKeyBuffer = info[0].As<Napi::Buffer<char>>();
    if (pubKeyBuffer.Length() != 32) {  // Ed25519 public key is 32 bytes
        Napi::TypeError::New(env, "Public key buffer must be exactly 32 bytes").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Convert to array for public key
    std::array<char, 32> pubKey;
    std::memcpy(pubKey.data(), pubKeyBuffer.Data(), 32);

    // Get the salt
    std::string salt = info[1].As<Napi::String>().Utf8Value();

    // Initiate the DHT get mutable item operation
    session_->dht_get_item(pubKey, salt);
    return env.Undefined();
}

Napi::Value Session::DhtPutMutableItem(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Check for required parameters
    if (info.Length() < 4 ||
        !info[0].IsBuffer() ||    // public key
        !(info[1].IsObject() || info[1].IsNumber() || info[1].IsString()) ||    // entry
        !info[2].IsBuffer() ||    // signature
        !info[3].IsNumber()) {    // seq
        Napi::TypeError::New(env, "Expected public key buffer, data buffer, signature buffer, and sequence number").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the public key buffer
    Napi::Buffer<char> pubKeyBuffer = info[0].As<Napi::Buffer<char>>();
    if (pubKeyBuffer.Length() != 32) {  // Ed25519 public key is 32 bytes
        Napi::TypeError::New(env, "Public key buffer must be exactly 32 bytes").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the data buffer and convert to entry
    lt::entry jitem = ValueToEntry(info[1].As<Napi::Object>());

    // Get the signature buffer
    Napi::Buffer<char> sigBuffer = info[2].As<Napi::Buffer<char>>();
    if (sigBuffer.Length() != 64) {  // Ed25519 signature is 64 bytes
        Napi::TypeError::New(env, "Signature buffer must be exactly 64 bytes").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the sequence number
    std::int64_t jseq = info[3].As<Napi::Number>().Int64Value();

    // Convert to arrays for public key and signature
    std::array<char, 32> pubKey;
    std::array<char, 64> jsig;
    std::memcpy(pubKey.data(), pubKeyBuffer.Data(), 32);
    std::memcpy(jsig.data(), sigBuffer.Data(), 64);

    // Get the optional salt
    std::string salt;
    if (info.Length() == 5 && info[4].IsString()) {
        salt = info[4].As<Napi::String>().Utf8Value();
    }

    // Put the mutable item to DHT
    session_->dht_put_item(pubKey, [jitem, jsig, jseq](entry& item, std::array<char, 64>& sig,
                                      std::int64_t& seq, std::string const& salt) {
        item = jitem;
        sig = jsig;
        seq = jseq;
    }, salt);
    return env.Undefined();
}


// Napi::Value Session::DhtPutMutableItem(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 2 ||
//         !info[0].IsBuffer() ||    // public key
//         !info[1].IsFunction()) {  // signing callback
//         Napi::TypeError::New(env, "Expected private key, public key, salt, and value arguments").ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     // Get the public key buffer
//     Napi::Buffer<char> pubKeyBuffer = info[0].As<Napi::Buffer<char>>();
//     if (pubKeyBuffer.Length() != 32) {  // Ed25519 public key is 32 bytes
//         Napi::TypeError::New(env, "Public key buffer must be exactly 32 bytes").ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     // Create a persistent reference to the callback function to keep it alive
//     Napi::Function callback = info[1].As<Napi::Function>();
//     // Create a persistent reference to the callback function to keep it alive
//     Napi::ThreadSafeFunction safeCallback = Napi::ThreadSafeFunction::New(
//         env,                           // Environment
//         callback,                      // JavaScript function
//         "dht_put_mutable_callback",    // Name for debugging
//         0,                             // No max queue size
//         1,                             // Initial thread count
//         []( Napi::Env ) {              // Finalizer when the thread-safe function is destroyed
//             // Cleanup if needed
//         }
//     );

//     // Convert to array for public key
//     std::array<char, 32> pubKey;
//     std::memcpy(pubKey.data(), pubKeyBuffer.Data(), 32);

//     // get the optional salt
//     std::string salt;
//     if (info.Length() == 3) {
//         // Check if salt is provided and is a string
//         if (!info[2].IsString()) {
//             Napi::TypeError::New(env, "Salt must be a string").ThrowAsJavaScriptException();
//             return env.Null();
//         }
//         salt = info[2].As<Napi::String>().Utf8Value();
//     }

//     // Put the mutable item to DHT
//     session_->dht_put_item(pubKey, [safeCallback](entry& item, std::array<char, 64>& sig,
//                                                std::int64_t& seq, std::string const& salt) {
//         safeCallback.BlockingCall([&item, &sig, &seq, salt](Napi::Env env, Napi::Function callback) {
//             // Convert the arguments into JavaScript objects
//             Napi::Value jsItem = EntryToValue(env, item);
//             Napi::Buffer<char> sigBuffer = Napi::Buffer<char>::Copy(env, sig.data(), sig.size());
//             Napi::Number jsSeq = Napi::Number::New(env, seq);
//             Napi::String jsSalt = Napi::String::New(env, salt);

//             // Execute the callback from the JavaScript side
//             Napi::Value retVal = callback.Call({jsItem, sigBuffer, jsSeq, jsSalt});

//             // Check if return value is an array with the expected items
//             if (!retVal.IsArray()) {
//                 Napi::Error::New(env, "Signature callback must return an array").ThrowAsJavaScriptException();
//                 return;
//             }

//             Napi::Array result = retVal.As<Napi::Array>();

//             // Check if array has at least 3
//             if (result.Length() < 3) {
//                 Napi::Error::New(env, "Signature callback must return array with [entry, signature, seq]").ThrowAsJavaScriptException();
//                 return;
//             }

//             // Read entry from return value (position 0)
//             item = ValueToEntry(result.Get((uint32_t)0));

//             // Read signature from return value (position 1)
//             if (result.Get((uint32_t)1).IsBuffer()) {
//                 Napi::Buffer<char> sigBuffer = result.Get((uint32_t)1).As<Napi::Buffer<char>>();
//                 if (sigBuffer.Length() == 64) {
//                     std::memcpy(sig.data(), sigBuffer.Data(), 64);
//                 } else {
//                     Napi::Error::New(env, "Signature buffer must be exactly 64 bytes").ThrowAsJavaScriptException();
//                     return;
//                 }
//             } else {
//                 Napi::Error::New(env, "Signature must be a buffer").ThrowAsJavaScriptException();
//                 return;
//             }

//             // Read sequence number from return value (position 2)
//             if (result.Get((uint32_t)2).IsNumber()) {
//                 seq = result.Get((uint32_t)2).As<Napi::Number>().Int64Value();
//             } else {
//                 Napi::Error::New(env, "Sequence number must be a number").ThrowAsJavaScriptException();
//                 return;
//             }
//         });
//     }, salt);
//     return env.Undefined();
// }


Napi::Object bind_session(Napi::Env env, Napi::Object exports) {
    return Session::Init(env, exports);
}

