cmake_minimum_required(VERSION 3.5.0)
project(GitAddonsManager LANGUAGES CXX)
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")
endif()
if(NOT DEFINED LIBGIT2_INCLUDE_DIR)
    set(LIBGIT2_INCLUDE_DIR "C:/libgit2/include")
endif()
if(NOT DEFINED LIBGIT2_LIBRARY)
    set(LIBGIT2_LIBRARY "C:/libgit2/lib/git2.lib")
endif()
find_package(Qt6 COMPONENTS Quick Core QuickControls2 Concurrent Network Widgets REQUIRED)
find_package(Qt6LinguistTools)

find_package(Git)


if(GIT_EXECUTABLE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

string(REGEX REPLACE "^v" "" VERSION_STRING "${${PROJECT_NAME}_GIT_TAG}")

if(NOT CLEAN_APP_VERSION)
    set(CLEAN_APP_VERSION "1.0.0")
endif()

if (NOT WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBGIT2 REQUIRED IMPORTED_TARGET libgit2)
else()
    # Try to find libgit2 on Windows. Allow overriding with LIBGIT2_DIR or LIBGIT2_INCLUDE_DIR/LIBGIT2_LIBRARY
    if (NOT DEFINED LIBGIT2_INCLUDE_DIR)
        find_path(LIBGIT2_INCLUDE_DIR git2.h
            PATHS
                ENV LIBGIT2_DIR
                "C:/Program Files/libgit2/include"
                "C:/Program Files (x86)/libgit2/include"
                "C:/libgit2/include"
            NO_DEFAULT_PATH
        )
    endif()
    if (NOT DEFINED LIBGIT2_LIBRARY)
        find_library(LIBGIT2_LIBRARY NAMES libgit2 git2
            PATHS
                ENV LIBGIT2_DIR
                "C:/Program Files/libgit2/lib"
                "C:/Program Files (x86)/libgit2/lib"
                "C:/libgit2/lib"
            NO_DEFAULT_PATH
        )
    endif()
    if (NOT LIBGIT2_INCLUDE_DIR OR NOT LIBGIT2_LIBRARY)
        message(FATAL_ERROR "libgit2 not found. Install libgit2 and set LIBGIT2_DIR or pass -DLIBGIT2_INCLUDE_DIR=<path> -DLIBGIT2_LIBRARY=<path> to CMake.")
    endif()
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
    singleapplication.h singleapplication_p.h singleapplication.cpp singleapplication_p.cpp
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

if(WIN32)
    find_program(WINDEPLOYQT windeployqt HINTS "${CMAKE_PREFIX_PATH}/bin")
    install(CODE "
        execute_process(
            COMMAND \"${WINDEPLOYQT}\"
                 --release
                 --qmldir \"${CMAKE_SOURCE_DIR}\"
                 \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/GitAddonsManager.exe\"
        )
    ")
    install(CODE "
        file(GLOB VCPKG_DLLS \"C:/vcpkg/installed/x64-windows/bin/*.dll\")
        file(INSTALL \${VCPKG_DLLS} DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")
    ")
endif()

if(NOT WIN32)
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/io.gitlab.woblight.GitAddonsManager.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/io.gitlab.woblight.GitAddonsManager.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
    )
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "GitAddonsManager")
set(CPACK_PACKAGE_VERSION "${VERSION_STRING}")
set(CPACK_PACKAGE_EXECUTABLES "GitAddonsManager" "GitAddonsManager")
set(CPACK_PACKAGE_FILE_NAME "GitAddonsManager-${VERSION_STRING}")
include(CPack)

qt_finalize_executable(GitAddonsManager)
