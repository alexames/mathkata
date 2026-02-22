Building for OS X    {#mathkata_guide_building_osx}
=================

# Version Requirements    {#mathkata_guide_building_osx_version}

Following are the minimum required versions of tools and libraries you
need to build [MathKata][] for [OS X][]:

   * [OS X][]: Mavericks 10.9.1.
   * [Xcode][]: 5.0.1
   * [CMake][]: 2.8.12.1

# Prerequisites    {#mathkata_guide_building_osx_prerequisites}

   * Install [Xcode][].
   * Install [CMake][].

# Building with Xcode    {#mathkata_guide_building_osx_building}

Firstly, the [Xcode][] project needs to be generated using [CMake][]:

   * Open a command line window.
   * Go to the [MathKata][] project directory.
   * Use [CMake][] to generate the [Xcode][] project.

~~~{.sh}
    cd mathkata
    cmake -G Xcode .
~~~

Then the project can be opened in [Xcode][] and built:

   * Double-click on `mathkata/MathKata.xcodeproj` to open the project in
     [Xcode][].
   * Select "Product-->Build" from the menu.

# Running Applications    {#mathkata_guide_building_osx_running}

## Running in Xcode    {#mathkata_guide_building_osx_run_xcode}

Open [Xcode][] select a build target and run it:

   * Double-click on `mathkata/MathKata.xcodeproj` to open the project in
     [Xcode][].
   * Select an application `Scheme`, for example
     "matrix_tests-->My Mac 64-bit", from the combo box to the right of the
     "Run" button.
   * Click the "Run" button.

## Running from the Command Line {#mathkata_guide_building_osx_cmdline}

To build:

   * Open a command line window.
   * Go to the [MathKata][] project directory.
   * Generate the [Xcode][] project.
   * Run xcodebuild.

For example:

~~~{.sh}
    cd mathkata
    cmake -G Xcode .
    xcodebuild
~~~

To run all benchmarks:

   * Open a command line window.
   * Go to the [MathKata][] project directory.
   * Run each benchmark binary in the `benchmarks/` directory.

For example:

~~~{.sh}
    cd mathkata
    for binary in ./benchmarks/Debug/*_benchmarks; do ${binary}; done
~~~

To run all unit tests:

   * Open a command line window.
   * Go to the [MathKata][] project directory.
   * Run each unit test binary in the `unit_tests/` directory.

For example:

~~~{.sh}
    cd mathkata
    for binary in ./unit_tests/Debug/*_tests; do ${binary}; done
~~~

<br>

  [CMake]: http://www.cmake.org
  [MathKata]: @ref mathkata_overview
  [OS X]: http://www.apple.com/osx/
  [Xcode]: http://developer.apple.com/xcode/
