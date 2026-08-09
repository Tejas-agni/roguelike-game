cmake_minimum_required(VERSION 3.25)
project(RoguelikeSurvival LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------
# Windows SDK paths for Clang-on-Windows (no separate MSVC needed)
# ---------------------------------------------------------------
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WINSDK_VER   "10.0.26100.0")
    set(WINSDK_ROOT  "C:/Program Files (x86)/Windows Kits/10")
    set(UCRT_INCLUDE "${WINSDK_ROOT}/Include/${WINSDK_VER}/ucrt")
    set(UM_INCLUDE   "${WINSDK_ROOT}/Include/${WINSDK_VER}/um")
    set(SHD_INCLUDE  "${WINSDK_ROOT}/Include/${WINSDK_VER}/shared")
    set(UCRT_LIB     "${WINSDK_ROOT}/Lib/${WINSDK_VER}/ucrt/x64")
    set(UM_LIB       "${WINSDK_ROOT}/Lib/${WINSDK_VER}/um/x64")

    add_compile_options(
        "-imsvc${UCRT_INCLUDE}"
        "-imsvc${UM_INCLUDE}"
        "-imsvc${SHD_INCLUDE}"
    )
    link_directories("${UCRT_LIB}" "${UM_LIB}")
endif()

# ---------------------------------------------------------------
# Compiler flags
# ---------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
elseif(MSVC)
    add_compile_options(/W4 /permissive- /utf-8 /MP /Zc:preprocessor /Zc:__cplusplus)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# ---------------------------------------------------------------
# Fetch Raylib 5.0
# ---------------------------------------------------------------
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(USE_AUDIO      ON  CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------
# Executable
# ---------------------------------------------------------------
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Show console window in Debug builds so we can read logs
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
endif()
