cmake_minimum_required(VERSION 3.1)
project(psydapt)
set (CMAKE_CXX_STANDARD 17) # std::optional

add_library(psydapt INTERFACE)
target_include_directories(psydapt
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(Test test.cpp)
target_link_libraries(Test PRIVATE psydapt)
