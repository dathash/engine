# Install script for directory: /Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/lib/libassimp.5.0.0.dylib"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/lib/libassimp.5.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.5.0.0.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.5.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND "/usr/bin/install_name_tool"
        -id "/usr/local/lib/libassimp.5.dylib"
        "${file}")
      execute_process(COMMAND /usr/bin/install_name_tool
        -delete_rpath "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/lib"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/lib/libassimp.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "/usr/local/lib/libassimp.5.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/anim.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/aabb.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/ai_assert.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/camera.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/color4.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/color4.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/config.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/defs.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Defines.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/cfileio.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/light.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/material.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/material.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/matrix3x3.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/matrix3x3.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/matrix4x4.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/matrix4x4.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/mesh.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/pbrmaterial.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/postprocess.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/quaternion.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/quaternion.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/scene.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/metadata.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/texture.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/types.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/vector2.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/vector2.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/vector3.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/vector3.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/version.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/cimport.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/importerdesc.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Importer.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/DefaultLogger.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/ProgressHandler.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/IOStream.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/IOSystem.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Logger.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/LogStream.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/NullLogger.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/cexport.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Exporter.hpp"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/DefaultIOStream.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/DefaultIOSystem.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/ZipArchiveIOSystem.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SceneCombiner.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/fast_atof.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/qnan.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/BaseImporter.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Hash.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/MemoryIOWrapper.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/ParsingUtils.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/StreamReader.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/StreamWriter.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/StringComparison.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/StringUtils.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SGSpatialSort.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/GenericProperty.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SpatialSort.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SkeletonMeshBuilder.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SmoothingGroups.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/SmoothingGroups.inl"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/StandardShapes.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/RemoveComments.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Subdivision.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Vertex.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/LineSplitter.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/TinyFormatter.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Profiler.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/LogAux.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Bitmap.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/XMLTools.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/IOStreamBuffer.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/CreateAnimMesh.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/irrXMLWrapper.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/BlobIOSystem.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/MathFunctions.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Macros.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Exceptional.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/ByteSwapper.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Compiler/poppack1.h"
    "/Users/alexhartford/Home/code/engine/ext/assimp-5.0.0/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

