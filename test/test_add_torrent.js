const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require("../build/Debug/libtorrent");

const { createNetwork } = require('./util');

const infohash = Buffer.from('a90117e4269a35b2ba8cdbc2efc6757ee343fdfc', 'hex');

describe("libtorrent.async_add_torrent", () => {
    it('works', (t, done) => {
        const session = createNetwork(5, 7100);
        session.async_add_torrent({ save_path: '.', info_hashes: infohash });

        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                switch (alert.what) {
                    case 'add_torrent':
                        assert(alert.torrent.info_hash().equals(infohash));
                        assert(alert.torrent instanceof lt.TorrentHandle);
                        session.abort();
                        done();
                        break;
                }
            }
        });

    });
});
