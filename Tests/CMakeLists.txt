project(Tests)

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE INCS *.h)
add_executable(Tests ${SOURCES} ${INCS})
source_group(headers FILES ${INCS})

add_dependencies(Tests SimpleRenderer)

include_directories(${SimpleRenderer_SOURCE_DIR}/SimpleRenderer/include)
include_directories(${SimpleRenderer_SOURCE_DIR}/SimpleRenderer/src)

target_include_directories(Tests SYSTEM PRIVATE "../vendor/sdl/include")
target_include_directories(Tests SYSTEM	PRIVATE "../vendor/googletest/googletest/include")

set_target_properties(Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SimpleRenderer_SOURCE_DIR}/bin/Tests)

target_link_libraries(Tests
	PRIVATE glm
	PRIVATE gtest_main
	PRIVATE SDL3::SDL3
	PRIVATE SimpleRenderer_Interface
	)

if (WIN32)
    add_custom_command(
        TARGET Tests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:Tests>"
	VERBATIM
    )
endif()