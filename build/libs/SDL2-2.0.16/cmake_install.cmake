# Install script for directory: F:/01_sdl/libs/SDL2-2.0.16

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/01_sdl")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Debug/SDL2-staticd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Release/SDL2-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/MinSizeRel/SDL2-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/RelWithDebInfo/SDL2-static.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Debug/SDL2d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Release/SDL2.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/01_sdl/build/libs/SDL2-2.0.16/MinSizeRel/SDL2.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/01_sdl/build/libs/SDL2-2.0.16/RelWithDebInfo/SDL2.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Debug/SDL2d.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Release/SDL2.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/MinSizeRel/SDL2.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/RelWithDebInfo/SDL2.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Debug/SDL2maind.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/Release/SDL2main.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/MinSizeRel/SDL2main.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/01_sdl/build/libs/SDL2-2.0.16/RelWithDebInfo/SDL2main.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake"
         "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "F:/01_sdl/build/libs/SDL2-2.0.16/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "F:/01_sdl/libs/SDL2-2.0.16/SDL2Config.cmake"
    "F:/01_sdl/build/SDL2ConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_assert.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_atomic.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_audio.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_bits.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_blendmode.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_clipboard.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_android.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_iphoneos.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_macosx.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_minimal.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_os2.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_pandora.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_psp.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_windows.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_winrt.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_config_wiz.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_copying.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_cpuinfo.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_egl.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_endian.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_error.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_events.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_filesystem.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_gamecontroller.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_gesture.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_haptic.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_hints.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_joystick.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_keyboard.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_keycode.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_loadso.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_locale.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_log.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_main.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_messagebox.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_metal.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_misc.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_mouse.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_mutex.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_name.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengl.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengl_glext.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles2.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles2_gl2.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles2_gl2ext.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles2_gl2platform.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_opengles2_khrplatform.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_pixels.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_platform.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_power.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_quit.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_rect.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_render.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_revision.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_rwops.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_scancode.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_sensor.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_shape.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_stdinc.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_surface.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_system.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_syswm.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_assert.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_common.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_compare.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_crc32.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_font.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_fuzzer.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_harness.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_images.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_log.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_md5.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_memory.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_test_random.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_thread.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_timer.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_touch.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_types.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_version.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_video.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/SDL_vulkan.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/begin_code.h"
    "F:/01_sdl/libs/SDL2-2.0.16/include/close_code.h"
    "F:/01_sdl/build/libs/SDL2-2.0.16/include/SDL_config.h"
    )
endif()

