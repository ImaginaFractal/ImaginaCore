cmake_minimum_required (VERSION 3.8)

FILE(GLOB SourceFiles "*.cpp")

add_library (ImaginaCore SHARED ${SourceFiles})