cc_library(
    name = "curl_handler_lib",
    srcs = [
        "CurlHandler.cpp",
        "CurlHandler.hpp",
        "ResponseFuture.cpp",
        "ResponseFuture.hpp",
    ],
    deps = [
        "@curl//:source",
        "@nlohmann_json//:nlohmann_json", 
        "//ThreadPool:thread_pool",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
