file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE INCS ../include/*.h)
add_executable(SimpleRenderer ${SOURCES} ${INCS})

add_library(SimpleRenderer_Interface INTERFACE)
# Link 
target_link_libraries(SimpleRenderer_Interface INTERFACE $<FILTER:$<TARGET_OBJECTS:SimpleRenderer>,EXCLUDE,/main.obj>)
set_target_properties(SimpleRenderer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SimpleRenderer_SOURCE_DIR}/bin)

source_group(headers FILES ${INCS})

include_directories(${SimpleRenderer_SOURCE_DIR}/SimpleRenderer/include)
target_include_directories(SimpleRenderer SYSTEM PRIVATE "../vendor/sdl/include")
target_link_libraries(SimpleRenderer
	PRIVATE SDL3::SDL3
	PRIVATE glm)

if (WIN32)
    add_custom_command(
        TARGET SimpleRenderer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:SimpleRenderer>"
		VERBATIM
    )
endif()

add_custom_command(TARGET SimpleRenderer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/models/ $<TARGET_FILE_DIR:SimpleRenderer>/models/
)