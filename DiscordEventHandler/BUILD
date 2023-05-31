cc_library(
    name = "discord_event_handler",
    srcs = [
        "DiscordEventHandler.cpp",
        "DiscordEventHandler.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
        ":discord_event",
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "discord_event",
    srcs = [
        "DiscordEvents.cpp",
        "DiscordEvents.hpp",
    ],
    deps = [
        "@nlohmann_json//:nlohmann_json", 
    ],
    copts = [
        "-Iexternal/nlohmann_json/include",
    ],
    visibility = ["//visibility:public"],
)