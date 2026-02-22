Utilities    {#mathkata_guide_utilities}
=========

# Memory Allocation    {#mathkata_guide_utilities_allocation}

[MathKata][]'s use of [SIMD][] instructions provides significant performance
gains in many use cases.  However, some architectures have memory
alignment requirements for data used by [SIMD][] operations (e.g [SIMD][]
data structures must be 16-byte aligned on x86).  Some STL implementations do
not respect data type alignment which makes it more difficult to respect the
[SIMD][] memory alignment requirements.

In order to adhere to [SIMD][] alignment requirements, [MathKata][] provides a
dynamic memory allocator [AllocateAligned()][] which ensures data is correctly
aligned.  Memory allocated by [AllocateAligned()][] **must** be deallocated
using [FreeAligned()][], for example:

~~~{.cpp}
   void *memory = mathkata::AllocateAligned(32);
   mathkata::FreeAligned(memory);
~~~

The [simd_allocator](@ref mathkata::simd_allocator) class is provided to perform
aligned memory allocation with STL classes like std::vector.  For example, the
following constructs a vector of `vec4` which uses the aligned memory
allocator:

~~~{.cpp}
    std::vector<vec4, mathkata::simd_allocator<mathkata::vec4>> myvector;
~~~

The aligned memory allocator uses [MATHKATA_ALIGNMENT](@ref MATHKATA_ALIGNMENT)
bytes of additional memory per allocation.  If the additional memory usage
per allocation is acceptable for an application, the most simple solution
is to override the global `new` and `delete` operators by adding
[MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE](@ref MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE)
to the source file containing an application's entry point (e.g `main.cpp`):

~~~{.cpp}
#include "mathkata/utilities.h"

MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE

int main(int argc, char *argv[]) {
  // The application.
  return 0;
}
~~~

# Miscellaneous Functions    {#mathkata_guide_utilities_misc}

[Clamp](@ref mathkata_Clamp) is used to clamp a value within a specified
range.  For example:

~~~{.cpp}
    float x = 1.0f;
    z = mathkata::Clamp(x, 1.0f, 2.0f);  // z = 1.0
    z = mathkata::Clamp(x, 1.5f, 2.0f);  // z = 1.5
    z = mathkata::Clamp(x, 0.0f, 0.5f);  // z = 0.5
~~~

[Lerp](@ref mathkata_Lerp) linearly interpolates between two values of an
arbitrary type in a specified range.  For example, an object whose position is
specified by a [Vector][] could be moved over a range as time elapses:

~~~{.cpp}
    const mathkata::vec2 start(2.0f, 3.0f);
    const mathkata::vec2 end(10.0f, 0.0f);
    // Update the position of an object every 100ms.
    for (float time = 0.0f; time < 1.1f; time += 0.1f) {
      mathkata::vec2 position = mathkata::Lerp(start, end, time);
      // Draw object.
      // Wait 100ms.
    }
~~~

<br>

  [AllocateAligned()]: @ref mathkata_AllocateAligned
  [FreeAligned()]: @ref mathkata_FreeAligned
  [MathKata]: @ref mathkata_overview
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [Vector]: @ref mathkata::Vector
