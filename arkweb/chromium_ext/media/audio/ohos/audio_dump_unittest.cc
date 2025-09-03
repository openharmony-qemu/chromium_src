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
#include "ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
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
        LOG(WARNING) << "Path exists but is not a directory: " << path;
        return false;
      }
    }

    size_t pos = path.find('/', 1);
    while (pos != std::string::npos) {
      std::string parentPath = path.substr(0, pos);
      if (stat(parentPath.c_str(), &st) != 0) {
        if (mkdir(parentPath.c_str(), 0755) != 0) {
          if (errno != EEXIST) {
            LOG(WARNING) << "Failed to create directory: " << parentPath
                         << " errno: " << errno;
            return false;
          }
        }
      }
      pos = path.find('/', pos + 1);
    }

    if (mkdir(path.c_str(), 0755) != 0) {
      if (errno != EEXIST) {
        LOG(WARNING) << "Failed to create directory: " << path
                     << " errno: " << errno;
        return false;
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

  uint8_t* buffer = new uint8_t[1];
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
}  // namespace media
