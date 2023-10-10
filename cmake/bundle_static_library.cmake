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

function(bundle_static_library target_name bundled_target_name)
    list(APPEND static_libs ${target_name})

    function(_recursively_collect_dependencies input_target)
        set(_input_link_libraries LINK_LIBRARIES)
        get_target_property(_input_type ${input_target} TYPE)
        if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
            set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
        endif()
        get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
        foreach(dependency IN LISTS public_dependencies)
            if(TARGET ${dependency})
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
                    _recursively_collect_dependencies(${dependency})
                endif()
            endif()
        endforeach()
        set(static_libs ${static_libs} PARENT_SCOPE)
    endfunction()

    _recursively_collect_dependencies(${target_name})

    list(REMOVE_DUPLICATES static_libs)

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
            set(extract_dir ${CMAKE_BINARY_DIR}/tmp)
            set(target_extract_dir ${extract_dir}/${tgt})

            # Create temporary directory to extract object files to
            add_custom_command(
                TARGET ${bundled_target_name}_bundling POST_BUILD
                COMMAND mkdir -p ${target_extract_dir}
                VERBATIM
            )

            # Extract object files
            add_custom_command(
                TARGET ${bundled_target_name}_bundling POST_BUILD
                COMMAND ${ar_tool} -x "$<TARGET_FILE:${tgt}>"
                WORKING_DIRECTORY ${target_extract_dir}
                VERBATIM)
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