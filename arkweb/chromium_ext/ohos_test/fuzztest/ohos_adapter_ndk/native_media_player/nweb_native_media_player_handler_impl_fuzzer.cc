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

#include "arkweb/ohos_nweb/src/native_media_player/nweb_native_media_player_handler_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class CefMediaPlayerListenerMock : public CefMediaPlayerListener {
public:
    CefMediaPlayerListenerMock() = default;
    void OnStatusChanged(uint32_t status) {}

    void OnVolumeChanged(double volume) {}

    void OnMutedChanged(bool muted) {}

    void OnPlaybackRateChanged(double playback_rate) {}

    void OnDurationChanged(double duration) {}

    void OnTimeUpdate(double current_time) {}

    void OnBufferedEndTimeChanged(double buffered_time) {}

    void OnEnded() {}

    void OnNetworkStateChanged(uint32_t state) {}

    void OnReadyStateChanged(uint32_t state) {}

    void OnFullscreenChanged(bool fullscreen) {}

    void OnSeeking() {}

    void OnSeekFinished() {}

    void OnError(uint32_t error_code, const CefString& error_msg) {}

    void OnVideoSizeChanged(int width, int height) {}

    void AddRef() const {}

    bool Release() const { return true; }

    bool HasOneRef() const { return true; }

    bool HasAtLeastOneRef() const { return true; }
};

bool PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(FuzzedDataProvider* fdp) {
    CefOwnPtr<CefMediaPlayerListener> listener(
        new CefMediaPlayerListenerMock()); 
    std::shared_ptr<NWebNativeMediaPlayerHandlerImpl> adapter =
        std::make_shared<NWebNativeMediaPlayerHandlerImpl>(std::move(listener));

    auto raw = fdp->ConsumeIntegralInRange<int32_t>(0, 1);
    PlaybackStatus status = static_cast<PlaybackStatus>(raw);
    adapter->HandleStatusChanged(status);

    double volume = fdp->ConsumeFloatingPoint<double>();
    adapter->HandleVolumeChanged(volume);

    auto randomBool = fdp->ConsumeBool();
    adapter->HandleMutedChanged(randomBool);

    double playback_rate = fdp->ConsumeFloatingPoint<double>();
    adapter->HandlePlaybackRateChanged(playback_rate);

    double duration = fdp->ConsumeFloatingPoint<double>();
    adapter->HandleDurationChanged(duration);
    adapter->HandleTimeUpdate(duration);
    adapter->HandleBufferedEndTimeChanged(duration);
    adapter->HandleEnded();

    adapter->HandleFullScreenChanged(randomBool);
    adapter->HandleSeeking();
    adapter->HandleSeekFinished();

    double width = fdp->ConsumeFloatingPoint<double>();
    double height = fdp->ConsumeFloatingPoint<double>();
    adapter->HandleVideoSizeChanged(width, height);

    return true;
}


} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    OHOS::PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(&fdp);

    return 0;
}