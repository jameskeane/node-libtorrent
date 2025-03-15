#include <napi.h>
#include <libtorrent/alert_types.hpp>

Napi::Value alert_to_js(const Napi::Env& env, const libtorrent::alert* a);
Napi::Object bind_alerts(Napi::Env env, Napi::Object exports);
