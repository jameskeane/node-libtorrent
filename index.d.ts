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

    type Alert = 
        | DHTPutAlert 
        | DHTImmutableItemAlert 
        | GenericAlert<'dht_bootstrap'>

    type SettingsPack = {
        listen_interfaces?: string,
        enable_dht?: boolean,
        alert_mask?: number,
        dht_bootstrap_nodes?: string[],
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
    }

    function session(options: SettingsPack): LtSession;
}
