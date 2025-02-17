#pragma once

#define FMT_REC          "{ %f, %f, %f, %f }"
#define FMT_POINT        "{ %f, %f }"

#define FMT_ARG_REC(...)    (__VA_ARGS__).x, (__VA_ARGS__).y, (__VA_ARGS__).width, (__VA_ARGS__).height
#define FMT_ARG_POINT(...)  (__VA_ARGS__).x, (__VA_ARGS__).y