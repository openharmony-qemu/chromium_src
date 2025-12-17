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
constexpr int64_t TIME_OUT = 0;
constexpr int64_t URL_NUM = 2;

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

    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
    avMetadata_ = nullptr;
    builder_ = nullptr;
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
    {
        std::lock_guard<std::mutex> lock(avsession_mutex_);
        if (avSession_) {
            AVSession_ErrCode ret = OH_AVSession_Destroy(avSession_);
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("DestroyAVSession Destroy() failed, ret: %{public}d", ret);
            } else {
                WVLOG_I("DestroyAVSession Destroy() success, ret: %{public}d", ret);
            }
            avSession_ = nullptr;
        }
    }
    if (avSessionKey_) {
        auto iter = avSessionMap.find(avSessionKey_->ToString());
        if (iter != avSessionMap.end()) {
            avSessionMap.erase(iter);
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
        WVLOG_E("ohmedia: media is null");
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
        WVLOG_E("ohmedia: media is null");
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
    AVSession_ErrCode ret = OH_AVSession_Destroy(iter->second->avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("DestroyAndEraseSession Destroy failed, ret: %{public}d", ret);
    } else {
        WVLOG_I("DestroyAndEraseSession Destroy success");
    }
    // clear adapter->avSession, otherwise it will crash when callback
    iter->second->avSession_ = nullptr;

    avSession_ = nullptr;
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
    }

    avSessionKey_->SetType(type);
    avSessionMap.insert(std::pair<std::string, MediaAVSessionAdapterImpl *>((avSessionKey_->ToString()),
        this));
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

} // namespace OHOS::NWeb