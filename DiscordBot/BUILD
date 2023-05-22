cc_library(
    name = "brainy_buddy",
    srcs = [
        "BrainyBuddy.cpp",
        "BrainyBuddy.hpp",
    ],
    deps = [
        "@websocketpp//:websocketpp",
        "@nlohmann_json//:nlohmann_json", 
        "@curl//:curl",
        "//WebSocketHandler/impl:websocket_handler_lib",
        "//DiscordClient:discord_client",
        "//OpenAIClient:openai_client",
        "//ThreadPool:thread_pool",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lcurl"],
    visibility = ["//visibility:public"],
)
