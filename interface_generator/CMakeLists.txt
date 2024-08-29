cmake_minimum_required (VERSION 3.8)

project ("InterfaceGenerator")
add_executable (InterfaceGenerator "main.cpp")

set_target_properties (InterfaceGenerator PROPERTIES CXX_STANDARD 17)


function(add_interfaces TargetName SourceDir GeneratedDir)
	message("TargetName: ${TargetName}")
	set(AbsoluteSourceDir "${CMAKE_CURRENT_LIST_DIR}/${SourceDir}")
	set(AbsoluteGeneratedDir "${CMAKE_CURRENT_BINARY_DIR}/include/${GeneratedDir}")
	foreach(FileName IN LISTS ARGN)
		string(REGEX REPLACE "\\.[^.]*$" ".h" DstFileName ${FileName}) # remove file extension

		add_custom_command(
			COMMAND InterfaceGenerator "${AbsoluteGeneratedDir}/${DstFileName}" "${AbsoluteSourceDir}/${FileName}"
			DEPENDS InterfaceGenerator "${AbsoluteSourceDir}/${FileName}"
			OUTPUT "${AbsoluteGeneratedDir}/${DstFileName}"
		)
    endforeach()

	list(TRANSFORM ARGN REPLACE "\\.[^.]*$" ".h")
	list(TRANSFORM ARGN PREPEND "${AbsoluteGeneratedDir}/")

	message(${ARGN})
	add_custom_target("${TargetName}" DEPENDS ${ARGN})
endfunction()
