cmake_minimum_required (VERSION 3.8)
project (rt)

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set (CMAKE_CXX_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SRC_LIST src/main.cpp src/geometry.cpp src/model.cpp src/Bitmap.cpp src/render.cpp)

add_executable(rt ${SRC_LIST})


target_link_libraries(rt ${ALL_LIBS} )

