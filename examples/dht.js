const lt = require('../build/Debug/libtorrent');


class SimpleSession {
    constructor() {
        this._readyPromiseResolver = null;

        this._dht_put_listeners = {};
        this._dht_get_listeners = {};
    }

    start() {
        return new Promise((resolve, reject) => {
            this._readyPromiseResolver = [resolve, reject];

            this._sess = lt.session({
                listen_interfaces: '0.0.0.0:6881',
                enable_dht: true,
                alert_mask: 0xffffffff
            });
            this._sess.set_alert_notify(this._handleAlert);
        });
    }

    abort() {
        this._sess.abort();
    }

    _handleAlert = () => {
        const alerts = this._sess.pop_alerts();
        for (const alert of alerts) {
            switch (alert.what) {
                case 'dht_bootstrap':
                    this._readyPromiseResolver[0]();
                    break;
                case 'dht_put':
                    const putresolver = this._dht_put_listeners[alert.target.toString('hex')];
                    if (putresolver) putresolver(alert);
                    break;
                case 'dht_immutable_item':
                    const getresolver = this._dht_get_listeners[alert.target.toString('hex')];
                    if (getresolver) getresolver(alert);
                    break;
            }
        }
    }

    put(item) {
        return new Promise((resolve, reject) => {
            // todo reject
            const target = this._sess.dht_put_immutable_item(item);
            this._dht_put_listeners[target.toString('hex')] = (alert) => {
                resolve({
                    num_success: alert.num_success,
                    target: alert.target
                });
            };
        });
    }

    get(target) {
        return new Promise((resolve, reject) => {
            // todo reject
            this._dht_get_listeners[target.toString('hex')] = (alert) => resolve(alert.item);
            this._sess.dht_get_immutable_item(target);
        });
    }
}


async function main() {
    const session = new SimpleSession();
    await session.start();

    // put and then immediately get
    const result = await session.put('hello world');
    console.log(await session.get(result.target));

    const result2 = await session.put({hello: 'world'});
    console.log(await session.get(result2.target));

    session.abort();
}

main();
