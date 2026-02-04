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

// DisplayOhos.cpp: OHOS implementation of egl::Display

#include "DisplayOhos.h"

#include "../../../../../../../../src/third_party/angle/src/libANGLE/Display.h"
#include "NativeBufferImageSiblingOhos.h"

namespace rx
{

DisplayOhos::DisplayOhos(const egl::DisplayState &state) : DisplayEGL(state) {}

DisplayOhos::~DisplayOhos() {}

bool DisplayOhos::isValidNativeWindow(EGLNativeWindowType window) const {
    return true; // TODO
}

egl::Error DisplayOhos::validateImageClientBuffer(const gl::Context *context,
                                                     EGLenum target,
                                                     EGLClientBuffer clientBuffer,
                                                     const egl::AttributeMap &attribs) const {
    switch (target) {
        case EGL_NATIVE_BUFFER_OHOS:
            return egl::NoError();

        default:
            return DisplayEGL::validateImageClientBuffer(context, target, clientBuffer, attribs);
    }
}

ExternalImageSiblingImpl *DisplayOhos::createExternalImageSibling(
    const gl::Context *context,
    EGLenum target,
    EGLClientBuffer buffer,
    const egl::AttributeMap &attribs) {
    switch (target) {
        case EGL_NATIVE_BUFFER_OHOS:
            return new NativeBufferImageSiblingOhos(buffer, attribs);

        default:
            return DisplayEGL::createExternalImageSibling(context, target, buffer, attribs);
    }
}

}  // namespace rx
