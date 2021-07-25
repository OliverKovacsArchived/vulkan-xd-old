cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(glslc)

project(vulkan-xd VERSION 0.1
                  DESCRIPTION "Higher dimensional Vulkan rendering"
                  LANGUAGES CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

# windows
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-pc-windows-gnu -stdlib=libstdc++")

# linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")

# pull git submodules
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/extern)
    execute_process(COMMAND mkdir ${PROJECT_SOURCE_DIR}/extern)
endif()
execute_process(COMMAND git submodule update
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# build GLFW
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/extern/build)
    execute_process(COMMAND mkdir ${PROJECT_SOURCE_DIR}/extern/build)
endif()
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/extern/build/glfw)
    execute_process(COMMAND mkdir ${PROJECT_SOURCE_DIR}/extern/build/glfw)
    # execute_process(COMMAND cmake -S ../../glfw -B . -G "MinGW Makefiles"
    execute_process(COMMAND cmake -S ../../glfw -B . -G "Unix Makefiles"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/build/glfw
    )
    message(STATUS test)
    execute_process(COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/build/glfw
    )
endif()

# function for compiling shaders
function(add_shader TARGET SHADER)
    find_program(GLSLC glslc)

    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER})
    set(current-output-path ${CMAKE_BINARY_DIR}/shaders/${SHADER}.spv)

    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
           OUTPUT ${current-output-path}
           COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
           DEPENDS ${current-shader-path}
           IMPLICIT_DEPENDS CXX ${current-shader-path}
           VERBATIM)

    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)

# create main
add_executable(main src/main.cpp)
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/src)

# compile shaders
add_shader(main shader.frag)
add_shader(main shader.vert)

# copy textures to build dir
add_custom_command(TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/textures $<TARGET_FILE_DIR:main>/textures
)

# link to Vulkan

# windows
# target_include_directories(main PUBLIC $ENV{VULKAN_SDK}/Include)
# target_link_libraries(main PUBLIC $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)

# linux
target_include_directories(main PUBLIC $ENV{VULKAN_SDK}/include)
target_link_libraries(main PUBLIC vulkan)
target_link_libraries(main PUBLIC pthread)
target_link_libraries(main PUBLIC X11)
target_link_libraries(main PUBLIC Xxf86vm)
target_link_libraries(main PUBLIC Xrandr)
target_link_libraries(main PUBLIC Xi)

target_link_libraries(main PUBLIC m)

# link to GLFW
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/extern/glfw/include)
target_link_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/extern/glfw-build/src)
target_link_libraries(main PUBLIC ${PROJECT_SOURCE_DIR}/extern/build/glfw/src/libglfw3.a)

# include GLM
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/extern/glm)

# include STB
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/extern/stb)

# link to OpenGL

# windows
# target_link_libraries(main PUBLIC C:/Windows/system32/opengl32.dll)
# target_link_libraries(main PUBLIC C:/Windows/system32/gdi32.dll)

# linux
find_package(OpenGL REQUIRED)
target_link_libraries(main PUBLIC ${OPENGL_LIBRARIES})
find_library(DL_LIBRARY libdl.so)
target_link_libraries(main PUBLIC ${DL_LIBRARY})
