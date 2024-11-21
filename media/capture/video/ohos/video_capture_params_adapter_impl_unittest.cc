
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
 
#include "gtest/gtest.h"
#include "video_capture_params_adapter_impl.h"
 
using namespace testing;
 
namespace OHOS::NWeb {
 
class VideoCaptureParamsAdapterImplTest : public testing::Test {
protected:
    VideoCaptureParamsAdapterImpl* adapter;
 
    virtual void SetUp() {
        adapter = new VideoCaptureParamsAdapterImpl();
    }
 
    virtual void TearDown() {
        delete adapter;
    }
};
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetWidth_ShouldReturnWidth_WhenCalled)
{
    uint32_t expectedWidth = 1920;
    adapter->SetWidth(expectedWidth);
    uint32_t actualWidth = adapter->GetWidth();
    ASSERT_EQ(expectedWidth, actualWidth);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetHeight_ShouldReturnCorrectHeight_WhenCalled)
{
    uint32_t expectedHeight = 1080;
    adapter->SetHeight(expectedHeight);
    uint32_t actualHeight = adapter->GetHeight();
    ASSERT_EQ(expectedHeight, actualHeight);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetFrameRate_ShouldReturnCorrectFrameRate_WhenCalled)
{
    float expectedFrameRate = 30.0;
    adapter->SetFrameRate(expectedFrameRate);
    float actualFrameRate = adapter->GetFrameRate();
    ASSERT_EQ(expectedFrameRate, actualFrameRate);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetPixelFormat_ShouldReturnCorrectPixelFormat_WhenCalled)
{
    VideoPixelFormatAdapter expectedFormat = VideoPixelFormatAdapter::FORMAT_YUV_420_SP;
    adapter->SetPixelFormat(expectedFormat);
    VideoPixelFormatAdapter actualFormat = adapter->GetPixelFormat();
    ASSERT_EQ(expectedFormat, actualFormat);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetEnableFaceDetection_ShouldReturnTrue_WhenEnabled)
{
    adapter->SetEnableFaceDetection(true);
    ASSERT_EQ(adapter->GetEnableFaceDetection(), true);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, GetEnableFaceDetection_ShouldReturnFalse_WhenDisabled)
{
    adapter->SetEnableFaceDetection(false);
    ASSERT_EQ(adapter->GetEnableFaceDetection(), false);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetWidth_ShouldSetWidth_WhenCalled)
{
    uint32_t expectedWidth = 1920;
    adapter->SetWidth(expectedWidth);
    ASSERT_EQ(adapter->GetWidth(), expectedWidth);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetHeight_ShouldSetHeight_WhenCalled)
{
    uint32_t expectedHeight = 1080;
    adapter->SetHeight(expectedHeight);
    ASSERT_EQ(adapter->GetHeight(), expectedHeight);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetFrameRate_ShouldSetCorrectly_WhenCalled)
{
    float expectedFrameRate = 30.0f;
    adapter->SetFrameRate(expectedFrameRate);
    ASSERT_EQ(adapter->GetFrameRate(), expectedFrameRate);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetPixelFormat_ShouldSetPixelFormat_WhenValidValue)
{
    VideoPixelFormatAdapter format = VideoPixelFormatAdapter::FORMAT_YCBCR_420_888;
    adapter->SetPixelFormat(format);
    ASSERT_EQ(adapter->GetPixelFormat(), format);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetEnableFaceDetection_ShouldSetTrue_WhenCalledWithTrue)
{
    adapter->SetEnableFaceDetection(true);
    ASSERT_EQ(adapter->GetEnableFaceDetection(), true);
}
 
TEST_F(VideoCaptureParamsAdapterImplTest, SetEnableFaceDetection_ShouldSetFalse_WhenCalledWithFalse)
{
    adapter->SetEnableFaceDetection(false);
    ASSERT_EQ(adapter->GetEnableFaceDetection(), false);
}
 
}
