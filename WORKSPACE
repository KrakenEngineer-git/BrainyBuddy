load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "websocketpp",
    url = "https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz",
    sha256 = "6ce889d85ecdc2d8fa07408d6787e7352510750daa66b5ad44aacb47bea76755",
    strip_prefix = "websocketpp-0.8.2",
    build_file = "@//:BUILD.websocketpp",
)

http_archive(
    name = "nlohmann_json",
    url = "https://github.com/nlohmann/json/archive/v3.10.4.tar.gz",
    sha256 = "1155fd1a83049767360e9a120c43c578145db3204d2b309eba49fbbedd0f4ed3",
    strip_prefix = "json-3.10.4",
    build_file = "@//:BUILD.nlohmann_json",
)