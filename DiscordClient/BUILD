cc_library(
    name = "discord_client",
    srcs = [
        "DiscordClient.cpp",
        "DiscordClient.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
        "//CurlHandler:curl_handler_lib",
        "//DiscordEventHandler:discord_event_handler",
        "//WebSocketHandler/impl:websocket_handler_lib",
        "//ThreadPool:thread_pool",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
