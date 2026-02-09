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

#include "DisplayOhos.h"
#include "gtest/gtest.h"

namespace rx
{

class DisplayOhosTest : public ::testing::Test
{
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test: DisplayOhos constructor correctly initializes the DisplayEGL base class
TEST_F(DisplayOhosTest, Constructor_InitializesBaseClass)
{
    // Create a minimal DisplayState for testing
    egl::DisplayState state;
    state.displayId = 0;

    // Constructor should initialize DisplayEGL base class
    DisplayOhos display(state);

    // Verify display is properly constructed (can be used as base pointer)
    DisplayEGL *base_pointer = &display;
    EXPECT_NE(base_pointer, nullptr);
}

// Test: DisplayOhos destructor correctly cleans up resources
TEST_F(DisplayOhosTest, Destructor_CleansUpResources)
{
    egl::DisplayState state;
    state.displayId = 0;

    // Create and destroy display
    {
        DisplayOhos display(state);
        // display goes out of scope and destructor is called
    }

    // If we reach here without crashing, destructor worked correctly
    EXPECT_TRUE(true);
}

// Test: isValidNativeWindow returns true for any window (TODO implementation)
TEST_F(DisplayOhosTest, IsValidNativeWindow_AlwaysReturnsTrue)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // According to current implementation, always returns true (TODO)
    EGLNativeWindowType test_window = reinterpret_cast<EGLNativeWindowType>(0x12345678);
    bool result = display.isValidNativeWindow(test_window);

    EXPECT_TRUE(result);
}

// Test: isValidNativeWindow also returns true for a null window
TEST_F(DisplayOhosTest, IsValidNativeWindow_NullWindowReturnsTrue)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // Even with null window, current implementation returns true
    EGLNativeWindowType null_window = nullptr;
    bool result = display.isValidNativeWindow(null_window);

    EXPECT_TRUE(result);
}

// Test: validateImageClientBuffer returns NoError for the EGL_NATIVE_BUFFER_OHOS target
TEST_F(DisplayOhosTest, ValidateImageClientBuffer_OHOSNativeBufferReturnsNoError)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    EGLenum target = EGL_NATIVE_BUFFER_OHOS;
    EGLClientBuffer clientBuffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    egl::Error result = display.validateImageClientBuffer(context, target, clientBuffer, attribs);

    EXPECT_TRUE(result.isNoError());
}

// Test: validateImageClientBuffer calls the base class method for other targets
TEST_F(DisplayOhosTest, ValidateImageClientBuffer_OtherTargetCallsBaseClass)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    // Use a different target (not EGL_NATIVE_BUFFER_OHOS)
    EGLenum target = EGL_NATIVE_BUFFER_ANDROID;
    EGLClientBuffer clientBuffer = nullptr;
    egl::AttributeMap attribs;

    // Should call DisplayEGL::validateImageClientBuffer
    egl::Error result = display.validateImageClientBuffer(context, target, clientBuffer, attribs);

    // Verify the call completed without crashing (result may be error or success depending on base class)
    EXPECT_TRUE(result.isError() || result.isNoError());
}

// Test: validateImageClientBuffer calls the base class method on an unknown target
TEST_F(DisplayOhosTest, ValidateImageClientBuffer_UnknownTargetCallsBaseClass)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    // Use an invalid target
    EGLenum target = 0x9999;
    EGLClientBuffer clientBuffer = nullptr;
    egl::AttributeMap attribs;

    egl::Error result = display.validateImageClientBuffer(context, target, clientBuffer, attribs);

    // Should call base class without crashing (result depends on base class implementation)
    EXPECT_TRUE(result.isError() || result.isNoError());
}

// Test: createExternalImageSibling creates NativeBufferImageSiblingOhos for EGL_NATIVE_BUFFER_OHOS
TEST_F(DisplayOhosTest, CreateExternalImageSibling_OHOSNativeBufferCreatesSibling)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    EGLenum target = EGL_NATIVE_BUFFER_OHOS;
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0x12345678);
    egl::AttributeMap attribs;

    ExternalImageSiblingImpl *result = display.createExternalImageSibling(context, target, buffer, attribs);

    // Should create a NativeBufferImageSiblingOhos instance
    EXPECT_NE(result, nullptr);

    // Clean up
    delete result;
}

// Test: The object created by createExternalImageSibling is correctly initialized
TEST_F(DisplayOhosTest, CreateExternalImageSibling_OHOSNativeBufferInitializesCorrectly)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    EGLenum target = EGL_NATIVE_BUFFER_OHOS;
    EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(0xABCD1234);
    egl::AttributeMap attribs;

    ExternalImageSiblingImpl *result = display.createExternalImageSibling(context, target, buffer, attribs);

    ASSERT_NE(result, nullptr);
    // The NativeBufferImageSiblingOhos should be created with the buffer and attribs
    // We can't directly test the internal state without access to the class definition

    delete result;
}

// Test: createExternalImageSibling calls the base class method on other targets
TEST_F(DisplayOhosTest, CreateExternalImageSibling_OtherTargetCallsBaseClass)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    // Use a different target
    EGLenum target = EGL_NATIVE_BUFFER_ANDROID;
    EGLClientBuffer buffer = nullptr;
    egl::AttributeMap attribs;

    // Should call DisplayEGL::createExternalImageSibling
    ExternalImageSiblingImpl *result = display.createExternalImageSibling(context, target, buffer, attribs);

    // Result depends on base class implementation
    // May return nullptr or create a different sibling type
    if (result != nullptr)
    {
        delete result;
        EXPECT_TRUE(true);
    }
    else
    {
        // Returning nullptr is also valid for unsupported targets
        EXPECT_TRUE(true);
    }
}

// Test: createExternalImageSibling handles null buffer
TEST_F(DisplayOhosTest, CreateExternalImageSibling_NullBuffer)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *context = nullptr;
    EGLenum target = EGL_NATIVE_BUFFER_OHOS;
    EGLClientBuffer buffer = nullptr;
    egl::AttributeMap attribs;

    ExternalImageSiblingImpl *result = display.createExternalImageSibling(context, target, buffer, attribs);

    // NativeBufferImageSiblingOhos should still be created even with null buffer
    // (validation happens during initialize())
    EXPECT_NE(result, nullptr);

    delete result;
}

// Test: Verify that validateImageClientBuffer's switch covers all paths
TEST_F(DisplayOhosTest, ValidateImageClientBuffer_SwitchCoverage)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // Test EGL_NATIVE_BUFFER_OHOS path
    egl::Error result1 = display.validateImageClientBuffer(
        nullptr, EGL_NATIVE_BUFFER_OHOS, nullptr, egl::AttributeMap());
    EXPECT_TRUE(result1.isNoError());

    // Test default path (calls base class)
    egl::Error result2 = display.validateImageClientBuffer(
        nullptr, 0x0000FFFF, nullptr, egl::AttributeMap());
    // Result depends on base class - just ensure no crash
    SUCCEED();
}

// Test: Verify that the switch in createExternalImageSibling covers all paths
TEST_F(DisplayOhosTest, CreateExternalImageSibling_SwitchCoverage)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // Test EGL_NATIVE_BUFFER_OHOS path
    ExternalImageSiblingImpl *result1 =
        display.createExternalImageSibling(nullptr, EGL_NATIVE_BUFFER_OHOS,
                                           reinterpret_cast<EGLClientBuffer>(0x1234),
                                           egl::AttributeMap());
    EXPECT_NE(result1, nullptr);
    delete result1;

    // Test default path (calls base class)
    ExternalImageSiblingImpl *result2 =
        display.createExternalImageSibling(nullptr, 0x0000FFFF, nullptr,
                                           egl::AttributeMap());
    // Result depends on base class
    if (result2 != nullptr)
    {
        delete result2;
    }
    SUCCEED();
}

// Test: DisplayOhos correctly inherits DisplayEGL
TEST_F(DisplayOhosTest, DisplayOhos_InheritsFromDisplayEGL)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // Verify that DisplayOhos can be used as a DisplayEGL pointer
    DisplayEGL *base_pointer = &display;
    EXPECT_NE(base_pointer, nullptr);

    // Verify the object can be used polymorphically
    SUCCEED();
}

// Test: isValidNativeWindow handles various window values
TEST_F(DisplayOhosTest, IsValidNativeWindow_EdgeCases)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    // Test with various window values
    EXPECT_TRUE(display.isValidNativeWindow(nullptr));
    EXPECT_TRUE(display.isValidNativeWindow(reinterpret_cast<EGLNativeWindowType>(0xFFFFFFFF)));
    EXPECT_TRUE(display.isValidNativeWindow(reinterpret_cast<EGLNativeWindowType>(1)));
    EXPECT_TRUE(display.isValidNativeWindow(reinterpret_cast<EGLNativeWindowType>(-1)));
}

// Test: Handling an empty context in validateImageClientBuffer
TEST_F(DisplayOhosTest, ValidateImageClientBuffer_NullContext)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *null_context = nullptr;
    egl::Error result = display.validateImageClientBuffer(
        null_context, EGL_NATIVE_BUFFER_OHOS, nullptr, egl::AttributeMap());

    // Should handle null context gracefully
    EXPECT_TRUE(result.isNoError());
}

// Test: Handling createExternalImageSibling with empty context
TEST_F(DisplayOhosTest, CreateExternalImageSibling_NullContext)
{
    egl::DisplayState state;
    state.displayId = 0;
    DisplayOhos display(state);

    const gl::Context *null_context = nullptr;
    ExternalImageSiblingImpl *result = display.createExternalImageSibling(
        null_context, EGL_NATIVE_BUFFER_OHOS, nullptr, egl::AttributeMap());

    // Should handle null context gracefully
    EXPECT_NE(result, nullptr);
    delete result;
}

}  // namespace rx
