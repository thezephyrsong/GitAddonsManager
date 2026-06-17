# Install script for directory: C:/Users/d4nk3/source/repos/GitAddonsManager

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/d4nk3/source/repos/GitAddonsManager/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/d4nk3/source/repos/GitAddonsManager/out/build/x64-Debug/GitAddonsManager.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
        execute_process(
            COMMAND "C:/Qt/6.11.1/msvc2022_64/bin/windeployqt.exe"
                 --release
                 --qmldir "C:/Users/d4nk3/source/repos/GitAddonsManager"
                 "${CMAKE_INSTALL_PREFIX}/bin/GitAddonsManager.exe"
        )
    
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
        file(GLOB VCPKG_DLLS "C:/vcpkg/installed/x64-windows/bin/*.dll")
        file(INSTALL ${VCPKG_DLLS} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/msvcp140.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/msvcp140_1.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/msvcp140_2.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/msvcp140_atomic_wait.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/msvcp140_codecvt_ids.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/vcruntime140_1.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/vcruntime140.dll"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/concrt140.dll"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE DIRECTORY FILES "")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/d4nk3/source/repos/GitAddonsManager/out/build/x64-Debug/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/d4nk3/source/repos/GitAddonsManager/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
