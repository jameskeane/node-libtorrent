#include <napi.h>
#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <sstream>

Napi::Value EntryToValue(Napi::Env env, const lt::entry& e) {
    switch (e.type()) {
        case lt::entry::int_t:
            return Napi::Number::New(env, e.integer());

        case lt::entry::string_t:
            return Napi::String::New(env, e.string());

        case lt::entry::list_t:
        {
            const lt::entry::list_type& list = e.list();
            Napi::Array result = Napi::Array::New(env, list.size());
            
            uint32_t index = 0;
            for (const lt::entry& item : list) {
                result.Set(index++, EntryToValue(env, item));
            }
            return result;
        }

        case lt::entry::dictionary_t:
        {
            const lt::entry::dictionary_type& dict = e.dict();
            Napi::Object result = Napi::Object::New(env);
            
            for (const auto& pair : dict) {
                result.Set(
                    Napi::String::New(env, pair.first),
                    EntryToValue(env, pair.second)
                );
            }
            return result;
        }

        case lt::entry::preformatted_t:
        {
            // Handle preformatted data as buffer
            const lt::entry::preformatted_type& preformatted = e.preformatted();
            Napi::Buffer<char> buffer = Napi::Buffer<char>::Copy(
                env, 
                preformatted.data(), 
                preformatted.size()
            );
            return buffer;
        }

        case lt::entry::undefined_t:
        default:
            return env.Undefined();
    }
}

lt::entry ValueToEntry(const Napi::Value& value) {
    if (value.IsString()) {
        return lt::entry(value.As<Napi::String>().Utf8Value());
    }
    else if (value.IsNumber()) {
        return lt::entry(static_cast<std::int64_t>(value.As<Napi::Number>().Int64Value()));
    }
    else if (value.IsArray()) {
        Napi::Array array = value.As<Napi::Array>();
        lt::entry::list_type list;
        
        for (uint32_t i = 0; i < array.Length(); i++) {
            list.push_back(ValueToEntry(array.Get(i)));
        }
        
        return lt::entry(list);
    }
    else if (value.IsObject() && !value.IsBuffer()) {
        Napi::Object obj = value.As<Napi::Object>();
        lt::entry::dictionary_type dict;
        
        Napi::Array properties = obj.GetPropertyNames();
        for (uint32_t i = 0; i < properties.Length(); i++) {
            Napi::Value key = properties.Get(i);
            if (key.IsString()) {
                std::string keyStr = key.As<Napi::String>().Utf8Value();
                dict[keyStr] = ValueToEntry(obj.Get(key));
            }
        }
        
        return lt::entry(dict);
    }
    else if (value.IsBuffer()) {
        Napi::Buffer<char> buffer = value.As<Napi::Buffer<char>>();
        std::vector<char> data(buffer.Data(), buffer.Data() + buffer.Length());
        return lt::entry::preformatted_type(std::move(data));
    }
    else if (value.IsNull() || value.IsUndefined()) {
        return lt::entry();
    }
    
    // Default case, return undefined entry
    return lt::entry();
}