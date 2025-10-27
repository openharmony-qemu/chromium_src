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
#include "codec_format_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

class CodecFormatAdapterImplTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

/**
 * @tc.name: GetWidth_ShouldReturnWidth_WhenCalled
 * @tc.number: CodecFormatAdapterImplTest_001
 * @tc.desc: Test GetWidth method of  CodecFormatAdapterImpl class
 */
HWTEST_F(CodecFormatAdapterImplTest,
         GetWidth_ShouldReturnWidth_WhenCalled, TestSize.Level0)
{
    CodecFormatAdapterImpl codecFormatAdapterImpl;
    int32_t width = 1280;
    codecFormatAdapterImpl.SetWidth(width);
    EXPECT_EQ(codecFormatAdapterImpl.GetWidth(), width);
}

/**
 * @tc.name: GetHeight_ShouldReturnHeight_WhenCalled
 * @tc.number: CodecFormatAdapterImplTest_002
 * @tc.desc: Test GetHeight method of  CodecFormatAdapterImpl class
 */
HWTEST_F(CodecFormatAdapterImplTest,
         GetHeight_ShouldReturnHeight_WhenCalled, TestSize.Level0)
{
    CodecFormatAdapterImpl codecFormatAdapterImpl;
    int32_t height = 720;
    codecFormatAdapterImpl.SetHeight(height);
    EXPECT_EQ(codecFormatAdapterImpl.GetHeight(), height);
}