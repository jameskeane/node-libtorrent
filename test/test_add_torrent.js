const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require("../build/Debug/libtorrent");

const { createQuietSession } = require('./util');


const testTorrent = '6431333a6372656174696f6e2064617465693137343233393732313865383a656e636f64696e67353a5554462d38343a696e666f64363a6c656e677468693465343a6e616d6532393a556e6e616d656420546f7272656e74203137343233393732313735393831323a7069656365206c656e67746869313633383465363a70696563657332303aa94a8fe5ccb19ba61c4c0873d391e987982fbbd365383a75726c2d6c6973746c6565';
const infohash = Buffer.from('a90117e4269a35b2ba8cdbc2efc6757ee343fdfc', 'hex');

describe("libtorrent.async_add_torrent", () => {
    it('works', (t, done) => {
        const session = createQuietSession();

        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                switch (alert.what) {
                    case 'add_torrent':
                        assert(alert.torrent.info_hashes().equals(infohash));
                        assert(alert.torrent instanceof lt.TorrentHandle);
                        session.abort();
                        done();
                        break;
                }
            }
        });

        session.async_add_torrent({ save_path: '.', info_hashes: infohash });
    });

    it('supports buffers', (t, done) => {
        const session = createQuietSession();

        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                switch (alert.what) {
                    case 'add_torrent':
                        assert(alert.torrent instanceof lt.TorrentHandle);
                        session.abort();
                        done();
                        break;
                }
            }
        });

        session.async_add_torrent(Buffer.from(testTorrent, 'hex'), { save_path: '.' });
    });
});

describe('libtorrent.load_torrent', () => {
    it('supports buffers', (t, done) => {
        const params = lt.load_torrent(Buffer.from(testTorrent, 'hex'));
        const infohash = Buffer.from('362baf22567dea11dca806d397a1deedc40f65b2', 'hex');
        assert(params.info_hashes.equals(infohash));

        const session = createQuietSession();
        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                switch (alert.what) {
                    case 'add_torrent':
                        assert(alert.torrent.info_hashes().equals(infohash));
                        assert(alert.torrent instanceof lt.TorrentHandle);
                        session.abort();
                        done();
                        break;
                }
            }
        });

        session.async_add_torrent({...params, 'save_path': '.'});
    });

    it('supports ti strings', (t, done) => {
    });
});

