cc_library(
    name = "openai_client",
    srcs = [
        "OpenAIClient.cpp",
        "OpenAIClient.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
        "//CurlHandler:curl_handler_lib",
        "//ThreadPool:thread_pool",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
