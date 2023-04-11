# Sources
set(VFS_SOURCE_DIR src)
set(VFS_SOURCES
  ${VFS_SOURCE_DIR}/virtual_file.cc
  ${VFS_SOURCE_DIR}/virtual_file_nonatomic.cc
  ${VFS_SOURCE_DIR}/virtual_file_noop.cc
  
	${VFS_SOURCE_DIR}/hash_table.cc
  )

# Headers
set(VFS_HEADER_DIR include)
set(VFS_HEADERS
  ${VFS_HEADER_DIR}/virtual_file.h
  ${VFS_HEADER_DIR}/virtual_file_nonatomic.h
  ${VFS_HEADER_DIR}/virtual_file_noop.h
  
  ${VFS_HEADER_DIR}/cpu.h
  
	${VFS_HEADER_DIR}/ds/hash_table.h
  )

add_library(vfs STATIC ${VFS_HEADERS} ${VFS_SOURCES})

target_include_directories(vfs
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${VFS_HEADER_DIR}"
  )

