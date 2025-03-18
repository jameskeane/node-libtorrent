#include "alert.h"
#include "entry.h"

using namespace lt;

Napi::Value alert_to_js(const Napi::Env& env, const lt::alert* a) {
    Napi::Object obj = Napi::Object::New(env);
    
    // Add base properties common to all alerts
    obj.Set("message", Napi::String::New(env, a->message()));
    obj.Set("what", Napi::String::New(env, a->what()));
    obj.Set("category", Napi::Number::New(env, a->category()));
    obj.Set("type", Napi::String::New(env, a->what()));
    obj.Set("timestamp", Napi::Date::New(env, std::chrono::duration_cast<std::chrono::milliseconds>(a->timestamp().time_since_epoch()).count()));
    
    // Handle specific alert types
    switch (a->type()) {
        case lt::listen_succeeded_alert::alert_type: {
            auto* lsa = static_cast<const lt::listen_succeeded_alert*>(a);
            obj.Set("address", Napi::String::New(env, lsa->address.to_string()));
            obj.Set("port", Napi::Number::New(env, lsa->port));
            break;
        }
        case lt::listen_failed_alert::alert_type: {
            auto* lfa = static_cast<const lt::listen_failed_alert*>(a);
            obj.Set("address", Napi::String::New(env, lfa->address.to_string()));
            obj.Set("port", Napi::Number::New(env, lfa->port));
            obj.Set("error_code", Napi::Number::New(env, lfa->error.value()));
            obj.Set("error_message", Napi::String::New(env, lfa->error.message()));
            break;
        }

        case lt::add_torrent_alert::alert_type: {
            auto* ata = static_cast<const lt::add_torrent_alert*>(a);
            auto info = ata->params.info_hashes.get_best();
            obj.Set("info_hashes", Napi::Buffer<char>::Copy(env, info.data(), info.size()));
            obj.Set("name", Napi::String::New(env, ata->params.name));
            obj.Set("save_path", Napi::String::New(env, ata->params.save_path));
            obj.Set("error_code", Napi::Number::New(env, ata->error.value()));
            obj.Set("error_message", Napi::String::New(env, ata->error.message()));
            break;
        }
        case lt::torrent_need_cert_alert::alert_type: {
            auto* tnca = static_cast<const lt::torrent_need_cert_alert*>(a);
            obj.Set("name", Napi::String::New(env, tnca->torrent_name()));

            // todo wrap the torrent handle and pass it along
            break;
        }
        case lt::torrent_error_alert::alert_type: {
            auto* tea = static_cast<const lt::torrent_error_alert*>(a);
            obj.Set("error_code", Napi::Number::New(env, tea->error.value()));
            obj.Set("error_message", Napi::String::New(env, tea->error.message()));
            break;
        }

        case lt::dht_bootstrap_alert::alert_type: break;
        case lt::dht_error_alert::alert_type: {
            auto* dea = static_cast<const lt::dht_error_alert*>(a);
            obj.Set("error_code", Napi::Number::New(env, dea->error.value()));
            break;
        }
        case lt::dht_put_alert::alert_type: {
            auto* dpa = static_cast<const lt::dht_put_alert*>(a);
            obj.Set("target", Napi::Buffer<char>::Copy(env, dpa->target.data(), dpa->target.size()));
            obj.Set("num_success", Napi::Number::New(env, dpa->num_success));
            obj.Set("public_key", Napi::Buffer<char>::Copy(env, dpa->public_key.data(), dpa->public_key.size()));
            obj.Set("signature", Napi::Buffer<char>::Copy(env, dpa->signature.data(), dpa->signature.size()));
            obj.Set("salt", Napi::String::New(env, dpa->salt));
            obj.Set("seq", Napi::Number::New(env, dpa->seq));
            break;
        }
        case lt::dht_immutable_item_alert::alert_type: {
            auto* dia = static_cast<const lt::dht_immutable_item_alert*>(a);
            obj.Set("target", Napi::Buffer<char>::Copy(env, dia->target.data(), dia->target.size()));

            try {
                obj.Set("item", EntryToValue(env, dia->item));
            } catch (const std::exception& e) {
                obj.Set("item_error", Napi::String::New(env, e.what()));
            }
            break;
        }

        case lt::dht_mutable_item_alert::alert_type: {
            auto* dmia = static_cast<const lt::dht_mutable_item_alert*>(a);
            obj.Set("key", Napi::Buffer<char>::Copy(env, dmia->key.data(), dmia->key.size()));
            obj.Set("signature", Napi::Buffer<char>::Copy(env, dmia->signature.data(), dmia->signature.size()));
            obj.Set("seq", Napi::Number::New(env, dmia->seq));
            obj.Set("salt", Napi::String::New(env, dmia->salt));
            obj.Set("authoritative", Napi::Boolean::New(env, dmia->authoritative));

            try {
                obj.Set("item", EntryToValue(env, dmia->item));
            } catch (const std::exception& e) {
                obj.Set("item_error", Napi::String::New(env, e.what()));
            }
            break;
        }

        // case lt::dht_announce_alert::alert_type: {
        //     auto* daa = static_cast<const lt::dht_announce_alert*>(a);
        //     obj.Set("ip", Napi::String::New(env, daa->ip.to_string()));
        //     obj.Set("port", Napi::Number::New(env, daa->port));
        //     obj.Set("info_hash", Napi::String::New(env, to_hex(daa->info_hash.to_string())));
        //     break;
        // }
        // case lt::dht_get_peers_alert::alert_type: {
        //     auto* dgpa = static_cast<const lt::dht_get_peers_alert*>(a);
        //     obj.Set("info_hash", Napi::String::New(env, to_hex(dgpa->info_hash.to_string())));
        //     break;
        // }
        // case lt::dht_reply_alert::alert_type: {
        //     auto* dra = static_cast<const lt::dht_reply_alert*>(a);
        //     obj.Set("num_peers", Napi::Number::New(env, dra->num_peers()));
        //     break;
        // }
        // case lt::dht_stats_alert::alert_type: {
        //     auto* dsa = static_cast<const lt::dht_stats_alert*>(a);
        //     Napi::Array active_requests = Napi::Array::New(env);
        //     for (int i = 0; i < int(dsa->active_requests.size()); ++i) {
        //         active_requests.Set(i, Napi::Number::New(env, dsa->active_requests[i]));
        //     }
        //     obj.Set("active_requests", active_requests);
        //     obj.Set("routing_table_size", Napi::Number::New(env, dsa->routing_table_size()));
        //     break;
        // }
        // case lt::dht_log_alert::alert_type: {
        //     auto* dla = static_cast<const lt::dht_log_alert*>(a);
        //     obj.Set("log_message", Napi::String::New(env, dla->log_message()));
        //     obj.Set("module", Napi::String::New(env, dla->module()));
        //     break;
        // }
        // case lt::dht_error_alert::alert_type: {
        //     auto* dea = static_cast<const lt::dht_error_alert*>(a);
        //     obj.Set("error_code", Napi::Number::New(env, dea->error.value()));
        //     obj.Set("error_message", Napi::String::New(env, dea->error.message()));
        //     obj.Set("operation", Napi::String::New(env, dea->operation));
        //     break;
        // }
        // case lt::dht_pkt_alert::alert_type: {
        //     auto* dpa = static_cast<const lt::dht_pkt_alert*>(a);
        //     obj.Set("pkt_type", Napi::String::New(env, dpa->pkt_type));
        //     obj.Set("node", Napi::String::New(env, dpa->node.address().to_string() + ":" + std::to_string(dpa->node.port())));
        //     break;
        // }
        // case lt::dht_mutable_item_alert::alert_type: {
        //     auto* dma = static_cast<const lt::dht_mutable_item_alert*>(a);
        //     obj.Set("key", Napi::String::New(env, to_hex(std::string(dma->key.data(), dma->key.size()))));
        //     obj.Set("signature", Napi::String::New(env, to_hex(std::string(dma->signature.data(), dma->signature.size()))));
        //     obj.Set("seq", Napi::Number::New(env, dma->seq));
        //     obj.Set("salt", Napi::String::New(env, dma->salt));
        //     obj.Set("item_size", Napi::Number::New(env, dma->item.size()));
        //     break;
        // }
        // case lt::torrent_alert::alert_type: {
        //     auto* ta = static_cast<const lt::torrent_alert*>(a);
        //     obj.Set("handle", Napi::String::New(env, to_hex(ta->handle.info_hash().to_string())));
        //     break;
        // }
        // case lt::tracker_alert::alert_type: {
        //     auto* ta = static_cast<const lt::tracker_alert*>(a);
        //     obj.Set("url", Napi::String::New(env, ta->tracker_url()));
        //     break;
        // }
        // case lt::torrent_added_alert::alert_type: {
        //     auto* taa = static_cast<const lt::torrent_added_alert*>(a);
        //     obj.Set("handle", Napi::String::New(env, to_hex(taa->handle.info_hash().to_string())));
        //     break;
        // }
        // case lt::torrent_removed_alert::alert_type: {
        //     auto* tra = static_cast<const lt::torrent_removed_alert*>(a);
        //     obj.Set("info_hash", Napi::String::New(env, to_hex(tra->info_hash.to_string())));
        //     break;
        // }
        // case lt::state_changed_alert::alert_type: {
        //     auto* sca = static_cast<const lt::state_changed_alert*>(a);
        //     obj.Set("state", Napi::Number::New(env, sca->state));
        //     obj.Set("prev_state", Napi::Number::New(env, sca->prev_state));
        //     break;
        // }
        // case lt::tracker_error_alert::alert_type: {
        //     auto* tea = static_cast<const lt::tracker_error_alert*>(a);
        //     obj.Set("url", Napi::String::New(env, tea->tracker_url()));
        //     obj.Set("error_code", Napi::Number::New(env, tea->error.value()));
        //     obj.Set("error_message", Napi::String::New(env, tea->error.message()));
        //     obj.Set("times_in_row", Napi::Number::New(env, tea->times_in_row));
        //     break;
        // }
        // case lt::tracker_warning_alert::alert_type: {
        //     auto* twa = static_cast<const lt::tracker_warning_alert*>(a);
        //     obj.Set("url", Napi::String::New(env, twa->tracker_url()));
        //     break;
        // }
        // case lt::metadata_received_alert::alert_type: {
        //     auto* mra = static_cast<const lt::metadata_received_alert*>(a);
        //     obj.Set("handle", Napi::String::New(env, to_hex(mra->handle.info_hash().to_string())));
        //     break;
        // }
        // case lt::piece_finished_alert::alert_type: {
        //     auto* pfa = static_cast<const lt::piece_finished_alert*>(a);
        //     obj.Set("piece_index", Napi::Number::New(env, pfa->piece_index));
        //     break;
        // }
        // case lt::torrent_finished_alert::alert_type: {
        //     auto* tfa = static_cast<const lt::torrent_finished_alert*>(a);
        //     obj.Set("handle", Napi::String::New(env, to_hex(tfa->handle.info_hash().to_string())));
        //     break;
        // }
        // case lt::peer_connect_alert::alert_type: {
        //     auto* pca = static_cast<const lt::peer_connect_alert*>(a);
        //     obj.Set("endpoint", Napi::String::New(env, pca->endpoint.address().to_string() + ":" + std::to_string(pca->endpoint.port())));
        //     obj.Set("pid", Napi::String::New(env, lt::aux::to_hex(pca->pid.to_string())));
        //     break;
        // }
        // case lt::peer_disconnected_alert::alert_type: {
        //     auto* pda = static_cast<const lt::peer_disconnected_alert*>(a);
        //     obj.Set("endpoint", Napi::String::New(env, pda->endpoint.address().to_string() + ":" + std::to_string(pda->endpoint.port())));
        //     obj.Set("error_code", Napi::Number::New(env, pda->error.value()));
        //     obj.Set("error_message", Napi::String::New(env, pda->error.message()));
        //     break;
        // }
        // case lt::torrent_error_alert::alert_type: {
        //     auto* tea = static_cast<const lt::torrent_error_alert*>(a);
        //     obj.Set("error_code", Napi::Number::New(env, tea->error.value()));
        //     obj.Set("error_message", Napi::String::New(env, tea->error.message()));
        //     break;
        // }
        // case lt::add_torrent_alert::alert_type: {
        //     auto* ata = static_cast<const lt::add_torrent_alert*>(a);
        //     obj.Set("handle", Napi::String::New(env, to_hex(ata->handle.info_hash().to_string())));
        //     obj.Set("error_code", Napi::Number::New(env, ata->error.value()));
        //     obj.Set("error_message", Napi::String::New(env, ata->error.message()));
        //     break;
        // }
        // Add more specific alert types as needed
    }
    
    return obj;
}

Napi::Object bind_alerts(Napi::Env env, Napi::Object exports) {
    Napi::Object alert = Napi::Object::New(env);
    
    // Create category_t object with alert categories
    Napi::Object category_t = Napi::Object::New(env);
    category_t.Set("error_notification", Napi::Number::New(env, lt::alert::error_notification));
    category_t.Set("peer_notification", Napi::Number::New(env, lt::alert::peer_notification));
    category_t.Set("port_mapping_notification", Napi::Number::New(env, lt::alert::port_mapping_notification));
    category_t.Set("storage_notification", Napi::Number::New(env, lt::alert::storage_notification));
    category_t.Set("tracker_notification", Napi::Number::New(env, lt::alert::tracker_notification));
    category_t.Set("connect_notification", Napi::Number::New(env, lt::alert::connect_notification));
    category_t.Set("status_notification", Napi::Number::New(env, lt::alert::status_notification));
    category_t.Set("ip_block_notification", Napi::Number::New(env, lt::alert::ip_block_notification));
    category_t.Set("performance_warning", Napi::Number::New(env, lt::alert::performance_warning));
    category_t.Set("dht_notification", Napi::Number::New(env, lt::alert::dht_notification));
    // category_t.Set("stats_notification", Napi::Number::New(env, lt::alert::stats_notification));
    category_t.Set("session_log_notification", Napi::Number::New(env, lt::alert::session_log_notification));
    category_t.Set("torrent_log_notification", Napi::Number::New(env, lt::alert::torrent_log_notification));
    category_t.Set("peer_log_notification", Napi::Number::New(env, lt::alert::peer_log_notification));
    category_t.Set("incoming_request_notification", Napi::Number::New(env, lt::alert::incoming_request_notification));
    category_t.Set("dht_log_notification", Napi::Number::New(env, lt::alert::dht_log_notification));
    category_t.Set("dht_operation_notification", Napi::Number::New(env, lt::alert::dht_operation_notification));
    category_t.Set("port_mapping_log_notification", Napi::Number::New(env, lt::alert::port_mapping_log_notification));
    category_t.Set("picker_log_notification", Napi::Number::New(env, lt::alert::picker_log_notification));
    category_t.Set("file_progress_notification", Napi::Number::New(env, lt::alert::file_progress_notification));
    category_t.Set("piece_progress_notification", Napi::Number::New(env, lt::alert::piece_progress_notification));
    category_t.Set("upload_notification", Napi::Number::New(env, lt::alert::upload_notification));
    category_t.Set("block_progress_notification", Napi::Number::New(env, lt::alert::block_progress_notification));
    category_t.Set("all_categories", Napi::Number::New(env, lt::alert::all_categories));
    
    // Add category_t to alerts namespace
    category_t.Freeze();
    alert.Set("category_t", category_t);
    
    // Add alerts namespace to exports
    alert.Freeze();
    exports.Set("alert", alert);  
    return exports;
}
