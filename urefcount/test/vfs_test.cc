#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include "virtual_file.h"

class VFSTest : public testing::Test {
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
const char *VFSTest::path = "test.db";

TEST_F(VFSTest, FileReadTotal) {
  virtual_file *file = new virtual_file(path);
  char buffer[BLOCK_SIZE * 3];
  memset(buffer, 0, BLOCK_SIZE * 3);
  file->read(buffer, BLOCK_SIZE * 3, 0);
  ASSERT_STREQ(buffer, "01234");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2], "56789");
  ASSERT_STREQ(&buffer[BLOCK_SIZE], "101112");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2 * 3], "131415");
  ASSERT_STREQ(&buffer[BLOCK_SIZE * 2], "161718");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2 * 5], "192021");
  delete file;
};

TEST_F(VFSTest, FileReadBlock) {
  virtual_file *file = new virtual_file(path);
  char buffer[BLOCK_SIZE];
  memset(buffer, 0, BLOCK_SIZE);
  file->read(buffer, BLOCK_SIZE, 0);
  ASSERT_STREQ(buffer, "01234");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2], "56789");
  memset(buffer, 0, BLOCK_SIZE);
  file->read(buffer, BLOCK_SIZE, BLOCK_SIZE);
  ASSERT_STREQ(buffer, "101112");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2], "131415");
  memset(buffer, 0, BLOCK_SIZE);
  file->read(buffer, BLOCK_SIZE, BLOCK_SIZE * 2);
  ASSERT_STREQ(buffer, "161718");
  ASSERT_STREQ(&buffer[BLOCK_SIZE / 2], "192021");
  delete file;
};

TEST_F(VFSTest, FileReadHalf) {
  virtual_file *file = new virtual_file(path);
  char buffer[BLOCK_SIZE / 2];
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, 0);
  ASSERT_STREQ(buffer, "01234");
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, BLOCK_SIZE / 2);
  ASSERT_STREQ(buffer, "56789");
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, BLOCK_SIZE);
  ASSERT_STREQ(buffer, "101112");
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, BLOCK_SIZE / 2 * 3);
  ASSERT_STREQ(buffer, "131415");
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, BLOCK_SIZE * 2);
  ASSERT_STREQ(buffer, "161718");
  memset(buffer, 0, BLOCK_SIZE / 2);
  file->read(buffer, BLOCK_SIZE / 2, BLOCK_SIZE / 2 * 5);
  ASSERT_STREQ(buffer, "192021");
  delete file;
};

TEST_F(VFSTest, DRBM) {
  virtual_file *file = new virtual_file(path);
  std::vector<std::thread> threads;
  constexpr long NTHREADS = 10;

  // Multi Readers
  for (long i = 0; i < NTHREADS; i++) {
    threads.emplace_back(
        [file](long id) {
          char buffer[BLOCK_SIZE];
          memset(buffer, 0, BLOCK_SIZE);
          size_t capacity = file->get_capacity();
          for (off_t bn = 0; bn < capacity; bn++) {
            file->read(buffer, BLOCK_SIZE, bn * BLOCK_SIZE);
            switch (bn) {
              case 0:
                ASSERT_STREQ(buffer, "01234");
                break;
              case 1:
                ASSERT_STREQ(buffer, "101112");
                break;
              case 2:
                ASSERT_STREQ(buffer, "161718");
                break;
              default:
                break;
            }
          }
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }

  delete file;
};

TEST_F(VFSTest, DRBMQuery) {
  virtual_file *file = new virtual_file(path);
  std::vector<std::thread> threads;
  constexpr long NTHREADS = 10;
  constexpr int NITERS = 1000;

  // Multi Readers
  for (long i = 0; i < NTHREADS; i++) {
    threads.emplace_back(
        [file](long id) {
          char buffer[BLOCK_SIZE];
          memset(buffer, 0, BLOCK_SIZE);
          size_t capacity = file->get_capacity();
          for (int i = 0; i < NITERS; i++) {
            for (off_t bn = 0; bn < capacity; bn++) {
              file->read(buffer, BLOCK_SIZE, bn * BLOCK_SIZE);
            }
          }
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }

  // Verify query
  size_t capacity = file->get_capacity();
  for (off_t bn = 0; bn < capacity; bn++) {
    EXPECT_EQ(0, file->query(bn));
  }

  delete file;
};

TEST_F(VFSTest, DRBHQuery) {
  virtual_file *file = new virtual_file(path);
  std::vector<std::thread> threads;
  constexpr long NTHREADS = 10;
  constexpr int NITERS = 1000;

  // Multi Readers
  for (long i = 0; i < NTHREADS; i++) {
    threads.emplace_back(
        [file](long id) {
          char buffer[BLOCK_SIZE];
          memset(buffer, 0, BLOCK_SIZE);
          for (int i = 0; i < NITERS; i++) {
            file->read(buffer, BLOCK_SIZE, 0);
          }
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }

  // Verify query
  EXPECT_EQ(0, file->query(0));

  delete file;
};
