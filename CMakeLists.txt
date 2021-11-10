cmake_minimum_required(VERSION 3.9.6)

project(Ak LANGUAGES CXX C)

#######################################################################
# Include CMakeRC, which is used to store the shaders into C strings. #
#######################################################################

include(CMakeRC.cmake)

####################################################
# Include BVH library, for ray tracing algorithms. #
####################################################

include(FetchContent)

FetchContent_Declare(bvh URL "https://github.com/madmann91/bvh/archive/master.zip")

FetchContent_MakeAvailable(bvh)

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
  blur_effect
  render_texture_quad_pair
  render_points
  render_lidar
  render_lidar_normal_estimation
  render_lidar_points_to_spheres
  hrt_render_mesh)

###########################
# Build the main library. #
###########################

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenMP)

add_library(Ak
  include/Ak/ObjMeshModel.h
  include/Ak/OpenGLBlurEffect.h
  include/Ak/OpenGLFramebuffer.h
  include/Ak/OpenGLHRTMeshRenderProgram.h
  include/Ak/OpenGLLidarRenderProgram.h
  include/Ak/OpenGLPointRenderProgram.h
  include/Ak/OpenGLRenderbuffer.h
  include/Ak/OpenGLScreenSpaceEffect.h
  include/Ak/OpenGLShaderProgram.h
  include/Ak/OpenGLTexture2D.h
  include/Ak/OpenGLTextureQuadPair.h
  include/Ak/GLFW.h
  include/Ak/SingleWindowGLFWApp.h
  src/ObjMeshModel.cpp
  src/OpenGLBlurEffect.cpp
  src/OpenGLFramebuffer.cpp
  src/OpenGLHRTMeshRenderProgram.cpp
  src/OpenGLLidarRenderProgram.cpp
  src/OpenGLPointRenderProgram.cpp
  src/OpenGLRenderbuffer.cpp
  src/OpenGLScreenSpaceEffect.cpp
  src/OpenGLShaderProgram.cpp
  src/OpenGLTexture2D.cpp
  src/OpenGLTextureQuadPair.cpp
  src/GLFW.cpp
  src/SingleWindowGLFWApp.cpp
  src/stb/stb_image.h
  src/stb/stb_image.c
  src/stb/stb_image_write.h
  src/stb/stb_image_write.c
  src/tiny_obj_loader/tiny_obj_loader.h
  src/tiny_obj_loader/tiny_obj_loader.cpp
  glad/src/glad.c)

if(TARGET OpenMP::OpenMP_CXX)
  target_link_libraries(Ak PUBLIC OpenMP::OpenMP_CXX)
endif(TARGET OpenMP::OpenMP_CXX)

if(NOT MSVC)
  target_compile_options(Ak PRIVATE -Wall -Wextra -Werror -Wfatal-errors)
endif(NOT MSVC)

target_include_directories(Ak
  PUBLIC
    include
    glad/include
  PRIVATE
    src/stb
    src/tiny_obj_loader)

target_link_libraries(Ak PUBLIC glm::glm AkShaders bvh)

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

add_example_program(render_points examples/render_points.cpp)

add_example_program(blur_effect examples/blur_effect.cpp)

add_example_program(render_to_texture examples/render_to_texture.cpp)

add_example_program(render_lidar examples/render_lidar.cpp)
