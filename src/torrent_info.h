#ifndef TORRENT_INFO_H
#define TORRENT_INFO_H

#include <napi.h>
#include <libtorrent/torrent_info.hpp>

class TorrentInfo : public Napi::ObjectWrap<TorrentInfo> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, const libtorrent::torrent_info &info);
    static Napi::FunctionReference constructor;
    TorrentInfo(const Napi::CallbackInfo& info);
    libtorrent::torrent_info* GetTorrentInfo() { return info_; }

private:
    // Instance methods
    Napi::Value GetFiles(const Napi::CallbackInfo& info);
    Napi::Value ToBuffer(const Napi::CallbackInfo& info);
    
    // Napi::Value Pause(const Napi::CallbackInfo& info);
    // Napi::Value Resume(const Napi::CallbackInfo& info);

    // The wrapped torrent_handle
    libtorrent::torrent_info *info_;
};

Napi::Object WrapTorrentInfo(Napi::Env env, const libtorrent::torrent_info& info);
Napi::Object bind_torrent_info(Napi::Env env, Napi::Object exports);


#endif // TORRENT_INFO_H