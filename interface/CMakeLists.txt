cmake_minimum_required (VERSION 3.8)

project ("InterfaceGenerator")
add_executable (InterfaceGenerator "main.cpp")

set_target_properties (InterfaceGenerator PROPERTIES CXX_STANDARD 17)