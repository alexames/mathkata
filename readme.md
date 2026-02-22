MathKata Version 1.1.0    {#mathkata_readme}
====================

MathKata is a C++ math library developed primarily for games focused on
simplicity and efficiency.

It provides a suite of [vector][], [matrix][] and [quaternion][] classes
to perform basic [geometry][] suitable for game developers.  This functionality
can be used to construct geometry for graphics libraries like [OpenGL][] or
perform calculations for animation or physics systems.

The library is written in portable C++ with [SIMD][] compiler intrinsics and
has been tested on the following platforms:

   * [Android][]
   * [Linux][] (x86_64)
   * [OS X][]
   * [Windows][]

Go to our [landing page][] to browse our documentation and see some examples.

   * Discuss MathKata with other developers and users on the
     [MathKata Google Group][].
   * File issues on the [MathKata Issues Tracker][]
   * Post your questions to [stackoverflow.com][] with a mention of
     **mathkata**.

**Important**: MathKata uses submodules to reference other components it depends
upon so download the source using:

    git clone --recursive https://github.com/google/mathkata.git

To contribute to this project see [CONTRIBUTING][].

  [Android]: http://www.android.com
  [Linux]: http://en.m.wikipedia.org/wiki/Linux
  [MathKata Google Group]: http://groups.google.com/group/mathkatalib
  [MathKata Issues Tracker]: http://github.com/google/mathkata/issues
  [OS X]: http://www.apple.com/osx/
  [OpenGL]: http://www.opengl.org/
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [Windows]: http://windows.microsoft.com/
  [geometry]: http://en.wikipedia.org/wiki/Geometry
  [landing page]: http://google.github.io/mathkata
  [matrix]: http://en.wikipedia.org/wiki/Matrix_(mathematics)
  [quaternion]: http://en.wikipedia.org/wiki/Quaternion
  [stackoverflow.com]: http://stackoverflow.com/search?q=mathkata
  [vector]: http://en.wikipedia.org/wiki/Euclidean_vector
  [CONTRIBUTING]: http://github.com/google/mathkata/blob/master/CONTRIBUTING
