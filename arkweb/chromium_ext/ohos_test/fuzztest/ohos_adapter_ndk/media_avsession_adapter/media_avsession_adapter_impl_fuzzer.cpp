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

#include <fuzzer/FuzzedDataProvider.h>

#include <cstdlib>
#include <ctime>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_adapter_helper.h"
#define private public
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"
#undef private
using namespace OHOS::NWeb;

namespace OHOS {

constexpr int32_t kFuzzMaxOriginalStreamNums = 2048;
static int64_t kFuzzMaxOriginalStream = 0;
static int64_t kFuzzMaxOriginalStreamNull = 0;
static int64_t kUpdateTime = 1776087281;

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() override {}
    void Pause() override {}
    void Stop() override {}
    void SeekTo(int64_t millisTime) override {}
    int32_t GetMediaCastCurrentTime() override { return 0; }
    void PullUpCastBackGround(const std::string& device_name) override {}
    void SetAvCast(bool is_avcast) override {}
    void UpdateUiPlayState(bool is_playing) override {}
    void UpdateUiPlayPosition(int64_t position) override {}
    void MediaCastStopped() override {}
    void NotifyCastControlShow(bool is_show) override {}
};

class MediaAVSessionKeyMock : public MediaAVSessionKey {
public:
    std::string bundleName = "test1";
    std::string abilityName = "testAbility";

    std::string &GetBundleName() {
        return bundleName;
    }

    std::string &GetAbilityName() {
        return abilityName;
    }
};

class MediaAVSessionMetadataAdapterMock : public MediaAVSessionMetadataAdapter {
public:
    MediaAVSessionMetadataAdapterMock() = default;
    void SetTitle(const std::string& title) override {}

    std::string GetTitle() override { return "test1"; }

    void SetArtist(const std::string& artist) override {}

    std::string GetArtist() override { return "test2"; }

    void SetAlbum(const std::string& album) override {}

    std::string GetAlbum() override { return "test3"; }

    void SetImageUrl(const std::string& imageUrl) override {}

    std::string GetImageUrl() override { return "test4"; }
};

class MediaAVSessionPositionAdapterMock : public MediaAVSessionPositionAdapter {
public:
    MediaAVSessionPositionAdapterMock() = default;

    void SetDuration(int64_t duration) override {}

    int64_t GetDuration() override { return 0; }

    void SetElapsedTime(int64_t elapsedTime) override {}

    int64_t GetElapsedTime() override { return 0; }

    void SetUpdateTime(int64_t updateTime) override {}

    int64_t GetUpdateTime() override { return 0; }
};

bool MediaAVSessionAdapterImplFuzzTest(FuzzedDataProvider* fdp)
{
    std::shared_ptr<MediaAVSessionMetadataAdapterMock> metadataadapter =
        std::make_shared<MediaAVSessionMetadataAdapterMock>();
    std::shared_ptr<MediaAVSessionPositionAdapterMock> pointeradapter =
        std::make_shared<MediaAVSessionPositionAdapterMock>();
    std::shared_ptr<MediaAVSessionAdapterImpl> avSessionAdapter = std::make_shared<MediaAVSessionAdapterImpl>();
    std::shared_ptr<MediaAVSessionKey> key = std::make_shared<MediaAVSessionKey>();
    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    if (kFuzzMaxOriginalStream <= kFuzzMaxOriginalStreamNums) {
        kFuzzMaxOriginalStream++;
        OH_AVSession_Create(SESSION_TYPE_AUDIO, "OH_AVSession_Create_001",
            "com.xxx.hmxx", "ndkxx", &(avSessionAdapter->avSession_));
    } else if (kFuzzMaxOriginalStreamNull <= kFuzzMaxOriginalStreamNums) {
        kFuzzMaxOriginalStreamNull++;
        avSessionAdapter->CreateAVSession(type);
        type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
        avSessionAdapter->CreateAVSession(type);

    } else {
        return false;
    }

    key->Init();
    key->GetPID();
    key->GetType();
    key->ToString();
    key->SetType(MediaAVSessionType::MEDIA_TYPE_INVALID);

    auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    avSessionAdapter->RegistCallback(mediaAVSessionCallbackAdapterMock);

    auto avSessionKey = std::make_shared<MediaAVSessionKeyMock>();
        avSessionAdapter->avSessionKey_ = avSessionKey;
    avSessionAdapter->avSessionKey_->Init();

    int32_t rawValue = fdp->ConsumeIntegralInRange<int32_t>(-1, 1);
    type = static_cast<MediaAVSessionType>(rawValue);
    avSessionAdapter->CreateAVSession(type);
    avSessionAdapter->IsActivated();
    avSessionAdapter->Activate();
    avSessionAdapter->SetMetadata(metadataadapter);
    rawValue = fdp->ConsumeIntegralInRange<int64_t>(0, 3);
    auto playState = static_cast<MediaAVSessionPlayState>(rawValue);
    avSessionAdapter->SetPlaybackState(playState);

    avSessionAdapter->SetPlaybackPosition(pointeradapter);
    avSessionAdapter->UpdateMetaData(metadataadapter);
    avSessionAdapter->UpdateDuration(pointeradapter);
    avSessionAdapter->UpdatePlaybackState(playState);
    avSessionAdapter->UpdateAVMetadata();
    avSessionAdapter->DeActivate();
    avSessionAdapter->DestroyAVSession();
    avSessionAdapter->DestroyAndEraseSession();

    std::string testUri = "http://test.com/media.mp4";
    avSessionAdapter->SetMediaCastUri(testUri);

    bool enabled = fdp->ConsumeBool();
    avSessionAdapter->SetRemoteCastEnabled(enabled);

    avSessionAdapter->PrepareMediaCastDescription();

    avSessionAdapter->GetMediaCastCurrentTime();

    avSessionAdapter->HandleStopMediaCast();

    bool isPlaying = fdp->ConsumeBool();
    avSessionAdapter->UpdateRemotePlayState(isPlaying);

    int64_t position = fdp->ConsumeIntegral<int64_t>();
    avSessionAdapter->UpdateRemotePlayPosition(position);
    avSessionAdapter->GetAVCastController();

    MediaCastDescription mediaCastDescription;
    mediaCastDescription.assetId = "test_asset_id";
    mediaCastDescription.title = "Test Title";
    mediaCastDescription.mediaUri = "http://test.com/test.mp4";
    mediaCastDescription.mediaType = "VIDEO";
    mediaCastDescription.duration = 1000;
    mediaCastDescription.startPosition = 0;
    mediaCastDescription.albumUrl = "http://test.com/poster.jpg";
    avSessionAdapter->Prepare(mediaCastDescription);

    avSessionAdapter->PlayRemote();
    avSessionAdapter->PauseRemote();
    avSessionAdapter->SetPlaybackPositionRemote(static_cast<int32_t>(position));

    avSessionAdapter->UpdateRemotePlayStateCast(isPlaying);
    avSessionAdapter->UpdateRemotePlayPositionCast(position);

    avSessionAdapter->GetAVCastPlaybackState();

    avSessionAdapter->IsAvCastPlaying();

    avSessionAdapter->GetPlaybackPosition();

    AVSession_PlaybackState avSessionPlaybackState = PLAYBACK_STATE_PLAYING;
    avSessionAdapter->SetAVCastUiPlayState(avSessionPlaybackState);

    AVSession_PlaybackPosition playbackPosition;
    playbackPosition.elapsedTime = kUpdateTime;
    playbackPosition.updateTime = kUpdateTime;
    avSessionAdapter->SetAVCastUiPlayPosition(playbackPosition);

    avSessionAdapter->SetAVCastUilastUiTime(position);

    bool isSeeking = fdp->ConsumeBool();
    avSessionAdapter->SetAVCastUiSeeking(isSeeking);

    avSessionAdapter->GetAVCastUiPlayState();

    avSessionAdapter->GetAVCastUiPlayPosition();

    avSessionAdapter->GetAVCastUilastUiTime();

    avSessionAdapter->GetAVCastUiSeeking();

    avSessionAdapter->UpdateUiPlayPosition(avSessionAdapter, position, isSeeking);

    avSessionAdapter->SetAVCastDevice("TestDevice");
    avSessionAdapter->GetAVCastDevice();

    avSessionAdapter->SetAvCast(true);
    avSessionAdapter->SetAvCast(false);

    avSessionAdapter->UpdateUiPlayState(isPlaying);
    avSessionAdapter->UpdateUiPlayPosition(position);

    avSessionAdapter->UpdateUiPlayStateByClient(avSessionPlaybackState);
    avSessionAdapter->SetUiPlayStateByClient(avSessionPlaybackState);
    avSessionAdapter->SetUiPlayPositionByClient(playbackPosition);

    avSessionAdapter->SetUilastUiTimeByClient(position);
    avSessionAdapter->SetUiSeekingByClient(isSeeking);

    avSessionAdapter->GetUiPlayStateByClient();
    avSessionAdapter->GetUilastUiTimeByClient();
    avSessionAdapter->GetUiSeekingByClient();

    avSessionAdapter->RegisterCallback();
    avSessionAdapter->UnregisterCallback();

    avSessionAdapter->UnregisterMediaCastOutputDeviceCallback();

    avSessionAdapter->SeekNative(position);
    avSessionAdapter->PlayNative();
    avSessionAdapter->PauseNative();

    avSessionAdapter->MediaCastStopped();

    avSessionAdapter->PullUpCastBackGround();

    avSessionAdapter->PrepareAndStartCast();

    avSessionAdapter->StartCast();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::MediaAVSessionAdapterImplFuzzTest(&fdp);
    return 0;
}