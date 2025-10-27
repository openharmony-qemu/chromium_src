/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MEDIA_AVSESSION_ADAPTER_IMPL_H
#define MEDIA_AVSESSION_ADAPTER_IMPL_H

#include <algorithm>
#include <deque>
#include <future>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>

#include <unordered_map>
#include <multimedia/av_session/native_avmetadata.h>
#include <multimedia/av_session/native_avsession.h>
#include <multimedia/av_session/native_avsession_errors.h>

#include "media_avsession_adapter.h"
#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"

namespace OHOS::NWeb {

class MediaAVSessionKey {
public:
    MediaAVSessionKey() = default;
    ~MediaAVSessionKey() = default;

    void Init();
    int32_t GetPID();
    std::string &GetBundleName();
    std::string &GetAbilityName();
    void SetType(MediaAVSessionType type);
    MediaAVSessionType GetType();
    std::string ToString();

private:
    int32_t pid_;
    std::string bundleName_;
    std::string abilityName_;
    MediaAVSessionType type_;
};

class MediaAVSessionAdapterImpl : public MediaAVSessionAdapter, public std::enable_shared_from_this<MediaAVSessionAdapterImpl> {
public:
    MediaAVSessionAdapterImpl();
    ~MediaAVSessionAdapterImpl() override;

    bool CreateAVSession(MediaAVSessionType type) override;
    void DestroyAVSession() override;
    bool RegistCallback(std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) override;
    bool IsActivated() override;
    bool Activate() override;
    void DeActivate() override;
    void SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) override;
    void SetPlaybackState(MediaAVSessionPlayState state) override;
    void SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) override;

private:
    static AVSessionCallback_Result AVSessionOnCommandCallback(OH_AVSession *session,
        AVSession_ControlCommand command, void *userData);
    static AVSessionCallback_Result AVSessionOnFastForwardCallback(OH_AVSession *session,
        uint32_t seekTime, void *userData);
    static AVSessionCallback_Result AVSessionOnRewindCallback(OH_AVSession *session,
        uint32_t seekTime, void *userData);
    static AVSessionCallback_Result AVSessionOnSeekCallback(OH_AVSession *session,
        uint64_t seekTime, void *userData);
    static AVSessionCallback_Result AVSessionOnSetSpeedCallback(OH_AVSession *session,
        uint32_t speed, void *userData);
    static AVSessionCallback_Result AVSessionOnSetLoopModeCallback(OH_AVSession *session,
        AVSession_LoopMode curLoopMode, void *userData);
    static AVSessionCallback_Result AVSessionOnToggleFavoriteCallback(OH_AVSession *session,
        const char *assertId, void *userData);
    static AVSessionCallback_Result AVSessionOnPlayFromAssertIdCallback(OH_AVSession *session,
        const char *assertId, void *userData);

    AVMetadata_Result UpdateAVMetadata();
    bool UpdateMetaData(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata);
    bool UpdateDuration(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    bool UpdatePlaybackState(MediaAVSessionPlayState state);
    bool UpdatePlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    bool IsUrlInQueue(const std::string& url);
    void AddUrl(const std::string& url);
    void ProcessPosterQueue();
    bool StartAsyncPosterUpdate();
    void DestroyAndEraseSession();
    bool CreateNewSession(const MediaAVSessionType& type);
    void InitMediaAVSessionAdapterImpl();

    std::shared_ptr<MediaAVSessionKey> avSessionKey_ = nullptr;
    AVSession_PlaybackState avPlaybackState_;
    OH_AVMetadataBuilder *builder_ = nullptr;
    OH_AVMetadata *avMetadata_ = nullptr;
    OH_AVSession *avSession_ = nullptr;
    bool isActived_ = false;

    static std::unordered_map<std::string, MediaAVSessionAdapterImpl *> avSessionMap;
    size_t callback_index_ = 0;
    static CallbackSharedWrapper<MediaAVSessionCallbackAdapter> callback_wrapper_;

    std::future<void> media_futures_;
    std::string poster_url_ = "";
    std::string poster_new_ = "";
    std::string title_ = "";
    std::string artist_ = "";
    std::string album_ = "";
    int duration_ = 0;
    std::deque<std::string> url_queue_;
    std::mutex url_mutex_;
    std::mutex avsession_mutex_;
};
} // namespace OHOS::NWeb

#endif // MEDIA_AVSESSION_ADAPTER_IMPL_H