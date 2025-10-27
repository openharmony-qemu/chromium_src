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
#include "buffer_info_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

class BufferInfoAdapterImplTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

/**
 * @tc.name: GetPresentationTimeUs_ShouldReturnPresentationTimeUs_WhenCalled
 * @tc.number: BufferInfoAdapterImplTest_001
 * @tc.desc: Test GetPresentationTimeUs method of BufferInfoAdapterImpl class
 */
HWTEST_F(BufferInfoAdapterImplTest,
         GetPresentationTimeUs_ShouldReturnPresentationTimeUs_WhenCalled, TestSize.Level0)
{
    BufferInfoAdapterImpl bufferInfoAdapterImpl;
    int64_t presentationTimeUs = 1234567890;
    bufferInfoAdapterImpl.SetPresentationTimeUs(presentationTimeUs);
    EXPECT_EQ(bufferInfoAdapterImpl.GetPresentationTimeUs(), presentationTimeUs);
}

/**
 * @tc.name: GetSize_ShouldReturnSize_WhenCalled
 * @tc.number: BufferInfoAdapterImplTest_002
 * @tc.desc: Test GetSize method of BufferInfoAdapterImpl class
 */
HWTEST_F(BufferInfoAdapterImplTest,
         GetSize_ShouldReturnSize_WhenCalled, TestSize.Level0)
{
    BufferInfoAdapterImpl bufferInfoAdapterImpl;
    int32_t size = 1024;
    bufferInfoAdapterImpl.SetSize(size);
    EXPECT_EQ(bufferInfoAdapterImpl.GetSize(), size);
}

/**
 * @tc.name: GetOffset_ShouldReturnOffset_WhenCalled
 * @tc.number: BufferInfoAdapterImplTest_003
 * @tc.desc: Test GetOffset method of BufferInfoAdapterImpl class
 */
HWTEST_F(BufferInfoAdapterImplTest,
         GetOffset_ShouldReturnOffset_WhenCalled, TestSize.Level0)
{
    BufferInfoAdapterImpl bufferInfoAdapterImpl;
    int32_t offset = 512;
    bufferInfoAdapterImpl.SetOffset(offset);
    EXPECT_EQ(bufferInfoAdapterImpl.GetOffset(), offset);
}