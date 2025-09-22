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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/mman.h>

#define private public
#include "arkweb/ohos_adapter_ndk/graphic_adapter/vsync_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

static void OnVsyncCallback() {}

class VSyncAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VSyncAdapterImplTest::SetUpTestCase() {}

void VSyncAdapterImplTest::TearDownTestCase() {}

void VSyncAdapterImplTest::SetUp() {}

void VSyncAdapterImplTest::TearDown() {}

TEST_F(VSyncAdapterImplTest, VSyncAdapterImplTest_001)
{
    VSyncAdapterNdkImpl &adapter = VSyncAdapterNdkImpl::GetInstance();
    adapter.Init();
    VSyncAdapterNdkImpl vsyncAdapter;
    void* client = nullptr;
    adapter.OnVsync(1, client);
    adapter.VsyncCallbackInner(0);
    adapter.VsyncCallbackInner(1);
    int64_t period = adapter.GetVSyncPeriod();
    EXPECT_EQ(period, 0);
    adapter.SetFrameRateLinkerEnable(false);
    adapter.SetFramePreferredRate(1);
    adapter.SetOnVsyncCallback(OnVsyncCallback);
    adapter.SetOnVsyncEndCallback(OnVsyncCallback);
    adapter.SetIsGPUProcess(false);
    adapter.SetDVSyncSwitch(false);
}

TEST_F(VSyncAdapterImplTest, VSyncAdapterImplTest_002)
{
    VSyncAdapterNdkImpl &adapter = VSyncAdapterNdkImpl::GetInstance();
    adapter.SetScene("", 1);
    adapter.hasRequestedVsync_ = true;
    EXPECT_EQ(VSyncErrorCode::SUCCESS, adapter.RequestVsync(nullptr, nullptr));
    adapter.hasRequestedVsync_ = false;
    EXPECT_EQ(VSyncErrorCode::SUCCESS, adapter.RequestVsync(nullptr, nullptr));
}
} // namespace OHOS::NWeb