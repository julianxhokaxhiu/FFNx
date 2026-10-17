file(WRITE "${SHADER_LOG}" "")

foreach(shader IN LISTS FFNX_SHADERS)
  foreach(varying flat smooth)
    foreach(platform gl vk d3d11 d3d12)
      if(platform STREQUAL "gl")
        set(platform_flags ${BGFX_SHADERC_OPENGL_FLAGS})
      elseif(platform STREQUAL "vk")
        set(platform_flags ${BGFX_SHADERC_VULKAN_FLAGS})
      else()
        set(platform_flags ${BGFX_SHADERC_DIRECT3D_FLAGS})
      endif()

      foreach(stage frag vert)
        if(stage STREQUAL "frag")
          set(shader_type f)
        else()
          set(shader_type v)
        endif()

        set(shader_source "${SHADER_SOURCE_DIR}/${shader}.${stage}")
        set(shader_varying "${SHADER_SOURCE_DIR}/FFNx.varying.${varying}.def.sc")
        set(shader_output "${SHADER_OUTPUT_DIR}/${shader}.${varying}.${platform}.${stage}")
        message(STATUS "Building ${shader}.${varying}.${platform}.${stage} shader...")
        execute_process(
          COMMAND "${SHADERC}"
            -i "${SHADER_INCLUDE_DIR}"
            -f "${shader_source}"
            -o "${shader_output}"
            --type "${shader_type}"
            --varyingdef "${shader_varying}"
            ${platform_flags}
          OUTPUT_VARIABLE shaderc_output
          ERROR_VARIABLE shaderc_output
          RESULT_VARIABLE shaderc_result
        )
        file(APPEND "${SHADER_LOG}" "--- ${shader_output} ---\n${shaderc_output}\n")

        if(NOT "${shaderc_result}" STREQUAL "0")
          message(FATAL_ERROR "shadercRelease failed (${shaderc_result}) for ${shader_output}:\n${shaderc_output}")
        endif()
      endforeach()
    endforeach()
  endforeach()
endforeach()

# Skinned vertex shader variants: the same sources with SMOOTH_SKINNING defined, smooth shading only
foreach(shader IN LISTS FFNX_SKINNED_SHADERS)
  foreach(platform gl vk d3d11 d3d12)
    if(platform STREQUAL "gl")
      set(platform_flags ${BGFX_SHADERC_OPENGL_FLAGS})
    elseif(platform STREQUAL "vk")
      set(platform_flags ${BGFX_SHADERC_VULKAN_FLAGS})
    else()
      set(platform_flags ${BGFX_SHADERC_DIRECT3D_FLAGS})
    endif()

    set(shader_source "${SHADER_SOURCE_DIR}/${shader}.vert")
    set(shader_varying "${SHADER_SOURCE_DIR}/FFNx.varying.smooth.def.sc")
    set(shader_output "${SHADER_OUTPUT_DIR}/${shader}.skinned.smooth.${platform}.vert")
    message(STATUS "Building ${shader}.skinned.smooth.${platform}.vert shader...")
    execute_process(
      COMMAND "${SHADERC}"
        -i "${SHADER_INCLUDE_DIR}"
        -f "${shader_source}"
        -o "${shader_output}"
        --type v
        --varyingdef "${shader_varying}"
        --define SMOOTH_SKINNING
        ${platform_flags}
      OUTPUT_VARIABLE shaderc_output
      ERROR_VARIABLE shaderc_output
      RESULT_VARIABLE shaderc_result
    )
    file(APPEND "${SHADER_LOG}" "--- ${shader_output} ---\n${shaderc_output}\n")

    if(NOT "${shaderc_result}" STREQUAL "0")
      message(FATAL_ERROR "shadercRelease failed (${shaderc_result}) for ${shader_output}:\n${shaderc_output}")
    endif()
  endforeach()
endforeach()
