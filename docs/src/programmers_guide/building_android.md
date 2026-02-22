Building for Android    {#mathkata_guide_building_android}
====================

# Version Requirements    {#mathkata_guide_building_android_version}

Following are the minimum tested versions of the tools and libraries you
need to build [MathKata][] for Android:

   * [Android SDK][]:  Android 2.3.3 (API Level 10) or above.
   * [Android Developer Tools][] ([ADT][]): 20140702
      - NDK plugn for Eclipse (bundled with [ADT][]) if using Eclipse to build.
   * [Android NDK][]: android-ndk-r10e

# Prerequisites    {#mathkata_guide_building_android_prerequisites}

Prior to building:

   * Install the [Android SDK][].
   * Install the [Android NDK][].

# Building    {#mathkata_guide_building_android_building}

Each [MathKata][] project for Android has an associated `AndroidManifest.xml`
file and `jni` subdirectory.  Unit tests and benchmarks directories contain
projects that each build an Android package ([apk][]) which can be installed
and executed on Android devices.

The following directories in the [MathKata] project contain [Android NDK][]
projects:

   * `mathkata/`
      - Rules used to build the [MathKata][] library.
   * `mathkata/benchmarks`
      - Each subdirectory under this directory contains a simple benchmark
        used to measure the performance of different build configurations.
   * `mathkata/unit_tests`
      - Each subdirectory under this directory contains a unit test application
        used to test different components of the library in different build
        configurations.

## Building with ndk-build  {#mathkata_guide_building_android_building_ndk_build}

To build a single [Android NDK][] project (without packaging the native
component in an [apk][]):

   * Open a command line window.
   * Go to the directory containing the project to build.
   * `ndk-build`

For example, to build the matrix test for the default build configuration
without generating an [apk][]:

~~~{.sh}
    cd mathkata/unit_tests/matrix_test/default
    ndk-build
~~~

# Installing and Running Applications {#mathkata_guide_building_android_running}

## Using Eclipse {#mathkata_guide_building_android_eclipse}

Running a sample requires the [Android Developer Tools][] ([ADT][]) plugin and
the [NDK Eclipse plugin][].

   * Build a project using `ndk-build`
     ([see above](@ref mathkata_guide_building_android_building_ndk_build)).
   * Open [ADT][] Eclipse.
   * Select "File->Import..." from the menu.
   * Select "Android > Existing Android Code Into Workspace", and click "Next".
   * Click the "Browse..." button next to `Root Directory:` and select the
     project folder (e.g. `mathkata/unit_tests/matrix_test/default`).
   * Click "Finish". Eclipse imports the project, and displays it in the
     Package Explorer pane.
   * Right-click the project, and select "Run->Run As->Android Application"
      from the menu.
   * If you do not have a physical device, you must define a virtual one.
     For details about how to define a virtual device, see [managing avds][].
     We don’t recommend a virtual device for development.
   * None of the applications have a visual component so their output is
     visible via the log ([adb][] logcat).

# Code Generation    {#mathkata_guide_building_android_code_generation}

By default, code is generated for devices that support the `armeabi-v7a`
ABI.  Alternatively, you can generate a fat `.apk` that includes code for all
ABIs.  To do so, override APP\_ABI on ndk-build's command line.

Using `ndk-build`:

~~~{.sh}
    ndk-build APP_ABI=all
~~~

<br>

  [MathKata]: @ref mathkata_overview
  [adb]: http://developer.android.com/tools/help/adb.html
  [ADT]: http://developer.android.com/tools/sdk/eclipse-adt.html
  [Android Developer Tools]: http://developer.android.com/sdk/index.html
  [Android NDK]: http://developer.android.com/tools/sdk/ndk/index.html
  [Android SDK]: http://developer.android.com/sdk/index.html
  [NDK Eclipse plugin]: http://developer.android.com/sdk/index.html
  [apk]: http://en.wikipedia.org/wiki/Android_application_package
  [managing avds]: http://developer.android.com/tools/devices/managing-avds.html
