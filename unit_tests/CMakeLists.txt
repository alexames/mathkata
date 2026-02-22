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

# Find googletest via vcpkg.
find_package(GTest REQUIRED)

include(GoogleTest)

# Generate a rule to build a unit test executable ${test_name} with
# source file ${source}.  For details of additional arguments, see
# mathfu_configure_flags().
function(test_executable test_name source)
  add_executable(${test_name} ${source} ${MATHFU_HEADERS})
  target_include_directories(${test_name} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..)
  target_link_libraries(${test_name} PRIVATE GTest::gtest)
  mathfu_configure_flags(${test_name} ${ARGN})
  mathfu_enable_warnings(${test_name})
  gtest_discover_tests(${test_name})
endfunction()

# Generate a rule to build unit test executables.  This only builds
# ${test_name}_tests if SIMD is disabled (see ${mathfu_enable_simd}) or
# ${test_name}_tests and ${test_name}_no_simd_tests if SIMD is enabled where
# the no_simd_tests binary has SIMD disabled.
function(test_executables test_name source)
  # Default build options for the target architecture.
  test_executable(${test_name}_tests ${source})
  if(mathfu_enable_simd)
    # NOTE: A build configuration below will deliberately duplicate the
    # default build configuration, since these configs could result in
    # different compile time preprocessor code paths.
    # SIMD enabled and padding enabled.
    test_executable(${test_name}_simd_padding_tests ${source} TRUE TRUE)
    # SIMD enabled and padding disabled.
    test_executable(${test_name}_simd_no_padding_tests ${source} TRUE FALSE)
    # SIMD disabled.
    test_executable(${test_name}_no_simd_tests ${source} FALSE)
  endif()
endfunction()

test_executables(vector vector_test/vector_test.cpp)
test_executables(quaternion quaternion_test/quaternion_test.cpp)
test_executables(matrix matrix_test/matrix_test.cpp)
test_executables(utilities utilities_test/utilities_test.cpp)
test_executables(rect rect_test/rect_test.cpp)
test_executables(aabb aabb_test/aabb_test.cpp)
test_executables(ray ray_test/ray_test.cpp)
