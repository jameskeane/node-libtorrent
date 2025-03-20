#include "torrent_handle.h"
#include "torrent_info.h"

#include <libtorrent/torrent_handle.hpp>
#include <napi.h>

// Initialize the static constructor reference
Napi::FunctionReference TorrentHandle::constructor;

Napi::Object WrapTorrentHandle(Napi::Env env, const libtorrent::torrent_handle& handle) {
    auto handleExternal = Napi::External<libtorrent::torrent_handle>::New(env,
        new libtorrent::torrent_handle(handle));
    return TorrentHandle::constructor.New({handleExternal});
}

Napi::Object TorrentHandle::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TorrentHandle", {
        InstanceMethod("info_hashes", &TorrentHandle::GetInfoHash),
        InstanceMethod("flags", &TorrentHandle::GetTorrentFlags),
        InstanceMethod("torrent_file", &TorrentHandle::GetTorrentInfo),
        // InstanceMethod("getName", &TorrentHandle::GetName),
        // InstanceMethod("isPaused", &TorrentHandle::IsPaused),
        // InstanceMethod("pause", &TorrentHandle::Pause),
        // InstanceMethod("resume", &TorrentHandle::Resume),
    });

    // Store the constructor in our static reference so it can be accessed from NewInstance
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("TorrentHandle", func);
    return exports;
}

TorrentHandle::TorrentHandle(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TorrentHandle>(info) {
    Napi::Env env = info.Env();

    // Prevent direct instantiation from JavaScript
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(env, "Cannot instantiate Session directly. Use session() instead")
            .ThrowAsJavaScriptException();
        return;
    }

    // Default constructor logic for internal use
    handle_ = info[0].As<Napi::External<lt::torrent_handle>>().Data();
}

// Example method implementation
Napi::Value TorrentHandle::GetInfoHash(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Get the info hash from the torrent handle
    libtorrent::info_hash_t hashes = handle_->info_hashes();

    // Return the info hash as a buffer
    return Napi::Buffer<char>::Copy(env, hashes.get_best().data(), hashes.get_best().size());
}

Napi::Value TorrentHandle::GetTorrentInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Get the torrent info from the torrent handle
    std::shared_ptr<const libtorrent::torrent_info> ti = handle_->torrent_file();

    // Check if the torrent info is nullptr
    if (!ti) {
        // Return null if no torrent info is available
        return env.Null();
    }

    // Create a TorrentInfo wrapper for the torrent_info object
    try {
        return WrapTorrentInfo(env, *ti);
    } catch (const std::exception& e) {
        Napi::Error::New(env, std::string("Failed to wrap torrent info: ") + e.what())
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value TorrentHandle::GetTorrentFlags(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    // Get the flags as a number from the torrent handle
    libtorrent::torrent_flags_t flags = handle_->flags();

    // Return the flags as an unsigned 64-bit integer
    return Napi::Number::New(env, static_cast<uint64_t>(flags));
}


Napi::Object bind_torrent_handle(Napi::Env env, Napi::Object exports) {
    Napi::Object flags = Napi::Object::New(env);
    flags.Set("seed_mode", static_cast<uint64_t>(libtorrent::torrent_flags::seed_mode));
    flags.Set("upload_mode", static_cast<uint64_t>(libtorrent::torrent_flags::upload_mode));
    flags.Set("share_mode", static_cast<uint64_t>(libtorrent::torrent_flags::share_mode));
    flags.Set("apply_ip_filter", static_cast<uint64_t>(libtorrent::torrent_flags::apply_ip_filter));
    flags.Set("paused", static_cast<uint64_t>(libtorrent::torrent_flags::paused));
    flags.Set("auto_managed", static_cast<uint64_t>(libtorrent::torrent_flags::auto_managed));
    flags.Set("duplicate_is_error", static_cast<uint64_t>(libtorrent::torrent_flags::duplicate_is_error));
    flags.Set("update_subscribe", static_cast<uint64_t>(libtorrent::torrent_flags::update_subscribe));
    flags.Set("super_seeding", static_cast<uint64_t>(libtorrent::torrent_flags::super_seeding));
    flags.Set("sequential_download", static_cast<uint64_t>(libtorrent::torrent_flags::sequential_download));
    flags.Set("stop_when_ready", static_cast<uint64_t>(libtorrent::torrent_flags::stop_when_ready));
    flags.Set("override_trackers", static_cast<uint64_t>(libtorrent::torrent_flags::override_trackers));
    flags.Set("override_web_seeds", static_cast<uint64_t>(libtorrent::torrent_flags::override_web_seeds));
    flags.Set("need_save_resume", static_cast<uint64_t>(libtorrent::torrent_flags::need_save_resume));
    flags.Set("disable_dht", static_cast<uint64_t>(libtorrent::torrent_flags::disable_dht));
    flags.Set("disable_lsd", static_cast<uint64_t>(libtorrent::torrent_flags::disable_lsd));
    flags.Set("disable_pex", static_cast<uint64_t>(libtorrent::torrent_flags::disable_pex));
    flags.Set("no_verify_files", static_cast<uint64_t>(libtorrent::torrent_flags::no_verify_files));
    flags.Set("default_dont_download", static_cast<uint64_t>(libtorrent::torrent_flags::default_dont_download));
    flags.Set("i2p_torrent", static_cast<uint64_t>(libtorrent::torrent_flags::i2p_torrent));
    flags.Freeze();

    exports.Set("torrent_flags_t", flags);

    return TorrentHandle::Init(env, exports);
}
