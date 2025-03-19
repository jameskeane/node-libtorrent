#include "torrent_handle.h"

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

Napi::Object bind_torrent_handle(Napi::Env env, Napi::Object exports) {
    return TorrentHandle::Init(env, exports);
}
