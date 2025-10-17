cmake_minimum_required(VERSION 3.16)

project(1023 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(1023 main.cpp
    fieldbutton.h fieldbutton.cpp
    gamefield.h gamefield.cpp
    common.h
    toppanel.h toppanel.cpp
    README.md
    datacontainer.h datacontainer.cpp
    bottompanel.h bottompanel.cpp

    .gitignore)
target_link_libraries(1023 PRIVATE fltk fltk_forms fltk_images fltk_gl)

include(GNUInstallDirs)
install(TARGETS 1023
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
