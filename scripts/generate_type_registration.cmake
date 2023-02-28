function(kengine_generate_type_registration)
    set(options)
    set(oneValueArgs TARGET REGISTRATIONS_JSON NAMESPACE INCLUDE_DIR)
    set(multiValueArgs SOURCES CLANG_ARGS)
    cmake_parse_arguments(ARGUMENTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(arg_name TARGET REGISTRATIONS_JSON NAMESPACE INCLUDE_DIR SOURCES)
        if(NOT ARGUMENTS_${arg_name})
            message(FATAL_ERROR "Missing ${arg_name} argument to kengine_generate_type_registration")
        endif()
    endforeach()

    set(python_script ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/generate_type_registration.py)
    putils_build_clang_arguments(clang_args ${ARGUMENTS_TARGET} ${ARGUMENTS_CLANG_ARGS})

    get_target_property(binary_dir ${ARGUMENTS_TARGET} BINARY_DIR)
    set(include_dir ${binary_dir}/type_registration)
    set(output_dir ${include_dir}/${ARGUMENTS_INCLUDE_DIR})
    file(MAKE_DIRECTORY ${output_dir})

    set(registration_src)
    foreach(header ${ARGUMENTS_SOURCES})
        get_target_property(source_dir ${ARGUMENTS_TARGET} SOURCE_DIR)
        file(RELATIVE_PATH header_relative_path ${source_dir} ${header})

        get_filename_component(header_directory ${header_relative_path} DIRECTORY)
        file(MAKE_DIRECTORY ${output_dir}/${header_directory})

        set(registration_file ${output_dir}/${header_relative_path}_registration.cpp)
        list(APPEND registration_src ${registration_file})

        set(
                command
                python ${python_script} ${header}
                --no-write-main-file
                --registrations ${ARGUMENTS_REGISTRATIONS_JSON}
                --namespace ${ARGUMENTS_NAMESPACE}
                --output ${output_dir}
                --root ${source_dir}
                --clang-args ${clang_args}
        )
        set(command_file "${registration_file}_command_$<CONFIG>.py")
        putils_generate_python_command_file(${command_file} "${command}")
        add_custom_command(
                OUTPUT ${registration_file}
                COMMENT "Generating type registration code for ${header}"
                COMMAND python ${command_file}
                DEPENDS ${header} ${python_script} ${ARGUMENTS_REGISTRATION_JSON}
        )
    endforeach()

    set(registration_target ${ARGUMENTS_TARGET}_type_registration)
    string(TOUPPER ${registration_target}_EXPORT export_macro)

    set(main_file ${output_dir}/add_type_registrator.cpp)
    list(APPEND registration_src ${main_file})

    set(
            command
            python ${python_script} ${ARGUMENTS_SOURCES}
            --no-write-type-files
            --registrations ${ARGUMENTS_REGISTRATIONS_JSON}
            --namespace ${ARGUMENTS_NAMESPACE}
            --output ${output_dir}
            --root ${source_dir}
            --export-macro ${export_macro}
            --clang-args ${clang_args}
    )
    set(command_file "${main_file}_command_$<CONFIG>.py")
    putils_generate_python_command_file(${command_file} "${command}")
    add_custom_command(
            OUTPUT ${main_file}
            COMMENT "Generating type registration code for ${ARGUMENTS_TARGET}"
            COMMAND python ${command_file}
            DEPENDS ${ARGUMENTS_SOURCES} ${python_script}
    )

    add_library(${registration_target} ${registration_src})
    target_include_directories(${registration_target} PUBLIC ${include_dir})
    putils_export_symbols(${registration_target})
endfunction()