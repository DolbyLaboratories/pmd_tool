
macro(add_ember_connector target_name)
    target_sources(${target_name}
        PRIVATE
            ember_connector.cpp
            ember_controller.cpp
            parameter_manager.cpp
            utils/utils.cpp
    )

    target_include_directories(${target_name}
        PRIVATE
            .
            utils
    )
endmacro()

add_ember_connector(dlb_pmd_studio)

if(BUILD_PMD_STUDIO_RIVERMAX)
    add_ember_connector(dlb_pmd_studio_rivermax)
endif()