#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include "virtual_file_cache_affinity.h"

using vfs_type = virtual_file_cache_affinity;

class VFSCacheAffinityTest : public testing::Test {
 protected:
  void SetUp() override {
    int fd = open(path, O_RDWR | O_CREAT | O_SYNC, 0644);
    char origin[BLOCK_SIZE * 3];
    memset(origin, 0, BLOCK_SIZE * 3);
    memcpy(origin, "01234", 5);
    memcpy(&origin[BLOCK_SIZE / 2], "56789", 5);
    memcpy(&origin[BLOCK_SIZE], "101112", 6);
    memcpy(&origin[BLOCK_SIZE / 2 * 3], "131415", 6);
    memcpy(&origin[BLOCK_SIZE * 2], "161718", 6);
    memcpy(&origin[BLOCK_SIZE / 2 * 5], "192021", 6);
    write(fd, origin, BLOCK_SIZE * 3);
    close(fd);
  }
  void TearDown() override { remove(path); }

  static const char *path;
};
const char *VFSCacheAffinityTest::path = "test.db";

TEST_F(VFSCacheAffinityTest, DRBMQuery) {
  const unsigned int NTHREADS = std::thread::hardware_concurrency();
  constexpr int NITERS = 10;

  vfs_type file(path);
  std::vector<std::thread> threads;

  // Multi Readers
  for (long i = 0; i < NTHREADS; i++) {
    threads.emplace_back(
        [&file](long id) {
          file.setup();
          char buffer[BLOCK_SIZE];
          memset(buffer, 0, BLOCK_SIZE);
          size_t capacity = file.get_capacity();
          for (int i = 0; i < NITERS; i++) {
            for (off_t bn = 0; bn < capacity; bn++) {
              file.read(buffer, BLOCK_SIZE, bn * BLOCK_SIZE);
            }
          }
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }

  // Verify query
  size_t capacity = file.get_capacity();
  for (off_t bn = 0; bn < capacity; bn++) {
    EXPECT_EQ(0, file.query(bn));
  }
};

TEST_F(VFSCacheAffinityTest, DRBHQuery) {
  const unsigned int NTHREADS = std::thread::hardware_concurrency();
  constexpr int NITERS = 10;

  vfs_type file(path);
  std::vector<std::thread> threads;

  // Multi Readers
  for (long i = 0; i < NTHREADS; i++) {
    threads.emplace_back(
        [&file](long id) {
          file.setup();
          char buffer[BLOCK_SIZE];
          memset(buffer, 0, BLOCK_SIZE);
          for (int i = 0; i < NITERS; i++) {
            file.read(buffer, BLOCK_SIZE, 0);
          }
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }

  // Verify query
  EXPECT_EQ(0, file.query(0));
};
