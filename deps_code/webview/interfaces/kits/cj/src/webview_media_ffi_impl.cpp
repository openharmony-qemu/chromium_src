/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webview_media_ffi.h"

#include "webview_utils.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "webview_log.h"
#include "parameters.h"
#include "cj_lambda.h"
#include "webview_utils.h"
#include "web_native_media_player.h"
#include "web_runtime_delegate.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
    // NativeMediaPlayerBridge
    void FfiOHOSNativeMediaPlayerBridgeRegisterCJFuncs(AtCMediaPlayer cjFuncs)
    {
        bool result = WebRuntimeDelegate::GetInstance().RegisterCJFuncs(cjFuncs);
        if (!result) {
            WEBVIEWLOGE("AtCMediaPlayer register failed");
        }
    }

    // NativeMediaPlayerHandler
    int64_t FfiOHOSNmphConstructor()
    {
        auto Nmph = FFIData::Create<NativeMediaPlayerHandlerImpl>();
        if (Nmph == nullptr) {
            return -1;
        }
        return Nmph->GetID();
    }

    int32_t FfiOHOSNmphhandleVideoSizeChanged(int64_t id, double width, double height)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleVideoSizeChanged(width, height);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleError(int64_t id, int32_t error, const char* errorMessage)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleError(static_cast<NWeb::MediaError>(error), errorMessage);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleSeekFinished(int64_t id)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleSeekFinished();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleSeeking(int64_t id)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleSeeking();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleFullscreenChanged(int64_t id, bool fullscreen)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleFullScreenChanged(fullscreen);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleReadyStateChanged(int64_t id, int32_t state)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleReadyStateChanged(static_cast<NWeb::ReadyState>(state));
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleNetworkStateChanged(int64_t id, int32_t state)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleNetworkStateChanged(static_cast<NWeb::NetworkState>(state));
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleEnded(int64_t id)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleEnded();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleBufferedEndTimeChanged(int64_t id, double bufferedEndTime)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleBufferedEndTimeChanged(bufferedEndTime);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleTimeUpdate(int64_t id, double currentPlayTime)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleTimeUpdate(currentPlayTime);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleDurationChanged(int64_t id, double duration)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleDurationChanged(duration);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandlePlaybackRateChanged(int64_t id, double playbackRate)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandlePlaybackRateChanged(playbackRate);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleMutedChanged(int64_t id, bool muted)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleMutedChanged(muted);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleVolumeChanged(int64_t id, double volume)
    {
        auto Nmph = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (Nmph == nullptr) {
            return NWebError::INIT_ERROR;
        }
        Nmph->HandleVolumeChanged(volume);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSNmphhandleStatusChanged(int64_t id, int32_t status)
    {
        if (status < static_cast<int32_t>(NWeb::PlaybackStatus::PAUSED)
        || status > static_cast<int32_t>(NWeb::PlaybackStatus::PLAYING)) {
            return NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
        }

        auto nativeMediaPlayerHandlerImpl = FFIData::GetData<NativeMediaPlayerHandlerImpl>(id);
        if (nativeMediaPlayerHandlerImpl == nullptr) {
            return NWebError::INIT_ERROR;
        }
        nativeMediaPlayerHandlerImpl->HandleStatusChanged(static_cast<NWeb::PlaybackStatus>(status));
        return NWebError::NO_ERROR;
    }
}
}
}