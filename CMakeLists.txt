cmake_minimum_required (VERSION 3.8)

project ("ImaginaCore")

add_subdirectory ("interface_generator")

add_interfaces (Interfaces "interface" "imagina/interface" "pixel_management.i" "pixel_processing.i" "controller.i" "location_manager.i" "evaluator.i" "engine.i")

add_library (ImaginaIncludes INTERFACE)
add_dependencies (ImaginaIncludes Interfaces)
target_include_directories (ImaginaIncludes SYSTEM INTERFACE "include" "${CMAKE_CURRENT_BINARY_DIR}/include")

add_library (ImaginaCoreStatic STATIC "pixel_processing.cpp")
set_target_properties (ImaginaCoreStatic PROPERTIES CXX_STANDARD 20)
target_link_libraries (ImaginaCoreStatic ImaginaIncludes)

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
	"engine.cpp"
)

target_link_libraries (ImaginaCore INTERFACE ImaginaIncludes ImaginaCoreStatic)

add_dependencies (ImaginaCore Interfaces)
target_include_directories (ImaginaCore PRIVATE "include" "${CMAKE_CURRENT_BINARY_DIR}/include")

set_target_properties (ImaginaCore PROPERTIES PREFIX "" OUTPUT_NAME "Imagina")
set_target_properties (ImaginaCore PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # It will be in the same directory as the executable

target_compile_definitions (ImaginaCore PRIVATE IM_BUILD_CORE)

set_target_properties (ImaginaCore PROPERTIES CXX_STANDARD 20)

if (MSVC)
	target_compile_options (ImaginaCore PUBLIC "/Zc:preprocessor")
endif ()