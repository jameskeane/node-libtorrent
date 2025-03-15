const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');

const lt = require("../build/Debug/libtorrent");

describe("libtorrent.session", () => {
    it("must be defined", () => assert(lt.session));

    it("can accept string,int,bool settings", () => {
        session = lt.session({
            user_agent: 'nodeltrb/',
            report_web_seed_downloads: false,
            stop_tracker_timeout: 10
        });
        session.abort();
    });

    it("will reject invalid settings", () => {
        assert.throws(() => lt.session({ foobar: false }), {
            name: 'TypeError',
            message: 'Unknown settingpack key "foobar"'
        });
        assert.throws(() => lt.session({ alert_mask: {} }), {
            name: 'TypeError',
            message: 'Unsupported type for settingpack key "alert_mask"'
        });
    });

    it("exposes the class, but can not be instantiated directly", () => {
        assert.throws(() => new lt.Session({ foobar: false }), {
            name: 'Error',
            message: 'Cannot instantiate Session directly. Use session() instead'
        });
    });

    it("allows you to set the alert_notify function", (t, done) => {
        session = lt.session({
            listen_interfaces: '0.0.0.0:6881',
            enable_dht: true,
            alert_mask: 0xffffffff
        });

        session.set_alert_notify(() => {
            done();
            session.abort();
        });
    })
});
