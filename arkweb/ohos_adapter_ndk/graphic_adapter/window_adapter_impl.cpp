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

#include "window_adapter_impl.h"

#include <cstdarg>
#include <native_window/external_window.h>
#include <native_buffer/native_buffer.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"


namespace OHOS::NWeb {

WindowAdapterNdkImpl& WindowAdapterNdkImpl::GetInstance()
{
    static WindowAdapterNdkImpl instance;
    return instance;
}

NWebNativeWindow WindowAdapterNdkImpl::CreateNativeWindowFromSurface(void* pSurface)
{
    WVLOG_D("CreateNativeWindowFromSurface");
    OHNativeWindow* window = OH_NativeWindow_CreateNativeWindow(pSurface);
    if (!window) {
        return nullptr;
    }
    int32_t usage = NATIVEBUFFER_USAGE_MEM_DMA;
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_USAGE, usage);
    return reinterpret_cast<NWebNativeWindow>(window);
}

void WindowAdapterNdkImpl::DestroyNativeWindow(NWebNativeWindow window)
{
    WVLOG_D("DestroyWindow");
    OH_NativeWindow_DestroyNativeWindow(reinterpret_cast<OHNativeWindow*>(window));
}

int32_t WindowAdapterNdkImpl::NativeWindowSetBufferGeometry(NWebNativeWindow window, int32_t width, int32_t height)
{
    WVLOG_D("NativeWindowSetBufferGeometry %{public}d * %{public}d", width, height);
    return OH_NativeWindow_NativeWindowHandleOpt(reinterpret_cast<OHNativeWindow*>(window),
        SET_BUFFER_GEOMETRY, width, height);
}

void WindowAdapterNdkImpl::NativeWindowSurfaceCleanCache(NWebNativeWindow window) {}

void WindowAdapterNdkImpl::NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll)
{
    WVLOG_D("[adapter mock] WindowAdapterNdkImpl::NativeWindowSurfaceCleanCacheWithPara");
}

void WindowAdapterNdkImpl::SetTransformHint(uint32_t rotation, NWebNativeWindow window)
{
    WVLOG_D("[adapter mock] WindowAdapterNdkImpl::SetTransformHint");
}

void WindowAdapterNdkImpl::AddNativeWindowRef(NWebNativeWindow window)
{
    int32_t ret = OH_NativeWindow_NativeObjectReference(reinterpret_cast<OHNativeWindow*>(window));
    if (ret != 0) {
        WVLOG_E("add window reference failed.");
    }
}

void WindowAdapterNdkImpl::NativeWindowUnRef(NWebNativeWindow window)
{
    int32_t ret = OH_NativeWindow_NativeObjectUnreference(reinterpret_cast<OHNativeWindow*>(window));
    if (ret != 0) {
        WVLOG_E("cancel window reference failed.");
    }
}

int WindowAdapterNdkImpl::GetNativeWindowQueueSize(NWebNativeWindow window)
{
    int bufferQueueSize = 0;
    int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(reinterpret_cast<OHNativeWindow*>(window),
        GET_BUFFERQUEUE_SIZE, &bufferQueueSize);
    if (ret != 0) {
        WVLOG_E("OH_NativeWindow get buffer queue size failed.");
    }
    return bufferQueueSize;
}
} // namespace OHOS::NWeb
