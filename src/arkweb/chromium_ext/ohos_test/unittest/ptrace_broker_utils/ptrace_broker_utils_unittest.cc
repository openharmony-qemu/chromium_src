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

#define private public
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/ptrace_broker_utils.h"
#undef private

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "third_party/crashpad/crashpad/util/linux/ptrace_broker.h"

class PtraceBrokerUtilsTest : public ::testing ::Test {
public:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(PtraceBrokerUtilsTest, ConvertRealtidToNstid){
    int real_tid = gettid();
    crashpad::PtraceBroker *raw_broker = new crashpad::PtraceBroker(123, 456, true);
    int ret = crashpad::PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, nullptr);
    EXPECT_EQ(ret, real_tid);

    ret = crashpad::PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, raw_broker);
    EXPECT_EQ(ret, real_tid);
    
    raw_broker->is_in_pid_ns_ = true;
    raw_broker->tid_nstid_map_[real_tid]++;
    ret = crashpad::PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, raw_broker);
    EXPECT_EQ(ret, 1);

    int error_tid = 22;
    ret = crashpad::PtraceBrokerUtils::ConvertRealtidToNstid(error_tid, raw_broker);
    EXPECT_EQ(ret, error_tid);
}