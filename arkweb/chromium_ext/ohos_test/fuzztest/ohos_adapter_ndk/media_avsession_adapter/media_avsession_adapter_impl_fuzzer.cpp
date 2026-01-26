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

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() {};
    void Pause() {};
    void Stop() {};
    void SeekTo(int64_t millisTime) {};
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
    void SetTitle(const std::string& title) {}

    std::string GetTitle() { return "test1"; }

    void SetArtist(const std::string& artist) {}

    std::string GetArtist() { return "test2"; }

    void SetAlbum(const std::string& album) {}

    std::string GetAlbum() { return "test3"; }

    void SetImageUrl(const std::string& imageUrl) {}

    std::string GetImageUrl() { return "test4"; }
};

class MediaAVSessionPositionAdapterMock : public MediaAVSessionPositionAdapter {
public:
    MediaAVSessionPositionAdapterMock() = default;

    void SetDuration(int64_t duration) {}

    int64_t GetDuration() { return 20; }

    void SetElapsedTime(int64_t elapsedTime) {}

    int64_t GetElapsedTime() { return 10; }

    void SetUpdateTime(int64_t updateTime) {}

    int64_t GetUpdateTime() { return 10; }
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