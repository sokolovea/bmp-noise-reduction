cmake_minimum_required(VERSION 3.16)

project(bmp_test LANGUAGES C)

add_executable(bmp_test main.c
    bitmap.h bitmap_util.h)

include(GNUInstallDirs)
install(TARGETS bmp_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
