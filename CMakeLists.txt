cmake_minimum_required (VERSION 3.8)

project("ImaginaCore")

FILE(GLOB SourceFiles "*.cpp")

add_library (ImaginaCore SHARED ${SourceFiles})
SET_TARGET_PROPERTIES(ImaginaCore PROPERTIES PREFIX "")
set_target_properties (ImaginaCore PROPERTIES OUTPUT_NAME "Imagina")

target_include_directories(ImaginaCore PRIVATE "include/Imagina")

target_compile_definitions(ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties(ImaginaCore PROPERTIES CXX_STANDARD 20)

if(MSVC)
	target_compile_options(ImaginaCore PUBLIC "/Zc:preprocessor")
else()
	target_compile_options(ImaginaCore PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Werror=array-bounds -Werror=return-type -Werror=implicit-fallthrough -Werror=shift-count-overflow>)
endif()