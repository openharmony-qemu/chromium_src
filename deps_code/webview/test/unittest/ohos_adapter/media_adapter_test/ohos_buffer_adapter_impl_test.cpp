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

#include "gtest/gtest.h"
#define private public
#include "ohos_buffer_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

class OhosBufferAdapterImplTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

/**
 * @tc.name: GetAddr_ShouldReturnAddr_WhenCalled
 * @tc.number: OhosBufferAdapterImplTest_001
 * @tc.desc: Test GetAddr method of OhosBufferAdapterImpl class
 */
HWTEST_F(OhosBufferAdapterImplTest,
         GetAddr_ShouldReturnAddr_WhenCalled, TestSize.Level0)
{
    std::unique_ptr<uint8_t[]> addr = std::make_unique<uint8_t[]>(1024);
    OhosBufferAdapterImpl ohosBufferAdapterImpl;
    ohosBufferAdapterImpl.SetAddr(addr.get());
    EXPECT_EQ(addr.get(), ohosBufferAdapterImpl.GetAddr());
}

/**
 * @tc.name: GetBufferSize_ShouldReturnBufferSize_WhenCalled
 * @tc.number: OhosBufferAdapterImplTest_002
 * @tc.desc: Test GetBufferSize method of OhosBufferAdapterImpl class
 */
HWTEST_F(OhosBufferAdapterImplTest,
         GetBufferSize_ShouldReturnBufferSize_WhenCalled, TestSize.Level0)
{
    OhosBufferAdapterImpl ohosBufferAdapterImpl;
    uint32_t bufferSize = 2048;
    ohosBufferAdapterImpl.SetBufferSize(bufferSize);
    EXPECT_EQ(ohosBufferAdapterImpl.GetBufferSize(), bufferSize);
}