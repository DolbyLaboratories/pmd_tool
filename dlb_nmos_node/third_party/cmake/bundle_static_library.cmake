# MIT License
#
# Copyright (c) 2019 Cristian Adam
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


# This code was originally taken from https://gist.github.com/cristianadam/ef920342939a89fae3e8a85ca9459b49.
# The original script was modified over time in various projects. The version in
# this repo is an attempt to gather all these improvements to create a "Single
# Source of Truth".


# Function: bundle_static_library
#
# Description:
#   This function collects all dependencies (direct and transitive) of provided
#   static library target and bundles them into one library. NOTE: input static
#   library and dependencies must be defined before running this function.
#
# Arguments:
#   TARGET  input static library to bundle
#   BUNDLE  output static library with all dependencies bundled
#   EXCLUDE optional list of dependencies to exclude from bundling
#
# Usage:
#   bundle_static_library(
#       TARGET  my_lib
#       BUNDLE  my_lib_bundled
#   )

include(CMakePrintHelpers)

function(bundle_static_library)
    cmake_parse_arguments(arg
        ""
        "TARGET;BUNDLE"
        "EXCLUDE"
        ${ARGN}
    )

    set(target_name ${arg_TARGET})
    set(bundled_target_name ${arg_BUNDLE})
    set(exclude_libs ${arg_EXCLUDE})

    if (NOT TARGET ${target_name})
        message(FATAL_ERROR "\"${target_name}\" is not a target name")
    endif()

    if ("${bundled_target_name}" STREQUAL "")
        message(FATAL_ERROR "BUNDLE target name not defined")
    endif()

    list(APPEND static_libs ${target_name})

    function(_recursively_collect_dependencies input_target excludes)
        set(_input_link_libraries LINK_LIBRARIES)
        get_target_property(_input_type ${input_target} TYPE)
        if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
            set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
        endif()
        get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
        foreach(dependency IN LISTS public_dependencies)
            if(TARGET "${dependency}" AND NOT "${dependency}" IN_LIST excludes)
                get_target_property(alias ${dependency} ALIASED_TARGET)
                if (TARGET ${alias})
                    set(dependency ${alias})
                endif()
                get_target_property(_type ${dependency} TYPE)
                if (${_type} STREQUAL "STATIC_LIBRARY" OR ${_type} STREQUAL "UNKNOWN_LIBRARY")
                    list(APPEND static_libs ${dependency})
                endif()

                get_property(library_already_added
                    GLOBAL PROPERTY _${target_name}_static_bundle_${dependency})

                if (NOT library_already_added)
                    set_property(GLOBAL PROPERTY _${target_name}_static_bundle_${dependency} ON)
                    _recursively_collect_dependencies(${dependency} "${excludes}")
                endif()
            endif()
        endforeach()
        set(static_libs ${static_libs} PARENT_SCOPE)
    endfunction()

    _recursively_collect_dependencies(${target_name} "${exclude_libs}")

    list(REMOVE_DUPLICATES static_libs)
    foreach(dependency IN LISTS static_libs)
        set_property(GLOBAL PROPERTY _${target_name}_static_bundle_${dependency} OFF)
    endforeach()

    set(bundled_target_full_name
        ${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${bundled_target_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

    add_custom_target(${bundled_target_name}_bundling ALL)

    if (CMAKE_C_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
        set(ar_tool ${CMAKE_AR})
        if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
            set(ar_tool ${CMAKE_C_COMPILER_AR})
        endif()

        if (CMAKE_C_COMPILER_ID MATCHES "^AppleClang$")
            set(ranlib_suppress_warnings "-no_warning_for_no_symbols")
        endif()

        add_custom_command(
            TARGET ${bundled_target_name}_bundling POST_BUILD
            COMMAND rm -f ${bundled_target_full_name}
            VERBATIM
        )

        foreach(tgt IN LISTS static_libs)
            set(extract_dir ${CMAKE_BINARY_DIR}/tmp_${target_name})
            set(target_extract_dir ${extract_dir}/${tgt})
            set(target_extract_dir_duplicates ${target_extract_dir}/tmp)
            set(target_obj_files_list_file ${target_extract_dir}/obj_files.txt)

            # Create temporary directory to extract object files to
            add_custom_command(
                TARGET ${bundled_target_name}_bundling POST_BUILD
                COMMAND mkdir -p ${target_extract_dir}
                COMMAND mkdir -p ${target_extract_dir_duplicates}
                VERBATIM
            )

            # Get list of all object files in archive
            add_custom_command(
                TARGET ${bundled_target_name}_bundling POST_BUILD
                COMMAND ar -t "$<TARGET_FILE:${tgt}>" > ${target_obj_files_list_file}
                VERBATIM
            )

            # For each object file in archive, extract i'th instance
            # and rename appropriately <i>.<obj_file_name>
            add_custom_command(
                TARGET ${bundled_target_name}_bundling POST_BUILD
                COMMAND bash -c "$( \
obj_files=$(cat ../obj_files.txt); \
for obj_file in $obj_files; \
    do counter=$(dir .. | grep -Fc \".$obj_file\"); \
    ar -xN $(expr $counter + 1) $<TARGET_FILE:${tgt}> $obj_file; \
    mv $obj_file ../$counter.$obj_file;\
done)"
                WORKING_DIRECTORY ${target_extract_dir_duplicates}
                VERBATIM
            )
        endforeach()

        # Merge all object files into one object file
        add_custom_command(
            TARGET ${bundled_target_name}_bundling POST_BUILD
            COMMAND ${CMAKE_LINKER} -r -x */*.o -o combined.o
            WORKING_DIRECTORY ${extract_dir}
        )

        # Generate library from single object file
        add_custom_command(
            TARGET ${bundled_target_name}_bundling POST_BUILD
            COMMAND ${ar_tool} -qc ${bundled_target_full_name} ${extract_dir}/combined.o
            COMMAND rm -rf ${extract_dir}
        )

    elseif(MSVC)
        # hint find_program() to look for lib.exe in the linker directory, i.e. link.exe
        get_filename_component(lib_path ${CMAKE_LINKER} DIRECTORY)
        find_program(lib_tool lib PATHS ${lib_path})

        foreach(tgt IN LISTS static_libs)
            list(APPEND static_libs_full_names $<TARGET_FILE:${tgt}>)
        endforeach()

        add_custom_command(
            TARGET ${bundled_target_name}_bundling POST_BUILD
            COMMAND ${lib_tool} /NOLOGO /OUT:${bundled_target_full_name} ${static_libs_full_names}
            COMMENT "Bundling ${bundled_target_name}"
            VERBATIM)
    else()
        message(FATAL_ERROR "Unknown bundle scenario!")
    endif()

    add_library(${bundled_target_name} STATIC IMPORTED GLOBAL)
    set_target_properties(${bundled_target_name}
        PROPERTIES
            IMPORTED_LOCATION ${bundled_target_full_name}
            INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${target_name},INTERFACE_INCLUDE_DIRECTORIES>)
    add_dependencies(${bundled_target_name} ${bundled_target_name}_bundling)

endfunction()
