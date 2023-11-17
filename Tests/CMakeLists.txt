project(Tests)

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE INCS *.h)
add_executable(Tests ${SOURCES} ${INCS})
#set_property(TARGET Tests PROPERTY
#             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
source_group(headers FILES ${INCS}) 

include_directories(${SimpleRenderer_SOURCE_DIR}/SimpleRenderer/include)
include_directories(${SimpleRenderer_SOURCE_DIR}/SimpleRenderer/src)

set(SDL_INCLUDES "../vendor/sdl/include")
target_include_directories(Tests SYSTEM PRIVATE ${SDL_INCLUDES})

set(GOOGLETEST_INCLUDES "../vendor/googletest/googletest/include")
target_include_directories(Tests SYSTEM PRIVATE ${GOOGLETEST_INCLUDES})

set_target_properties(Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SimpleRenderer_SOURCE_DIR}/bin/Tests)

target_link_libraries(Tests
	PRIVATE glm
	gtest
	PRIVATE SDL3::SDL3
	)
	
if (WIN32)
    add_custom_command(
        TARGET Tests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:Tests>"
	VERBATIM
    )
endif()