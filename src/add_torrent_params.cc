#include "add_torrent_params.h"
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/load_torrent.hpp>

using namespace lt;


Napi::Object add_torrent_params_to_js(Napi::Env env, lt::add_torrent_params params) {
    Napi::Object obj = Napi::Object::New(env);
    
    // Add properties to the object
    if (!params.name.empty()) obj.Set("name", params.name);
    if (!params.save_path.empty()) obj.Set("save_path", params.save_path);
    
    // Handle info hash
    auto info = params.info_hashes.get_best();
    obj.Set("info_hashes", Napi::Buffer<char>::Copy(env, info.data(), info.size()));

    // Add ti as external (torrent info)
    if (params.ti) {
        // Create an external wrapper for the torrent_info pointer
        Napi::External<std::shared_ptr<torrent_info>> tiExternal = 
            Napi::External<std::shared_ptr<torrent_info>>::New(
                env, 
                new std::shared_ptr<torrent_info>(params.ti)
            );
        obj.Set("ti", tiExternal);
    }
    // // Handle trackers
    // if (!params.trackers.empty()) {
    //     Napi::Array trackers = Napi::Array::New(env, params.trackers.size());
    //     for (size_t i = 0; i < params.trackers.size(); i++) {
    //         trackers[i] = params.trackers[i];
    //     }
    //     obj.Set("trackers", trackers);
    // }

    // // Handle tracker tiers
    // if (!params.tracker_tiers.empty()) {
    //     Napi::Array tiers = Napi::Array::New(env, params.tracker_tiers.size());
    //     for (size_t i = 0; i < params.tracker_tiers.size(); i++) {
    //         tiers[i] = params.tracker_tiers[i];
    //     }
    //     obj.Set("tracker_tiers", tiers);
    // }

    // Handle DHT nodes
    // if (!params.dht_nodes.empty()) {
    //     Napi::Array nodes = Napi::Array::New(env, params.dht_nodes.size());
    //     for (size_t i = 0; i < params.dht_nodes.size(); i++) {
    //         Napi::Array node = Napi::Array::New(env, 2);
    //         node[0] = params.dht_nodes[i].first;
    //         node[1] = params.dht_nodes[i].second;
    //         nodes[i] = node;
    //     }
    //     obj.Set("dhtNodes", nodes);
    // }

    // Handle storage mode
    // obj.Set("storage_mode", static_cast<int>(params.storage_mode));

    // // Handle file priorities
    // if (!params.file_priorities.empty()) {
    //     Napi::Array priorities = Napi::Array::New(env, params.file_priorities.size());
    //     for (size_t i = 0; i < params.file_priorities.size(); i++) {
    //         priorities[i] = static_cast<int>(params.file_priorities[i]);
    //     }
    //     obj.Set("file_priorities", priorities);
    // }

    // // Handle trackerid
    // if (!params.trackerid.empty()) obj.Set("tracker_id", params.trackerid);

    // Handle flags
    // obj.Set("flags", params.flags);

    // Handle limits
    if (params.max_uploads != -1) obj.Set("max_uploads", params.max_uploads);
    if (params.max_connections != -1) obj.Set("max_connections", params.max_connections);
    if (params.upload_limit != -1) obj.Set("upload_limit", params.upload_limit);
    if (params.download_limit != -1) obj.Set("download_limit", params.download_limit);

    // Handle stats
    obj.Set("total_uploaded", params.total_uploaded);
    obj.Set("total_downloaded", params.total_downloaded);
    obj.Set("active_time", params.active_time);
    obj.Set("finished_time", params.finished_time);
    obj.Set("seeding_time", params.seeding_time);

    // Handle timestamps
    if (params.added_time != 0) obj.Set("added_time", Napi::Date::New(env, params.added_time * 1000.0));
    if (params.completed_time != 0) obj.Set("completed_time", Napi::Date::New(env, params.completed_time * 1000.0));
    if (params.last_seen_complete != 0) obj.Set("lastSeen_complete", Napi::Date::New(env, params.last_seen_complete * 1000.0));

    // Handle peer counts
    if (params.num_complete != -1) obj.Set("num_complete", params.num_complete);
    if (params.num_incomplete != -1) obj.Set("num_incomplete", params.num_incomplete);
    if (params.num_downloaded != -1) obj.Set("num_downloaded", params.num_downloaded);

    // // Handle HTTP and URL seeds
    // if (!params.http_seeds.empty()) {
    //     Napi::Array httpSeeds = Napi::Array::New(env, params.http_seeds.size());
    //     for (size_t i = 0; i < params.http_seeds.size(); i++) {
    //         httpSeeds[i] = params.http_seeds[i];
    //     }
    //     obj.Set("http_seeds", httpSeeds);
    // }

    // if (!params.url_seeds.empty()) {
    //     Napi::Array urlSeeds = Napi::Array::New(env, params.url_seeds.size());
    //     for (size_t i = 0; i < params.url_seeds.size(); i++) {
    //         urlSeeds[i] = params.url_seeds[i];
    //     }
    //     obj.Set("url_seeds", urlSeeds);
    // }

    // Handle peers and banned peers
    // if (!params.peers.empty()) {
    //     Napi::Array peers = Napi::Array::New(env, params.peers.size());
    //     for (size_t i = 0; i < params.peers.size(); i++) {
    //         Napi::Array peer = Napi::Array::New(env, 2);
    //         peer[0] = params.peers[i].first;
    //         peer[1] = params.peers[i].second;
    //         peers[i] = peer;
    //     }
    //     obj.Set("peers", peers);
    // }

    // if (!params.banned_peers.empty()) {
    //     Napi::Array bannedPeers = Napi::Array::New(env, params.banned_peers.size());
    //     for (size_t i = 0; i < params.banned_peers.size(); i++) {
    //         Napi::Array peer = Napi::Array::New(env, 2);
    //         peer[0] = params.banned_peers[i].first;
    //         peer[1] = params.banned_peers[i].second;
    //         bannedPeers[i] = peer;
    //     }
    //     obj.Set("bannedPeers", bannedPeers);
    // }

    // Handle piece priorities
    // if (!params.piece_priorities.empty()) {
    //     Napi::Array priorities = Napi::Array::New(env, params.piece_priorities.size());
    //     for (size_t i = 0; i < params.piece_priorities.size(); i++) {
    //         priorities[i] = static_cast<int>(params.piece_priorities[i]);
    //     }
    //     obj.Set("piecePriorities", priorities);
    // }

    // Handle root certificate
    if (!params.root_certificate.empty()) obj.Set("root_certificate", params.root_certificate);

    // Additional timestamps if available
    if (params.last_download != 0) obj.Set("last_download", Napi::Date::New(env, params.last_download * 1000.0));
    if (params.last_upload != 0) obj.Set("last_upload", Napi::Date::New(env, params.last_upload * 1000.0));

    return obj;
}



Napi::Value load_torrent(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
        
    try {
        if (info.Length() < 1) {
            throw Napi::Error::New(env, "Expected at least 1 argument");
        }

        libtorrent::add_torrent_params params;

        if (info[0].IsString()) {
            // Handle filename-based loading
            std::string filename = info[0].As<Napi::String>().Utf8Value();
            
            if (info.Length() > 1 && info[1].IsObject()) {
                // With limits config
                libtorrent::load_torrent_limits cfg;
                Napi::Object config = info[1].As<Napi::Object>();
                
                if (config.Has("max_pieces")) cfg.max_pieces = config.Get("max_pieces").ToNumber().Uint32Value();
                if (config.Has("max_buffer_size")) cfg.max_buffer_size = config.Get("max_buffer_size").ToNumber().Int64Value();
                if (config.Has("max_decode_depth")) cfg.max_decode_depth = config.Get("max_decode_depth").ToNumber().Int32Value();
                if (config.Has("max_decode_tokens")) cfg.max_decode_tokens = config.Get("max_decode_tokens").ToNumber().Int32Value();

                params = libtorrent::load_torrent_file(filename, cfg);
            } else {
                // Without limits
                params = libtorrent::load_torrent_file(filename);
            }
        } else if (info[0].IsBuffer()) {
            // Handle buffer-based loading
            Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();
            libtorrent::span<char const> span(buffer.Data(), buffer.Length());
            
            if (info.Length() > 1 && info[1].IsObject()) {
                // With limits config
                libtorrent::load_torrent_limits cfg;
                Napi::Object config = info[1].As<Napi::Object>();
                
                if (config.Has("max_pieces")) cfg.max_pieces = config.Get("max_pieces").ToNumber().Uint32Value();
                if (config.Has("max_buffer_size")) cfg.max_buffer_size = config.Get("max_buffer_size").ToNumber().Int64Value();
                if (config.Has("max_decode_depth")) cfg.max_decode_depth = config.Get("max_decode_depth").ToNumber().Int32Value();
                if (config.Has("max_decode_tokens")) cfg.max_decode_tokens = config.Get("max_decode_tokens").ToNumber().Int32Value();

                params = libtorrent::load_torrent_buffer(span, cfg);
            } else {
                // Without limits
                params = libtorrent::load_torrent_buffer(span);
            }
        } else {
            throw Napi::Error::New(env, "Argument must be a string filepath or a buffer");
        }

        // Convert params to a JavaScript object
        return add_torrent_params_to_js(env, params);
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Object bind_add_torrent_params(Napi::Env env, Napi::Object exports) {
    exports.Set("load_torrent", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Value load_torrent_ret = load_torrent(info);
        return load_torrent_ret;
    }));
    return exports;
}