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

    interface AddTorentAlert extends GenericAlert {
        what: 'add_torrent';
        info_hashes: Buffer;
        name: string;
        save_path: string;
        error_code: number;
        error_message: string;
    }

    type Alert = 
        | AddTorentAlert
        | DHTPutAlert 
        | DHTImmutableItemAlert 
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
        // todo
    };

    interface Session {
        set_alert_notify(callback: () => void): void;
        pop_alerts(): Alert[];
        abort(): void;
        dht_put_immutable_item(item: Entry): Buffer;
        dht_get_immutable_item(target: Buffer): void;
        async_add_torrent(params: add_torrent_params): void;
    }

    function session(options: SettingsPack): LtSession;

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
