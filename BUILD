cc_binary(
    name = "my_bot",
    srcs = ["main.cpp"],
    deps = [
        "@websocketpp//:websocketpp",
        "//WebSocketHandler/impl:websocket_handler_lib",
    ],
    linkopts = ["-lssl", "-lcrypto"],
    copts = ["-std=c++11"],
    visibility = ["//visibility:public"],
)
