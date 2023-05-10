cc_library(
    name = "openai_client",
    srcs = [
        "OpenAIClient.cpp",
        "OpenAIClient.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
        "//CurlHandler:curl_handler_lib",
        "//utilities",
        "//ThreadPool:thread_pool",
    ],
    copts = [
        "-std=c++11",
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
