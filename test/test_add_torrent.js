const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require("../build/Debug/libtorrent");

const { createQuietSession } = require('./util');


const testTorrent = '64383a616e6e6f756e636534303a7564703a2f2f747261636b65722e6c656563686572732d70617261646973652e6f72673a3639363931333a616e6e6f756e63652d6c6973746c6c34303a7564703a2f2f747261636b65722e6c656563686572732d70617261646973652e6f72673a36393639656c33343a7564703a2f2f747261636b65722e636f707065727375726665722e746b3a36393639656c33333a7564703a2f2f747261636b65722e6f70656e747261636b722e6f72673a31333337656c32333a7564703a2f2f6578706c6f6469652e6f72673a36393639656c33313a7564703a2f2f747261636b65722e656d706972652d6a732e75733a31333337656c32363a7773733a2f2f747261636b65722e62746f7272656e742e78797a656c33323a7773733a2f2f747261636b65722e6f70656e776562746f7272656e742e636f6d656c32383a7773733a2f2f747261636b65722e776562746f7272656e742e646576656531333a6372656174696f6e2064617465693137343233353935333265383a656e636f64696e67353a5554462d38343a696e666f64363a6c656e677468693465343a6e616d6532393a556e6e616d656420546f7272656e74203137343233353935333137303131323a7069656365206c656e67746869313633383465363a70696563657332303aa94a8fe5ccb19ba61c4c0873d391e987982fbbd36565';
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
                        console.log(alert);
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
    it('supports buffers', (t) => {
        const params = lt.load_torrent(Buffer.from(testTorrent, 'hex'));
        assert(params.info_hashes.toString('hex') == '17c3baa0caa3e280e6ddaac5f6af6387d3a0d55b');
    });
});

