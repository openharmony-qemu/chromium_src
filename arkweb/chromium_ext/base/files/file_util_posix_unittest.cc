/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "testing/gtest/include/gtest/gtest.h"

#include "base/files/file_util.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wheader-hygiene"
#include "base/files/file_util_posix.cc"

namespace base {
TEST(FileUtilPosixTest, MakeAbsoluteFilePath_001) {
  FilePath input_;
  const FilePath& input = input_;
  MakeAbsoluteFilePath(input);
  char full_path[PATH_MAX];
  EXPECT_EQ(FilePath(full_path), MakeAbsoluteFilePath(input));
}

TEST(FileUtilPosixTest, PathExists) {
  const FilePath path_;
  const FilePath& path = path_;
  PathExists(path);
  EXPECT_EQ(PathExists(path), false);
}

TEST(FileUtilPosixTest, ReadFromFD_001) {
  int fd = 10;
  const size_t bytes = 20;
  char array[bytes] = {0};
  std::span<char> buf(array, bytes);
  bool result = ReadFromFD(fd, buf);
  EXPECT_EQ(result, false);
}

TEST(FileUtilPosixTest, ReadFromFD_002) {
  const char* filename = "example.txt";
  int fd = open(filename, O_RDONLY);
  const size_t bytes = 1024;
  char buffer[bytes] = {0};
  std::span<char> buf(buffer, bytes);
  bool result = ReadFromFD(fd, buffer);
  EXPECT_EQ(result, false);
}

}  // namespace base
