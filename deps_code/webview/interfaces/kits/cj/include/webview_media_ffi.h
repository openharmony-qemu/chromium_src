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

#ifndef WEBVIEW_MEDIA_FFI_H
#define WEBVIEW_MEDIA_FFI_H

#include "ffi_remote_data.h"
#include "webview_utils.h"
#include "cj_common_ffi.h"

extern "C" {
    struct AtCMediaPlayer {
        void (*atCOHOSNativeMediaPlayerBridgeResumePlayer)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeSuspendPlayer)(int64_t self, int32_t ctype) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeExitFullscreen)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeEnterFullscreen)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeRelease)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeSetPlaybackRate)(int64_t self, double rate) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeSetMuted)(int64_t self, bool muted) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeSetVolume)(int64_t self, double volume) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeSeek)(int64_t self, double targetTime) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgePause)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgePlay)(int64_t self) = nullptr;
        void (*atCOHOSNativeMediaPlayerBridgeUpdateRect)(int64_t self, OHOS::Webview::CRectEvent cRectEvent) = nullptr;
    };

    // NativeMediaPlayerBridge
    FFI_EXPORT void FfiOHOSNativeMediaPlayerBridgeRegisterCJFuncs(AtCMediaPlayer cjFuncs);

    // NativeMediaPlayerHandler
    FFI_EXPORT int64_t FfiOHOSNmphConstructor();
    FFI_EXPORT int32_t FfiOHOSNmphhandleVideoSizeChanged(int64_t id, double width, double height);
    FFI_EXPORT int32_t FfiOHOSNmphhandleError(int64_t id, int32_t error, const char* errorMessage);
    FFI_EXPORT int32_t FfiOHOSNmphhandleSeekFinished(int64_t id);
    FFI_EXPORT int32_t FfiOHOSNmphhandleSeeking(int64_t id);
    FFI_EXPORT int32_t FfiOHOSNmphhandleFullscreenChanged(int64_t id, bool fullscreen);
    FFI_EXPORT int32_t FfiOHOSNmphhandleReadyStateChanged(int64_t id, int32_t state);
    FFI_EXPORT int32_t FfiOHOSNmphhandleNetworkStateChanged(int64_t id, int32_t state);
    FFI_EXPORT int32_t FfiOHOSNmphhandleEnded(int64_t id);
    FFI_EXPORT int32_t FfiOHOSNmphhandleBufferedEndTimeChanged(int64_t id, double bufferedEndTime);
    FFI_EXPORT int32_t FfiOHOSNmphhandleTimeUpdate(int64_t id, double currentPlayTime);
    FFI_EXPORT int32_t FfiOHOSNmphhandleDurationChanged(int64_t id, double duration);
    FFI_EXPORT int32_t FfiOHOSNmphhandlePlaybackRateChanged(int64_t id, double playbackRate);
    FFI_EXPORT int32_t FfiOHOSNmphhandleMutedChanged(int64_t id, bool muted);
    FFI_EXPORT int32_t FfiOHOSNmphhandleVolumeChanged(int64_t id, double volume);
    FFI_EXPORT int32_t FfiOHOSNmphhandleStatusChanged(int64_t id, int32_t status);
}

#endif // WEBVIEW_MEDIA_FFI_H