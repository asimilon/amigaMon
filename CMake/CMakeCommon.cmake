get_property(
    BSM_COMMON_INITIALIZED GLOBAL ""
    PROPERTY BSM_COMMON_INITIALIZED
    SET
)
if(BSM_COMMON_INITIALIZED)
    return()
endif()

set_property(GLOBAL PROPERTY BSM_COMMON_INITIALIZED true)

include(CMake/CPM.cmake)
include(CMakePrintHelpers)

macro (createProject projectName)

    if(PROJECT_IS_TOP_LEVEL)
        message("Create Project ${projectName}")

        set(PROJECT_NAME "${projectName}" CACHE INTERNAL "")

        # By default we don't want schemes to be made for modules, etc
        set(CMAKE_XCODE_GENERATE_SCHEME OFF)

        # No ZERO_CHECK target (it helps bust cache for cmake)
        set(CMAKE_SUPPRESS_REGENERATION true)

        #Minimum MacOS target, set globally
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)

        option(UniversalBinary "Build universal binary for mac" ON)

        if (UniversalBinary)
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
        endif()

        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        # Adds all the module sources so they appear correctly in the IDE
        # Must be set before JUCE is added as a sub-dir (or any targets are made)
        # https://github.com/juce-framework/JUCE/commit/6b1b4cf7f6b1008db44411f2c8887d71a3348889
        set_property(GLOBAL PROPERTY USE_FOLDERS YES)

        option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

        # This is a failed attempt to bury ALL_BUILD in Targets/
        # This should be called before any target is made
        # Bug in Xcode? https://gitlab.kitware.com/cmake/cmake/-/issues/21383
        set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "Targets")

        # add_subdirectory(JUCE)
        CPMAddPackage(
                NAME JUCE
                GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
                GIT_TAG origin/master)

        set(ProjectDependsOn)
        set(BinaryDataTargets)

    else()
        # message("${projectName} is not top level")
    endif()
endmacro()

function(addBinaryData fileGlob targetName)
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    message("Adding binary data : ${targetName}")

    file(GLOB_RECURSE BinaryFiles CONFIGURE_DEPENDS ${fileGlob})

    juce_add_binary_data(${targetName}
            HEADER_NAME ${targetName}.h
            NAMESPACE ${targetName}
            SOURCES ${BinaryFiles}
    )

    list(APPEND BinaryDataTargets ${targetName})
endfunction()

function(addGuiApp appName AppSourceFiles Modules SourceFolders iconPath juceOptions)
    juce_add_gui_app(
        "${appName}"
        ICON_BIG "${iconPath}"
        ICON_SMALL "${iconPath}"
    )

    target_compile_definitions("${appName}"
        PUBLIC
        ${juceOptions}
    )

    target_compile_features("${appName}" PRIVATE cxx_std_17)

    target_sources("${appName}" PRIVATE "${AppSourceFiles}")

    target_link_libraries("${appName}" PRIVATE ${Modules} ${ProjectDependsOn} ${BinaryDataTargets})

    list(TRANSFORM SourceFolders PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
    target_include_directories("${appName}" PRIVATE ${SourceFolders})

    set_target_properties("${appName}" PROPERTIES XCODE_GENERATE_SCHEME ON)

    if(MSVC)
        target_compile_options(${appName}
                PUBLIC
                "/wd4101"
                "/wd4018"
                "/wd4127"
        )
    else ()
        target_compile_options(${appName}
                PUBLIC
                -Wno-error=unused-variable
                -Wno-error=sign-compare
                -Wno-error=maybe-uninitialized
        )
    endif()

endfunction()

function (cleanTargetLayout)
    list(APPEND CMAKE_MESSAGE_INDENT "   ")

    message("Clean Target Layout ${PROJECT_NAME}")

    # This cleans up the folder organization, especially on Xcode.
    # It tucks the Plugin varieties into a "Targets" folder and generate an Xcode Scheme manually
    # Xcode scheme generation is turned off globally to limit noise from other targets
    # The non-hacky way of doing this is via the global PREDEFINED_TARGETS_FOLDER propety
    # However that doesn't seem to be working in Xcode
    # Not all plugin types (au, vst) available on each build type (win, macos, linux)
    foreach(target ${BinaryDataTargets} "${PROJECT_NAME}_AU" "${PROJECT_NAME}_AUv3" "${PROJECT_NAME}_VST" "${PROJECT_NAME}_VST3" "${PROJECT_NAME}_All" "${PROJECT_NAME}_Standalone")
        if(TARGET ${target})
            set_target_properties(${target} PROPERTIES
                # Tuck the actual targets in a folder where they won't bother us
                FOLDER "Targets"
                # But let us actually build the target in Xcode
                XCODE_GENERATE_SCHEME ON
                XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
                )
        endif()
    endforeach()
endfunction()