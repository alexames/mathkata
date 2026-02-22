Introduction    {#mathkata_guide_introduction}
============

# About MathKata    {#mathkata_guide_about_mathkata}

[MathKata][] is a C++ math library developed primarily for games focused on
simplicity and efficiency.

It provides a suite of [vector][], [matrix][] and [quaternion][] classes
to perform basic [geometry][] suitable for game developers.  This functionality
can be used to construct geometry for graphics libraries like [OpenGL][] or
perform calculations for animation or physics systems.

# Prerequisites    {#mathkata_guide_prerequisites}

[MathKata][] is written in C++, you are expected to be experienced in C++
programming. [MathKata][] should not be your first C++ programming project! You
should be comfortable with compiling, linking, and debugging.

# About This Guide    {#mathkata_guide_about_guide}

This guide provides an overview of the [MathKata] API, it does *not* cover
every aspect of functionality provided by the library.  The entire API
is documented by the [API reference][].  In addition, the unit tests
(under `mathkata/unit_tests`) provide example usage of each class and function
in the library.

# Concepts    {#mathkata_guide_concepts}

The core functionality of [MathKata][] is provided by the following classes:
   * [Vector](@ref mathkata::Vector)
   * [Matrix](@ref mathkata::Matrix)
   * [Quaternion](@ref mathkata::Quaternion)

Each class is described in the following sections of the guide:
   * [Vector](@ref mathkata_guide_vectors)
      - A geometric concept with a magnitude and direction, defined in any
        dimensional space.
   * [Matrix](@ref mathkata_guide_matrices)
      - A set of data organized in rows and columns.
        [MathKata][] matricies may have any number of rows and columns.
   * [Quaternion](@ref mathkata_guide_quaternions)
      - A specific type of four dimensional vector and defines a rotation in
        three dimensional space.

In addition, [MathKata][] provides a suite of general math functions and method
that make it easier to handle the special requirements of [SIMD][] data types
described in the [Utilities](@ref mathkata_guide_utilities) section.

# Optimization    {#mathkata_guide_optimization}

[MathKata][] is optimized using [SIMD][] instructions (including [NEON][] for
[ARM][] and [SSE][] for [x86][] architectures).  [SIMD][] optimization is
enabled by default based upon the target platform and compiler options used
to build a project.  In addition, [MathKata][] provides compile time options to
modify code generation, see [Build Configurations][] for more details.

## Supporting Additional Architectures

[MathKata][]'s [SIMD][] implementation uses [vectorial][] as an abstraction
layer for common [SIMD][] instructions and data types.  To support additional
architectures, contributors can add support for new [SIMD][] instructions and
data types to the [vectorial][] project and then modify the code in
mathkata/utilities.h to define the macro <code>MATHKATA_COMPILE_WITH_SIMD</code>
for the new architecture.

  [ARM]: http://en.wikipedia.org/wiki/ARM_architecture
  [API reference]: @ref mathkata_api_reference
  [matrix]: http://en.wikipedia.org/wiki/Matrix_(mathematics)
  [geometry]: http://en.wikipedia.org/wiki/Geometry
  [vector]: http://en.wikipedia.org/wiki/Euclidean_vector
  [quaternion]: http://en.wikipedia.org/wiki/Quaternion
  [OpenGL]: http://www.opengl.org/
  [MathKata]: @ref mathkata_overview
  [vectorial]: http://github.com/scoopr/vectorial
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [SSE]: http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
  [x86]: http://en.wikipedia.org/wiki/X86
  [NEON]: http://www.arm.com/products/processors/technologies/neon.php
  [Build Configurations]: @ref mathkata_build_config