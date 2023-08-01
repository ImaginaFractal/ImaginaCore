cmake_minimum_required (VERSION 3.8)

project ("ImaginaCore")

add_subdirectory ("interface_generator")

add_library (ImaginaCommon INTERFACE)
if (NOT MSVC)
	target_compile_options(ImaginaCommon INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Werror=array-bounds -Werror=return-type -Werror=implicit-fallthrough -Werror=shift-count-overflow>)
endif ()

add_custom_command(
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management" "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/controller" "${CMAKE_CURRENT_LIST_DIR}/interfaces/controller.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager" "${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager.i"
	DEPENDS InterfaceGenerator
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager.i"
	OUTPUT
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager"
)

add_custom_target(Interfaces DEPENDS
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager"
)

FILE (GLOB SourceFiles "*.cpp")
add_library (ImaginaCore SHARED ${SourceFiles} "interfaces/pixel_management")
target_link_libraries (ImaginaCore PRIVATE ImaginaCommon)

target_include_directories (ImaginaCore PRIVATE "include/Imagina")
target_include_directories (ImaginaCore SYSTEM PUBLIC "include")

set_target_properties (ImaginaCore PROPERTIES PREFIX "")
set_target_properties (ImaginaCore PROPERTIES OUTPUT_NAME "Imagina")

target_compile_definitions (ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties (ImaginaCore PROPERTIES CXX_STANDARD 20)

if (MSVC)
	target_compile_options (ImaginaCore PUBLIC "/Zc:preprocessor")
endif ()