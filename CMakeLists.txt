cmake_minimum_required(VERSION 3.24)
project(42_ft_ping C)

set(CMAKE_C_STANDARD 11)

include_directories(inc)

add_executable(42_ft_ping
        src/ft_ping.c
        src/proc_v4.c
        src/send_v4.c
        src/send_v6.c
        src/proc_v6.c
        src/utils.c
        src/readloop.c
        inc/ft_ping.h
        )

