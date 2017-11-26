add_executable(example
    example.cpp
    imgui_impl_sdl_gl3.cpp
    imgui_impl_sdl_gl3.h
    EditorPupper.cpp
    EditorPupper.h
    EditorConfig.h
    puppers.h
    wrapper.h
    traits.h)

target_link_libraries(example PUBLIC ${CONAN_TARGETS})

install(
    TARGETS example
    RUNTIME DESTINATION bin)

install(
    FILES
        ${CMAKE_SOURCE_DIR}/SourceCodePro-Semibold.ttf
        ${CMAKE_SOURCE_DIR}/SourceCodePro-license.txt
    DESTINATION bin)

add_custom_target(
    valgrind
    COMMAND valgrind
        --track-origins=yes
        --tool=memcheck
        --leak-check=full
        --num-callers=40 ./bin/example
    DEPENDS example)