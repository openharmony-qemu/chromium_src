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
#include <multimedia/av_session/native_avqueueitem.h>
#include <multimedia/av_session/native_avcastcontroller.h>
#include <multimedia/av_session/native_avplaybackstate.h>
#include <multimedia/av_session/native_deviceinfo.h>

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

struct MediaCastDescription {
    std::string assetId;
    std::string title;
    std::string subtitle;
    std::string artist;
    const char* mediaType = nullptr;
    std::string lyricContent;
    int32_t duration = 0;
    std::string mediaUri;
    int32_t startPosition = 0;
    int32_t mediaSize = 0;
    std::string albumTitle;
    std::string appName;
};

class MediaAVSessionAdapterImpl
    : public MediaAVSessionAdapter,
      public std::enable_shared_from_this<MediaAVSessionAdapterImpl> {
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
    void SetMediaCastUri(const std::string& mediaUri) override;
    void SetRemoteCastEnabled(bool enabled) override;
    void PrepareMediaCastDescription() override;
    void HandleStopMediaCast() override;
    void UpdateRemotePlayState(bool is_playing) override;
    void UpdateRemotePlayPosition(int64_t position) override;

    bool GetAVCastController();
    bool Prepare(const MediaCastDescription& mediaCastDescription);
    bool StartCast();
    bool RegisterCallback();
    bool UnregisterCallback();
    void PlayRemote();
    void PauseRemote();
    bool IsAvCastPlaying();
    int64_t GetPlaybackPosition();
    void SetPlaybackPositionRemote(const int32_t millis);
    void UpdateRemotePlayStateCast(bool is_playing);    
    void UpdateRemotePlayPositionCast(int64_t position);
    AVSession_PlaybackState GetAVCastPlaybackState();
    void SetAVCastUiPlayState(AVSession_PlaybackState& avSessionPlaybackState);
    void SetAVCastUiPlayPosition(AVSession_PlaybackPosition& playbackPosition);
    void SetAVCastUilastUiTime(int64_t position);
    void SetAVCastUiSeeking(bool is_seeking);
    AVSession_PlaybackState GetAVCastUiPlayState();
    AVSession_PlaybackPosition GetAVCastUiPlayPosition();
    int64_t GetAVCastUilastUiTime();
    bool GetAVCastUiSeeking();
    static void UpdateUiPlayPosition(std::shared_ptr<MediaAVSessionAdapterImpl> adapter,
        int64_t position, bool is_seek);
    static AVSessionCallback_Result PlaybackStateChangedCallback(OH_AVCastController* avcastcontroller,
        OH_AVSession_AVPlaybackState* playbackState, void* userData);
    static AVSessionCallback_Result MediaItemChangeCallback(OH_AVCastController* avcastcontroller,
        OH_AVSession_AVQueueItem* avQueueItem, void* userData);
    static AVSessionCallback_Result SeekDoneCallback(OH_AVCastController* avcastcontroller,
        int32_t position, void* userData);
    static AVSessionCallback_Result EndOfStreamCallback(OH_AVCastController* avcastcontroller,
        void* userData);
    static AVSessionCallback_Result ErrorCallback(OH_AVCastController* avcastcontroller,
        void* userData, AVSession_ErrCode error);

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
    static AVSessionCallback_Result OutputDeviceChangeCallback(OH_AVSession* session, 
        AVSession_ConnectionState state, AVSession_OutputDeviceInfo *outputDeviceInfo);
    static void AVCastStateConnect(OH_AVSession *session, AVSession_OutputDeviceInfo *outputDeviceInfo);
    static void AVCastStateDisconnect(OH_AVSession *session);

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
    void RegistAVSessionCallbackOutputDeviceChange();
    bool PrepareAndStartCast();
    int32_t GetMediaCastCurrentTime();
    void PullUpCastBackGround();
    void SetAvCast(bool is_avcast);
    void UpdateUiPlayState(bool is_playing);
    void UpdateUiPlayPosition(int64_t position);
    void UpdateUiPlayStateByClient(AVSession_PlaybackState& avSessionPlaybackState);
    void SetUiPlayStateByClient(AVSession_PlaybackState& avSessionPlaybackState);
    void SetUiPlayPositionByClient(AVSession_PlaybackPosition& playbackPosition);
    void SetUilastUiTimeByClient(int64_t position);
    void SetUiSeekingByClient(bool is_seeking);

    AVSession_PlaybackState GetUiPlayStateByClient();
    int64_t GetUilastUiTimeByClient();
    bool GetUiSeekingByClient();

    void SetAVCastDevice(const char* deviceName);
    std::string GetAVCastDevice();
    void UpdateAVCastDevice(AVSession_OutputDeviceInfo *outputDeviceInfo);
    void MediaCastStopped();
    void PlayNative();
    void PauseNative();
    void SeekNative(const int64_t millis);
    void UnregisterMediaCastOutputDeviceCallback();

    std::shared_ptr<MediaAVSessionKey> avSessionKey_ = nullptr;
    AVSession_PlaybackState avPlaybackState_;
    OH_AVMetadataBuilder *builder_ = nullptr;
    OH_AVMetadata *avMetadata_ = nullptr;
    OH_AVSession *avSession_ = nullptr;
    bool isActived_ = false;
    MediaCastDescription MediaCastDescription_;

    static std::unordered_map<std::string, MediaAVSessionAdapterImpl *> avSessionMap;
    static std::unordered_map<OH_AVSession*, MediaAVSessionAdapterImpl *> avSessionMapOther_;
    size_t callback_index_ = 0;
    static CallbackSharedWrapper<MediaAVSessionCallbackAdapter> callback_wrapper_;
    size_t avsession_callback_index_ = 0;
    static CallbackSharedWrapper<MediaAVSessionAdapterImpl> avsession_callback_wrapper_;

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
    std::string media_uri_storage_;
    std::string pid_avsession_;
    std::string deviceName_;
    OH_AVCastController* avCastController_ = nullptr;
    OH_AVSession_AVMediaDescriptionBuilder* avMediaDescriptionBuilder_ = nullptr;
    OH_AVSession_AVMediaDescription* avMediaDescription_ = nullptr;
    OH_AVSession_AVQueueItem avQueueItem_;
    AVSession_PlaybackState playbackState_;
    AVSession_PlaybackPosition playbackPosition_;
    int64_t lastUiTime_ = 0;
    bool avCastStarted_ = false;
    bool is_seeking_ = false;
    bool is_avcast_ = false;
};
} // namespace OHOS::NWeb

#endif // MEDIA_AVSESSION_ADAPTER_IMPL_H