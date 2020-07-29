cmake_minimum_required(VERSION 3.17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(UWPApp)

set(TARGETNAME ${PROJECT_NAME})

add_executable(${TARGETNAME} "")

set_target_properties(${TARGETNAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

set(ASSETS
    Assets/LockScreenLogo.scale-200.png
    Assets/SplashScreen.scale-200.png
    Assets/Square44x44Logo.scale-200.png
    Assets/Square44x44Logo.targetsize-24_altform-unplated.png
    Assets/Square150x150Logo.scale-200.png
    Assets/StoreLogo.png
    Assets/Wide310x150Logo.scale-200.png
)

target_sources(${TARGETNAME} PRIVATE
    App.h
    DeviceResources.h
    Renderer.h
    sokol/hhm.h
    sokol/sokol_app.h
    sokol/sokol_gfx.h
    sokol/sokol_glue.h

    App.cpp
    DeviceResources.cpp
    Renderer.cpp
    sokol/sokol.c

    Package.appxmanifest
    ${ASSETS}
)

set_source_files_properties(${ASSETS} PROPERTIES
    VS_DEPLOYMENT_CONTENT 1
    VS_DEPLOYMENT_LOCATION "Assets"
)