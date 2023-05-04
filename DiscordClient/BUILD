cc_library(
    name = "discord_client",
    srcs = [
        "DiscordClient.cpp",
        "DiscordClient.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
        "@curl//:source",
        "//DiscordEventHandler:discord_event_handler",
        "//WebSocketHandler/impl:websocket_handler_lib",
        "//utilities",
    ],
    copts = [
        "-std=c++11",
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
