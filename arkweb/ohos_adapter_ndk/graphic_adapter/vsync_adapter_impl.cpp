/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "vsync_adapter_impl.h"

#include "nweb_log.h"
#include <deviceinfo.h>
#include <unistd.h>
#include <native_vsync/graphic_error_code.h>

namespace OHOS::NWeb {
const int MAX_FRAME_RATE = 120;
const int MIN_FRAME_RATE = 60;
const int DISABLE_FRAME_RATE = 0;
const int FRAME_RATE_VERSION = 20;

namespace {
const std::string THREAD_NAME = "VSync-webview";
}

void (*VSyncAdapterNdkImpl::callback_)() = nullptr;

VSyncAdapterNdkImpl::~VSyncAdapterNdkImpl()
{
    OH_NativeVSync_Destroy(vsyncReceiver_);
}

VSyncAdapterNdkImpl& VSyncAdapterNdkImpl::GetInstance()
{
    static VSyncAdapterNdkImpl instance;
    return instance;
}

VSyncErrorCode VSyncAdapterNdkImpl::Init()
{
    if (!vsyncReceiver_) {
        const std::string vsyncName = "NWeb_" + std::to_string(getpid());
        vsyncReceiver_ = OH_NativeVSync_Create(vsyncName.c_str(), vsyncName.length());
        if (!vsyncReceiver_) {
            WVLOG_E("CreateVSyncReceiver failed");
            return VSyncErrorCode::ERROR;
        }
    }
    return VSyncErrorCode::SUCCESS;
}

VSyncErrorCode VSyncAdapterNdkImpl::RequestVsync(void* data, NWebVSyncCb cb)
{
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdapter init fail");
        return VSyncErrorCode::ERROR;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    vsyncCallbacks_.insert({data, cb});

    if (hasRequestedVsync_) {
        return VSyncErrorCode::SUCCESS;
    }

    auto ret = OH_NativeVSync_RequestFrame(vsyncReceiver_, frameCallback_, this);
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter RequestNextVSync fail, ret=%{public}d", ret);
        return VSyncErrorCode::ERROR;
    }
    hasRequestedVsync_ = true;
    return VSyncErrorCode::SUCCESS;
}

void VSyncAdapterNdkImpl::OnVsync(long long timestamp, void* client)
{
    auto vsyncClient = static_cast<VSyncAdapterNdkImpl*>(client);
    if (vsyncClient) {
        vsyncClient->VsyncCallbackInner(timestamp);
    } else {
        WVLOG_E("VsyncClient is null");
    }
}

void VSyncAdapterNdkImpl::VsyncCallbackInner(long long timestamp)
{
    std::unordered_map<void*, NWebVSyncCb> vsyncCallbacks;
    std::lock_guard<std::mutex> lock(mtx_);
    if (callback_) {
        callback_();
    }
    vsyncCallbacks = vsyncCallbacks_;
    vsyncCallbacks_.clear();

    for (const auto& callback : vsyncCallbacks) {
        auto func = callback.second;
        if (func) {
            func(timestamp, callback.first);
        }
    }
    hasRequestedVsync_ = false;
}

int64_t VSyncAdapterNdkImpl::GetVSyncPeriod()
{
    int64_t period = 0;
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdapter init fail");
        return period;
    }

    auto ret = OH_NativeVSync_GetPeriod(vsyncReceiver_, reinterpret_cast<long long *>(&period));
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter GetVSyncPeriod fail, ret=%{public}d", ret);
    }
    return period;
}

void VSyncAdapterNdkImpl::SetFrameRateLinkerEnable(bool enabled)
{
    if (enabled) {
        return;
    }

    if (OH_GetSdkApiVersion() < FRAME_RATE_VERSION) {
        WVLOG_I("The current API version does not support setting dynamic frame rate.");
        return;
    }

    OH_NativeVSync_ExpectedRateRange range = {DISABLE_FRAME_RATE, MAX_FRAME_RATE, DISABLE_FRAME_RATE};
    int ret = OH_NativeVSync_SetExpectedFrameRateRange(vsyncReceiver_, &range);
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter set rate fail, ret=%{public}d", ret);
    }
}

void VSyncAdapterNdkImpl::SetFramePreferredRate(int32_t preferredRate)
{
    if (preferredRate < MIN_FRAME_RATE || preferredRate > MAX_FRAME_RATE) {
        WVLOG_E("Param error, rate=%{public}d", preferredRate);
        return;
    }

    if (OH_GetSdkApiVersion() < FRAME_RATE_VERSION) {
        WVLOG_I("The current API version does not support setting dynamic frame rate.");
        return;
    }

    OH_NativeVSync_ExpectedRateRange range = {MIN_FRAME_RATE, MAX_FRAME_RATE, preferredRate};
    int ret = OH_NativeVSync_SetExpectedFrameRateRange(vsyncReceiver_, &range);
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter set rate fail, ret=%{public}d", ret);
    }
}

void VSyncAdapterNdkImpl::SetOnVsyncCallback(void (*callback)())
{
    std::lock_guard<std::mutex> lock(mtx_);
    callback_ = callback;
}

void VSyncAdapterNdkImpl::SetIsGPUProcess(bool isGPU)
{
    isGPUProcess_ = isGPU;
}

void VSyncAdapterNdkImpl::SetOnVsyncEndCallback(void (*onVsyncEndCallback)())
{
}

void VSyncAdapterNdkImpl::SetScene(const std::string& sceneName, uint32_t state)
{
    WVLOG_D("[adapter mock] SetScene");
}

void VSyncAdapterNdkImpl::SetDVSyncSwitch(bool dvsyncSwitch)
{
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdatrper init fail!");
        return;
    }

    if (!vsyncReceiver_) {
        WVLOG_E("NWebWindowAdatrper SetDVSyncSwitch: receiver_ is nullptr!");
        return;
    }

    WVLOG_D("NWebWindowAdatrper SetDVSyncSwitch: dvsyncSwitch = %{public}d", dvsyncSwitch);
    int ret = OH_NativeVSync_DVSyncSwitch(vsyncReceiver_, dvsyncSwitch);
    if (ret != 0) {
        WVLOG_E("SetNativeDVSyncSwitch failed, ret = %{public}d", ret);
        return;
    }
}

} // namespace OHOS::NWeb
