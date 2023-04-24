cc_binary(
    name = "my_bot",
    srcs = ["main.cpp"],
    deps = [
        "@websocketpp//:websocketpp",
        "@nlohmann_json//:nlohmann_json", 
        "//WebSocketHandler/impl:websocket_handler_lib",
        "//DiscordClient:discord_client",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lssl", "-lcrypto"],
    visibility = ["//visibility:public"],
)
