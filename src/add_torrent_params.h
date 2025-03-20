#include <napi.h>
#include <libtorrent/add_torrent_params.hpp>

Napi::Value add_torrent_params_to_js(Napi::Env env, libtorrent::add_torrent_params params);
Napi::Object bind_add_torrent_params(Napi::Env env, Napi::Object exports);
