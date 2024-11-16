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
                GIT_REPOSITORY git@github.com:Bom-Shanka-Machines/JUCE.git
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
endfunction()

function(createJuceModule moduleName)
    juce_add_module("${moduleName}")
    target_link_libraries("${moduleName}" INTERFACE ${ProjectDependsOn})
endfunction()

function(setupPlugin sourceFolders sourceFiles dependencies juceOptions)

    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    message("Setup Plugin ${PROJECT_NAME}")

    list(TRANSFORM sourceFolders PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

    target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_17)

    target_sources("${PROJECT_NAME}" PRIVATE ${sourceFiles})

    # No, we don't want our source buried in extra nested folders
    set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

    # The source tree should still look like the source tree
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${sourceFiles})

    target_include_directories("${PROJECT_NAME}" PRIVATE "${sourceFolders}")

    target_compile_definitions("${PROJECT_NAME}"
        PUBLIC
        ${juceOptions}
    )

    target_link_libraries("${PROJECT_NAME}"
        PRIVATE
        ${dependencies}
        ${ProjectDependsOn}
        ${BinaryDataTargets}
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
    )
endfunction()

function(addAbletonLink)
    CPMAddPackage(
            NAME link
            GIT_TAG origin/master
            GITHUB_REPOSITORY Ableton/link
            DOWNLOAD_ONLY YES
    )

    if(link_ADDED)
        include(${link_SOURCE_DIR}/AbletonLinkConfig.cmake)
        list(APPEND ProjectDependsOn Ableton::Link)
        set(ProjectDependsOn "${ProjectDependsOn}" PARENT_SCOPE)
    endif()
endfunction()


macro (addMelatoninInspector)
    Include (FetchContent)
    FetchContent_Declare (melatonin_inspector
            GIT_REPOSITORY https://github.com/sudara/melatonin_inspector.git
            GIT_TAG origin/main
            SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector)
    FetchContent_MakeAvailable (melatonin_inspector)
    list(APPEND ProjectDependsOn melatonin_inspector)
endmacro()

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

function(createMidiPlugin productName pluginCode pluginCompanyCode companyName bundleId)
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    message("Create MIDI Plugin ${PROJECT_NAME}")

    juce_add_plugin(
        "${PROJECT_NAME}"
        COMPANY_NAME ${companyName}
        BUNDLE_ID com.${bundleId}
        IS_SYNTH TRUE
        IS_MIDI_EFFECT TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        VST3_CATEGORIES Instrument
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE ${pluginCompanyCode}
        PLUGIN_CODE "${pluginCode}"
        FORMATS AU VST3
        PRODUCT_NAME "${productName}"
        HARDENED_RUNTIME_ENABLED
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endfunction()

function(createInstrumentPlugin productName pluginCode pluginCompanyCode companyName bundleId)
    list(APPEND CMAKE_MESSAGE_INDENT "   ")

    message("Create Instrument Plugin ${PROJECT_NAME}")

    # Check the readme at `docs/CMake API.md` in the JUCE repo for full config
    juce_add_plugin(
            "${PROJECT_NAME}"
            COMPANY_NAME ${companyName}
            BUNDLE_ID com.${bundleId}
            IS_SYNTH TRUE
            NEEDS_MIDI_INPUT TRUE
            EDITOR_WANTS_KEYBOARD_FOCUS TRUE
            VST3_CATEGORIES Instrument
            AU_MAIN_TYPE kAudioUnitType_MusicEffect
            COPY_PLUGIN_AFTER_BUILD FALSE
            PLUGIN_MANUFACTURER_CODE ${pluginCompanyCode}
            PLUGIN_CODE "${pluginCode}"
            FORMATS AU VST3
            PRODUCT_NAME "${productName}"
            HARDENED_RUNTIME_ENABLED
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endfunction()

function(createEffectPlugin productName pluginCode pluginCompanyCode companyName bundleId InMidi vst3Category)
    list(APPEND CMAKE_MESSAGE_INDENT "   ")

    message("Create Effect Plugin ${PROJECT_NAME}")

    juce_add_plugin(
        "${PROJECT_NAME}"
        COMPANY_NAME ${companyName}
        BUNDLE_ID com.${bundleId}
        NEEDS_MIDI_INPUT ${InMidi}
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        VST3_CATEGORIES "${vst3Category}"
        AU_MAIN_TYPE kAudioUnitType_Effect
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE ${pluginCompanyCode}
        PLUGIN_CODE "${pluginCode}"
        FORMATS AU VST3
        PRODUCT_NAME "${productName}"
        HARDENED_RUNTIME_ENABLED
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endfunction()