#include <napi.h>

#include <libtorrent/entry.hpp>

Napi::Value EntryToValue(Napi::Env env, const libtorrent::entry& e);
lt::entry ValueToEntry(const Napi::Value& value);
