cmake_minimum_required (VERSION 3.10.0)

project (game_spop)

# Add cmake directory to the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Add project files
set (HEADERS
    cpu.h
    emulator.h
    opcodes.h
    prefixed_opcodes.h
    graphics.h
    gb_interrupts.h
    audio.h
    instr_lists.h
    mbc.h
)

set (SOURCES
    cpu.c
    emulator.c
    main.c
    opcodes.c
    prefixed_opcodes.c
    graphics.c
    gb_interrupts.c
    audio.c
    instr_lists.c
    mbc.c
)

find_package (SDL2 REQUIRED)

# ============= SDL2 ============
if (NOT SDL2_FOUND)
    message (FATAL_ERROR "Error: SDL2 not found.")
else ()
    message (STATUS "SDL2 include directory : ${SDL2_INCLUDE_DIRS}")
    message (STATUS "SDL2 library directory : ${SDL2_LIBRARIES}")
    include_directories(${SDL2_INCLUDE_DIRS})
    link_directories(${SDL2_LIBRARIES})
endif (NOT SDL2_FOUND)

set (CMAKE_C_STANDARD 17)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (game_spop ${SOURCES} ${HEADERS})
target_link_libraries (game_spop ${SDL2_LIBRARIES} m)



