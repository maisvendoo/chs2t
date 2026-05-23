cmake_minimum_required (VERSION 3.29)

project (chs2t)

set(RRS_VERSION 1.9.0)

# No lib prefix in all shared libraries
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_subdirectory(chs2t)
add_subdirectory(chs2t-autopilot)
