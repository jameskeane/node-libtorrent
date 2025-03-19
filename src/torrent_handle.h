#ifndef TORRENT_HANDLE_H
#define TORRENT_HANDLE_H

#include <napi.h>
#include <libtorrent/torrent_handle.hpp>

class TorrentHandle : public Napi::ObjectWrap<TorrentHandle> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, const libtorrent::torrent_handle& handle);
    static Napi::FunctionReference constructor;
    TorrentHandle(const Napi::CallbackInfo& info);

private:
    // Instance methods
    Napi::Value GetInfoHash(const Napi::CallbackInfo& info);
    // Napi::Value GetName(const Napi::CallbackInfo& info);
    // Napi::Value IsPaused(const Napi::CallbackInfo& info);
    // Napi::Value Pause(const Napi::CallbackInfo& info);
    // Napi::Value Resume(const Napi::CallbackInfo& info);

    // The wrapped torrent_handle
    libtorrent::torrent_handle *handle_;
};

Napi::Object WrapTorrentHandle(Napi::Env env, const libtorrent::torrent_handle& handle);
Napi::Object bind_torrent_handle(Napi::Env env, Napi::Object exports);


#endif // TORRENT_HANDLE_H