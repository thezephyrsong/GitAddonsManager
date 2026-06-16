# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\GitAddonsManager_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\GitAddonsManager_autogen.dir\\ParseCache.txt"
  "GitAddonsManager_autogen"
  )
endif()
