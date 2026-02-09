/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_util.h"
#include "gtest/gtest.h"

namespace angle
{

namespace ohos
{

class OhosUtilTest : public ::testing::Test
{
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGBA_8888
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGBA8888)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGBA_8888);
    EXPECT_EQ(result, GL_RGBA8);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGBX_8888
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGBX8888)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGBX_8888);
    EXPECT_EQ(result, GL_RGB8);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGB_888
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGB888)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGB_888);
    EXPECT_EQ(result, GL_RGB8);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGB_565
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGB565)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGB_565);
    EXPECT_EQ(result, GL_RGB565);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for BGRA_8888
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_BGRA8888)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_BGRA_8888);
    EXPECT_EQ(result, GL_BGRA8_EXT);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for BGRA_5551
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_BGRA5551)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_BGRA_5551);
    EXPECT_EQ(result, GL_RGB5_A1);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for BGRA_4444
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_BGRA4444)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_BGRA_4444);
    EXPECT_EQ(result, GL_RGBA4);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGBA16_FLOAT
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGBA16Float)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGBA16_FLOAT);
    EXPECT_EQ(result, GL_RGBA16F);
}

// Test: NativePixelFormatToGLInternalFormat returns correct GL format for RGBA_1010102
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_RGBA1010102)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_RGBA_1010102);
    EXPECT_EQ(result, GL_RGB10_A2);
}

// Test: NativePixelFormatToGLInternalFormat returns GL_NONE for BLOB format
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_Blob)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_BLOB);
    EXPECT_EQ(result, GL_NONE);
}

// Test: NativePixelFormatToGLInternalFormat returns GL_RGB8 for YUV formats
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_YUV420SP)
{
    GLenum result = NativePixelFormatToGLInternalFormat(ANGLE_OHOS_FORMAT_YCBCR_420_SP);
    EXPECT_EQ(result, GL_RGB8);
}

// Test: NativePixelFormatToGLInternalFormat returns GL_RGB8 for unknown YUV formats
TEST_F(OhosUtilTest, NativePixelFormatToGLInternalFormat_UnknownFormat)
{
    GLenum result = NativePixelFormatToGLInternalFormat(9999);
    EXPECT_EQ(result, GL_RGB8);
}

// Test: NativePixelFormatIsYUV returns false for RGBA_8888
TEST_F(OhosUtilTest, NativePixelFormatIsYUV_RGBA8888)
{
    bool result = NativePixelFormatIsYUV(ANGLE_OHOS_FORMAT_RGBA_8888);
    EXPECT_FALSE(result);
}

// Test: NativePixelFormatIsYUV returns false for RGB_565
TEST_F(OhosUtilTest, NativePixelFormatIsYUV_RGB565)
{
    bool result = NativePixelFormatIsYUV(ANGLE_OHOS_FORMAT_RGB_565);
    EXPECT_FALSE(result);
}

// Test: NativePixelFormatIsYUV returns true for YCBCR_420_SP
TEST_F(OhosUtilTest, NativePixelFormatIsYUV_YUV420SP)
{
    bool result = NativePixelFormatIsYUV(ANGLE_OHOS_FORMAT_YCBCR_420_SP);
    EXPECT_TRUE(result);
}

// Test: NativePixelFormatIsYUV returns true for YCBCR_422_SP
TEST_F(OhosUtilTest, NativePixelFormatIsYUV_YUV422SP)
{
    bool result = NativePixelFormatIsYUV(ANGLE_OHOS_FORMAT_YCBCR_422_SP);
    EXPECT_TRUE(result);
}

// Test: NativePixelFormatIsYUV returns true for unknown vendor formats
TEST_F(OhosUtilTest, NativePixelFormatIsYUV_UnknownFormat)
{
    bool result = NativePixelFormatIsYUV(9999);
    EXPECT_TRUE(result);
}

// Test: ClientBufferToOHNativeBuffer returns nullptr for invalid buffer
TEST_F(OhosUtilTest, ClientBufferToOHNativeBuffer_NullBuffer)
{
    EGLClientBuffer null_buffer = nullptr;
    void *result = ClientBufferToOHNativeBuffer(null_buffer);
    EXPECT_EQ(result, nullptr);
}

// Test: GetOHNativeBufferProperties handles null buffer gracefully
TEST_F(OhosUtilTest, GetOHNativeBufferProperties_NullBuffer)
{
    int width = 0;
    int height = 0;
    int depth = 0;
    int pixelFormat = 0;
    int usage = 0;

    GetOHNativeBufferProperties(nullptr, &width, &height, &depth, &pixelFormat, &usage);

    // Should not crash and values should remain unchanged or default
    EXPECT_EQ(width, 0);
}

// Test: GetOHNativeBufferProperties handles null output pointers
TEST_F(OhosUtilTest, GetOHNativeBufferProperties_NullPointers)
{
    void *dummy_buffer = reinterpret_cast<void*>(0x12345678);

    // Should handle null pointers gracefully without crashing
    GetOHNativeBufferProperties(dummy_buffer, nullptr, nullptr, nullptr, nullptr, nullptr);

    // If we reach here without crashing, the test passes
    EXPECT_TRUE(true);
}

}  // namespace ohos

}  // namespace angle
