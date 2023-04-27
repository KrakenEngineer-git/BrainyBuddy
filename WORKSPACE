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

http_archive(
    name = "bazel_skylib",
    sha256 = "07b4117379dde7ab382345c3b0f5edfc6b7cff6c93756eac63da121e0bbcc5de",
    strip_prefix = "bazel-skylib-1.1.1",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/refs/tags/1.1.1.tar.gz"],
)


# http_archive(
#     name = "rules_foreign_cc",
#     sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
#     strip_prefix = "rules_foreign_cc-0.9.0",
#     urls = ["https://github.com/bazelbuild/rules_foreign_cc/archive/0.9.0.tar.gz"],
# )

# load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# rules_foreign_cc_dependencies()

# load("@rules_foreign_cc//:tools/build_defs/configure.bzl", "configure_repository")

# configure_repository(name = "cmake_tool")


# cmake(
#     name = "com_github_curl",
#     cache_entries = {
#         "CMAKE_USE_OPENSSL": "ON",
#         "CMAKE_USE_LIBSSH2": "OFF",
#         "CMAKE_USE_WOLFSSL": "OFF",
#         "BUILD_SHARED_LIBS": "OFF",
#         "BUILD_CURL_EXE": "OFF",
#     },
#     lib_source = "@com_github_curl//:all",
#     out_include_dir = "include",
#     out_lib_dir = "lib",
#     repo_version = "curl-7_79_1",
#     shallow_since = "2022-08-30",
#     strip_include_prefix = "include",
#     version = "7.79.1",
#     urls = ["https://github.com/curl/curl/releases/download/curl-7_79_1/curl-7.79.1.tar.gz"],
#     sha256 = "0606f74b1182ab732a17c11613cbbaf7084f2e6cca432642d0e3ad7c224c3689",
# )