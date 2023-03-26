#include "virtual_file_nonatomic.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdio.h>

class VFSNonatomicTest : public testing::Test {
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
const char *VFSNonatomicTest::path = "test.db";

TEST_F(VFSNonatomicTest, FileReadTotal) {
  virtual_file *file = new virtual_file_nonatomic(path);
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

TEST_F(VFSNonatomicTest, FileReadBlock) {
  virtual_file *file = new virtual_file_nonatomic(path);
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

TEST_F(VFSNonatomicTest, FileReadHalf) {
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