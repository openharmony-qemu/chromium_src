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

#include "buffer_desc_adapter_impl.h"
#include "gtest/gtest.h"

using namespace OHOS::NWeb;

class BufferDescAdapterImplTest : public testing::Test
{
protected:
    void SetUp() override
    {
        bufferDescAdapterImpl = new BufferDescAdapterImpl();
    }
    void TearDown() override
    {
        delete bufferDescAdapterImpl;
    }
    BufferDescAdapterImpl *bufferDescAdapterImpl;
};

TEST_F(BufferDescAdapterImplTest, GetBuffer_ShouldReturnBuffer)
{
    uint8_t *expectedBuffer = new uint8_t[10];
    bufferDescAdapterImpl->SetBuffer(expectedBuffer);
    uint8_t *actualBuffer = bufferDescAdapterImpl->GetBuffer();
    EXPECT_EQ(expectedBuffer, actualBuffer);
    delete[] expectedBuffer;
}

TEST_F(BufferDescAdapterImplTest, GetBuffer_ShouldReturnNull)
{
    bufferDescAdapterImpl->SetBuffer(nullptr);
    uint8_t *actualBuffer = bufferDescAdapterImpl->GetBuffer();
    EXPECT_EQ(nullptr, actualBuffer);
}

TEST_F(BufferDescAdapterImplTest, GetBufLength_ShouldReturnNonZero)
{
    size_t expectedBufLength = 10;
    bufferDescAdapterImpl->SetBufLength(expectedBufLength);
    size_t actualBufLength = bufferDescAdapterImpl->GetDataLength();
    EXPECT_EQ(actualBufLength, expectedBufLength);
}

TEST_F(BufferDescAdapterImplTest, GetBufLength_ShouldReturnMaxValue)
{
    bufferDescAdapterImpl->SetBufLength(SIZE_MAX);
    size_t actualBufLength = bufferDescAdapterImpl->GetDataLength();
    EXPECT_EQ(actualBufLength, SIZE_MAX);
}

TEST_F(BufferDescAdapterImplTest, GetDataLength_ShouldReturnNonZero)
{
    size_t expectedDataLength = 10;
    bufferDescAdapterImpl->SetDataLength(expectedDataLength);
    size_t actualDataLength = bufferDescAdapterImpl->GetDataLength();
    EXPECT_EQ(actualDataLength, expectedDataLength);
}

TEST_F(BufferDescAdapterImplTest, GetDataLength_ShouldReturnMaxValue)
{
    bufferDescAdapterImpl->SetDataLength(SIZE_MAX);
    size_t actualDataLength = bufferDescAdapterImpl->GetDataLength();
    EXPECT_EQ(actualDataLength, SIZE_MAX);
}

TEST_F(BufferDescAdapterImplTest, SetBuffer_ShouldSetBuffer)
{
    uint8_t *expectedBuffer = new uint8_t[10];
    bufferDescAdapterImpl->SetBuffer(expectedBuffer);
    EXPECT_EQ(bufferDescAdapterImpl->GetBuffer(), expectedBuffer);
}

TEST_F(BufferDescAdapterImplTest, SetBufLength_ShouldSetBufLength)
{
    size_t expectedBufLength = 10;
    bufferDescAdapterImpl->SetBufLength(expectedBufLength);
    EXPECT_EQ(bufferDescAdapterImpl->GetBufLength(), expectedBufLength);
}

TEST_F(BufferDescAdapterImplTest, SetDataLength_ShouldSetDataLength)
{
    size_t expectedDataLength = 10;
    bufferDescAdapterImpl->SetDataLength(expectedDataLength);
    EXPECT_EQ(bufferDescAdapterImpl->GetDataLength(), expectedDataLength);
}
