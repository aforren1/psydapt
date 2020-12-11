cmake_minimum_required(VERSION 3.1)
project(psydapt)
set (CMAKE_CXX_STANDARD 17) # std::optional
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
add_subdirectory(ext)

option(BUILD_TESTS "Build unit tests" OFF)

add_library(psydapt INTERFACE)
target_include_directories(psydapt
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${xtensor_INCLUDE_DIRS}
)

add_executable(Test test.cpp)
target_link_libraries(Test PRIVATE psydapt xtensor)

add_executable(XTensScratch xtensor_scratch.cpp)
target_link_libraries(XTensScratch PUBLIC xtensor psydapt)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
