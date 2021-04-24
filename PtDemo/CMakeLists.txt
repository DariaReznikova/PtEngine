cmake_minimum_required(VERSION 3.10)
project(PtDemo)

file(GLOB_RECURSE PtDemo_HEADERS "${PROJECT_SOURCE_DIR}/src/**.hpp")
file(GLOB_RECURSE PtDemo_SOURCES "${PROJECT_SOURCE_DIR}/src/**.cpp")

add_executable(PtDemo ${PtDemo_HEADERS} ${PtDemo_SOURCES})

set_target_properties(PtDemo PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

target_include_directories(PtDemo PRIVATE "${CMAKE_SOURCE_DIR}/PtEngine")

target_link_libraries(PtDemo PRIVATE PtEngine)