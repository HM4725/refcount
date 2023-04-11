cmake_minimum_required(VERSION 3.26)

project(urefcount_benchmark VERSION 1.0.0)

include(CTest)

# C++ settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options for libraries
option(THREADS_PREFER_PTHREAD_FLAG "Use pthread library" ON)
option(USE_VFS "Use the VFS library" ON)
option(USE_GOOGLE_TEST "Use GoogleTest for testing" ON)

# Thread library
find_package(Threads REQUIRED)

# VFS project library
if(USE_VFS)
  add_subdirectory(vfs)
  list(APPEND EXTRA_LIBS vfs)
endif()

# GoogleTest
if(USE_GOOGLE_TEST)
  add_subdirectory(test)
endif()

add_executable(${CMAKE_PROJECT_NAME} main.cc)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${EXTRA_LIBS} PRIVATE Threads::Threads)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  "${PROJECT_BINARY_DIR}"
  )
