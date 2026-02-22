# Copyright 2014 Google Inc. All rights reserved.
# Copyright 2026 Alexander Ames. All rights reserved.
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

project(MathKata LANGUAGES CXX)

# Options that control the build configuration.
# To configure MathKata flags per build target, see the
# mathkata_configure_flags() function.
option(mathkata_enable_simd "Use SIMD implementations when available." ON)
option(mathkata_build_benchmarks "Build MathKata benchmarks." ON)
option(mathkata_build_tests "Build MathKata unit tests." ON)

# Save the mathkata directory, store this in the cache so that it's globally
# accessible from mathkata_configure_flags().
set(mathkata_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "mathkata directory")

# Set build options for ${target} that are required to build with MathKata.
# This takes the optional arguments "enable_simd force_padding".
#
# If enable_simd is specified it configures whether the project should be
# built with SIMD optimized functions.  If force_padding is specified and
# SIMD is enabled, some data structures are padded to work more efficiently
# with SIMD instructions.
#
# If enable_simd is not specified, the mathkata_enable_simd option is used.
# If force_padding isn't specified padding is enabled based upon the
# best general configuration for the target architecture.
function(mathkata_configure_flags target)
  set(enable_simd ${mathkata_enable_simd})

  # Require C++20 for concepts and std::numbers.
  target_compile_features(${target} PRIVATE cxx_std_20)

  # Add required includes to the target.
  target_include_directories(${target}
    PRIVATE ${mathkata_dir}/include)

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
    # Add xsimd headers (SIMD abstraction layer).
    find_package(xsimd REQUIRED)
    target_link_libraries(${target} PRIVATE xsimd)
    # Enable SIMD flags when building with GCC / Clang.
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|AMD64|i[3-6]86)")
        target_compile_options(${target} PRIVATE -msse4.1)
      elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm|ARM)")
        # Enable NEON for 32-bit ARM. AArch64 has NEON by default.
        target_compile_options(${target} PRIVATE -mfpu=neon)
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
          MATHKATA_COMPILE_FORCE_PADDING=1)
      else()
        target_compile_definitions(${target} PRIVATE
          MATHKATA_COMPILE_FORCE_PADDING=0)
      endif()
    endif()
  else()
    target_compile_definitions(${target} PRIVATE
      MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT)
  endif()
endfunction()

# Enable a high warning level for the target.
function(mathkata_enable_warnings target)
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

file(GLOB_RECURSE MATHKATA_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/mathkata/*.h)

if(mathkata_build_benchmarks)
  add_subdirectory(benchmarks)
endif()
if(mathkata_build_tests)
  enable_testing()
  add_subdirectory(unit_tests)
endif()
