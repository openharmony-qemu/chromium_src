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

#include <cstdlib>
#include <string>

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_system_properties_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "audio_system_manager_adapter.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/system/sys_info.h"
#include "base/system/system_monitor.h"
#include "base/task/bind_post_task.h"
#include "gtest/gtest.h"
#include "media/audio/audio_thread.h"
#include "media/base/media_switches.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"

#define private public
#define protected public
#include "audio_dump.h"
#undef protected
#undef private
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

const char* DUMP_AUDIO_PARA = "web.debug.audiodump";
const char* DUMP_APP_DIR = "/data/storage/el2/base/cache/";

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;
using namespace OHOS::NWeb;
using namespace std;

namespace media {

class DumpFileUtilTest : public testing::Test {
  void SetUp() override { ASSERT_EQ(CreateDir(DUMP_APP_DIR), true); }

  void TearDown() override {}

 public:
  bool CreateDir(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
      if (S_ISDIR(st.st_mode)) {
        return true;
      } else {
        return false;
      }
    }

    return CreateParentDirectories(path);
  }

  bool CreateParentDirectories(const std::string& path) {
    size_t pos = path.find('/', 1);
    while (pos != std::string::npos) {
      std::string parentPath = path.substr(0, pos);
      if (!CreateDirectoryIfNotExists(parentPath)) {
        return false;
      }
      pos = path.find('/', pos + 1);
    }

    return CreateDirectoryIfNotExists(path);
  }

  bool CreateDirectoryIfNotExists(const std::string& dirPath) {
    struct stat st;
    if (stat(dirPath.c_str(), &st) != 0) {
      if (mkdir(dirPath.c_str(), 0755) != 0) {
        if (errno != EEXIST) {
          return false;
        }
      }
    }
    return true;
  }
};

TEST_F(DumpFileUtilTest, WriteDumpFile001) {
  FILE* file = nullptr;
  std::string filename = "test";

  std::string filePath = DUMP_APP_DIR + filename;
  file = fopen(filePath.c_str(), "wb+");

  ASSERT_NE(file, nullptr);

  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  buffer[1] = 1;
  DumpFileUtil::WriteDumpFile(nullptr, buffer, 1024);
  DumpFileUtil::WriteDumpFile(file, nullptr, 0);
  DumpFileUtil::WriteDumpFile(file, buffer, 1);
  DumpFileUtil::WriteDumpFile(file, buffer, 100);
  fclose(file);
  file = fopen(filePath.c_str(), "rb");

  uint8_t* bufferRes = new uint8_t[10];
  fread(bufferRes, sizeof(uint8_t), 1, file);
  ASSERT_EQ(buffer[0], bufferRes[0]);
  fclose(file);
  delete[] buffer;
  delete[] bufferRes;
}

TEST_F(DumpFileUtilTest, WriteDumpFile002) {
  FILE* file = nullptr;
  std::string filename = "test";

  std::string filePath = DUMP_APP_DIR + filename;
  file = fopen(filePath.c_str(), "wb+");

  ASSERT_NE(file, nullptr);

  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 11;
  buffer[1] = 1;
  fclose(file);
  file = fopen(filePath.c_str(), "rb");
  DumpFileUtil::WriteDumpFile(file, buffer, 100);
  file = fopen(filePath.c_str(), "rb");

  uint8_t* bufferRes = new uint8_t[10];
  fread(bufferRes, sizeof(uint8_t), 1, file);
  ASSERT_NE(buffer[0], bufferRes[0]);
  fclose(file);
  delete[] buffer;
  delete[] bufferRes;
}

TEST_F(DumpFileUtilTest, CloseDumpFile001) {
  FILE* file = nullptr;
  std::string filename = "test.pcm";

  std::string filePath = DUMP_APP_DIR + filename;
  DumpFileUtil::CloseDumpFile(nullptr);
  DumpFileUtil::CloseDumpFile(&file);
  file = fopen(filePath.c_str(), "wb+");

  ASSERT_NE(file, nullptr);

  fclose(file);
  DumpFileUtil::CloseDumpFile(&file);
  ASSERT_EQ(file, nullptr);

  file = fopen(filePath.c_str(), "wb+");
  DumpFileUtil::CloseDumpFile(&file);
  ASSERT_EQ(file, nullptr);
}

TEST_F(DumpFileUtilTest, OpenDumpFile001) {
  FILE* file = nullptr;
  std::string filename = "￥？/|*O$%!a/aa/test.pcm";
  DumpFileUtil::OpenDumpFile(filename, nullptr);
  DumpFileUtil::OpenDumpFile(filename, &file);
  ASSERT_EQ(file, nullptr);
}

TEST_F(DumpFileUtilTest, OpenDumpFile002) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(true));
  FILE* file = nullptr;
  std::string filename = "test.pcm";
  DumpFileUtil::OpenDumpFile(filename, &file);
  ASSERT_NE(file, nullptr);
  DumpFileUtil::CloseDumpFile(&file);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(DumpFileUtilTest, OpenDumpFile003) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(true));
  FILE* file = nullptr;
  std::string filename = "￥？/|*O$%!a/aa/test.pcm";
  DumpFileUtil::OpenDumpFile(filename, &file);
  ASSERT_EQ(file, nullptr);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(DumpFileUtilTest, OpenDumpFile004) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(false));
  FILE* file = nullptr;
  std::string filename = "test.pcm";
  DumpFileUtil::OpenDumpFile(filename, &file);
  ASSERT_EQ(file, nullptr);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(DumpFileUtilTest, WriteDumpScopedFile001) {
  base::ScopedFILE file;
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  buffer[1] = 1;
  DumpFileUtil::WriteDumpScopedFile(file, buffer, 1024);
  ASSERT_EQ(file.get(), nullptr);
  delete[] buffer;
}

TEST_F(DumpFileUtilTest, WriteDumpScopedFile002) {
  std::string filename = "test";
  std::string filePath = DUMP_APP_DIR + filename;
  base::ScopedFILE file(fopen(filePath.c_str(), "wb+"));
  ASSERT_NE(file, nullptr);

  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  buffer[1] = 1;

  DumpFileUtil::WriteDumpScopedFile(file, nullptr, 0);
  DumpFileUtil::WriteDumpScopedFile(file, buffer, 1);
  DumpFileUtil::WriteDumpScopedFile(file, buffer, 100);
  file = nullptr;
  FILE* file1 = fopen(filePath.c_str(), "rb");

  uint8_t* bufferRes = new uint8_t[10];
  fread(bufferRes, sizeof(uint8_t), 1, file1);
  ASSERT_EQ(buffer[0], bufferRes[0]);
  fclose(file1);
  delete[] buffer;
  delete[] bufferRes;
}

TEST_F(DumpFileUtilTest, WriteDumpScopedFile003) {
  std::string filename = "test";
  std::string filePath = DUMP_APP_DIR + filename;
  base::ScopedFILE file(fopen(filePath.c_str(), "rb"));
  ASSERT_NE(file, nullptr);
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 11;
  buffer[1] = 1;

  DumpFileUtil::WriteDumpScopedFile(file, buffer, 100);
  FILE* file1 = fopen(filePath.c_str(), "rb");

  uint8_t* bufferRes = new uint8_t[10];
  bufferRes[0] = 0;
  fread(bufferRes, sizeof(uint8_t), 1, file1);
  ASSERT_NE(buffer[0], bufferRes[0]);
  fclose(file1);
  delete[] buffer;
  delete[] bufferRes;
}

TEST_F(DumpFileUtilTest, CloseDumpScopedFile) {
  std::string filename = "test";
  std::string filePath = DUMP_APP_DIR + filename;
  base::ScopedFILE file(fopen(filePath.c_str(), "wb+"));
  DumpFileUtil::CloseDumpScopedFile(nullptr);
  DumpFileUtil::CloseDumpScopedFile(&file);
  ASSERT_EQ(file.get(), nullptr);
}

TEST_F(DumpFileUtilTest, OpenDumpScoped001) {
  base::ScopedFILE* file = nullptr;
  std::string filename = "test.pcm";
  ASSERT_NO_FATAL_FAILURE(DumpFileUtil::OpenDumpScopedFile(filename, file));
}

TEST_F(DumpFileUtilTest, OpenDumpScoped002) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(false));
  base::ScopedFILE file;
  std::string filename = "test.pcm";
  DumpFileUtil::OpenDumpScopedFile(filename, &file);
  ASSERT_EQ(file.get(), nullptr);
  DumpFileUtil::CloseDumpScopedFile(&file);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(DumpFileUtilTest, OpenDumpScoped003) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(true));
  base::ScopedFILE file;
  std::string filename = "￥？/|*O$%!a/aa/test.pcm";
  DumpFileUtil::OpenDumpScopedFile(filename, &file);
  ASSERT_EQ(file.get(), nullptr);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(DumpFileUtilTest, OpenDumpScoped004) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance())
      .WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetBoolParameter(DUMP_AUDIO_PARA, false))
      .WillOnce(Return(true));
  base::ScopedFILE file;
  std::string filename = "test.pcm";
  DumpFileUtil::OpenDumpScopedFile(filename, &file);
  ASSERT_NE(file.get(), nullptr);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}
}  // namespace media
