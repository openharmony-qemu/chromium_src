/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <utility>

#include "base/containers/heap_array.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#define private public
#include "components/download/public/common/base_file.h"
#undef private
#include "components/download/public/common/download_interrupt_reasons_utils.h"

namespace download {
namespace {
static constexpr char kTestContent[] = "ArkWebTestData";  // "ArkWebTestData"
static constexpr size_t kTestContentSize = sizeof(kTestContent) - 1;
}  // namespace
class ArkWebBaseFileExtTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_file_path_ =
        CreateTestFile("test_base_file.bin", kTestContent, kTestContentSize);
  }
  void TearDown() override {
    base::DeletePathRecursively(
        base::FilePath("/data/arkweb_base_file_ext_test/"));
  }

  base::FilePath CreateTestFile(const char* test_file,
                                const char* text,
                                size_t size) {
    base::FilePath test_file_path("/data/arkweb_base_file_ext_test/");
    test_file_path = test_file_path.Append("download/testdata");
    EXPECT_TRUE(base::CreateDirectory(test_file_path));
    test_file_path = test_file_path.Append(test_file);
    EXPECT_TRUE(base::WriteFile(test_file_path, text));
    return test_file_path;
  }

  base::FilePath GetTestFilePath() { return test_file_path_; }

 private:
  base::FilePath test_file_path_;
};

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_Success) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;  // "ArkWebTestData" length

  // no file_, init it
  char buffer[kTestContentSize + 1] = {0};
  bool result =
      file_ext.ReadDataFromFile(0, buffer, kTestContentSize, GetTestFilePath());
  EXPECT_TRUE(result);
  EXPECT_STREQ(buffer, kTestContent);

  // file valid
  result =
      file_ext.ReadDataFromFile(0, buffer, kTestContentSize, GetTestFilePath());
  EXPECT_TRUE(result);
  EXPECT_STREQ(buffer, kTestContent);
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_OffsetTooLarge) {
  ArkWebBaseFileExt file_ext(1);
  const int offset = 10;
  file_ext.bytes_so_far_ = 5;  // set length to 5 to simulate partial read
  char buffer[kTestContentSize + 1] = {0};

  // no file opened
  bool result = file_ext.ReadDataFromFile(offset, buffer, kTestContentSize,
                                          GetTestFilePath());
  EXPECT_FALSE(result);

  base::File file(GetTestFilePath(),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  file_ext.file_ = std::move(file);
  result = file_ext.ReadDataFromFile(offset, buffer, kTestContentSize,
                                     GetTestFilePath());
  EXPECT_FALSE(result);
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_DataLenZero) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;
  char buffer[kTestContentSize + 1] = {0};

  // no file opened
  bool result = file_ext.ReadDataFromFile(0, buffer, 0, GetTestFilePath());
  EXPECT_TRUE(result);
  EXPECT_TRUE(strlen(buffer) == 0);

  base::File file(GetTestFilePath(),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  file_ext.file_ = std::move(file);
  result = file_ext.ReadDataFromFile(0, buffer, 0, GetTestFilePath());
  EXPECT_TRUE(result);
  EXPECT_TRUE(strlen(buffer) == 0);
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_DataLenTooLarge) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;
  char buffer[kTestContentSize + 1 + 1] = {0};

  // no file opened
  bool result = file_ext.ReadDataFromFile(0, buffer, kTestContentSize + 1,
                                          GetTestFilePath());
  EXPECT_FALSE(result);

  base::File file(GetTestFilePath(),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  file_ext.file_ = std::move(file);
  result = file_ext.ReadDataFromFile(0, buffer, kTestContentSize + 1,
                                     GetTestFilePath());
  EXPECT_FALSE(result);
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_InvalidFile) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;
  char buffer[kTestContentSize + 1 + 1] = {0};
  std::ofstream ofs(GetTestFilePath().value(), std::ios::binary);
  ofs.close();

  // no file opened
  bool result = file_ext.ReadDataFromFile(0, buffer, kTestContentSize + 1,
                                          GetTestFilePath());
  EXPECT_FALSE(result);

  base::File file(GetTestFilePath(),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  file_ext.file_ = std::move(file);
  result = file_ext.ReadDataFromFile(-1, buffer, kTestContentSize + 1,
                                     GetTestFilePath());
  EXPECT_FALSE(result);
}

TEST_F(ArkWebBaseFileExtTest, GetSaveDirectory_Default) {
  ArkWebBaseFileExt file_ext(1);
  base::FilePath dir = file_ext.GetSaveDirectory(base::FilePath());
  EXPECT_TRUE(dir.BaseName().value() == "Download");
  EXPECT_TRUE(base::DirectoryExists(dir));

  // delete the directory
  base::DeletePathRecursively(dir);
  dir = file_ext.GetSaveDirectory(base::FilePath());
  EXPECT_TRUE(dir.BaseName().value() == "Download");
  EXPECT_TRUE(base::DirectoryExists(dir));
}

TEST_F(ArkWebBaseFileExtTest, GetSaveDirectory_Custom) {
  ArkWebBaseFileExt file_ext(1);
  base::FilePath custom_dir("custom_dir");
  base::CreateDirectory(custom_dir);
  base::FilePath dir = file_ext.GetSaveDirectory(custom_dir);
  EXPECT_EQ(dir.value(), custom_dir.value());
  EXPECT_TRUE(base::DirectoryExists(dir));

  // delete the directory
  base::DeletePathRecursively(dir);
  dir = file_ext.GetSaveDirectory(custom_dir);
  EXPECT_EQ(dir.value(), custom_dir.value());
  EXPECT_FALSE(base::DirectoryExists(dir));
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_InitializeFileFails) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;

  std::string long_path = "/data/";
  long_path.append(10000, 'a');
  base::FilePath invalid_file(long_path);
  char buffer[kTestContentSize + 1] = {0};
  bool result =
      file_ext.ReadDataFromFile(0, buffer, kTestContentSize, invalid_file);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebBaseFileExtTest, ReadDataFromFile_FileAlreadyValid_NoReopen) {
  ArkWebBaseFileExt file_ext(1);
  file_ext.bytes_so_far_ = kTestContentSize;

  base::File file(GetTestFilePath(),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  file_ext.file_ = std::move(file);
  EXPECT_TRUE(file_ext.file_.IsValid());

  char buffer[kTestContentSize + 1] = {0};
  bool result =
      file_ext.ReadDataFromFile(0, buffer, kTestContentSize, GetTestFilePath());
  EXPECT_TRUE(result);
  EXPECT_STREQ(buffer, kTestContent);
  EXPECT_TRUE(file_ext.file_.IsValid());
}

#endif
}  // namespace download
