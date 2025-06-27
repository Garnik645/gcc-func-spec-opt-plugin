cmake_minimum_required(VERSION 3.10)
project(definition CXX)

execute_process(
        COMMAND gcc -print-file-name=plugin
        OUTPUT_VARIABLE GCC_PLUGIN_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(PLUGIN_NAME func-spec-opt)
set(SOURCE_FILE ${PLUGIN_NAME}.cpp)

add_library(${PLUGIN_NAME} SHARED ${SOURCE_FILE})

set_target_properties(${PLUGIN_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${PLUGIN_NAME}
)

target_include_directories(${PLUGIN_NAME} PRIVATE ${GCC_PLUGIN_DIR}/include)

target_compile_options(${PLUGIN_NAME} PRIVATE -fno-rtti)

