Vectors    {#mathkata_guide_vectors}
=======

Vectors consist of a set of elements (usually floating point or integer
scalars) that are regularly used to describe a point in space or a direction.
For more information see this description of
[Euclidiean Vectors](http://en.wikipedia.org/wiki/Euclidean_vector).

The [MathKata][] [Vector][] class is a template declared in
[mathkata/vector.h](@ref mathkata/vector.h) which has been specialized and
optimized for regularly used cases.  Implementing [Vector][] as a template
reduces code duplication, provides compile time optimization opportunities
through specialization and allows users to use the class with any scalar
type.

# Declaration  {#mathkata_guide_vectors_declaration}

[Vector][] template takes two arguments: the type and number of elements in
the [Vector][].

For example, a 2-dimensional floating point vector variable is declared using
the following:

~~~{.cpp}
    mathkata::Vector<float, 2> vector;
~~~

To eliminate the need for explicit template instantiation, [GLSL][] style
`typedef`s are provided in
[mathkata/glsl_mappings.h](@ref mathkata/glsl_mappings.h).  Using a [GLSL][]
style `typedef` a 2-dimensional floating point vector variable is declared
using the following:

~~~{.cpp}
    math::vec2 vector;
~~~

## Initialization  {#mathkata_guide_vectors_initialization}

[Vector][] has no default constructor: `mathkata::vec2 v;` does not compile, so
a value cannot be left unassigned by accident, and neither can a member of an
enclosing type that forgets to initialize it. Where the elements really are
about to be overwritten, `auto v = mathkata::vec2::uninitialized();` says so
explicitly and writes nothing. Constructors are provided for common vector sizes
that allow initialization on construction:

~~~{.cpp}
    mathkata::vec2 vector(1.0f, 2.0f);
~~~

It's also possible to initialize a [Vector][] with another instance:

~~~{.cpp}
    mathkata::vec2 vector1(1.0f, 2.0f);
    mathkata::vec2 vector2(vector1);
~~~

This can also be achieved with:

~~~{.cpp}
    mathkata::vec2 vector1(1.0f, 2.0f);
    mathkata::vec2 vector2 = vector1;
~~~

# Accessors    {#mathkata_guide_vectors_accessors}

[Vector][] provides array and [GLSL][] style accessors.  For example, to
read two elements from a 2-dimensional vector using array accessors:

~~~{.cpp}
    const mathkata::vec2 vector(1.0f, 2.0f);
    float x = vector[0];
    float y = vector[1];
~~~

It's also possible to read elements from 2, 3 and 4-dimensional vectors using
[GLSL][] style accessors:

~~~{.cpp}
    const mathkata::vec4 vector(1.0f, 2.0f, 3.0f, 4.0f);
    float x = vector.x();
    float y = vector.y();
    float z = vector.z();
    float w = vector.w();
~~~

Similar to [GLSL][], [Vector][] provides accessors which allow a subset of
elements to be accessed:

~~~{.cpp}
    const mathkata::vec3 vector1(1.0f, 2.0f, 3.0f);
    mathkata::vec3 xy = vector1.xy();
~~~

## Assignment    {#mathkata_guide_vectors_assignment}

Individual elements returned by [Vector][]'s array accessors are references
that can be assigned values to update the contents of the class:

~~~{.cpp}
    mathkata::vec2 vector(1.0f, 2.0f);
    vector[0] = 3.0f;
    vector[1] = 4.0f;
~~~

# Arithmetic    {#mathkata_guide_vectors_arithmetic}

[Vector][] supports in-place and out-of-place arithmetic operators
(addition, subtraction, multiplication, division) that perform
component-wise operations.

For example, two vectors can be added together using the following:

~~~{.cpp}
    const mathkata::vec2 vector1(1.0f, 2.0f), vector2(3.0f, 4.0f);
    mathkata::vec2 vector3 = vector1 + vector2;
~~~

The above results in the values `(4.0f, 6.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

The same can be achieved with an in-place addition which mutates `vector1`:

~~~{.cpp}
    mathkata::vec2 vector1(1.0f, 2.0f);
    const mathkata::vec2 vector2(3.0f, 4.0f);
    vector1 += vector2;
~~~

Subtraction is similar to addition:

~~~{.cpp}
    const mathkata::vec2 vector1(4.0f, 6.0f), vector2(3.0f, 4.0f);
    mathkata::vec2 vector3 = vector2 - vector1;
~~~

Multiplication is performed component-wise, which means that each component
is multiplied with the same index component in the other [Vector][] involved
in the operation:

~~~{.cpp}
    const mathkata::vec2 vector1(2.0f, 0.5f), vector2(3.0f, 10.0f);
    vector3 = vector1 * vector2;
~~~

The above results in the values `(6.0f, 5.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

Similar to the other operators, multiplication can be performed in place:

~~~{.cpp}
    mathkata::vec2 vector1(2.0f, 0.5f);
    const mathkata::vec2 vector2(3.0f, 10.0f);
    vector1 *= vector2;
~~~

Division is also a component-wise operation:

~~~{.cpp}
    const mathkata::vec2 vector1(4.0f, 4.0f), vector2(2.0f, 4.0f);
    vector3 = vector1 / vector2;
~~~

The above results in the values `(2.0f, 1.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

# Constants    {#mathkata_guide_vectors_constants}

Commonly used constants are provided by
[mathkata/constants.h](@ref mathkata/constants.h).  These values eliminate the need
to construct [Vector][] objects for common values like cardinal axes.

For example, the following initializes a 2-dimensional vector with the X-axis:

~~~{.cpp}
    const mathkata::vec2 vector = mathkata::kAxisX2f;
~~~

# Geometric Operations    {#mathkata_guide_vectors_geometric}

[Vector][] also provides functions for commonly used geometric operations
that are utilized by graphics and games developers.

For example, the length of a [Vector][] is calculated using
[Length()](@ref mathkata::Vector::Length):

~~~{.cpp}
    const mathkata::vec2 vector(3.0f, 4.0f);
    float length = vector.Length();
~~~

The projection of one [Vector][] onto another (dot product) can be calculated
using [DotProduct()](@ref mathkata::Vector::DotProduct).  For example, the
following calculates the projection of a vector onto the X-axis:

~~~{.cpp}
  float projection = mathkata::vec2::DotProduct(mathkata::vec2(5.0f, 2.0f),
                                              mathkata::kAxisX2f);
~~~

It's possible to normalize (scale to a length of 1) a vector in-place using
[Normalize()](@ref mathkata::Vector::Normalize) or out-of-place using
[Normalized()](@ref mathkata::Vector::Normalized).  For example, the following
normalizes the vector in-place:

~~~{.cpp}
    mathkata::vec2 vector(3.0f, 4.0f);
    vector.Normalize();
~~~

The cross product of two 3-dimensional [Vectors][] (the vector perpendicular
to two vectors) can be calculated using
[CrossProduct()](@ref mathkata::Vector::CrossProduct), for example:

~~~{.cpp}
    mathkata::vec3 zaxis = mathkata::vec3::CrossProduct(mathkata::kAxisX3f,
                                                    mathkata::kAxisY3f);
~~~

Alternatively, to create three points and compute the normal of the plane
defined by the points use:

~~~{.cpp}
    mathkata::vec3 point1(0.5f, 0.4f, 0.1f);
    mathkata::vec3 point2(0.4f, 0.9f, 0.1f);
    mathkata::vec3 point3(0.1f, 0.8f, 0.6f);

    mathkata::vec3 vector1 = point2 - point1;
    mathkata::vec3 vector2 = point3 - point1;

    mathkata::vec3 normal = Vector<float, 3>::CrossProduct(vector2, vector1);
~~~

# Other Operations    {#mathkata_guide_vectors_other}

In addition, to basic arithmetic and geometric operations, [Vector][] also
implements functions to perform the following:

   * [Lerp](@ref mathkata::Vector::Lerp) to linearly interpolate between two
     vectors.

# Packing    {#mathkata_guide_vectors_packing}

The size of the class can change based upon the [Build Configuration][] so it
should *not* be treated like a C style array.  To serialize the class to a
flat array see [VectorPacked](@ref mathkata::VectorPacked).

For example, to pack (store) an unpacked to packed vector:

~~~{.cpp}
    mathkata::vec3 vector(3.0f, 2.0f, 1.0f);
    mathkata::vec3_packed packed = vector;
~~~

Since [VectorPacked][] is plain-old-data (POD) it can be cast to an array
of elements of the same type used by the [Vector][] so it's possible to use
an array of [VectorPacked][] data structures to contiguous arrays of data like
vertex buffers.

Similarly, [VectorPacked][] can be used to deserialize (load) data into
[Vector][]:

~~~{.cpp}
    VectorPacked<float, 3> packed = { 3, 2, 1 };
    Vector<float, 3> vector(packed);
~~~

<br>

  [Build Configuration]: @ref mathkata_build_config
  [MathKata]: @ref mathkata_overview
  [GLSL]: http://www.opengl.org/documentation/glsl/
  [Vector]: @ref mathkata::Vector
  [VectorPacked]: @ref mathkata::VectorPacked
