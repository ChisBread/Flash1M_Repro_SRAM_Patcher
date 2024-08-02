cmake_minimum_required(VERSION 3.19)

if(DEFINED ENV{MINGW})
    # 设置交叉编译的目标操作为windows
    SET(CMAKE_SYSTEM_NAME Windows)

    # 指定c/c++编译器
    SET(CMAKE_C_COMPILER  x86_64-w64-mingw32-gcc)
    SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    SET(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

    # 指定编译目标操作系统版本时的搜索根路径
    SET(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32 )

    # adjust the default behaviour of the FIND_XXX() commands:
    # search headers and libraries in the target environment, search 
    # programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

project(Flash1M_Repro_SRAM_Patcher)
add_executable(${PROJECT_NAME} "main.cpp"
    "patchset.cpp"
    "flash512.cpp"
    "flash1m.cpp"
    "eeprom.cpp"
    "flash.cpp")

add_executable(SaveIdentifier SaveIdentifier.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET SaveIdentifier PROPERTY CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")