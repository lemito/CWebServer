cc_library(
    name = "responsesLib",
    srcs = ["responses.c"],
    hdrs = ["responses.h", "config.h"],
    deps= [":loggerLib"]
)

cc_library(
    name = "handlersLib",
    srcs = ["handlers.c"],
    hdrs = ["handlers.h"],
    visibility = ["//visibility:public"],
    deps = [":responsesLib", ":loggerLib", ":html_helpersLib"], #
)

cc_library(
    name = "loggerLib",
    srcs = ["logger.c"],
    hdrs = ["logger.h"],
)

cc_library(
    name = "socketSetupLib",
    srcs = ["socket_setup.c"],
    hdrs = ["socket_setup.h", "config.h"],
        deps = [":loggerLib"]
)

cc_library(
    name = "file_handlingLib",
    srcs = ["file_handling.c"],
    hdrs = ["file_handling.h"],
    visibility = ["//visibility:public"],
    deps = [":responsesLib"],
)

cc_library(
    name = "utilsLib",
    srcs = ["utils.c"],
    hdrs = ["utils.h", "config.h"],
)

cc_library(
    name = "html_helpersLib",
    srcs = ["html_helpers.c"],
    hdrs = ["html_helpers.h"],
    visibility = ["//visibility:public"],
    deps = [":responsesLib", ":file_handlingLib", ":utilsLib"],
)

cc_binary(
    name = "CWebServer_bin",
    srcs = ["main.cpp", "config.h"],
    deps = [
        "@abseil-cpp//absl/strings",
                "@abseil-cpp//absl/strings:str_format",
        ":responsesLib",
        ":handlersLib",
        ":loggerLib",
        ":socketSetupLib",
        ":file_handlingLib",
        ":utilsLib",
        ":html_helpersLib",
    ],
    data=["index.html", "about.html"]
)