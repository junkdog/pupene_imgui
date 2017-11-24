
set(link_libraries "")

hunter_add_package(pupene)
find_package(pupene CONFIG REQUIRED)
list(APPEND link_libraries pupene::pupene)

hunter_add_package(imgui)
find_package(imgui REQUIRED)
list(APPEND link_libraries imgui::imgui)

hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)
list(APPEND link_libraries SDL2::SDL2)

hunter_add_package(glew)
find_package(glew REQUIRED)
list(APPEND link_libraries glew::glew)

hunter_add_package(glm)
find_package(glm REQUIRED)
list(APPEND link_libraries glm)

#    main.cpp
add_executable(example
    example.cpp
    pupdefs.h
    imgui_impl_sdl_gl3.cpp
    imgui_impl_sdl_gl3.h
    EditorPupper.cpp
    EditorPupper.h
    EditorConfig.h
    puppers.h
    wrapper.h
    traits.h)
target_link_libraries(example PUBLIC ${link_libraries})

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
    DEPENDS ./bin/example
)