cmake_minimum_required(VERSION 3.9.6)

project(Ak LANGUAGES CXX C)

#######################################################################
# Include CMakeRC, which is used to store the shaders into C strings. #
#######################################################################

include(CMakeRC.cmake)

#####################################################################################
# Use the SPIRV-Cross Tools to transpile the shaders to run on the target platform. #
#####################################################################################

find_program(GLSLANG_VALIDATOR glslangValidator)

if(NOT GLSLANG_VALIDATOR)
  message(FATAL_ERROR "Need glslangValidator to compile this project.")
endif(NOT GLSLANG_VALIDATOR)

find_program(SPIRV_CROSS spirv-cross)

if(NOT SPIRV_CROSS)
  message(FATAL_ERROR "Need spirv-cross to compile this project.")
endif(NOT SPIRV_CROSS)

function(transpile_shaders target)

  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")

  set(shader_prefix_list ${ARGN})

  set(shader_output_list)

  foreach(shader_prefix ${shader_prefix_list})

    set(vert_path "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${shader_prefix}.vert")
    set(frag_path "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${shader_prefix}.frag")

    set(vert_spirv_path "${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_prefix}_vert.spr")
    set(frag_spirv_path "${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_prefix}_frag.spr")

    set(vert_cross_path "${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_prefix}.vert")
    set(frag_cross_path "${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_prefix}.frag")

    add_custom_command(OUTPUT "${vert_cross_path}" "${frag_cross_path}"
      COMMAND ${GLSLANG_VALIDATOR} -G "${vert_path}" -o "${vert_spirv_path}"
      COMMAND ${GLSLANG_VALIDATOR} -G "${frag_path}" -o "${frag_spirv_path}"
      COMMAND ${SPIRV_CROSS} "${vert_spirv_path}" --output "${vert_cross_path}"
      COMMAND ${SPIRV_CROSS} "${frag_spirv_path}" --output "${frag_cross_path}"
      DEPENDS "${vert_path}" "${frag_path}"
      COMMENT "Transpiling ${shader_prefix}")

    list(APPEND shader_output_list
      "${vert_cross_path}"
      "${frag_cross_path}")

  endforeach(shader_prefix ${shader_prefix_list})

  cmrc_add_resource_library(AkShaders NAMESPACE AkShaders
    ${shader_output_list}
    WHENCE "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    PREFIX "shaders")

endfunction(transpile_shaders output_list_var)

transpile_shaders(shaderlib
  render_texture_quad_pair)

###########################
# Build the main library. #
###########################

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

add_library(Ak
  include/Ak/OpenGLTexture2D.h
  include/Ak/OpenGLTextureQuadPair.h
  include/Ak/OpenGLShaderProgram.h
  include/Ak/GLFW.h
  include/Ak/SingleWindowGLFWApp.h
  src/OpenGLTexture2D.cpp
  src/OpenGLTextureQuadPair.cpp
  src/OpenGLShaderProgram.cpp
  src/GLFW.cpp
  src/SingleWindowGLFWApp.cpp
  glad/src/glad.c)

if(NOT MSVC)
  target_compile_options(Ak PRIVATE -Wall -Wextra -Werror -Wfatal-errors)
endif(NOT MSVC)

target_include_directories(Ak
  PUBLIC
    include
    glad/include)

target_link_libraries(Ak PUBLIC glm::glm AkShaders)

###############################
# Build the example programs. #
###############################

function(add_example_program name)

  set(target AkExample_${name})

  add_executable(${target} ${ARGN})

  target_link_libraries(${target} PRIVATE Ak glfw)

  set_target_properties(${target}
    PROPERTIES
      OUTPUT_NAME ${name})

endfunction(add_example_program name)

add_example_program(cpp_path_tracer examples/cpp_path_tracer.cpp)
