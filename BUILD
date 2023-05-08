cc_binary(
    name = "my_bot",
    srcs = ["main.cpp"],
    deps = [
        "//DiscordBot:brainy_buddy",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    linkopts = ["-lssl", "-lcrypto"],
    visibility = ["//visibility:public"],
)
