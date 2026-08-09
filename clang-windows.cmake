# Clang on Windows toolchain using only LLVM + Windows SDK (no MSVC CRT)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(WINSDK_VER   "10.0.26100.0")
set(WINSDK_ROOT  "C:/Program Files (x86)/Windows Kits/10")

set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")
set(CMAKE_C_COMPILER   "C:/Program Files/LLVM/bin/clang.exe")
set(CMAKE_RC_COMPILER  "C:/Program Files/LLVM/bin/llvm-rc.exe")
set(CMAKE_LINKER       "C:/Program Files/LLVM/bin/lld-link.exe")

# Use UCRT (Windows 10 Universal CRT) instead of MSVC CRT
set(CMAKE_EXE_LINKER_FLAGS_INIT
    "/DEFAULTLIB:ucrt.lib /DEFAULTLIB:libcmt.lib /DEFAULTLIB:libvcruntime.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:oldnames.lib /LIBPATH:\"${WINSDK_ROOT}/Lib/${WINSDK_VER}/ucrt/x64\" /LIBPATH:\"${WINSDK_ROOT}/Lib/${WINSDK_VER}/um/x64\""
)

# Tell Clang to target 64-bit Windows using lld
set(CMAKE_CXX_FLAGS_INIT "-target x86_64-pc-windows-msvc --sysroot=\"C:/Program Files/LLVM\" -fms-compatibility-version=19.36")
set(CMAKE_C_FLAGS_INIT   "-target x86_64-pc-windows-msvc --sysroot=\"C:/Program Files/LLVM\" -fms-compatibility-version=19.36")

# SDK include paths
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    "${WINSDK_ROOT}/Include/${WINSDK_VER}/ucrt"
    "${WINSDK_ROOT}/Include/${WINSDK_VER}/um"
    "${WINSDK_ROOT}/Include/${WINSDK_VER}/shared"
    "C:/Program Files/LLVM/lib/clang/22/include"
)
