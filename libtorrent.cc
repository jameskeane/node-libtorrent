#include <napi.h>

#include "src/session.h"
#include "src/alert.h"
#include "src/torrent_handle.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  bind_session(env, exports);
  bind_alerts(env, exports);
  bind_torrent_handle(env, exports);

  return exports;
}

NODE_API_MODULE(libtorrent, InitAll)
