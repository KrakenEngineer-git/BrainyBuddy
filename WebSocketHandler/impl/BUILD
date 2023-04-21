cc_library(
    name = "websocket_handler_lib",
    srcs = [
        "WebsocketClient.cpp",
        "WebsocketClientHandler.cpp",
        "WebsocketClient.hpp",
        "WebsocketClientHandler.hpp",
    ],
    includes = ["*"],
    deps = [
        "@websocketpp//:websocketpp",
        "//WebSocketHandler:websocket_client_base",
        "//utilities",
    ],
    linkopts = ["-lssl", "-lcrypto"],
    copts = ["-std=c++11"],
    visibility = ["//visibility:public"],
)