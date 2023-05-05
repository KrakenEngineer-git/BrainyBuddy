cc_library(
    name = "curl_handler_lib",
    srcs = [
        "CurlHandler.cpp",
        "CurlHandler.hpp",
    ],
    deps = [
        "@curl//:source",
    ],
    copts = [
        "-std=c++11",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
