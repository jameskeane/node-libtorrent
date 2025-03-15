#include <napi.h>

#include "src/session.h"


Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  InitSession(env, exports);

  return exports;
}

NODE_API_MODULE(libtorrent, InitAll)
