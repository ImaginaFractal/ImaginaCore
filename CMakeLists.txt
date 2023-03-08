cmake_minimum_required (VERSION 3.8)

FILE(GLOB SourceFiles "*.cpp")

add_library (ImaginaCore SHARED ${SourceFiles})

target_include_directories(ImaginaCore PRIVATE "include")

target_compile_definitions(ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties(ImaginaCore PROPERTIES CXX_STANDARD 20)