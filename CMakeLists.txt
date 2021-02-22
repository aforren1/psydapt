cmake_minimum_required(VERSION 3.1)
project(psydapt)

set(PSYDAPT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(STRINGS "${PSYDAPT_INCLUDE_DIR}/config.hpp" psydapt_version_defines
     REGEX "#define PSYDAPT_VERSION_(MAJOR|MINOR|PATCH)")

foreach(ver ${psydapt_version_defines})
  if(ver MATCHES "#define PSYDAPT_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
    set(PSYDAPT_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
  endif()
endforeach()
set(${PROJECT_NAME}_VERSION
    ${PSYDAPT_VERSION_MAJOR}.${PSYDAPT_VERSION_MINOR}.${PSYDAPT_VERSION_PATCH})
message(STATUS "Building psydapt v${${PROJECT_NAME}_VERSION}")

set(CMAKE_CXX_STANDARD 17) # std::optional (and many other things)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
add_subdirectory(submodules)

option(PSYDAPT_BUILD_TESTS "Build unit tests" OFF)
option(PSYDAPT_BUILD_SCRATCH "Build scratch files" OFF)
option(PSYDAPT_DISABLE_EXCEPTIONS "Disable use of C++ exceptions" OFF)

add_library(psydapt INTERFACE)
target_link_libraries(psydapt INTERFACE xtensor)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mavx2 -ffast-math -funroll-loops")
target_include_directories(psydapt
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${xtensor_INCLUDE_DIRS}
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

# just temporary test files
set(PSYDAPT_SCRATCH_DIR "${PROJECT_SOURCE_DIR}/scratch/")
if (PSYDAPT_BUILD_SCRATCH)
    add_executable(Test ${PSYDAPT_SCRATCH_DIR}/test.cpp)
    target_link_libraries(Test PUBLIC psydapt)

    add_executable(TestQP ${PSYDAPT_SCRATCH_DIR}/test_quest.cpp)
    target_link_libraries(TestQP PUBLIC psydapt)

    add_executable(XTensScratch ${PSYDAPT_SCRATCH_DIR}/xtensor_scratch.cpp)
    target_link_libraries(XTensScratch PUBLIC psydapt)

    add_executable(TestCSF ${PSYDAPT_SCRATCH_DIR}/test_csf.cpp)
    target_link_libraries(TestCSF PUBLIC psydapt)
    
    add_executable(XTensScratch ${PSYDAPT_SCRATCH_DIR}/xtensor_scratch.cpp)
    target_link_libraries(XTensScratch PUBLIC psydapt)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND PSYDAPT_BUILD_TESTS AND NOT EMSCRIPTEN)
    enable_testing()
    add_subdirectory(tests)
endif()
