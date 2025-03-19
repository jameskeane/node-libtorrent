declare module '@pulseapp/node-libtorrent' {
    type Entry = string | number | Buffer | { [key: string]: Entry };

    interface GenericAlert<T> {
        message: string;
        what: T;
        category: number;
        type: string;
        timestamp: number;
    }

    interface DHTPutAlert extends GenericAlert {
        what: 'dht_put';
        target: Buffer;
        num_success: number;
    }

    interface DHTImmutableItemAlert extends GenericAlert {
        what: 'dht_immutable_item';
        target: Buffer;
        item: Entry;
    }

    interface DHTMutableItemAlert extends GenericAlert {
        what: 'dht_mutable_item';
        key: Buffer;
        sig: Buffer;
        seq: number;
        salt: string;
        authoritative: boolean;
        item: Entry;
    }

    interface AddTorentAlert extends GenericAlert {
        what: 'add_torrent';
        info_hashes: Buffer;
        name: string;
        save_path: string;
        error_code: number;
        error_message: string;
        torrent: TorrentHandle;
    }

    type Alert = 
        | AddTorentAlert
        | DHTPutAlert 
        | DHTImmutableItemAlert 
        | DHTMutableItemAlert
        | GenericAlert<'dht_bootstrap'>

    type SettingsPack = {
        listen_interfaces?: string,
        enable_dht?: boolean,
        alert_mask?: number,
        dht_bootstrap_nodes?: string,
        dht_announce_interval?: number,
        dht_max_peers?: number,
        dht_max_torrents?: number,
        dht_max_dht_items?: number,
        dht_max_torrent_search_reply?: number,
        allow_multiple_connections_per_ip?: boolean,
        enable_natpmp?: boolean,
        enable_lsd?: boolean,
        dht_restrict_routing_ips?: boolean,
        dht_restrict_search_ips?: boolean,
        // todo
    };

    interface Session {
        set_alert_notify(callback: () => void): void;
        pop_alerts(): Alert[];
        abort(): void;
        dht_put_immutable_item(item: Entry): Buffer;
        dht_get_immutable_item(target: Buffer): void;
        dht_put_mutable_item(pubKey: Buffer, entry: Entry, sig: Buffer, seq: number, salt?: string): Buffer;
        dht_get_mutable_item(key: Buffer, salt: string): void;
        async_add_torrent(params: add_torrent_params): void;
        async_add_torrent(torrent: Buffer, opt_params: add_torrent_params): void;
    }

    // todo type config
    function session(options: SettingsPack): LtSession;

    type load_torrent_config_t = {
        max_pieces?: number,
        max_buffer_size?: number,
        max_decode_depth?: number,
        max_decode_tokens?: number,
    }

    function load_torrent(filename: string): add_torrent_params;
    function load_torrent(filename: string, cfg: load_torrent_config_t): add_torrent_params;
    function load_torrent(torrent: Buffer): add_torrent_params;
    function load_torrent(torrent: Buffer, cfg: load_torrent_config_t): add_torrent_params;

    enum storage_mode_t {
        storage_mode_allocate = 0,
        storage_mode_sparse = 1,
    }

    enum download_priority_t {
        dont_download = 0,
        default_priority = 4,
        low_priority = 1,
        top_priority = 7,
    }

    class TorrentHandle {
        info_hashes(): Buffer;
    }

    type torrent_extension_t = any; // todo

    type TorrentInfo = any; // todo
    type add_torrent_params = {
        ti?: TorrentInfo,
        trackers? : string[],
        tracker_tiers?: number[],
        dht_nodes?: [string, number][],
        name?: string,
        save_path?: string,
        storage_mode?: storage_mode_t,
        userdata?: any,
        file_priorities?: download_priority_t[],
        extensions?: torrent_extension_t[],  // todo would be cool to define these in javascript
        trackerid?: string,
        flags?: number,
        info_hashes?: Buffer,
        max_uploads?: number,
        max_connections?: number,
        upload_limit?: number,
        download_limit?: number,
        total_uploaded?: number,
        total_downloaded?: number,
        active_time?: number,
        finished_time?: number,
        seeding_time?: number,
        added_time?: Date,
        completed_time?: Date,
        last_seen_complete?: Date,
        num_complete?: number,
        num_incomplete?: number,
        num_downloaded?: number,
        http_seeds?: string[],
        url_seeds?: string[],
        peers?: [string, number][],
        banned_peers?: [string, number][],
        // todo
        // unfinished_pieces?: [number, Buffer][],
        // have_pieces?: number,
        // verified_pieces?: number,
        piece_priorities?: download_priority_t[],
        merkle_trees?: [Buffer[], number][];
        merkle_tree_mask?: [boolean[], number][];
        verified_leaf_hashes?: [boolean[], number][];
        renamed_files?: Map<number, string>,
        last_download?: Date,
        last_upload?: Date,
        root_certificate?: string
    }
}
