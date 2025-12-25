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

#ifndef MEDIA_AVCAST_ADAPTER_IMPL_H
#define MEDIA_AVCAST_ADAPTER_IMPL_H

#include <algorithm>
#include <deque>
#include <future>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <shared_mutex>

#include <unordered_map>
#include <multimedia/av_session/native_avmetadata.h>
#include <multimedia/av_session/native_avsession.h>
#include <multimedia/av_session/native_avsession_errors.h>
#include <multimedia/av_session/native_avqueueitem.h>
#include <multimedia/av_session/native_avcastcontroller.h>
#include <multimedia/av_session/native_avplaybackstate.h>
#include <multimedia/av_session/native_deviceinfo.h>
#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"

namespace OHOS::NWeb {

class MediaAVSessionAdapterImpl;

struct MediaCastDescription {
    const char* assetId;
    const char* title;
    const char* subtitle;
    const char* artist;
    const char* mediaType;
    const char* lyricContent;
    int32_t duration;
    const char* mediaUri;
    int32_t startPosition;
    int32_t mediaSize;
    const char* albumTitle;
    const char* appName;
};

class MediaAVCastAdapterImpl {
public:
    explicit MediaAVCastAdapterImpl(std::shared_ptr<MediaAVSessionAdapterImpl> avsession_adapter);

    ~MediaAVCastAdapterImpl();

    void GetAVCastController();

    bool Prepare(const MediaCastDescription& mediaCastDescription);

    void StartCast();

    void RegisterCallback();

    void UnregisterCallback();

    void PlayRemote();

    void PauseRemote();

    bool IsAvCastPlaying();

    int64_t GetPlaybackPosition();

    void SetPlaybackPositionRemote(const int32_t millis);

    void UpdateRemotePlayState(bool is_playing);
    
    void UpdateRemotePlayPosition(int64_t position);

    static void UpdateUiPlayState(std::shared_ptr<MediaAVSessionAdapterImpl> adapter, AVSession_PlaybackState playbackState);

    static void UpdateUiPlayPosition(std::shared_ptr<MediaAVSessionAdapterImpl> adapter, int64_t position, bool is_seek);

    static std::shared_mutex& GetAVCastAdapterMutex() { return avcast_adapter_mutex_; }

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
    OH_AVCastController* avCastController_ = nullptr;
    OH_AVSession_AVMediaDescriptionBuilder* avMediaDescriptionBuilder_ = nullptr;
    OH_AVSession_AVMediaDescription* avMediaDescription_ = nullptr;
    OH_AVSession_AVQueueItem avQueueItem_;
    std::shared_ptr<MediaAVSessionAdapterImpl> avsession_adapter_;
    AVSession_PlaybackState playbackState_;
    AVSession_PlaybackPosition playbackPosition_;
    int64_t lastUiTime_ = 0;
    static std::shared_mutex avcast_adapter_mutex_;
    size_t callback_index_ = 0;
    static CallbackSharedWrapper<MediaAVSessionAdapterImpl> callback_wrapper_;
    bool avCastStarted_ = false;
};
} // namespace OHOS::NWeb

#endif // MEDIA_AVCAST_ADAPTER_IMPL_H