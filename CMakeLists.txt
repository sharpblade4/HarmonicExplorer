cmake_minimum_required(VERSION 3.22)

project(HarmonicExplorerCm)

set(CMAKE_CXX_STANDARD 23)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/cpm.cmake)
# cmake -S . -B build

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 7.0.7
    VERSION 7.0.7
    SOURCE_DIR ${LIB_DIR}/juce
)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(plugin)
# cmake --build build

# in VSCode you need to save.
# then build, and link to a custom directory in ableton live:
# /Volumes/workplace/personal/juce_projects/harmonics_explorer/build/plugin/HarmonicExplorerPlugin_artefacts/VST3
# to reload plugin ableton needs to be restarted. 