# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/ec25eea8f8237cf86c30703f59747e42f34b6f75.zip)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(VFS_TESTS
  vfs_test.cc
  vfs_nonatomic_test.cc
  vfs_noop_test.cc
  vfs_cache_affinity_test.cc
  hash_table_test.cc
	)

add_executable(vfs_test ${VFS_TESTS})

target_link_libraries(
  vfs_test
  vfs
  gtest_main
  )

include(GoogleTest)
gtest_discover_tests(vfs_test)

