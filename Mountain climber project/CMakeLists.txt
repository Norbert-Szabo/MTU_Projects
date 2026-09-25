cmake_minimum_required(VERSION 4.1)
project(project C)

set(CMAKE_C_STANDARD 23)

add_executable(project
        gradient.c
        gradient_sol.c)
