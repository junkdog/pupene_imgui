
#set(link_libraries "")

#    main.cpp
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
#    ${CONAN_LIBS}
#    CONAN_PKG::pupene
#    CONAN_PKG::SDL2
#    CONAN_PKG::imgui
#    CONAN_PKG::glm
#    CONAN_PKG::glew)

install(
    TARGETS example
    RUNTIME DESTINATION bin
)
add_custom_target(
    valgrind
    COMMAND valgrind
        --track-origins=yes
        --tool=memcheck
        --leak-check=full
        --num-callers=40 ./bin/example
    DEPENDS example
)