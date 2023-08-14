# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

message(${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(
                always_run_target ALL
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main/web/index.h
)
            
add_custom_command(
                OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/main/web/index.h
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main/web/index.html
                COMMAND python scripts/build.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(leida)