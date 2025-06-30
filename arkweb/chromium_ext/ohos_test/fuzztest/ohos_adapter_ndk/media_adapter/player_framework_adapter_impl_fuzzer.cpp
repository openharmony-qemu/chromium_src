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

#include "arkweb/ohos_adapter_ndk/media_adapter/player_framework_adapter_impl.h"

#include "ohos_adapter_helper.h"
#include "player_framework_adapter_impl.h"
#include <fuzzer/FuzzedDataProvider.h>
using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class PlayerCallbackTest : public PlayerCallbackAdapter {
public:
    PlayerCallbackTest() = default;
    ~PlayerCallbackTest() override = default;
    void OnInfo(PlayerOnInfoType type, int32_t extra, int32_t value) override
    {
        infoType_ = type;
    }
    void OnError(PlayerAdapterErrorType errorType) override
    {
        errorType_ = errorType;
    }
    PlayerOnInfoType infoType_ = PlayerOnInfoType::INFO_TYPE_UNSET;
    PlayerAdapterErrorType errorType_ = PlayerAdapterErrorType::INVALID_CODE;
};

bool PlayerFrameworkAdapterImpl_SetSourceFuzzTest(FuzzedDataProvider* fdp) {
    PlayerAdapterImpl playerAdapter;
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    playerAdapter.SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapter.SetVideoSurface(std::move(surfaceAdapter));

    std::string sourceUrl = fdp->ConsumeRandomLengthString(32);
    playerAdapter.SetSource(sourceUrl);

    int32_t fd = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int32_t offset = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int32_t size = fdp->ConsumeIntegralInRange<int32_t>(offset, MAX_SET_NUMBER+1);
    playerAdapter.SetSource(fd, offset, size);
    return true;
}

bool PlayerFrameworkAdapterImpl_SetVolumeFuzzTest(FuzzedDataProvider* fdp) {
    PlayerAdapterImpl playerAdapter;
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    playerAdapter.SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapter.SetVideoSurface(std::move(surfaceAdapter));

    auto leftVolume = fdp->ConsumeFloatingPoint<float>();
    auto rightVolume = fdp->ConsumeFloatingPoint<float>();
    playerAdapter.SetVolume(leftVolume, rightVolume);
    return true;
}

bool PlayerFrameworkAdapterImpl_SeekFuzzTest(FuzzedDataProvider* fdp) {
    PlayerAdapterImpl playerAdapter;
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    playerAdapter.SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapter.SetVideoSurface(std::move(surfaceAdapter));

    int32_t rawValue = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
    auto playerseekmode = static_cast<PlayerSeekMode>(rawValue);
    int32_t seekpoint = fdp->ConsumeIntegralInRange(0, MAX_SET_NUMBER);
    playerAdapter.Seek(seekpoint, playerseekmode);
    playerAdapter.Play();
    playerAdapter.Pause();
    playerAdapter.PrepareAsync();

    int32_t currentTime = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    playerAdapter.GetCurrentTime(currentTime);

    int32_t duration = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    playerAdapter.GetDuration(duration);
    return true;
}

bool PlayerFrameworkAdapterImpl_SetPlaybackSpeedFuzzTest(FuzzedDataProvider* fdp) {
    PlayerAdapterImpl playerAdapter;
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    playerAdapter.SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapter.SetVideoSurface(std::move(surfaceAdapter));

    auto rawValue = fdp->ConsumeIntegralInRange<int32_t>(0, 4);
    auto type = static_cast<PlaybackRateMode>(rawValue);
    playerAdapter.SetPlaybackSpeed(type);
    return true;
}


bool PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(FuzzedDataProvider* fdp) {
    PlayerAdapterImpl playerAdapter;
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    playerAdapter.SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapter.SetVideoSurface(std::move(surfaceAdapter));

    std::string headerUrl = fdp->ConsumeRandomLengthString(32);
    std::map<std::string, std::string> header;
    playerAdapter.SetMediaSourceHeader(headerUrl, header);
    return true;
}


} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    OHOS::PlayerFrameworkAdapterImpl_SetSourceFuzzTest(&fdp);
    OHOS::PlayerFrameworkAdapterImpl_SetVolumeFuzzTest(&fdp);
    OHOS::PlayerFrameworkAdapterImpl_SeekFuzzTest(&fdp);
    OHOS::PlayerFrameworkAdapterImpl_SetPlaybackSpeedFuzzTest(&fdp);
    OHOS::PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(&fdp);

    return 0;
}