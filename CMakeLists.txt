cmake_minimum_required (VERSION 3.8)

project ("ImaginaCore")

add_subdirectory ("interface")

add_library (ImaginaCommon INTERFACE)
if (NOT MSVC)
	target_compile_options(ImaginaCommon INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Werror=array-bounds -Werror=return-type -Werror=implicit-fallthrough -Werror=shift-count-overflow>)
endif ()

add_custom_command(
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/include/Imagina/interfaces/pixel_management" "${CMAKE_CURRENT_LIST_DIR}/pixel_management.i"
	DEPENDS InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/pixel_management.i"
	OUTPUT "${CMAKE_CURRENT_LIST_DIR}/include/Imagina/interfaces/pixel_management"
)

add_custom_target(Interfaces DEPENDS "${CMAKE_CURRENT_LIST_DIR}/include/Imagina/interfaces/pixel_management")

FILE (GLOB SourceFiles "*.cpp")
add_library (ImaginaCore SHARED ${SourceFiles} "include/Imagina/interfaces/pixel_management")
target_link_libraries (ImaginaCore PRIVATE ImaginaCommon)

target_include_directories (ImaginaCore PRIVATE "include/Imagina")
target_include_directories (ImaginaCore SYSTEM INTERFACE "include")

set_target_properties (ImaginaCore PROPERTIES PREFIX "")
set_target_properties (ImaginaCore PROPERTIES OUTPUT_NAME "Imagina")

target_compile_definitions (ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties (ImaginaCore PROPERTIES CXX_STANDARD 20)

if (MSVC)
	target_compile_options (ImaginaCore PUBLIC "/Zc:preprocessor")
endif ()