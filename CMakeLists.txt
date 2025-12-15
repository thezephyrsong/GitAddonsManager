cmake_minimum_required(VERSION 3.5.0)
project(GitAddonsManager LANGUAGES CXX)
find_package(Qt6 COMPONENTS Quick Core QuickControls2 Concurrent Network Widgets REQUIRED)
find_package(Qt6LinguistTools)

find_package(Git)


if(GIT_EXECUTABLE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --long --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

if (NOT WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBGIT2 REQUIRED IMPORTED_TARGET libgit2)
else()
    include_directories(${LIBGIT2_INCLUDE_DIR})
endif()

add_compile_definitions(QT_DEPRECATED_WARNINGS)

qt_standard_project_setup()

qt_add_resources(RESOURCES
    qml.qrc
)

set(qml_files
    AddonsPanel.qml 
    AddonUpdateButton.qml
    ColorPicker.qml
    main.qml
    MaterialSettings.qml
    Options.qml
    StyleSettingsBase.qml
    Tray.qml
    UniversalSettings.qml
    Update.qml
    LogWindow.qml
)

set(${PROJECT_NAME}_BUILD_NAME "" CACHE STRING "name of the build pipeline for self updates")

set(${PROJECT_NAME}_BRANCH_NAME "" CACHE STRING "name of the branch for self updates")

option(${PROJECT_NAME}_SELF_UPDATE "Enable the self-update feature")

qt_add_executable(GitAddonsManager
    addon.h  control.h  utils.h
    main.cpp addon.cpp  control.cpp
    CI.mk .gitlab-ci.yml
        ${RESOURCES}
        MANUAL_FINALIZATION
)

set_target_properties(GitAddonsManager PROPERTIES
    CXX_STANDARD 20
    WIN32_EXECUTABLE ON
)


add_compile_definitions(
    GIT_DESCRIBE="${${PROJECT_NAME}_GIT_DESCRIBE}"
    GAM_EXEC="GitAddonsManager${CMAKE_EXECUTABLE_SUFFIX}"
    )

if (${PROJECT_NAME}_SELF_UPDATE)
    find_package(QuaZip-Qt6)
    if (${PROJECT_NAME}_BUILD_NAME STREQUAL "")
        message(FATAL_ERROR "${PROJECT_NAME}_SELF_UPDATE is enabled but ${PROJECT_NAME}_BUILD_NAME is not set" )
    endif()
    if (${PROJECT_NAME}_BRANCH_NAME STREQUAL "")
        message(FATAL_ERROR "${PROJECT_NAME}_SELF_UPDATE is enabled but ${PROJECT_NAME}_BRANCH_NAME is not set" )
    endif()
    add_compile_definitions(GAM_BRANCH_NAME="${${PROJECT_NAME}_BRANCH_NAME}")
    add_compile_definitions(GAM_BUILD_NAME="${${PROJECT_NAME}_BUILD_NAME}")
    add_compile_definitions(GAM_SELF_UPDATE)
    target_link_libraries(GitAddonsManager PUBLIC QuaZip::QuaZip)
endif()


if (qt6QuickCompiler_FOUND)
    qtquick_compiler_add_resources(RESOURCES qml.qrc)
    target_link_libraries(GitAddonsManager PRIVATE Qt6::QmlPrivate Qt6::QuickPrivate)
    message(STATUS "Using qtquickcompiler")
else()
    message(STATUS "qtquickcompiler not found, skipping")
endif()

target_link_libraries(GitAddonsManager PUBLIC Qt6::Core)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Quick)
target_link_libraries(GitAddonsManager PUBLIC Qt6::QuickControls2)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Concurrent)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Network)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Widgets)
if (NOT WIN32)
    target_link_libraries(GitAddonsManager PUBLIC PkgConfig::LIBGIT2)
else()
    target_link_libraries(GitAddonsManager PUBLIC ${LIBGIT2_LIBRARY})
endif()


set(APPNAME GitAddonsManager)

install(TARGETS GitAddonsManager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/io.gitlab.woblight.GitAddonsManager.desktop
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install(
  PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/io.gitlab.woblight.GitAddonsManager.metainfo.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
)

qt_finalize_executable(GitAddonsManager)
