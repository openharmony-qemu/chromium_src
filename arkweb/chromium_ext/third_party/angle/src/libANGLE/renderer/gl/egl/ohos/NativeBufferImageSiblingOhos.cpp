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

// NativeBufferImageSiblingOhos.cpp: Implements the NativeBufferImageSiblingOhos class

#include "NativeBufferImageSiblingOhos.h"
#include "../../../../../common/ohos_util.h"
#include "../../../../../../../../../src/third_party/ohos_ndk/includes/ohos_adapter/hilog_adapter.h"

namespace rx
{
NativeBufferImageSiblingOhos::NativeBufferImageSiblingOhos(EGLClientBuffer buffer,
                                                            const egl::AttributeMap &attribs)
    : mBuffer(buffer), mFormat(GL_NONE), mYUV(false), mColorSpace(EGL_GL_COLORSPACE_LINEAR_KHR) {
        if (attribs.contains(EGL_GL_COLORSPACE_KHR)) {
            mColorSpace = attribs.getAsInt(EGL_GL_COLORSPACE_KHR);
        }
}

NativeBufferImageSiblingOhos::~NativeBufferImageSiblingOhos() {}

egl::Error NativeBufferImageSiblingOhos::initialize(const egl::Display *display) {
    int pixelFormat = 0;
    int usage  = 0;
    angle::ohos::GetOHNativeBufferProperties(
        angle::ohos::ClientBufferToOHNativeBuffer(mBuffer), &mSize.width, &mSize.height,
        &mSize.depth, &pixelFormat, &usage);

    mFormat = gl::Format(angle::ohos::NativePixelFormatToGLInternalFormat(pixelFormat));
    mYUV    = angle::ohos::NativePixelFormatIsYUV(pixelFormat);
    mHasProtectedContent = false;

    return egl::NoError();
}

gl::Format NativeBufferImageSiblingOhos::getFormat() const {
    return mFormat;
}

bool NativeBufferImageSiblingOhos::isRenderable(const gl::Context *context) const {
    return true;
}

bool NativeBufferImageSiblingOhos::isTexturable(const gl::Context *context) const {
    return true;
}

bool NativeBufferImageSiblingOhos::isYUV() const {
    return mYUV;
}

bool NativeBufferImageSiblingOhos::hasProtectedContent() const {
    return mHasProtectedContent;
}

gl::Extents NativeBufferImageSiblingOhos::getSize() const {
    return mSize;
}

size_t NativeBufferImageSiblingOhos::getSamples() const {
    return 0;
}

EGLClientBuffer NativeBufferImageSiblingOhos::getBuffer() const {
    return mBuffer;
}

void NativeBufferImageSiblingOhos::getImageCreationAttributes(
    std::vector<EGLint> *outAttributes) const {
    EGLenum kForwardedAttribs[] = {EGL_WIDTH,
                                   EGL_HEIGHT,
                                   EGL_PROTECTED_CONTENT_EXT,
                                   EGL_YUV_COLOR_SPACE_HINT_EXT,
                                   EGL_SAMPLE_RANGE_HINT_EXT,
                                   EGL_YUV_CHROMA_HORIZONTAL_SITING_HINT_EXT,
                                   EGL_YUV_CHROMA_VERTICAL_SITING_HINT_EXT};

    for (EGLenum forwardedAttrib : kForwardedAttribs) {
        if (mAttribs.contains(forwardedAttrib)) {
            outAttributes->push_back(forwardedAttrib);
            outAttributes->push_back(mAttribs.getAsInt(forwardedAttrib));
        }
    }
}

}  // namespace rx
