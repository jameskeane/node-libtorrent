{
  "targets": [
    {
      "target_name": "libtorrent",
      "product_name": "libtorrent",
      "sources": [
          "libtorrent.cc",
          "src/session.cc"
        ],

      "cflags_cc": [
        "-fPIC",
        "`pkg-config --cflags libtorrent-rasterbar`"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ],
      "ldflags": [],
      "libraries": [
        "`pkg-config --libs libtorrent-rasterbar`"
      ],

      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
