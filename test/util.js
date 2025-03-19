const lt = require('../build/Debug/libtorrent');


function createNetwork(numberOfNodes, startPort = 6881) {
    const nodes = [];
    for (let i = 0; i < numberOfNodes; i++) {
        const node = lt.session({
            listen_interfaces: `127.0.0.1:${startPort+i}`,
            dht_bootstrap_nodes: `127.0.0.1:${startPort}`, allow_multiple_connections_per_ip: true,
            enable_natpmp: false, enable_lsd: false, enable_upnp: false,
            dht_restrict_routing_ips: false, dht_restrict_search_ips: false,
            alert_mask: 0xffffffff,
        });
        nodes.push(node);
    }
    return nodes[nodes.length-1];
}

module.exports = { createNetwork };
