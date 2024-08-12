cmake_minimum_required(VERSION 3.1.0)
project(GitAddonsManager LANGUAGES CXX)
find_package(Qt6 COMPONENTS Quick Core QuickControls2 Concurrent Network Widgets REQUIRED)
find_package(Qt6LinguistTools)
find_package(QuaZip-Qt6)

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

if (qt6QuickCompiler_FOUND)
    qtquick_compiler_add_resources(RESOURCES qml.qrc)
    message(STATUS "Using qtquickcompiler")
else()
    message(STATUS "qtquickcompiler not found, skipping")
endif()

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
)

set(${PROJECT_NAME}_BUILD_NAME "" CACHE STRING "name of the build pipeline for self updates")

set(${PROJECT_NAME}_BRANCH_NAME "" CACHE STRING "name of the branch for self updates")

qt_add_executable(GitAddonsManager
    addon.h  control.h  utils.h
    main.cpp addon.cpp  control.cpp
    CI.mk .gitlab-ci.yml
        ${RESOURCES}
        MANUAL_FINALIZATION
)

set_property(TARGET GitAddonsManager PROPERTY CXX_STANDARD 20)

add_compile_definitions(
    GIT_DESCRIBE="${GIT_DESCRIBE}"
    GAM_EXEC="GitAddonsManager${CMAKE_EXECUTABLE_SUFFIX}"
    )

if (NOT ${PROJECT_NAME}_BUILD_NAME STREQUAL "")
    add_compile_definitions(GAM_BUILD_NAME="${${PROJECT_NAME}_BUILD_NAME}")
endif()
if (NOT ${PROJECT_NAME}_BRANCH_NAME STREQUAL "")
    add_compile_definitions(GAM_BRANCH_NAME="${${PROJECT_NAME}_BRANCH_NAME}")
endif()

target_link_libraries(GitAddonsManager PUBLIC Qt6::Core)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Quick)
target_link_libraries(GitAddonsManager PUBLIC Qt6::QuickControls2)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Concurrent)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Network)
target_link_libraries(GitAddonsManager PUBLIC Qt6::Widgets)
target_link_libraries(GitAddonsManager PUBLIC QuaZip::QuaZip)
if (NOT WIN32)
    target_link_libraries(GitAddonsManager PUBLIC PkgConfig::LIBGIT2)
else()
    target_link_libraries(GitAddonsManager PUBLIC ${LIBGIT2_LIBRARY})
endif()


target_link_libraries(GitAddonsManager PRIVATE Qt6::QmlPrivate Qt6::QuickPrivate)
set(APPNAME GitAddonsManager)

install(TARGETS GitAddonsManager
        CONFIGURATIONS Release
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

qt_finalize_executable(GitAddonsManager)
