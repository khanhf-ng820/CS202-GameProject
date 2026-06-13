cmake_minimum_required (VERSION 3.16)

project("Project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)

#Automatically pick up files added to src
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(Project ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(Project PRIVATE
	${CMAKE_SOURCE_DIR}/include
)

add_subdirectory("raylib")

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)