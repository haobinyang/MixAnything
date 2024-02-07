if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../modules/" ${CMAKE_MODULE_PATH})
endif()

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED GL Trade Sdl2Application)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

corrade_add_resource(TexturedQuad_RESOURCES resources.conf)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
)

add_executable(MyApplication WIN32
    MyApplication.cpp
    TexturedQuadShader.h
    ${TexturedQuad_RESOURCES})
target_link_libraries(MyApplication PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::Trade
    PkgConfig::FFMPEG)

target_link_libraries(MyApplication PRIVATE cpr::cpr)

# So the TgaImporter gets built implicitly
add_dependencies(MyApplication
    Magnum::AnyImageImporter
    MagnumPlugins::StbImageImporter)

install(TARGETS MyApplication DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyApplication)
