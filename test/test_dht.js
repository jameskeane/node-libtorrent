const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require('../build/Debug/libtorrent');


describe("libtorrent dht functions", () => {
    it("can put mutable items", (t, done) => {
        session = lt.session({
            listen_interfaces: '0.0.0.0:6881',
            enable_dht: true,
            alert_mask: 0xffffffff,
        });

        const value = 'Hello World!';
        const pub = Buffer.from('a196635861b6d574283c5456b43238f3224a91e4c75b5146f649da5961ca90b9', 'hex');
        const priv = Buffer.from('0d8f552d4b568461c1df818695faa231f88b569a07f2dff0572a12c898b477bfa196635861b6d574283c5456b43238f3224a91e4c75b5146f649da5961ca90b9', 'hex');
        const sig = Buffer.from('5b801808f320e556aeab5061a0320644cadd4505b4b7314179a233ca610ccc7a0432e6a7eaf419e77ab81276710e0f09538ef99aa387be08b691461105219a0f', 'hex');

        session.set_alert_notify(() => {
            const alerts = session.pop_alerts();
            for (const alert of alerts) {
                if (alert.category == lt.alert.category_t.error_notification) {
                    done(new Error(alert.message));
                }
                if (alert.what == 'dht_bootstrap') {
                    session.dht_put_mutable_item(pub, value, sig, 1, 'test');
                } else if (alert.what == 'dht_put') {
                    session.dht_get_mutable_item(pub, 'test');
                } else if (alert.what == 'dht_mutable_item') {
                    assert(alert.salt == 'test');
                    assert(alert.seq == 1);
                    assert(alert.item == value);
                    session.abort();
                    done();
                }
            }
        });

    });
});
