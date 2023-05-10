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
        "@nlohmann_json//:nlohmann_json", 
        "@websocketpp//:websocketpp",
        "//utilities",
        "//ThreadPool:thread_pool",
    ],
    linkopts = ["-lssl", "-lcrypto"],
    copts = [
        "-std=c++11",
        "-Iexternal/nlohmann_json/include",
    ],
    visibility = ["//visibility:public"],
)