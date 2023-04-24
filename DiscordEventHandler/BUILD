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
        "-std=c++11",
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
        "-std=c++11",
        "-Iexternal/nlohmann_json/include",
    ],
    visibility = ["//visibility:public"],
)