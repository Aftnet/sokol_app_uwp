# sokol_app UWP implementation

UWP implementation of sokol_app and sample app (spinning cube from sokol_samples, all in `SampleAppCode.c`) to test/develop it.

Can be built with CMake for UWP and Desktop (need to clean build directory and configure/generate again) by:

- Setting `CMAKE_SYSTEM_NAME` to `WindowsStore` and `CMAKE_SYSTEM_VERSION` to `10`for UWP
- Setting `CMAKE_SYSTEM_NAME` to `Windows` for Win32

Useful to compare behavior between implementation.

UWP implementation requires C++/17 and needs to be built in a different compilation unit from sokol_gfx due to it setting `D3D11_NO_HELPERS`, `CINTERFACE`, `WIN32_LEAN_AND_MEAN` and the UWP implementation needing `winrt::comptr`. Look at `sokol.c` and `sokol.cpp` for how this can be done without one implementation file per sokol header.
