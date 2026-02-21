# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required(VERSION 3.14)

project(MathFu LANGUAGES CXX)

# Options that control the build configuration.
# To configure MathFu flags per build target, see the
# mathfu_configure_flags() function.
option(mathfu_enable_simd "Use SIMD implementations when available." ON)
option(mathfu_build_benchmarks "Build MathFu benchmarks." ON)
option(mathfu_build_tests "Build MathFu unit tests." ON)

# Save the mathfu directory, store this in the cache so that it's globally
# accessible from mathfu_configure_flags().
set(mathfu_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "mathfu directory")

# Set build options for ${target} that are required to build with MathFu.
# This takes the optional arguments "enable_simd force_padding".
#
# If enable_simd is specified it configures whether the project should be
# built with SIMD optimized functions.  If force_padding is specified and
# SIMD is enabled, some data structures are padded to work more efficiently
# with SIMD instructions.
#
# If enable_simd is not specified, the mathfu_enable_simd option is used.
# If force_padding isn't specified padding is enabled based upon the
# best general configuration for the target architecture.
function(mathfu_configure_flags target)
  set(enable_simd ${mathfu_enable_simd})

  # Add required includes to the target.
  target_include_directories(${target}
    PRIVATE ${mathfu_dir}/include)

  # Parse optional arguments.
  set(additional_args ${ARGN})
  list(LENGTH additional_args num_additional_args)
  if(${num_additional_args} GREATER 0)
    list(GET additional_args 0 enable_simd)
  endif()
  if(${num_additional_args} GREATER 1)
    list(GET additional_args 1 force_padding)
  endif()

  # If the SIMD build option is enabled.
  if(enable_simd)
    # Enable SSE4.1 when building with GCC / Clang on x86/x64.
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|AMD64|i[3-6]86)")
        target_compile_options(${target} PRIVATE -msse4.1)
      endif()
    endif()
    # Enable SSE2 by default when building with MSVC for 32-bit targets.
    # Note that SSE2 is enabled by default for 64-bit targets, and the
    # compile option will generate an "unknown option" warning.
    if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)
      target_compile_options(${target} PRIVATE /arch:SSE2)
    endif()
    # Conditionally enable padding.
    if(DEFINED force_padding)
      if(force_padding)
        target_compile_definitions(${target} PRIVATE
          MATHFU_COMPILE_FORCE_PADDING=1)
      else()
        target_compile_definitions(${target} PRIVATE
          MATHFU_COMPILE_FORCE_PADDING=0)
      endif()
    endif()
  else()
    target_compile_definitions(${target} PRIVATE
      MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT)
  endif()
endfunction()

# Enable a high warning level for the target.
function(mathfu_enable_warnings target)
  if(MSVC)
    # C4127: conditional expression is constant
    # C4577: 'noexcept' used with no exception handling mode specified.
    target_compile_options(${target} PRIVATE /W4 /WX /wd4127 /wd4577)
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
      CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror
      -Wno-long-long -Wno-variadic-macros)
  endif()
endfunction()

file(GLOB_RECURSE MATHFU_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/mathfu/*.h)

if(mathfu_build_benchmarks)
  add_subdirectory(benchmarks)
endif()
if(mathfu_build_tests)
  enable_testing()
  add_subdirectory(unit_tests)
endif()
