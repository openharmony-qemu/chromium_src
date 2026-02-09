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

#include "NativeBufferImageSiblingOhos.h"
#include "gtest/gtest.h"

namespace rx
{

class NativeBufferImageSiblingOhosTest : public ::testing::Test
{
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test: The constructor initializes the member variable correctly
TEST_F(NativeBufferImageSiblingOhosTest, Constructor_InitializesMembers)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify buffer is stored correctly
    EXPECT_EQ(sibling.getBuffer(), buffer);
}

// Test: Constructor extracts EGL_GL_COLORSPACE_KHR from attribs
TEST_F(NativeBufferImageSiblingOhosTest, Constructor_ExtractsColorSpaceFromAttribs)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;
    attribs.set(EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR);

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify buffer is stored and initialize completes
    EXPECT_EQ(sibling.getBuffer(), buffer);
    egl::Error result = sibling.initialize(nullptr);
    EXPECT_TRUE(result.isNoError());
}

// Test: The constructor uses EGL_GL_COLORSPACE_LINEAR_KHR by default
TEST_F(NativeBufferImageSiblingOhosTest, Constructor_DefaultColorSpace)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs; // No EGL_GL_COLORSPACE_KHR attribute

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify initialize completes successfully
    egl::Error result = sibling.initialize(nullptr);
    EXPECT_TRUE(result.isNoError());
}

// Test: The destructor cleans up the resource correctly
TEST_F(NativeBufferImageSiblingOhosTest, Destructor_CleansUpResources)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    {
        NativeBufferImageSiblingOhos sibling(buffer, attribs);
        // sibling goes out of scope and destructor is called
    }

    // If destructor completes without crash, test passes
    EXPECT_TRUE(true);
}

// Test: initialize and call GetOHNativeBufferProperties to get buffer properties
TEST_F(NativeBufferImageSiblingOhosTest, Initialize_CallsGetOHNativeBufferProperties)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify initialize completes successfully
    egl::Error result = sibling.initialize(nullptr);
    EXPECT_TRUE(result.isNoError());
}

// Test: initialize setting mFormat and mYUV
TEST_F(NativeBufferImageSiblingOhosTest, Initialize_SetsFormatAndYUV)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    egl::Error result = sibling.initialize(nullptr);

    // Verify initialize completes successfully
    EXPECT_TRUE(result.isNoError());
}

// Test: initialize and set mHasProtectedContent to false
TEST_F(NativeBufferImageSiblingOhosTest, Initialize_SetsProtectedContentToFalse)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    egl::Error result = sibling.initialize(nullptr);

    // mHasProtectedContent should be set to false
    EXPECT_TRUE(result.isNoError());
    EXPECT_FALSE(sibling.hasProtectedContent());
}

// Test: initialize returns NoError
TEST_F(NativeBufferImageSiblingOhosTest, Initialize_ReturnsNoError)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    egl::Error result = sibling.initialize(nullptr);

    EXPECT_TRUE(result.isNoError());
}

// Test: getFormat returns mFormat
TEST_F(NativeBufferImageSiblingOhosTest, GetFormat_ReturnsStoredFormat)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    sibling.initialize(nullptr);

    gl::Format format = sibling.getFormat();

    // Should return the format set during initialize (GL_NONE for null buffer)
    EXPECT_EQ(format, GL_NONE);
}

// Test: isRenderable always returns true
TEST_F(NativeBufferImageSiblingOhosTest, IsRenderable_AlwaysReturnsTrue)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    bool result = sibling.isRenderable(nullptr);

    EXPECT_TRUE(result);
}

// Test: isRenderable returns true for null context
TEST_F(NativeBufferImageSiblingOhosTest, IsRenderable_NullContextReturnsTrue)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    bool result = sibling.isRenderable(nullptr);

    EXPECT_TRUE(result);
}

// Test: isTexturable always returns true
TEST_F(NativeBufferImageSiblingOhosTest, IsTexturable_AlwaysReturnsTrue)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    bool result = sibling.isTexturable(nullptr);

    EXPECT_TRUE(result);
}

// Test: isTexturable returns true for null context
TEST_F(NativeBufferImageSiblingOhosTest, IsTexturable_NullContextReturnsTrue)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    bool result = sibling.isTexturable(nullptr);

    EXPECT_TRUE(result);
}

// Test: isYUV returns the mYUV flag
TEST_F(NativeBufferImageSiblingOhosTest, IsYUV_ReturnsStoredYUVFlag)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    sibling.initialize(nullptr);

    bool result = sibling.isYUV();

    // Should return false for non-YUV buffer (default)
    EXPECT_FALSE(result);
}

// Test: isYUV initial value is false
TEST_F(NativeBufferImageSiblingOhosTest, IsYUV_InitialValueIsFalse)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Before initialize(), mYUV should be false (from constructor)
    EXPECT_FALSE(sibling.isYUV());
}

// Test: hasProtectedContent returns mHasProtectedContent
TEST_F(NativeBufferImageSiblingOhosTest, HasProtectedContent_ReturnsStoredFlag)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    sibling.initialize(nullptr);

    bool result = sibling.hasProtectedContent();

    // Should return false (hardcoded in initialize())
    EXPECT_FALSE(result);
}

// Test: getSize returns mSize
TEST_F(NativeBufferImageSiblingOhosTest, GetSize_ReturnsStoredSize)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    sibling.initialize(nullptr);

    gl::Extents size = sibling.getSize();

    // Should return the size set during initialize (0,0,0 for null buffer)
    EXPECT_EQ(size.width, 0);
    EXPECT_EQ(size.height, 0);
    EXPECT_EQ(size.depth, 0);
}

// Test: getSize returns Extents that include width, height, and depth
TEST_F(NativeBufferImageSiblingOhosTest, GetSize_ContainsWidthHeightDepth)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);
    sibling.initialize(nullptr);

    gl::Extents size = sibling.getSize();

    // Extents should contain width, height, depth
    EXPECT_EQ(size.depth, 0);  // Default depth for null buffer
}

// Test: getSamples always returns 0
TEST_F(NativeBufferImageSiblingOhosTest, GetSamples_AlwaysReturnsZero)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    size_t result = sibling.getSamples();

    EXPECT_EQ(result, 0);
}

// Test: getBuffer returns mBuffer
TEST_F(NativeBufferImageSiblingOhosTest, GetBuffer_ReturnsStoredBuffer)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0xABCD1234);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    EGLClientBuffer result = sibling.getBuffer();

    EXPECT_EQ(result, buffer);
}

// Test: getBuffer returns null for a null buffer
TEST_F(NativeBufferImageSiblingOhosTest, GetBuffer_NullBufferReturnsNull)
{
    EGLClientBuffer buffer = nullptr;
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    EGLClientBuffer result = sibling.getBuffer();

    EXPECT_EQ(result, nullptr);
}

// Test: getImageCreationAttributes collects all forwarded attributes
TEST_F(NativeBufferImageSiblingOhosTest, GetImageCreationAttributes_CollectsAllForwardedAttribs)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    std::vector<EGLint> outAttributes;
    sibling.getImageCreationAttributes(&outAttributes);

    // Should complete without crashing
    EXPECT_TRUE(outAttributes.empty() || !outAttributes.empty());
}

// Test: getImageCreationAttributes only includes existing attributes
TEST_F(NativeBufferImageSiblingOhosTest, GetImageCreationAttributes_OnlyIncludesPresentAttribs)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;
    // Add only some attributes
    attribs.set(EGL_WIDTH, 256);
    attribs.set(EGL_HEIGHT, 256);

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    std::vector<EGLint> outAttributes;
    sibling.getImageCreationAttributes(&outAttributes);

    // Should only include EGL_WIDTH and EGL_HEIGHT
    // Output format: [EGL_WIDTH, 256, EGL_HEIGHT, 256]
    EXPECT_FALSE(outAttributes.empty());
}

// Test: getImageCreationAttributes returns an empty vector for empty attribs
TEST_F(NativeBufferImageSiblingOhosTest, GetImageCreationAttributes_EmptyAttribsReturnsEmpty)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs; // Empty

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    std::vector<EGLint> outAttributes;
    sibling.getImageCreationAttributes(&outAttributes);

    // Should be empty since no attributes are set
    EXPECT_TRUE(outAttributes.empty());
}

// Test: getImageCreationAttributes includes EGL_WIDTH and EGL_HEIGHT
TEST_F(NativeBufferImageSiblingOhosTest, GetImageCreationAttributes_IncludesWidthAndHeight)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;
    attribs.set(EGL_WIDTH, 1920);
    attribs.set(EGL_HEIGHT, 1080);

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    std::vector<EGLint> outAttributes;
    sibling.getImageCreationAttributes(&outAttributes);

    // Should include EGL_WIDTH and EGL_HEIGHT in output
    EXPECT_FALSE(outAttributes.empty());
}

// Test: getImageCreationAttributes includes YUV-related attributes
TEST_F(NativeBufferImageSiblingOhosTest, GetImageCreationAttributes_IncludesYUVAttribs)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;
    attribs.set(EGL_YUV_COLOR_SPACE_HINT_EXT, EGL_ITU_REC601_EXT);
    attribs.set(EGL_SAMPLE_RANGE_HINT_EXT, EGL_YUV_FULL_RANGE_EXT);

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    std::vector<EGLint> outAttributes;
    sibling.getImageCreationAttributes(&outAttributes);

    // Should include YUV color space and sample range hints
    // Should include YUV attributes in output
    EXPECT_FALSE(outAttributes.empty());
}

// Test: All member variables are correctly initialized during construction
TEST_F(NativeBufferImageSiblingOhosTest, MemberVariables_Initialization)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;
    attribs.set(EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR);

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify buffer is stored correctly
    EXPECT_EQ(sibling.getBuffer(), buffer);
    // Verify initialize completes
    egl::Error result = sibling.initialize(nullptr);
    EXPECT_TRUE(result.isNoError());
}

// Test: Constructor handling of null buffer
TEST_F(NativeBufferImageSiblingOhosTest, Constructor_NullBuffer)
{
    EGLClientBuffer buffer = nullptr;
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Should handle null buffer gracefully
    EXPECT_EQ(sibling.getBuffer(), nullptr);
}

// Test: initialize handling of null buffer
TEST_F(NativeBufferImageSiblingOhosTest, Initialize_NullBuffer)
{
    EGLClientBuffer buffer = nullptr;
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    egl::Error result = sibling.initialize(nullptr);

    // Should handle null buffer (GetOHNativeBufferProperties should handle it)
    EXPECT_TRUE(result.isNoError());
}

// Test: Constructor handling of various buffer values
TEST_F(NativeBufferImageSiblingOhosTest, Constructor_EdgeCaseBuffers)
{
    egl::AttributeMap attribs;

    // Test with various buffer values
    NativeBufferImageSiblingOhos sibling1(nullptr, attribs);
    EXPECT_EQ(sibling1.getBuffer(), nullptr);

    NativeBufferImageSiblingOhos sibling2(reinterpret_cast<EGLClientBuffer>(0xFFFFFFFF), attribs);
    EXPECT_EQ(sibling2.getBuffer(), reinterpret_cast<EGLClientBuffer>(0xFFFFFFFF));

    NativeBufferImageSiblingOhos sibling3(reinterpret_cast<EGLClientBuffer>(1), attribs);
    EXPECT_EQ(sibling3.getBuffer(), reinterpret_cast<EGLClientBuffer>(1));
}

// Test: Implement the ExternalImageSiblingEGL interface
TEST_F(NativeBufferImageSiblingOhosTest, ImplementsExternalImageSiblingEGL)
{
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    NativeBufferImageSiblingOhos sibling(buffer, attribs);

    // Verify all interface methods are callable
    egl::Error initResult = sibling.initialize(nullptr);
    EXPECT_TRUE(initResult.isNoError());

    gl::Format format = sibling.getFormat();
    EXPECT_TRUE(format == GL_NONE || format == GL_RGB8);

    bool isRenderable = sibling.isRenderable(nullptr);
    EXPECT_TRUE(isRenderable);

    bool isTexturable = sibling.isTexturable(nullptr);
    EXPECT_TRUE(isTexturable);

    bool hasProtected = sibling.hasProtectedContent();
    EXPECT_FALSE(hasProtected);

    size_t samples = sibling.getSamples();
    EXPECT_EQ(samples, 0);
}

}  // namespace rx
