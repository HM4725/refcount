# Sources
set(VFS_SOURCE_DIR src)
set(VFS_SOURCES
  ${VFS_SOURCE_DIR}/thread_manager.cc
  ${VFS_SOURCE_DIR}/virtual_file.cc
  ${VFS_SOURCE_DIR}/virtual_file_nonatomic.cc
  ${VFS_SOURCE_DIR}/virtual_file_noop.cc
  ${VFS_SOURCE_DIR}/virtual_file_cache_affinity.cc
  ${VFS_SOURCE_DIR}/virtual_file_refcache.cc
  )

# Headers
set(VFS_HEADER_DIR include)
set(VFS_HEADERS
  ${VFS_HEADER_DIR}/params.h
  ${VFS_HEADER_DIR}/utils.h
  ${VFS_HEADER_DIR}/thread_manager.h
  ${VFS_HEADER_DIR}/timer.h
  ${VFS_HEADER_DIR}/virtual_file_types.h

  ${VFS_HEADER_DIR}/virtual_file.h
  ${VFS_HEADER_DIR}/virtual_file_nonatomic.h
  ${VFS_HEADER_DIR}/virtual_file_noop.h
  ${VFS_HEADER_DIR}/virtual_file_cache_affinity.h
  ${VFS_HEADER_DIR}/virtual_file_refcache.h
  )

add_library(vfs STATIC ${VFS_HEADERS} ${VFS_SOURCES})

target_include_directories(vfs
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${VFS_HEADER_DIR}"
  )

