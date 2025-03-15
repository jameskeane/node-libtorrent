const { describe, mock, it } = require('node:test');
const assert = require('node:assert/strict');
const lt = require("../build/Debug/libtorrent");

describe("libtorrent.alert", () => {

    it('defines category types', () => {
        assert.ok(lt.alert.category_t, "alert.category_t should be defined");
        
        // Check for expected category types
        assert.equal(typeof lt.alert.category_t.error_notification, 'number');
        assert.equal(typeof lt.alert.category_t.peer_notification, 'number');
        assert.equal(typeof lt.alert.category_t.port_mapping_notification, 'number');
        assert.equal(typeof lt.alert.category_t.storage_notification, 'number');
        assert.equal(typeof lt.alert.category_t.tracker_notification, 'number');
        assert.equal(typeof lt.alert.category_t.connect_notification, 'number');
        assert.equal(typeof lt.alert.category_t.status_notification, 'number');
        assert.equal(typeof lt.alert.category_t.ip_block_notification, 'number');
        assert.equal(typeof lt.alert.category_t.performance_warning, 'number');
        assert.equal(typeof lt.alert.category_t.dht_notification, 'number');
        // assert.equal(typeof lt.alert.category_t.stats_notification, 'number');
        assert.equal(typeof lt.alert.category_t.session_log_notification, 'number');
        assert.equal(typeof lt.alert.category_t.torrent_log_notification, 'number');
        assert.equal(typeof lt.alert.category_t.peer_log_notification, 'number');
        assert.equal(typeof lt.alert.category_t.incoming_request_notification, 'number');
        assert.equal(typeof lt.alert.category_t.dht_log_notification, 'number');
        assert.equal(typeof lt.alert.category_t.dht_operation_notification, 'number');
        assert.equal(typeof lt.alert.category_t.port_mapping_log_notification, 'number');
        assert.equal(typeof lt.alert.category_t.picker_log_notification, 'number');        
        assert.equal(typeof lt.alert.category_t.file_progress_notification, 'number');
        assert.equal(typeof lt.alert.category_t.piece_progress_notification, 'number');
        assert.equal(typeof lt.alert.category_t.upload_notification, 'number');
        assert.equal(typeof lt.alert.category_t.block_progress_notification, 'number');
        assert.equal(typeof lt.alert.category_t.all_categories, 'number');
    });

    

});