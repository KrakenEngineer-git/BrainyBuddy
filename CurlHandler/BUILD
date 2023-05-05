cc_library(
    name = "curl_handler_lib",
    srcs = [
        "CurlHandler.cpp",
        "CurlHandler.hpp",
    ],
    deps = [
        "@curl//:source",
        "@nlohmann_json//:nlohmann_json", 
    ],
    copts = [
        "-std=c++11",
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
