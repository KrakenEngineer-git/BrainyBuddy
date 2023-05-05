load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

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

http_archive(
    name = "bazel_skylib",
    sha256 = "07b4117379dde7ab382345c3b0f5edfc6b7cff6c93756eac63da121e0bbcc5de",
    strip_prefix = "bazel-skylib-1.1.1",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/refs/tags/1.1.1.tar.gz"],
)


maybe(
    http_archive,
    name = "curl",
    build_file = "@//:BUILD.curl",
    url = "https://github.com/curl/curl/archive/curl-8_0_1.tar.gz",
    sha256 = "d9aefeb87998472cd79418edd4fb4dc68c1859afdbcbc2e02400b220adc64ec1",
    strip_prefix = "curl-curl-8_0_1",
)