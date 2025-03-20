#include "torrent_info.h"

#include <sstream>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_info.hpp>
#include <napi.h>

// Initialize the static constructor reference
Napi::FunctionReference TorrentInfo::constructor;

Napi::Object WrapTorrentInfo(Napi::Env env, const libtorrent::torrent_info& info) {
    auto handleExternal = Napi::External<libtorrent::torrent_info>::New(env,
        new libtorrent::torrent_info(info));
    return TorrentInfo::constructor.New({handleExternal});
}

Napi::Object TorrentInfo::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TorrentInfo", {
        InstanceMethod("files", &TorrentInfo::GetFiles),
        InstanceMethod("toBuffer", &TorrentInfo::ToBuffer),
        // InstanceMethod("isPaused", &TorrentInfo::IsPaused),
        // InstanceMethod("pause", &TorrentInfo::Pause),
        // InstanceMethod("resume", &TorrentInfo::Resume),
    });

    // Store the constructor in our static reference so it can be accessed from NewInstance
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("TorrentInfo", func);
    return exports;
}

TorrentInfo::TorrentInfo(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TorrentInfo>(info) {
    Napi::Env env = info.Env();

    // Prevent direct instantiation from JavaScript
    if (info.Length() == 0 || !info[0].IsExternal()) {
        Napi::Error::New(env, "Cannot instantiate Session directly. Use session() instead")
            .ThrowAsJavaScriptException();
        return;
    }

    // Default constructor logic for internal use
    info_ = info[0].As<Napi::External<lt::torrent_info>>().Data();
}

Napi::Value TorrentInfo::GetFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array files = Napi::Array::New(env);

    try {
        const lt::file_storage& fs = info_->files();
        int numFiles = fs.num_files();

        for (int i = 0; i < numFiles; i++) {
            Napi::Object file = Napi::Object::New(env);
            
            // Get file information
            file.Set("path", fs.file_path(i));
            file.Set("size", Napi::Number::New(env, fs.file_size(i)));
            file.Set("offset", Napi::Number::New(env, fs.file_offset(i)));
            
            // Add file to array
            files.Set(i, file);
        }
        
        return files;
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value TorrentInfo::ToBuffer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    // Create torrent from torrent_info
    libtorrent::create_torrent ct(*info_);

    // Bencode the torrent
    std::ostringstream oss;
    libtorrent::bencode(std::ostream_iterator<char>(oss), ct.generate());
    std::string torrent_content = oss.str();
    
    // Create a buffer from the bencode data
    Napi::Buffer<char> buffer = Napi::Buffer<char>::Copy(
      env, 
      torrent_content.c_str(), 
      torrent_content.length()
    );
    
    return buffer;
  } catch (const std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}


Napi::Object bind_torrent_info(Napi::Env env, Napi::Object exports) {
    return TorrentInfo::Init(env, exports);
}
