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

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "media_avcast_adapter_impl.h"
#include <multimedia/av_session/native_avsession.h>
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"

namespace OHOS::NWeb {

std::shared_mutex MediaAVCastAdapterImpl::avcast_adapter_mutex_;
CallbackSharedWrapper<MediaAVSessionAdapterImpl> MediaAVCastAdapterImpl::callback_wrapper_;

MediaAVCastAdapterImpl::MediaAVCastAdapterImpl(std::shared_ptr<MediaAVSessionAdapterImpl> avsession_adapter)
    : avsession_adapter_(avsession_adapter) {
        if (callback_index_ > 0) {
            callback_wrapper_.Clear(callback_index_);
            callback_index_ = 0;
        }
        callback_index_ = callback_wrapper_.AddCallback(avsession_adapter);
        playbackPosition_.elapsedTime = 0;
        playbackPosition_.updateTime = 0;
    }

MediaAVCastAdapterImpl::~MediaAVCastAdapterImpl() {
    std::unique_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    avCastStarted_ = false;
    UnregisterCallback();
    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
}

void MediaAVCastAdapterImpl::GetAVCastController() {
    WVLOG_I("GetAVCastController enter.");
    std::shared_lock<std::shared_mutex> lock_avsession_adapter(MediaAVSessionAdapterImpl::GetAVSessionAdapterMutex());
    AVSession_ErrCode ret = OH_AVSession_GetAVCastController(avsession_adapter_->GetAVSession(), &avCastController_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVSession_GetAVCastController failed. ret: %{public}d", ret);
    }
}

bool MediaAVCastAdapterImpl::Prepare(const MediaCastDescription& mediaCastDescription) {
    WVLOG_I("MediaAVCastAdapterImpl::Prepare enter.");
    AVQueueItem_Result ret = OH_AVSession_AVMediaDescriptionBuilder_Create(&avMediaDescriptionBuilder_);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_Create failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetDuration(avMediaDescriptionBuilder_, mediaCastDescription.duration);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetDuration failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(avMediaDescriptionBuilder_, mediaCastDescription.mediaUri);
    WVLOG_I("OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri url: ret: %{public}s", mediaCastDescription.mediaUri);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(avMediaDescriptionBuilder_, mediaCastDescription.startPosition);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(avMediaDescriptionBuilder_, mediaCastDescription.mediaType);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetMediaType failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetTitle(avMediaDescriptionBuilder_, mediaCastDescription.title);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetTitle failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(avMediaDescriptionBuilder_, mediaCastDescription.assetId);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetAssetId failed. ret: %{public}d", ret);
        return false;
    }

    WVLOG_I("MediaAVCastAdapterImpl::Prepare mediaUri: %{public}s, startPosition: %{public}d, duration: %{public}d, title: %{public}s, assetId: %{public}s",
                mediaCastDescription.mediaUri, mediaCastDescription.startPosition, mediaCastDescription.duration, mediaCastDescription.title, mediaCastDescription.assetId);

    ret = OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(avMediaDescriptionBuilder_, &avMediaDescription_);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription failed. ret: %{public}d", ret);
        return false;
    }

    avQueueItem_.itemId = 1;
    avQueueItem_.description = avMediaDescription_;

    return true;
}

void MediaAVCastAdapterImpl::StartCast() {
    WVLOG_I("OH_AVCastController_Start enter.");
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    
    char* mediaUri;
    AVQueueItem_Result rets = OH_AVSession_AVMediaDescription_GetMediaUri(avQueueItem_.description, &mediaUri);
    WVLOG_I("StartCast mediaUri. ret: %{public}s", mediaUri);

    int startPosition;
    rets = OH_AVSession_AVMediaDescription_GetStartPosition(avQueueItem_.description, &startPosition);
    WVLOG_I("StartCast startPosition. ret: %{public}d", startPosition);

    int duration;
    rets = OH_AVSession_AVMediaDescription_GetDuration(avQueueItem_.description, &duration);
    WVLOG_I("StartCast duration. ret: %{public}d", duration);

    char* title;
    rets = OH_AVSession_AVMediaDescription_GetTitle(avQueueItem_.description, &title);
    WVLOG_I("StartCast title. ret: %{public}s", title);

    char* assetId;
    rets = OH_AVSession_AVMediaDescription_GetAssetId(avQueueItem_.description, &assetId);
    WVLOG_I("StartCast assetId. ret: %{public}s", assetId);

    AVSession_ErrCode retErr = OH_AVCastController_Prepare(avCastController_, &avQueueItem_);
    if (retErr != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVCastController_Prepare failed. ret: %{public}d", retErr);
        return;
    }
    RegisterCallback();
    retErr = OH_AVCastController_Start(avCastController_, &avQueueItem_);
    if (retErr != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVCastController_Start failed. ret: %{public}d", retErr);
        return;
    }
    avCastStarted_ = true;
}

void MediaAVCastAdapterImpl::RegisterCallback() {
    AVSession_ErrCode errCode = OH_AVCastController_RegisterPlaybackStateChangedCallback(avCastController_,
        &MediaAVCastAdapterImpl::PlaybackStateChangedCallback, reinterpret_cast<void *>(callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl RegisterPlaybackStateChangedCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_RegisterMediaItemChangedCallback(avCastController_,
        &MediaAVCastAdapterImpl::MediaItemChangeCallback, reinterpret_cast<void *>(callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl RegisterMediaItemChangedCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_RegisterSeekDoneCallback(avCastController_,
        &MediaAVCastAdapterImpl::SeekDoneCallback, reinterpret_cast<void *>(callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl RegisterSeekDoneCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_RegisterEndOfStreamCallback(avCastController_,
        &MediaAVCastAdapterImpl::EndOfStreamCallback, reinterpret_cast<void *>(callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl RegisterEndOfStreamCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_RegisterErrorCallback(avCastController_,
        &MediaAVCastAdapterImpl::ErrorCallback, reinterpret_cast<void *>(callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl RegisterErrorCallback err: %{public}d", errCode);
        return;
    }
}

void MediaAVCastAdapterImpl::UnregisterCallback() {
    AVSession_ErrCode errCode = OH_AVCastController_UnregisterPlaybackStateChangedCallback(avCastController_,
        &MediaAVCastAdapterImpl::PlaybackStateChangedCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl UnregisterPlaybackStateChangedCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_UnregisterMediaItemChangedCallback(avCastController_,
        &MediaAVCastAdapterImpl::MediaItemChangeCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl UnregisterMediaItemChangedCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_UnregisterSeekDoneCallback(avCastController_,
        &MediaAVCastAdapterImpl::SeekDoneCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl UnregisterSeekDoneCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_UnregisterEndOfStreamCallback(avCastController_,
        &MediaAVCastAdapterImpl::EndOfStreamCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl UnregisterEndOfStreamCallback err: %{public}d", errCode);
        return;
    }

    errCode = OH_AVCastController_UnregisterErrorCallback(avCastController_,
        &MediaAVCastAdapterImpl::ErrorCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl UnregisterErrorCallback err: %{public}d", errCode);
        return;
    }
}

void MediaAVCastAdapterImpl::UpdateUiPlayState(std::shared_ptr<MediaAVSessionAdapterImpl> adapter,
                                               AVSession_PlaybackState playbackState) {
    if (playbackState == AVSession_PlaybackState::PLAYBACK_STATE_PLAYING) {
        adapter->UpdateUiPlayState(true);
    } else if (playbackState == AVSession_PlaybackState::PLAYBACK_STATE_PAUSED ||
               playbackState == AVSession_PlaybackState::PLAYBACK_STATE_STOPPED) {
        adapter->UpdateUiPlayState(false);
    }
}

void MediaAVCastAdapterImpl::UpdateUiPlayPosition(std::shared_ptr<MediaAVSessionAdapterImpl> adapter,
                                                  int64_t position, bool is_seek) {
    if (adapter == nullptr || adapter->avCastAdapter_ == nullptr) {
        WVLOG_E("MediaAVCastAdapterImpl avsession or avcast adapter is null, skip seek");
        return;
    }
    if (is_seek) {
        WVLOG_I("MediaAVCastAdapterImpl UpdateUiPlayPosition isseek");
        adapter->UpdateUiPlayPosition(position);
        adapter->avCastAdapter_->lastUiTime_ = position;
        return;
    }

    if (position == 0) {
        WVLOG_I("MediaAVCastAdapterImpl UpdateUiPlayPosition position is 0, skip");
        return;
    }
    WVLOG_I("MediaAVCastAdapterImpl UpdateUiPlayPosition position %{public}d", position);
    if (std::abs(position - adapter->avCastAdapter_->lastUiTime_) >= 200) {
        WVLOG_I("MediaAVCastAdapterImpl UpdateUiPlayPosition lastUiTime_ %{public}d", adapter->avCastAdapter_->lastUiTime_);
        adapter->UpdateUiPlayPosition(position);
        adapter->avCastAdapter_->lastUiTime_ = position;
    }
}

AVSessionCallback_Result MediaAVCastAdapterImpl::PlaybackStateChangedCallback(OH_AVCastController* avcastcontroller,
    OH_AVSession_AVPlaybackState* playbackState, void* userData) {
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    WVLOG_I("lmh MediaAVCastAdapterImpl::PlaybackStateChangedCallback");
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<MediaAVSessionAdapterImpl> adapter = callback_wrapper_.GetCallback(callback_index);
    if (!adapter || !adapter->avCastAdapter_) {
        WVLOG_I("MediaAVCastAdapterImpl: adapter is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    AVSession_PlaybackState avSessionPlaybackState;
    // Store a status here; if it's the same as before, do nothing.
    if (OH_AVSession_GetPlaybackState(playbackState, &avSessionPlaybackState) == AV_SESSION_ERR_SUCCESS) {
        if (avSessionPlaybackState != adapter->avCastAdapter_->playbackState_) {
            UpdateUiPlayState(adapter, avSessionPlaybackState);
        }
        WVLOG_I("MediaAVCastAdapterImpl::PlaybackStateChangedCallback avSessionPlaybackState %{public}d", avSessionPlaybackState);
        if (avSessionPlaybackState != PLAYBACK_STATE_INITIAL) {
            adapter->avCastAdapter_->playbackState_ = avSessionPlaybackState;
            WVLOG_I("MediaAVCastAdapterImpl::PlaybackStateChangedCallback playbackState_ %{public}d", adapter->avCastAdapter_->playbackState_);
        }
    }

    AVSession_PlaybackPosition playbackPosition;
    if (OH_AVSession_GetPlaybackPosition(playbackState, &playbackPosition) == AV_SESSION_ERR_SUCCESS) {
        // Synchronize playback position to the kernel here
        WVLOG_I("MediaAVCastAdapterImpl::PlaybackStateChangedCallback playbackPosition %{public}d", playbackPosition);
        UpdateUiPlayPosition(adapter, playbackPosition.elapsedTime, false);
        if (playbackPosition.elapsedTime != 0) {
            adapter->avCastAdapter_->playbackPosition_ = playbackPosition;
        }
    }

    int32_t speed;
    if (OH_AVSession_GetPlaybackSpeed(playbackState, &speed) == AV_SESSION_ERR_SUCCESS) {
        // Synchronize playback speed to the kernel here
        WVLOG_I("MediaAVCastAdapterImpl::PlaybackStateChangedCallback speed %{public}d", speed);
    }

    int32_t volume;
    if (OH_AVSession_GetPlaybackVolume(playbackState, &volume) == AV_SESSION_ERR_SUCCESS) {
        // Synchronize playback volume to the kernel here
        WVLOG_I("MediaAVCastAdapterImpl::PlaybackStateChangedCallback volume %{public}d", volume);
    }
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVCastAdapterImpl::MediaItemChangeCallback(OH_AVCastController* avcastcontroller,
    OH_AVSession_AVQueueItem* avQueueItem, void* userData) {
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    WVLOG_I("MediaAVCastAdapterImpl::MediaItemChangeCallback itemId");
    WVLOG_I("MediaAVCastAdapterImpl::MediaItemChangeCallback itemId %{public}d", avQueueItem->itemId);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVCastAdapterImpl::SeekDoneCallback(OH_AVCastController* avcastcontroller,
    int32_t position, void* userData) {
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    WVLOG_I("MediaAVCastAdapterImpl::SeekDoneCallback");
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<MediaAVSessionAdapterImpl> adapter = callback_wrapper_.GetCallback(callback_index);
    if (!adapter || !adapter->avCastAdapter_) {
        WVLOG_I("MediaAVCastAdapterImpl: adapter is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    WVLOG_I("MediaAVCastAdapterImpl::SeekDoneCallback position:  %{public}d", position);
    UpdateUiPlayPosition(adapter, position, true);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVCastAdapterImpl::EndOfStreamCallback(OH_AVCastController* avcastcontroller,
    void* userData) {
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    WVLOG_I("MediaAVCastAdapterImpl::EndOfStreamCallback");
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVCastAdapterImpl::ErrorCallback(OH_AVCastController* avcastcontroller,
    void* userData, AVSession_ErrCode error) {
    std::shared_lock<std::shared_mutex> lock_avcast_adapter(avcast_adapter_mutex_);
    WVLOG_I("MediaAVCastAdapterImpl::ErrorCallback assetId %{public}d", error);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

void MediaAVCastAdapterImpl::PlayRemote() {
    WVLOG_I("MediaAVCastAdapterImpl::PlayRemote");
    if (!avCastStarted_) {
        WVLOG_E("MediaAVCastAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVCastAdapterImpl PlayRemote avCastController_ is null");
        return;
    }

    AVSession_AVCastControlCommandType cmdType = AVSession_AVCastControlCommandType::CAST_CONTROL_CMD_PLAY;
    AVSession_ErrCode errCode = OH_AVCastController_SendCommonCommand(avCastController_, &cmdType);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl SetPlaybackStateRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVCastAdapterImpl::PauseRemote() {
    WVLOG_I("MediaAVCastAdapterImpl::PauseRemote");
    if (!avCastStarted_) {
        WVLOG_E("MediaAVCastAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVCastAdapterImpl PauseRemote avCastController_ is null");
        return;
    }

    AVSession_AVCastControlCommandType cmdType = AVSession_AVCastControlCommandType::CAST_CONTROL_CMD_PAUSE;
    AVSession_ErrCode errCode =  OH_AVCastController_SendCommonCommand(avCastController_, &cmdType);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl SetPlaybackStateRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVCastAdapterImpl::SetPlaybackPositionRemote(const int32_t millis) {
    WVLOG_I("MediaAVCastAdapterImpl::SetPlaybackPosition %{public}d", millis); 
    if (!avCastStarted_) {
        WVLOG_E("MediaAVCastAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVCastAdapterImpl SetPlaybackPositionRemote avCastController_ is null");
        return;
    }

    AVSession_ErrCode errCode = OH_AVCastController_SendSeekCommand(avCastController_, millis);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVCastAdapterImpl SetPlaybackPositionRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVCastAdapterImpl::UpdateRemotePlayState(bool is_playing) {
    WVLOG_I("MediaAVCastAdapterImpl UpdateRemotePlayState is_playing: %{public}d", is_playing);
    if(is_playing) {
        PlayRemote();
    } else {
       PauseRemote();
    }
}

void MediaAVCastAdapterImpl::UpdateRemotePlayPosition(int64_t position) {
    SetPlaybackPositionRemote(static_cast<int32_t>(position));
}

bool MediaAVCastAdapterImpl::IsAvCastPlaying() {
    return playbackState_ == PLAYBACK_STATE_PLAYING;
}

int64_t MediaAVCastAdapterImpl::GetPlaybackPosition() {
    return playbackPosition_.elapsedTime;
}

} // OHOS::NWeb