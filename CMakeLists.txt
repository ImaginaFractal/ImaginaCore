cmake_minimum_required (VERSION 3.8)

project ("ImaginaCore")

option(GENERATE_INTERFACES "Generate interfaces" TRUE)

add_subdirectory ("interface_generator")

add_custom_command(
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management" "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_processing" "${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_processing.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/controller" "${CMAKE_CURRENT_LIST_DIR}/interfaces/controller.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager" "${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager.i"
	COMMAND InterfaceGenerator "${CMAKE_CURRENT_LIST_DIR}/interfaces/evaluator" "${CMAKE_CURRENT_LIST_DIR}/interfaces/evaluator.i"
	DEPENDS InterfaceGenerator
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_processing.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager.i"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/evaluator.i"
	OUTPUT
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_processing"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager"
		"${CMAKE_CURRENT_LIST_DIR}/interfaces/evaluator"
)

add_custom_target(Interfaces DEPENDS
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_management"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/pixel_processing"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/controller"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/location_manager"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces/evaluator"
)

add_library (ImaginaCore SHARED
	"basic_pixel_manager.cpp"
	"computation.cpp"
	"controller.cpp"
	"evaluator.cpp"
	"fractal_context.cpp"
	"imp_lite.cpp"
	"location_manager.cpp"
	"module.cpp"
	"multi-precision.cpp"
	"pixel_management.cpp"
	"pixel_processing.cpp"
	"platform_dependent.cpp"
)

add_library (ImaginaCoreStatic STATIC "pixel_processing.cpp")
set_target_properties (ImaginaCoreStatic PROPERTIES CXX_STANDARD 20)
target_include_directories (ImaginaCoreStatic PRIVATE "include")

target_link_libraries (ImaginaCore INTERFACE ImaginaCoreStatic)

if (GENERATE_INTERFACES)
	add_dependencies (ImaginaCore Interfaces)
	add_dependencies (ImaginaCoreStatic Interfaces)
endif()

target_include_directories (ImaginaCore PRIVATE "include")
target_include_directories (ImaginaCore SYSTEM INTERFACE "include")

set_target_properties (ImaginaCore PROPERTIES PREFIX "")
set_target_properties (ImaginaCore PROPERTIES OUTPUT_NAME "Imagina")
set_target_properties (ImaginaCore PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # It will be in the same directory as the executable

target_compile_definitions (ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties (ImaginaCore PROPERTIES CXX_STANDARD 20)

if (MSVC)
	target_compile_options (ImaginaCore PUBLIC "/Zc:preprocessor")
endif ()