cmake_minimum_required(VERSION 3.10)
project(PtEngine)

file(GLOB_RECURSE PtEngine_HEADERS "${PROJECT_SOURCE_DIR}/pt/**.hpp")
file(GLOB_RECURSE PtEngine_SOURCES "${PROJECT_SOURCE_DIR}/pt/**.cpp")

add_library(PtEngine STATIC ${PtEngine_HEADERS} ${PtEngine_SOURCES})

set_target_properties(PtEngine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}"
)