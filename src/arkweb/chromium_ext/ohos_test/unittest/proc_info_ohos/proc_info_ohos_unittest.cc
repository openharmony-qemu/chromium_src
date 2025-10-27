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

#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/proc_info_ohos.h"
#include <gtest/gtest.h>
#include <string>
#include <sys/time.h>
#include <iostream>

#include "base/process/process_handle.h"

namespace crashpad{
class ProcInfoOhosTest : public ::testing::Test {

};

TEST_F(ProcInfoOhosTest, GetProcStatusByPid001) {
   int realPid = getpid();
   struct ProcInfo procInfo;
   bool result = GetProcStatusByPid(realPid, procInfo);
   EXPECT_TRUE(result);
}

 TEST_F(ProcInfoOhosTest, GetProcStatusByPid002) {
   int realPid = getpid() + 5;
   struct ProcInfo procInfo;
   bool result = GetProcStatusByPid(realPid, procInfo);
   EXPECT_FALSE(result);
}

TEST_F(ProcInfoOhosTest, GetProcStatus) {
   struct ProcInfo procInfo;
   bool result = GetProcStatus(procInfo);
   EXPECT_TRUE(result);
}

TEST_F(ProcInfoOhosTest, TidToNstid001) {
   const int realPid = getpid();
   const int realTid = gettid();
   int nstid = 0;
   bool result = TidToNstid(realPid, realTid, nstid);
   EXPECT_TRUE(result);
}

TEST_F(ProcInfoOhosTest, TidToNstid002) {
   const int realPid = getpid() + 5;
   const int realTid = gettid() + 5;
   int nstid = 0;
   bool result = TidToNstid(realPid, realTid, nstid);
   EXPECT_FALSE(result);
}

TEST_F(ProcInfoOhosTest, TidToNstid003) {
   const int realPid = 12444;
   const int realTid = 213132;
   int nstid = 0;
   bool result = TidToNstid(realPid, realTid, nstid);
   EXPECT_FALSE(result);
}

TEST_F(ProcInfoOhosTest, IsThreadInPid001) {
   int realPid = getpid();
   int realTid = gettid();
   bool result = IsThreadInPid(realPid, realTid);
   EXPECT_TRUE(result);
}

TEST_F(ProcInfoOhosTest, IsThreadInPid002) {
   int unRealPid = getpid() + 1;
   int realTid = gettid();
   bool result = IsThreadInPid(unRealPid, realTid);
   EXPECT_FALSE(result);
}

TEST_F(ProcInfoOhosTest, GetTidsByPidWithFunc) {
   std::function<bool(int)> func = nullptr;
   const int realPid = getpid();
   bool result = GetTidsByPidWithFunc(realPid, func);
   EXPECT_TRUE(result);
}

TEST_F(ProcInfoOhosTest, GetTidMapByPid) {
   std::unordered_map<int, int> tid_nstid_map;
   const int realPid = getpid();
   bool result = GetTidMapByPid(realPid, tid_nstid_map);
   EXPECT_FALSE(result);
}
}//namespace crashpad