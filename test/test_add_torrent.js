const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require("../build/Debug/libtorrent");

describe("libtorrent.async_add_torrent", () => {
    it('works', () => {
        return;

        session = lt.session({
            listen_interfaces: '0.0.0.0:6881',
            alert_mask: 0xffffffff
        });

        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                switch (alert.what) {
                    case 'dht_bootstrap':
                        this._sess.async_add_torrent({ save_path: '.', infohashes: infohash });
                        break;
                    case 'add_torrent':
                        session.abort();  // todo this hangs
                        done();
                        break;
                }
            }
        });
    });
});
