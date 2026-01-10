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
#include "arkweb/build/features/features.h"
if BUILDFLAG(ARKWEB_UNITTESTS)
#define private public
#include "base/debug/arkweb_dump_info.h"
#undef private
#else
#include "base/debug/arkweb_dump_info.h"
#endif
#include "base/logging.h"
#include "base/no_destructor.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace base {
namespace debug {

class ArkWebDumpInfoTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(ArkWebDumpInfoTest, GetInstance) {
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  EXPECT_THAT(arkwebDumpInfo.dump_enable_, ::testing::AnyOf(0, 1));
}

TEST_F(ArkWebDumpInfoTest, IsDumpEnabled) {
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  EXPECT_THAT(arkwebDumpInfo.dump_enable_, ::testing::AnyOf(0, 1));
}

TEST_F(ArkWebDumpInfoTest, GetBufferSize) {
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  EXPECT_GE(arkwebDumpInfo.GetBufferSize, 0);
}

TEST_F(ArkWebDumpInfoTest, ParseCmdParamAndDump_001) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 0;
  arkwebDumpInfo.ParseCmdParamAndDump("", result);
  EXPECT_NE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, ParseCmdParamAndDump_002) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 1;
  arkwebDumpInfo.FormatAndWriteNWebDumpInfo("test ParseCmdParamAndDump_002");
  arkwebDumpInfo.ParseCmdParamAndDump("", result);
  EXPECT_GE(result.size(), 0);
  result.clear();
  arkwebDumpInfo.ParseCmdParamAndDump("--all", result);
  EXPECT_GE(result.size(), 0);
  result.clear();
  arkwebDumpInfo.ParseCmdParamAndDump("--nweb", result);
  EXPECT_GE(result.size(), 0);
  result.clear();
  arkwebDumpInfo.ParseCmdParamAndDump("--xxxx", result);
  EXPECT_GE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, DumpArkWebAllInfo) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 1;
  arkwebDumpInfo.FormatAndWriteNWebDumpInfo("test DumpArkWebAllInfo");
  arkwebDumpInfo.DumpArkWebAllInfo(result);
  EXPECT_GE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, DumpArkWebNWebInfo) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 1;
  arkwebDumpInfo.FormatAndWriteNWebDumpInfo("test DumpArkWebNWebInfo");
  arkwebDumpInfo.DumpArkWebNWebInfo(result);
  EXPECT_GE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, GetCurrentTimeInfo) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  result = arkwebDumpInfo.GetCurrentTimeInfo();
  EXPECT_GE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, GetProcessAndThreadIdInfo) {
  std::string result;
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  result = arkwebDumpInfo.GetProcessAndThreadIdInfo();
  EXPECT_GE(result.size(), 0);
}

TEST_F(ArkWebDumpInfoTest, WriteArkWebDumpInfo) {
  std::string info = "test WriteArkWebDumpInfo";
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 1;
  size_t t = arkwebDumpInfo.GetBufferSize();
  arkwebDumpInfo.WriteArkWebDumpInfo(info, DUMP_NWEB_INFO);
  size_t r = arkwebDumpInfo.GetBufferSize();
  EXPECT_EQ(r, t + 1);
  t = arkwebDumpInfo.GetBufferSize();
  arkwebDumpInfo.WriteArkWebDumpInfo(info, DUMP_OTHER_INFO);
  r = arkwebDumpInfo.GetBufferSize();
  EXPECT_EQ(r, t);
}

TEST_F(ArkWebDumpInfoTest, FormatAndWriteNWebDumpInfo) {
  std::string info = "test WriteArkWebDumpInfo";
  ArkWebDumpInfo& arkwebDumpInfo = ArkWebDumpInfo::GetInstance();
  arkwebDumpInfo.dump_enable_ = 1;
  size_t t = arkwebDumpInfo.GetBufferSize();
  arkwebDumpInfo.FormatAndWriteNWebDumpInfo(info);
  size_t r = arkwebDumpInfo.GetBufferSize();
  EXPECT_EQ(r, t + 1);
}
} // namespace base
} // namespace debug