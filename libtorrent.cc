#include <napi.h>

#include "src/session.h"
#include "src/alert.h"


Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  bind_session(env, exports);
  bind_alerts(env, exports);

  return exports;
}

NODE_API_MODULE(libtorrent, InitAll)
