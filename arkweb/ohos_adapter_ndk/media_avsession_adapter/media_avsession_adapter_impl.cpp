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
#include "media_avsession_adapter_impl.h"

#include <bundle/native_interface_bundle.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <unistd.h>

namespace OHOS::NWeb {

std::unordered_map<std::string, MediaAVSessionAdapterImpl *> MediaAVSessionAdapterImpl::avSessionMap;
CallbackSharedWrapper<MediaAVSessionCallbackAdapter> MediaAVSessionAdapterImpl::callback_wrapper_;
std::unordered_map<OH_AVSession*, MediaAVSessionAdapterImpl *> MediaAVSessionAdapterImpl::avSessionMapOther_;
CallbackSharedWrapper<MediaAVSessionAdapterImpl> MediaAVSessionAdapterImpl::avsession_callback_wrapper_;
constexpr int64_t TIME_OUT = 0;
constexpr int64_t URL_NUM = 2;
constexpr int64_t UiTIME_UPDATE_INTERVAL = 200;

void MediaAVSessionKey::Init() {
    pid_ = getpid();
    OH_NativeBundle_ElementName bundleInfo = OH_NativeBundle_GetMainElementName();
    if (bundleInfo.bundleName) {
        bundleName_ = std::string(bundleInfo.bundleName);
    }
    type_ = MediaAVSessionType::MEDIA_TYPE_INVALID;
    if (bundleInfo.abilityName) {
        abilityName_ = std::string(bundleInfo.abilityName);
    }
    return;
}

int32_t MediaAVSessionKey::GetPID() {
    return pid_;
}

std::string &MediaAVSessionKey::GetBundleName() {
    return bundleName_;
}

std::string &MediaAVSessionKey::GetAbilityName() {
    return abilityName_;
}

void MediaAVSessionKey::SetType(MediaAVSessionType type) {
    type_ = type;
}

MediaAVSessionType MediaAVSessionKey::GetType() {
    return type_;
}

std::string MediaAVSessionKey::ToString() {
    return (std::to_string(pid_) + "_" + bundleName_ + "_" + abilityName_);
}

MediaAVSessionAdapterImpl::MediaAVSessionAdapterImpl() {
    InitMediaAVSessionAdapterImpl();
}

void MediaAVSessionAdapterImpl::InitMediaAVSessionAdapterImpl() {
    avSessionKey_ = std::make_shared<MediaAVSessionKey>();
    avSessionKey_->Init();
    AVMetadata_Result ret = OH_AVMetadataBuilder_Create(&builder_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("create metadata builder failed, ret=%{public}d", ret);
        return;
    }

    ret = OH_AVMetadataBuilder_SetAssetId(builder_, std::to_string(avSessionKey_->GetPID()).c_str());
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("set assert id failed, ret=%{public}d", ret);
        return;
    }

    ret = OH_AVMetadataBuilder_GenerateAVMetadata(builder_, &avMetadata_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("generate avmetadata failed, ret=%{public}d", ret);
        return;
    }

    avPlaybackState_ = PLAYBACK_STATE_INITIAL;
}

MediaAVSessionAdapterImpl::~MediaAVSessionAdapterImpl() {
    WVLOG_I("ohmedia: ~MediaAVSessionAdapterImpl");

    avCastStarted_ = false;
    if (!UnregisterCallback()) {
        WVLOG_E("UnregisterCallback failed");
    }

    if (avMetadata_) {
        AVMetadata_Result ret = OH_AVMetadata_Destroy(avMetadata_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("destroy avmetadata failed, ret=%{public}d", ret);
        }
    }

    if (builder_) {
        AVMetadata_Result ret = OH_AVMetadataBuilder_Destroy(builder_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("destroy avmetadata builder failed, ret=%{public}d", ret);
        }
    }

    if (avMediaDescriptionBuilder_) {
        AVQueueItem_Result ret = OH_AVSession_AVMediaDescriptionBuilder_Destroy(avMediaDescriptionBuilder_);
        if (ret != AVQUEUEITEM_SUCCESS) {
            WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_Destroy failed. ret: %{public}d", ret);
        }
    }

    if (avMediaDescription_) {
        AVQueueItem_Result ret = OH_AVSession_AVMediaDescription_Destroy(avMediaDescription_);
        if (ret != AVQUEUEITEM_SUCCESS) {
            WVLOG_E("OH_AVSession_AVMediaDescription_Destroy failed. ret: %{public}d", ret);
        }
    }

    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
    avMetadata_ = nullptr;
    builder_ = nullptr;
    avMediaDescriptionBuilder_ = nullptr;
    avMediaDescription_ = nullptr;
    DestroyAVSession();
}

bool MediaAVSessionAdapterImpl::CreateAVSession(MediaAVSessionType type) {
    WVLOG_I("CreateAVSession in, type=%{public}d", int32_t(type));
    if (type == MediaAVSessionType::MEDIA_TYPE_INVALID) {
        WVLOG_E("CreateAVSession, type invalid return false");
        return false;
    }
    if (avSession_ && avSessionKey_ &&(type != avSessionKey_->GetType())) {
        DestroyAVSession();
    }
    auto findIter = avSessionMap.find(avSessionKey_->ToString());
    if (findIter != avSessionMap.end() && findIter->second && findIter->second->avSession_ != avSession_) {
        findIter->second->UnregisterMediaCastOutputDeviceCallback();
    }
    if (!avSession_) {
        if (findIter != avSessionMap.end()) {
            DestroyAndEraseSession();
        }
        return CreateNewSession(type);
    } else {
        if (findIter != avSessionMap.end()) {
            if (findIter->second && findIter->second->avSession_ != avSession_) {
                DestroyAndEraseSession();
                DestroyAVSession();
            } else {
                WVLOG_E("CreateAVSession, return false");
                return false;
            }
        }
        return CreateNewSession(type);
    }
}

void MediaAVSessionAdapterImpl::DestroyAVSession() {
    WVLOG_I("DestroyAVSession in");
    SetAvCast(false);
    UnregisterMediaCastOutputDeviceCallback();
    HandleStopMediaCast();
    {
        std::lock_guard<std::mutex> lock(avsession_mutex_);
        if (avSession_) {
            auto it_avsession = avSessionMapOther_.find(avSession_);
            if (it_avsession != avSessionMapOther_.end()) {
                avSessionMapOther_.erase(it_avsession);
            }
            AVSession_ErrCode ret = OH_AVSession_Destroy(avSession_);
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("DestroyAVSession Destroy() failed, ret: %{public}d", ret);
            } else {
                WVLOG_I("DestroyAVSession Destroy() success, ret: %{public}d", ret);
                auto media = callback_wrapper_.GetCallback(callback_index_);
                if (media) {
                    media->NotifyCastControlShow(false);
                }
            }
            ret = OH_AVCastController_Destroy(avCastController_);
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("DestroyAVSession OH_AVCastController_Destroy failed, ret: %{public}d", ret);
            }
            avSession_ = nullptr;
            avCastController_ = nullptr;
        }
    }
    if (avSessionKey_) {
        auto iter = avSessionMap.find(avSessionKey_->ToString());
        if (iter != avSessionMap.end()) {
            if (iter->second && iter->second->is_avcast_) {
                WVLOG_I("DestroyAVSession, current process is casting, no need to clear avSessionMap");
            } else {
                avSessionMap.erase(iter);
            }
        }
    }
    poster_url_ = "";
    WVLOG_I("DestroyAVSession out");
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(OH_AVSession *session,
    AVSession_ControlCommand command, void *userData) {
    if (!userData) {
        WVLOG_E("ohmedia: userData is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    auto media = callback_wrapper_.GetCallback(callback_index);
    if (!media) {
        WVLOG_E("AVSessionOnCommandCallback, ohmedia: media is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    switch (command) {
        case CONTROL_CMD_PLAY:
            media->Play();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        case CONTROL_CMD_PAUSE:
            media->Pause();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        case CONTROL_CMD_STOP:
            media->Stop();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        default:
            WVLOG_E("invalid cmd: %{public}d", command);
    }

    return AVSESSION_CALLBACK_RESULT_FAILURE;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(OH_AVSession *session,
    uint64_t seekTime, void *userData) {
    WVLOG_I("SeekCallback seekTime: %{public}lu", seekTime);
    if (!userData) {
        WVLOG_E("ohmedia: userData is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    auto media = callback_wrapper_.GetCallback(callback_index);
    if (!media) {
        WVLOG_E("AVSessionOnSeekCallback, ohmedia: media is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    media->SeekTo(seekTime);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

bool MediaAVSessionAdapterImpl::RegistCallback(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) {
    WVLOG_I("RegistCallback in");
    if (avSession_ && Activate()) {
        AVSession_ErrCode ret;
        std::shared_ptr<MediaAVSessionCallbackAdapter> callback_adapter = callbackAdapter;
        if (callback_adapter == nullptr) {
            WVLOG_E("Create callback_adapter failed");
            return false;
        }
        if (callback_index_ > 0) {
            callback_wrapper_.Clear(callback_index_);
        }
        callback_index_ = callback_wrapper_.AddCallback(callback_adapter);
        for (AVSession_ControlCommand command = CONTROL_CMD_PLAY;
            command <= CONTROL_CMD_STOP;
            command = (AVSession_ControlCommand)(command + 1)) {
            ret = OH_AVSession_RegisterCommandCallback(avSession_,
                command, &MediaAVSessionAdapterImpl::AVSessionOnCommandCallback,
                reinterpret_cast<void *>(callback_index_));
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("RegisterCommandCallback failed. ret: %{public}d", ret);
                return false;
            }
        }

        ret = OH_AVSession_RegisterSeekCallback(avSession_,
            &MediaAVSessionAdapterImpl::AVSessionOnSeekCallback, reinterpret_cast<void *>(callback_index_));
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("RegisterSeekCallback failed. ret: %{public}d", ret);
            return false;
        }

        return true;
    }

    WVLOG_I("RegistCallback out return false");
    return false;
}

bool MediaAVSessionAdapterImpl::IsActivated() {
    WVLOG_I("IsActivated in");
    if (avSession_) {
        return isActived_;
    }
    WVLOG_E("IsActivated out avSession is null, return false");
    return false;
}

bool MediaAVSessionAdapterImpl::Activate() {
    if (!avSession_) {
        WVLOG_E("Activate avSession_ is null, return false");
        return false;
    }

    AVSession_ErrCode ret = OH_AVSession_Activate(avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("Activate failed. ret: %{public}d", ret);
        return false;
    }

    isActived_ = true;
    WVLOG_I("Activate success.");
    return true;
}

void MediaAVSessionAdapterImpl::DeActivate() {
    if (avSession_) {
        AVSession_ErrCode ret = OH_AVSession_Deactivate(avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("DeActivate failed. ret: %{public}d", ret);
            return;
        }
        isActived_ = false;
    }
}

void MediaAVSessionAdapterImpl::SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) {
    UpdateMetaData(metadata);
    if (avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetAVMetadata(avSession_, avMetadata_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetMetadata failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetMetadata success");
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackState(MediaAVSessionPlayState state) {
    if (UpdatePlaybackState(state) && avSession_) {
        Activate();
        if (avSession_ && avPlaybackState_) {
            AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("SetPlaybackState failed. ret: %{public}d", ret);
            } else {
                WVLOG_I("SetPlaybackState success");
            }
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    if (UpdateDuration(position) && avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetAVMetadata(avSession_, avMetadata_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetAVMetadata failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetAVMetadata success");
        }
    }
    if (UpdatePlaybackPosition(position) && avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetPlaybackPosition failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetPlaybackPosition success");
        }
    }
    if (position->GetDuration() < INT32_MAX && position->GetElapsedTime() < INT32_MAX) {
        MediaCastDescription_.duration = position->GetDuration();
        MediaCastDescription_.startPosition = position->GetElapsedTime();
    } else {
        WVLOG_E("Duration and CurrentTime exceeds the maximum range of int32.");
    }
}

AVMetadata_Result MediaAVSessionAdapterImpl::UpdateAVMetadata(void) {
    AVMetadata_Result ret = AVMETADATA_SUCCESS;

    if (avMetadata_) {
        ret = OH_AVMetadata_Destroy(avMetadata_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("OH_AVMetadata_Destroy failed. ret: %{public}d", ret);
        }
    }

    avMetadata_ = nullptr;
    ret = OH_AVMetadataBuilder_GenerateAVMetadata(builder_, &avMetadata_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("generate avmetadata failed, ret=%{public}d", ret);
    }

    return ret;
}

bool MediaAVSessionAdapterImpl::UpdateMetaData(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) {
    if (!metadata) {
        WVLOG_E("ohmedia: metadata is null");
        return false;
    }
    bool updated = false;
    AVMetadata_Result ret;
    if (title_ != metadata->GetTitle()) {
        title_ = metadata->GetTitle();
        ret = OH_AVMetadataBuilder_SetTitle(builder_, metadata->GetTitle().c_str());
        if (ret == AVMETADATA_SUCCESS)
            updated = true;
    }

    if (artist_ != metadata->GetArtist()) {
        artist_ = metadata->GetArtist();
        ret = OH_AVMetadataBuilder_SetArtist(builder_, metadata->GetArtist().c_str());
        if (ret == AVMETADATA_SUCCESS)
            updated = true;
    }

    if (album_ != metadata->GetAlbum()) {
        album_ = metadata->GetAlbum();
        ret = OH_AVMetadataBuilder_SetAlbum(builder_, metadata->GetAlbum().c_str());
        if (ret == AVMETADATA_SUCCESS)
            updated = true;
    }

    if (poster_url_ != metadata->GetImageUrl()) {
        poster_new_ = metadata->GetImageUrl();
        AddUrl(metadata->GetImageUrl());
    }
    album_url_ = metadata->GetImageUrl();

    if (updated) {
        OH_AVMetadataBuilder_SetMediaImageUri(builder_, "");
        ret = UpdateAVMetadata();
        if (ret != AVMETADATA_SUCCESS) {
           return false;
        }
    }
    WVLOG_I("UpdateMetaData return updated: %{public}d", updated);
    return updated;
}

bool MediaAVSessionAdapterImpl::UpdateDuration(const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    if (!position) {
        WVLOG_E("ohmedia: position is null");
        return false;
    }
    if (duration_ != position->GetDuration()) {
        AVMetadata_Result ret = AVMETADATA_SUCCESS;
        if (position->GetDuration() < INT64_MAX) {
            ret = OH_AVMetadataBuilder_SetDuration(builder_, position->GetDuration());
        } else {
            ret = OH_AVMetadataBuilder_SetDuration(builder_, -1);
        }
        
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("UpdateDuration failed. ret: %{public}d", ret);
            return false;
        }
        duration_ = position->GetDuration();
        ret = OH_AVMetadataBuilder_SetMediaImageUri(builder_, "");
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("OH_AVMetadataBuilder_SetMediaImageUri failed. ret: %{public}d", ret);
            return false;
        }
        ret = UpdateAVMetadata();
        if (ret != AVMETADATA_SUCCESS) {
            return false;
        }
    }
    if (poster_url_ != poster_new_) {
        StartAsyncPosterUpdate();
    }
    WVLOG_I("ohmedia: UpdateDuration success.");
    return true;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackState(MediaAVSessionPlayState state) {
    switch (state) {
        case MediaAVSessionPlayState::STATE_PLAY:
            avPlaybackState_ = PLAYBACK_STATE_PLAYING;
            break;
        case MediaAVSessionPlayState::STATE_PAUSE:
            avPlaybackState_ = PLAYBACK_STATE_PAUSED;
            break;
        case MediaAVSessionPlayState::STATE_INITIAL:
        default:
            avPlaybackState_ = PLAYBACK_STATE_PAUSED;
            break;
    }
    if (!avSession_) {
        WVLOG_E("UpdatePlaybackState avsession is null");
        return false;
    }
    AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("UpdatePlaybackState failed. ret: %{public}d", ret);
        return false;
    }
    WVLOG_I("UpdatePlaybackState success.");
    return true;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackPosition(
    const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    if (!position) {
        WVLOG_E("ohmedia: position is null");
        return false;
    }
    bool updated = false;

    AVSession_PlaybackPosition playbackPosition = {
        .elapsedTime = position->GetElapsedTime(),
        .updateTime = position->GetUpdateTime(),
    };

    AVSession_ErrCode err = OH_AVSession_SetPlaybackPosition(avSession_, &playbackPosition);
    if (err == AV_SESSION_ERR_SUCCESS) {
        updated = true;
    }
    WVLOG_I("UpdatePlaybackPosition return updated: %{public}d", updated);
    return updated;
}

void MediaAVSessionAdapterImpl::DestroyAndEraseSession() {
    WVLOG_I("DestroyAndEraseSession in");
    if (!avSessionKey_) {
        WVLOG_E("ohmedia: avSessionKey_ is null");
        return;
    }
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter == avSessionMap.end()) {
        WVLOG_E("DestroyAndEraseSession invalid iterator return");
        return;
    }
    if (!iter->second || !iter->second->avSession_) {
        avSessionMap.erase(iter);
        WVLOG_E("DestroyAndEraseSession avsession is null pointer return");
        return;
    }
    iter->second->SetAvCast(false);
    iter->second->UnregisterMediaCastOutputDeviceCallback();
    iter->second->HandleStopMediaCast();
    if (iter->second->avSession_) {
        auto it_avsession = avSessionMapOther_.find(iter->second->avSession_);
        if (it_avsession != avSessionMapOther_.end()) {
            avSessionMapOther_.erase(it_avsession);
        }
    }
    AVSession_ErrCode ret = OH_AVSession_Destroy(iter->second->avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("DestroyAndEraseSession Destroy failed, ret: %{public}d", ret);
    } else {
        WVLOG_I("DestroyAndEraseSession Destroy success");
        auto media = callback_wrapper_.GetCallback(iter->second->callback_index_);
        if (media) {
            media->NotifyCastControlShow(false);
        }
    }
    ret = OH_AVCastController_Destroy(avCastController_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("DestroyAVSession OH_AVCastController_Destroy failed, ret: %{public}d", ret);
    }
    // clear adapter->avSession, otherwise it will crash when callback
    iter->second->avSession_ = nullptr;
    iter->second->avCastController_ = nullptr;

    avSession_ = nullptr;
    avCastController_ = nullptr;
    avSessionMap.erase(iter);    
    WVLOG_I("DestroyAndEraseSession out");
}

bool MediaAVSessionAdapterImpl::CreateNewSession(const MediaAVSessionType& type) {
    WVLOG_I("CreateNewSession in type: %{public}d", type);
    AVSession_Type sessionType;

    switch (type) {
        case MediaAVSessionType::MEDIA_TYPE_AUDIO:
            sessionType = SESSION_TYPE_AUDIO;
            break;

        case MediaAVSessionType::MEDIA_TYPE_VIDEO:
            sessionType = SESSION_TYPE_VIDEO;
            break;

        default:
            return false;
    }
    if (!avSessionKey_) {
        WVLOG_E("ohmedia: avSessionKey_ is null");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(avsession_mutex_);
        AVSession_ErrCode ret = OH_AVSession_Create(sessionType, "av_media_session",
            avSessionKey_->GetBundleName().c_str(), avSessionKey_->GetAbilityName().c_str(),
            &avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_I("OH_AVSession_Create failed. ret: %{public}d", ret);
            return false;
        }
        SetAvCast(false);
    }

    avSessionKey_->SetType(type);
    avSessionMap.insert(std::pair<std::string, MediaAVSessionAdapterImpl *>((avSessionKey_->ToString()), this));
    avSessionMapOther_.insert(std::pair<OH_AVSession*, MediaAVSessionAdapterImpl *>(avSession_, this));
    return true;
}

bool MediaAVSessionAdapterImpl::IsUrlInQueue(const std::string& url) {
    return std::find(url_queue_.begin(), url_queue_.end(), url) != url_queue_.end();
}

void MediaAVSessionAdapterImpl::AddUrl(const std::string& url) {
    std::lock_guard<std::mutex> lock(url_mutex_);
    if (!IsUrlInQueue(url)) {
        if (url_queue_.size() >= URL_NUM) {
            url_queue_.pop_front();
        }
        url_queue_.push_back(url);
    }
}

bool MediaAVSessionAdapterImpl::StartAsyncPosterUpdate() {
    if (media_futures_.valid() &&
        media_futures_.wait_for(std::chrono::seconds(TIME_OUT)) != std::future_status::ready) {
        WVLOG_E("ohmedia: previous task not finished yet");
        return false;
    }
    std::weak_ptr<MediaAVSessionAdapterImpl> weak_this = shared_from_this();
    media_futures_ = std::async(std::launch::async, [weak_this]() {
        if (auto weak = weak_this.lock()) {
            weak->ProcessPosterQueue();
        }
    });
    return true;
}

void MediaAVSessionAdapterImpl::ProcessPosterQueue() {
    WVLOG_I("ohmedia: start async task");
    std::string url;
    while (!url_queue_.empty()) {
        {
            std::lock_guard<std::mutex> lock(url_mutex_);
            url = url_queue_.front();
            url_queue_.pop_front();
        }
        AVMetadata_Result ret = OH_AVMetadataBuilder_SetMediaImageUri(builder_, url.c_str());
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("OH_AVMetadataBuilder_SetMediaImageUri failed. ret: %{public}d", ret);
            continue;
        }
        poster_url_ = url;
        OH_AVMetadata *avMetadata = nullptr;
        ret = OH_AVMetadataBuilder_GenerateAVMetadata(builder_, &avMetadata);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("GenerateAVMetadata failed. ret: %{public}d", ret);
            ret = OH_AVMetadata_Destroy(avMetadata);
            if (ret != AVMETADATA_SUCCESS) {
                WVLOG_E("destory avmetadata failed. ret: %{public}d", ret);
            }
            continue;
        }
           
        {
            std::lock_guard<std::mutex> lock(avsession_mutex_);
            if (!avSession_) {
                ret = OH_AVMetadata_Destroy(avMetadata);
                if (ret != AVMETADATA_SUCCESS) {
                    WVLOG_E("destory avmetadata failed. ret: %{public}d", ret);
                }
                continue;
            }
            Activate();
            AVSession_ErrCode avsessionCode = OH_AVSession_SetAVMetadata(avSession_, avMetadata);
            if (avsessionCode != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("SetMetadata failed. ret: %{public}d", avsessionCode );
                ret = OH_AVMetadata_Destroy(avMetadata);
                if (ret != AVMETADATA_SUCCESS) {
                    WVLOG_E("destory avmetadata failed. ret: %{public}d", ret);
                }
                continue;
            }
        }
        ret = OH_AVMetadata_Destroy(avMetadata);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("destory avmetadata failed. ret: %{public}d", ret);
        }
    }
}

void MediaAVSessionAdapterImpl::PrepareMediaCastDescription() {

    if (!Prepare(MediaCastDescription_)) {
        WVLOG_E(" PrepareMediaCastDescription prepare failed.");
    }
    RegistAVSessionCallbackOutputDeviceChange();
}

int32_t MediaAVSessionAdapterImpl::GetMediaCastCurrentTime() {
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("GetMediaCastCurrentTime, media is null");
        return 0;
    }
    return media->GetMediaCastCurrentTime();
}

void MediaAVSessionAdapterImpl::RegistAVSessionCallbackOutputDeviceChange() {
    WVLOG_I("OH_AVSession_RegisterOutputDeviceChangeCallback enter");
    avsession_callback_index_ = avsession_callback_wrapper_.AddCallback(shared_from_this());
    AVSession_ErrCode ret = OH_AVSession_RegisterOutputDeviceChangeCallback(avSession_, OutputDeviceChangeCallback);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVSession_RegisterOutputDeviceChangeCallback failed. ret: %{public}d", ret);
    }
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::OutputDeviceChangeCallback(OH_AVSession* session,
    AVSession_ConnectionState state, AVSession_OutputDeviceInfo *outputDeviceInfo) {
    WVLOG_I("MediaAVSessionAdapterImpl::OutputDeviceChange enter");
    switch (state) {
        case STATE_CONNECTED: {
            WVLOG_I("MediaAVSessionAdapterImpl::OutputDeviceChange, STATE_CONNECTED");
            AVCastStateConnect(session, outputDeviceInfo);
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        }
        case STATE_DISCONNECTED: {
            WVLOG_I("MediaAVSessionAdapterImpl::OutputDeviceChange STATE_DISCONNECTED");
            AVCastStateDisconnect(session);
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        }
        default:
            WVLOG_E("MediaAVSessionAdapterImpl::OutputDeviceChange, STATE_DEFAULT: %{public}d", state);
            AVCastStateDisconnectDefault(session);
            return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
}

void MediaAVSessionAdapterImpl::AVCastStateConnect(OH_AVSession *session,
    AVSession_OutputDeviceInfo *outputDeviceInfo) {
    WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateConnect, enter");
    auto it = avSessionMapOther_.find(session);
    if (it != avSessionMapOther_.end()) {
        WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateConnect");
        MediaAVSessionAdapterImpl* adapter = it->second;
        if (!adapter) {
            WVLOG_E("AVCastStateConnect adapter is nullptr");
            return;
        }
        adapter->UpdateAVCastDevice(outputDeviceInfo);
        std::string LOCAL_DEVICE = "LocalDevice";
        if (adapter->GetAVCastDevice() != LOCAL_DEVICE) {
            WVLOG_I("AVCastStateConnect, not LocalDevice");
            if (!adapter->PrepareAndStartCast()) {
                WVLOG_E("AVCastStateConnect, PrepareAndStartCast failed");
                return;
            }            
            adapter->PullUpCastBackGround();
            adapter->PauseNative();
            adapter->SetAvCast(true);
        }
    }
}

void MediaAVSessionAdapterImpl::AVCastStateDisconnect(OH_AVSession *session) {
    WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateDisconnect, enter");
    auto it = avSessionMapOther_.find(session);
    if (it != avSessionMapOther_.end()) {
        WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateDisconnect");
        MediaAVSessionAdapterImpl* adapter = it->second;
        if (!adapter) {
            WVLOG_E("AVCastStateDisconnect adapter is nullptr");
            return;
        }
        adapter->MediaCastStopped();
        adapter->UnregisterCallback();
        if (!adapter->is_error_ || adapter->is_avcast_) {
            WVLOG_I("AVCastStateDisconnect SeekNative: %{public}d", adapter->GetPlaybackPosition());
            adapter->SeekNative(adapter->GetPlaybackPosition());
            adapter->is_error_ = false;
        }
        if (adapter->IsAvCastPlaying()) {
            WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateDisconnect PlayNative");
            adapter->PlayNative();
        } else {
            WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateDisconnect PauseNative");
            adapter->PauseNative();
        }
        adapter->SetAvCast(false);
    }
}

void MediaAVSessionAdapterImpl::AVCastStateDisconnectDefault(OH_AVSession *session) {
    auto it = avSessionMapOther_.find(session);
    if (it != avSessionMapOther_.end()) {
        WVLOG_I("MediaAVSessionAdapterImpl::AVCastStateDisconnectDefault");
        MediaAVSessionAdapterImpl* adapter = it->second;
        if (!adapter) {
            WVLOG_E("AVCastStateDisconnectDefault adapter is nullptr");
            return;
        }
        adapter->is_error_ = false;
        adapter->SetAvCast(false);
    }
}

void MediaAVSessionAdapterImpl::MediaCastStopped() {
    WVLOG_I("MediaAVSessionAdapterImpl::MediaCastStopped, enter");
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("MediaCastStopped, media is null");
        return;
    }
    media->MediaCastStopped();
}

void MediaAVSessionAdapterImpl::UpdateAVCastDevice(AVSession_OutputDeviceInfo *outputDeviceInfo) {
    if (!outputDeviceInfo) {
        WVLOG_E("PullUpCastBackGround failed outputDeviceInfo is nullptr");
        return;
    }
    if (!outputDeviceInfo->deviceInfos) {
        WVLOG_E("PullUpCastBackGround failed outputDeviceInfo->deviceInfos is nullptr");
        return;
    }

    char* deviceName;
    AVSession_ErrCode ret = OH_DeviceInfo_GetDeviceName(*(outputDeviceInfo->deviceInfos), &deviceName);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_DeviceInfo_GetDeviceName failed. ret: %{public}d", ret);
        return;
    }
    SetAVCastDevice(deviceName);
}

void MediaAVSessionAdapterImpl::PullUpCastBackGround() {
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("PullUpCastBackGround, media is null");
        return;
    }
    std::string deviceName = GetAVCastDevice();
    media->PullUpCastBackGround(deviceName);
}

bool MediaAVSessionAdapterImpl::PrepareAndStartCast() {
    WVLOG_I("PrepareAndStartCast enter.");
    if (!GetAVCastController()) {
        WVLOG_E("MediaAVSessionAdapterImpl::PrepareAndStartCast, GetAVCastController failed");
        return false;
    }
    if (!StartCast()) {
        WVLOG_E("MediaAVSessionAdapterImpl::PrepareAndStartCast, StartCast failed");
        return false;
    }
    // need to seek remotely here
    int32_t currentTime = GetMediaCastCurrentTime();
    WVLOG_I ("MediaAVSessionAdapterImpl::PrepareAndStartCast, currentTime: %{public}d", currentTime);
    UpdateRemotePlayPositionCast(currentTime);
    UpdateRemotePlayStateCast(avPlaybackState_ == PLAYBACK_STATE_PLAYING);
    return true;
}

void MediaAVSessionAdapterImpl::SetRemoteCastEnabled(bool enabled) {
    WVLOG_I ("MediaAVSessionAdapterImpl::SetRemoteCastEnabled, enable: %{public}d", enabled);
    AVSession_ErrCode ret = OH_AVSession_SetRemoteCastEnabled(avSession_, enabled);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("SetRemoteCastEnabled failed, ret=%{public}d", ret);
    }
}

void MediaAVSessionAdapterImpl::SetMediaCastUri(const std::string& mediaUri) {
    // set media Url
    media_uri_storage_ = mediaUri;
    MediaCastDescription_.mediaUri = media_uri_storage_;

    // set media type
    const char* media_type = "VIDEO";
    MediaCastDescription_.mediaType = media_type;

    // set media title
    MediaCastDescription_.title = title_;

    // set media assetId
    pid_avsession_ = std::to_string(avSessionKey_->GetPID());
    MediaCastDescription_.assetId = pid_avsession_;

    // set poster url
    MediaCastDescription_.albumUrl = album_url_;
}

void MediaAVSessionAdapterImpl::HandleStopMediaCast() {
    WVLOG_I("HandleStopMediaCast enter");
    AVSession_ErrCode ret = OH_AVSession_StopCasting(avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("HandleStopMediaCast OH_AVSession_StopCasting failed. ret: %{public}d", ret);
    }
}

void MediaAVSessionAdapterImpl::UpdateUiPlayStateByClient(AVSession_PlaybackState& avSessionPlaybackState) {
    if (avSessionPlaybackState != GetAVCastPlaybackState()) {
        if (avSessionPlaybackState == AVSession_PlaybackState::PLAYBACK_STATE_PLAYING) {
            UpdateUiPlayState(true);
        } else if (avSessionPlaybackState == AVSession_PlaybackState::PLAYBACK_STATE_PAUSED ||
                avSessionPlaybackState == AVSession_PlaybackState::PLAYBACK_STATE_STOPPED) {
            UpdateUiPlayState(false);
        }
    }
}

void MediaAVSessionAdapterImpl::SetUiPlayStateByClient(AVSession_PlaybackState& avSessionPlaybackState) {
    SetAVCastUiPlayState(avSessionPlaybackState);
}

void MediaAVSessionAdapterImpl::SetUiPlayPositionByClient(AVSession_PlaybackPosition& playbackPosition) {
    SetAVCastUiPlayPosition(playbackPosition);
}

void MediaAVSessionAdapterImpl::SetUilastUiTimeByClient(int64_t position) {
    SetAVCastUilastUiTime(position);
}

void MediaAVSessionAdapterImpl::SetUiSeekingByClient(bool is_seeking) {
    SetAVCastUiSeeking(is_seeking);
}

AVSession_PlaybackState MediaAVSessionAdapterImpl::GetUiPlayStateByClient() {
    return GetAVCastUiPlayState();
};

int64_t MediaAVSessionAdapterImpl::GetUilastUiTimeByClient() {
    return GetAVCastUilastUiTime();
}

bool MediaAVSessionAdapterImpl::GetUiSeekingByClient() {
    return GetAVCastUiSeeking();
}

void MediaAVSessionAdapterImpl::SetAvCast(bool is_avcast) {
    WVLOG_I("MediaAVSessionAdapterImpl SetAvCast: %{public}d", is_avcast);
    is_avcast_ = is_avcast;
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("SetAvCast, ohmedia: media is null");
        return;
    }
    media->SetAvCast(is_avcast);
}

void MediaAVSessionAdapterImpl::UpdateUiPlayState(bool is_playing) {
    WVLOG_I("MediaAVSessionAdapterImpl UpdateUiPlayState: %{public}d", is_playing);
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("UpdateUiPlayState, ohmedia: media is null");
        return;
    }
    media->UpdateUiPlayState(is_playing);
}

void MediaAVSessionAdapterImpl::UpdateUiPlayPosition(int64_t position) {
    WVLOG_D("MediaAVSessionAdapterImpl UpdateUiPlayPosition: %{public}d", position);
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("UpdateUiPlayPosition, ohmedia: media is null");
        return;
    }
    media->UpdateUiPlayPosition(position);
}

void MediaAVSessionAdapterImpl::UpdateRemotePlayState(bool is_playing) {
    UpdateRemotePlayStateCast(is_playing);
}

void MediaAVSessionAdapterImpl::UpdateRemotePlayPosition(int64_t position) {
    UpdateRemotePlayPositionCast(position);
}

void MediaAVSessionAdapterImpl::SeekNative(const int64_t millis) {
    WVLOG_I("MediaAVSessionAdapterImpl seekTime: %{public}lu", millis);
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("SeekNative, ohmedia: media is null");
    }
    media->SeekTo(millis);
}

void MediaAVSessionAdapterImpl::PlayNative() {
    WVLOG_I("MediaAVSessionAdapterImpl PlayNative");
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("PlayNative, ohmedia: media is null");
        return;
    }
    media->Play();
}

void MediaAVSessionAdapterImpl::PauseNative() {
    WVLOG_I("MediaAVSessionAdapterImpl PauseNative");
    auto media = callback_wrapper_.GetCallback(callback_index_);
    if (!media) {
        WVLOG_E("PauseNative, ohmedia: media is null");
        return;
    }
    media->Pause();
}

void MediaAVSessionAdapterImpl::UnregisterMediaCastOutputDeviceCallback() {
    WVLOG_I("MediaAVSessionAdapterImpl::UnregisterMediaCastOutputDeviceCallback, enter");
    if (avSession_) {
        AVSession_ErrCode retReg = OH_AVSession_UnregisterOutputDeviceChangeCallback(avSession_,
                                       OutputDeviceChangeCallback);
        if (retReg != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("UnregisterOutputDeviceChangeCallback failed ret: %{public}d", retReg);
        }
    }
    MediaCastStopped();
}

bool MediaAVSessionAdapterImpl::GetAVCastController() {
    WVLOG_I("GetAVCastController enter.");
    if (!avSession_) {
        WVLOG_E("GetAVCastController avSession_ is nullptr.");
        return false;
    }
    AVSession_ErrCode ret = OH_AVSession_CreateAVCastController(avSession_, &avCastController_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_I("OH_AVSession_CreateAVCastController failed. ret: %{public}d", ret);
        return false;
    }
    return true;
}

bool MediaAVSessionAdapterImpl::Prepare(const MediaCastDescription& mediaCastDescription) {
    WVLOG_I("MediaAVSessionAdapterImpl::Prepare enter.");
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

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(avMediaDescriptionBuilder_,
        mediaCastDescription.mediaUri.c_str());
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(avMediaDescriptionBuilder_,
        mediaCastDescription.startPosition);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(avMediaDescriptionBuilder_,
        mediaCastDescription.mediaType);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetMediaType failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetTitle(avMediaDescriptionBuilder_,
        mediaCastDescription.title.c_str());
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetTitle failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(avMediaDescriptionBuilder_,
        mediaCastDescription.assetId.c_str());
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetAssetId failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri(avMediaDescriptionBuilder_,
        mediaCastDescription.albumUrl.c_str());
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(avMediaDescriptionBuilder_,
        &avMediaDescription_);
    if (ret != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription failed. ret: %{public}d", ret);
        return false;
    }

    avQueueItem_.itemId = 1; // Assign initial values only
    avQueueItem_.description = avMediaDescription_;

    return true;
}

bool MediaAVSessionAdapterImpl::StartCast() {
    WVLOG_I("OH_AVCastController_Start enter.");
    
    char* mediaUri;
    AVQueueItem_Result rets = OH_AVSession_AVMediaDescription_GetMediaUri(avQueueItem_.description, &mediaUri);
    if (rets != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescription_GetMediaUri failed. ret: %{public}d", rets);
        return false;
    }

    int startPosition;
    rets = OH_AVSession_AVMediaDescription_GetStartPosition(avQueueItem_.description, &startPosition);
    if (rets != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescription_GetStartPosition failed. ret: %{public}d", rets);
        return false;
    }

    int duration;
    rets = OH_AVSession_AVMediaDescription_GetDuration(avQueueItem_.description, &duration);
    if (rets != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescription_GetDuration failed. ret: %{public}d", rets);
        return false;
    }

    char* title;
    rets = OH_AVSession_AVMediaDescription_GetTitle(avQueueItem_.description, &title);
    if (rets != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescription_GetTitle failed. ret: %{public}d", rets);
        return false;
    }

    char* assetId;
    rets = OH_AVSession_AVMediaDescription_GetAssetId(avQueueItem_.description, &assetId);
    if (rets != AVQUEUEITEM_SUCCESS) {
        WVLOG_E("OH_AVSession_AVMediaDescription_GetAssetId failed. ret: %{public}d", rets);
        return false;
    }

    AVSession_ErrCode retErr = OH_AVCastController_Prepare(avCastController_, &avQueueItem_);
    if (retErr != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVCastController_Prepare failed. ret: %{public}d", retErr);
        return false;
    }
    if (!RegisterCallback()) {
        WVLOG_E("MediaAVSessionAdapterImpl::StartCast, RegisterCallback failed.");
        return false;
    }
    retErr = OH_AVCastController_Start(avCastController_, &avQueueItem_);
    if (retErr != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("OH_AVCastController_Start failed. ret: %{public}d", retErr);
        return false;
    }
    avCastStarted_ = true;

    return true;
}

bool MediaAVSessionAdapterImpl::RegisterCallback() {
    int filter = FILTER_STATE + FILTER_POSITION;
    AVSession_ErrCode errCode = OH_AVCastController_RegisterPlaybackStateChangedCallback(avCastController_, filter,
        &MediaAVSessionAdapterImpl::PlaybackStateChangedCallback, reinterpret_cast<void *>(avsession_callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl RegisterPlaybackStateChangedCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_RegisterMediaItemChangedCallback(avCastController_,
        &MediaAVSessionAdapterImpl::MediaItemChangeCallback, reinterpret_cast<void *>(avsession_callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl RegisterMediaItemChangedCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_RegisterSeekDoneCallback(avCastController_,
        &MediaAVSessionAdapterImpl::SeekDoneCallback, reinterpret_cast<void *>(avsession_callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl RegisterSeekDoneCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_RegisterEndOfStreamCallback(avCastController_,
        &MediaAVSessionAdapterImpl::EndOfStreamCallback, reinterpret_cast<void *>(avsession_callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl RegisterEndOfStreamCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_RegisterErrorCallback(avCastController_,
        &MediaAVSessionAdapterImpl::ErrorCallback, reinterpret_cast<void *>(avsession_callback_index_));
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl RegisterErrorCallback err: %{public}d", errCode);
        return false;
    }
    return true;
}

bool MediaAVSessionAdapterImpl::UnregisterCallback() {
    AVSession_ErrCode errCode = OH_AVCastController_UnregisterPlaybackStateChangedCallback(avCastController_,
        &MediaAVSessionAdapterImpl::PlaybackStateChangedCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl UnregisterPlaybackStateChangedCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_UnregisterMediaItemChangedCallback(avCastController_,
        &MediaAVSessionAdapterImpl::MediaItemChangeCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl UnregisterMediaItemChangedCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_UnregisterSeekDoneCallback(avCastController_,
        &MediaAVSessionAdapterImpl::SeekDoneCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl UnregisterSeekDoneCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_UnregisterEndOfStreamCallback(avCastController_,
        &MediaAVSessionAdapterImpl::EndOfStreamCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl UnregisterEndOfStreamCallback err: %{public}d", errCode);
        return false;
    }

    errCode = OH_AVCastController_UnregisterErrorCallback(avCastController_,
        &MediaAVSessionAdapterImpl::ErrorCallback);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("MediaAVSessionAdapterImpl UnregisterErrorCallback err: %{public}d", errCode);
        return false;
    }
    return true;
}

void MediaAVSessionAdapterImpl::UpdateUiPlayPosition(std::shared_ptr<MediaAVSessionAdapterImpl> adapter,
                                                     int64_t position, bool is_seek) {
    if (adapter == nullptr) {
        WVLOG_E("MediaAVSessionAdapterImpl avsession or avcast adapter is null, skip seek");
        return;
    }
    if (is_seek) {
        WVLOG_I("MediaAVSessionAdapterImpl UpdateUiPlayPosition isseek");
        adapter->UpdateUiPlayPosition(position);
        adapter->SetUilastUiTimeByClient(position);
        adapter->SetUiSeekingByClient(false);
        return;
    }

    if (position == 0) {
        return;
    }
    if (adapter->GetUiSeekingByClient()) {
        WVLOG_I("MediaAVSessionAdapterImpl UpdateUiPlayPosition is seeking, skip");
        return;
    }
    if (std::abs(position - adapter->GetUilastUiTimeByClient()) >= UiTIME_UPDATE_INTERVAL) {
        adapter->UpdateUiPlayPosition(position);
        adapter->SetUilastUiTimeByClient(position);
    }
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::PlaybackStateChangedCallback(OH_AVCastController* avcastcontroller,
    OH_AVSession_AVPlaybackState* playbackState, void* userData) {
    if (!userData) {
        WVLOG_E("PlaybackStateChangedCallback, userData is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<MediaAVSessionAdapterImpl> adapter = avsession_callback_wrapper_.GetCallback(callback_index);
    if (!adapter) {
        WVLOG_I("MediaAVSessionAdapterImpl: adapter is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    AVSession_PlaybackState avSessionPlaybackState;
    if (OH_AVSession_GetPlaybackState(playbackState, &avSessionPlaybackState) == AV_SESSION_ERR_SUCCESS) {
        adapter->UpdateUiPlayStateByClient(avSessionPlaybackState);
        if (avSessionPlaybackState != PLAYBACK_STATE_INITIAL) {
            adapter->SetUiPlayStateByClient(avSessionPlaybackState);
        }
    }

    AVSession_PlaybackPosition playbackPosition;
    if (OH_AVSession_GetPlaybackPosition(playbackState, &playbackPosition) == AV_SESSION_ERR_SUCCESS) {
        UpdateUiPlayPosition(adapter, playbackPosition.elapsedTime, false);
        if (playbackPosition.elapsedTime != 0) {
            adapter->SetUiPlayPositionByClient(playbackPosition);
        }
    }
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::MediaItemChangeCallback(OH_AVCastController* avcastcontroller,
    OH_AVSession_AVQueueItem* avQueueItem, void* userData) {
    WVLOG_I("MediaAVSessionAdapterImpl::MediaItemChangeCallback itemId");
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::SeekDoneCallback(OH_AVCastController* avcastcontroller,
    int32_t position, void* userData) {
    WVLOG_I("MediaAVSessionAdapterImpl::SeekDoneCallback");
    if (!userData) {
        WVLOG_E("SeekDoneCallback, userData is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<MediaAVSessionAdapterImpl> adapter = avsession_callback_wrapper_.GetCallback(callback_index);
    if (!adapter) {
        WVLOG_I("MediaAVSessionAdapterImpl:SeekDoneCallback adapter is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    WVLOG_I("MediaAVSessionAdapterImpl::SeekDoneCallback position:  %{public}d", position);
    UpdateUiPlayPosition(adapter, position, true);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::EndOfStreamCallback(OH_AVCastController* avcastcontroller,
    void* userData) {
    WVLOG_I("MediaAVSessionAdapterImpl::EndOfStreamCallback");
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::ErrorCallback(OH_AVCastController* avcastcontroller,
    void* userData, AVSession_ErrCode error) {
    WVLOG_I("MediaAVSessionAdapterImpl::ErrorCallback assetId %{public}d", error);
    if (!userData) {
        WVLOG_E("ErrorCallback, userData is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<MediaAVSessionAdapterImpl> adapter = avsession_callback_wrapper_.GetCallback(callback_index);
    if (!adapter) {
        WVLOG_I("MediaAVSessionAdapterImpl:SeekDoneCallback adapter is null");
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }
    if (error != AV_SESSION_ERR_SUCCESS) {
        adapter->is_error_ = true;
    } else {
        adapter->is_error_ = false;
    }
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

void MediaAVSessionAdapterImpl::PlayRemote() {
    WVLOG_I("MediaAVSessionAdapterImpl::PlayRemote");
    if (!avCastStarted_) {
        WVLOG_E("MediaAVSessionAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVSessionAdapterImpl PlayRemote avCastController_ is null");
        return;
    }

    AVSession_AVCastControlCommandType cmdType = AVSession_AVCastControlCommandType::CAST_CONTROL_CMD_PLAY;
    AVSession_ErrCode errCode = OH_AVCastController_SendCommonCommand(avCastController_, &cmdType);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_I("MediaAVSessionAdapterImpl SetPlaybackStateRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVSessionAdapterImpl::PauseRemote() {
    WVLOG_I("MediaAVSessionAdapterImpl::PauseRemote");
    if (!avCastStarted_) {
        WVLOG_E("MediaAVSessionAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVSessionAdapterImpl PauseRemote avCastController_ is null");
        return;
    }

    AVSession_AVCastControlCommandType cmdType = AVSession_AVCastControlCommandType::CAST_CONTROL_CMD_PAUSE;
    AVSession_ErrCode errCode =  OH_AVCastController_SendCommonCommand(avCastController_, &cmdType);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_I("MediaAVSessionAdapterImpl SetPlaybackStateRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackPositionRemote(const int32_t millis) {
    WVLOG_I("MediaAVSessionAdapterImpl::SetPlaybackPosition %{public}d", millis); 
    if (!avCastStarted_) {
        WVLOG_E("MediaAVSessionAdapterImpl PlayRemote avCastStarted_ is false");
        return;
    }
    if (!avCastController_) {
        WVLOG_E("MediaAVSessionAdapterImpl SetPlaybackPositionRemote avCastController_ is null");
        return;
    }

    AVSession_ErrCode errCode = OH_AVCastController_SendSeekCommand(avCastController_, millis);
    if (errCode != AV_SESSION_ERR_SUCCESS) {
        WVLOG_I("MediaAVSessionAdapterImpl SetPlaybackPositionRemote err. ret: %{public}d", errCode);
    }
}

void MediaAVSessionAdapterImpl::UpdateRemotePlayStateCast(bool is_playing) {
    WVLOG_I("MediaAVSessionAdapterImpl UpdateRemotePlayStateCast is_playing: %{public}d", is_playing);
    if(is_playing) {
        PlayRemote();
    } else {
       PauseRemote();
    }
}

void MediaAVSessionAdapterImpl::UpdateRemotePlayPositionCast(int64_t position) {
    is_seeking_ = true;
    SetPlaybackPositionRemote(static_cast<int32_t>(position));
}

AVSession_PlaybackState MediaAVSessionAdapterImpl::GetAVCastPlaybackState() {
    return playbackState_;
}

bool MediaAVSessionAdapterImpl::IsAvCastPlaying() {
    return playbackState_ == PLAYBACK_STATE_PLAYING;
}

int64_t MediaAVSessionAdapterImpl::GetPlaybackPosition() {
    return playbackPosition_.elapsedTime;
}

void MediaAVSessionAdapterImpl::SetAVCastUiPlayState(AVSession_PlaybackState& avSessionPlaybackState) {
    playbackState_ = avSessionPlaybackState;
}

void MediaAVSessionAdapterImpl::SetAVCastUiPlayPosition(AVSession_PlaybackPosition& playbackPosition) {
    playbackPosition_ = playbackPosition;
}

void MediaAVSessionAdapterImpl::SetAVCastUilastUiTime(int64_t position) {
    lastUiTime_ = position;
}

void MediaAVSessionAdapterImpl::SetAVCastUiSeeking(bool is_seeking) {
    is_seeking_ = is_seeking;
}

AVSession_PlaybackState MediaAVSessionAdapterImpl::GetAVCastUiPlayState() {
    return playbackState_;
}

AVSession_PlaybackPosition MediaAVSessionAdapterImpl::GetAVCastUiPlayPosition() {
    return playbackPosition_;
}

int64_t MediaAVSessionAdapterImpl::GetAVCastUilastUiTime() {
    return lastUiTime_;
}

bool MediaAVSessionAdapterImpl::GetAVCastUiSeeking() {
    return is_seeking_;
}

void MediaAVSessionAdapterImpl::SetAVCastDevice(const char* deviceName) {
    deviceName_ = deviceName ? std::string(deviceName) : std::string();
}

std::string MediaAVSessionAdapterImpl::GetAVCastDevice() {
    return deviceName_;
}

} // namespace OHOS::NWeb