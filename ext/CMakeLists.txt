
# set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
# set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
# add_subdirectory(eigen EXCLUDE_FROM_ALL)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(xtl)
add_subdirectory(xtensor)

# only include corrade if we're testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
    set(WITH_INTERCONNECT OFF CACHE BOOL "" FORCE)
    set(WITH_PLUGINMANAGER OFF CACHE BOOL "" FORCE)
    set(WITH_UTILITY OFF CACHE BOOL "" FORCE)
    set(WITH_RC OFF CACHE BOOL "" FORCE)
    add_subdirectory(corrade EXCLUDE_FROM_ALL)
endif()

