#include <napi.h>

#include <libtorrent/session.hpp>

Napi::Object InitSession(Napi::Env env, Napi::Object exports);

class Session : public Napi::ObjectWrap<Session> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        static Napi::FunctionReference constructor;
        
        Session(const Napi::CallbackInfo& info);
        ~Session();

    Napi::Value SetAlertNotify(const Napi::CallbackInfo& info);
    Napi::Value PopAlerts(const Napi::CallbackInfo& info);
    Napi::Value Abort(const Napi::CallbackInfo& info);

    private:
        libtorrent::session* session_;
        Napi::ThreadSafeFunction alertNotifyRef_;
   };
   