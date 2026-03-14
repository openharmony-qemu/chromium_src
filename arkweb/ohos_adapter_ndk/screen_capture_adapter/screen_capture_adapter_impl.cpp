/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screen_capture_adapter_impl.h"

#include <shared_mutex>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include <multimedia/player_framework/native_avscreen_capture.h>
#include <native_buffer/native_buffer.h>

namespace OHOS::NWeb {
std::shared_mutex surface_map_lock_;
std::shared_mutex audio_map_lock_;
std::unordered_map<int, std::queue<std::shared_ptr<SurfaceBufferAdapter>>> bufferAvailableQueueMap_;
std::unordered_map<int, std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>>> audioBufferAvailableQueueMap_;
OH_AVScreenCaptureConfig avConfig_;
namespace {
OH_CaptureMode GetOHCaptureMode(const CaptureModeAdapter& mode)
{
    switch (mode) {
        case CaptureModeAdapter::CAPTURE_HOME_SCREEN:
            return OH_CaptureMode::OH_CAPTURE_HOME_SCREEN;
        case CaptureModeAdapter::CAPTURE_SPECIFIED_SCREEN:
            return OH_CaptureMode::OH_CAPTURE_SPECIFIED_SCREEN;
        case CaptureModeAdapter::CAPTURE_SPECIFIED_WINDOW:
            return OH_CaptureMode::OH_CAPTURE_SPECIFIED_WINDOW;
        default:
            break;
    }
    return OH_CaptureMode::OH_CAPTURE_INVAILD;
}

OH_DataType GetOHDataType(const DataTypeAdapter& type)
{
    switch (type) {
        case DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE:
            return OH_DataType::OH_ORIGINAL_STREAM;
        case DataTypeAdapter::ENCODED_STREAM_DATA_TYPE:
            return OH_DataType::OH_ENCODED_STREAM;
        case DataTypeAdapter::CAPTURE_FILE_DATA_TYPE:
            return OH_DataType::OH_CAPTURE_FILE;
        default:
            break;
    }
    return OH_DataType::OH_INVAILD;
}

OH_AudioCaptureSourceType GetOHAudioCaptureSourceType(const AudioCaptureSourceTypeAdapter& type)
{
    switch (type) {
        case AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT:
            return OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT;
        case AudioCaptureSourceTypeAdapter::MIC:
            return OH_AudioCaptureSourceType::OH_MIC;
        case AudioCaptureSourceTypeAdapter::ALL_PLAYBACK:
            return OH_AudioCaptureSourceType::OH_ALL_PLAYBACK;
        case AudioCaptureSourceTypeAdapter::APP_PLAYBACK:
            return OH_AudioCaptureSourceType::OH_APP_PLAYBACK;
        default:
            break;
    }
    return OH_AudioCaptureSourceType::OH_SOURCE_INVALID;
}

OH_AudioCodecFormat GetOHAudioCodecFormat(const AudioCodecFormatAdapter& format)
{
    switch (format) {
        case AudioCodecFormatAdapter::AUDIO_DEFAULT:
            return OH_AudioCodecFormat::OH_AUDIO_DEFAULT;
        case AudioCodecFormatAdapter::AAC_LC:
            return OH_AudioCodecFormat::OH_AAC_LC;
        default:
            break;
    }
    return OH_AudioCodecFormat::OH_AUDIO_CODEC_FORMAT_BUTT;
}

OH_VideoSourceType GetOHVideoSourceType(const VideoSourceTypeAdapter& type)
{
    switch (type) {
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_YUV:
            return OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_YUV;
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_ES:
            return OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_ES;
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA:
            return OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_RGBA;
        default:
            break;
    }
    return OH_VideoSourceType::OH_VIDEO_SOURCE_BUTT;
}

OH_VideoCodecFormat GetOHVideoCodecFormat(const VideoCodecFormatAdapter& format)
{
    switch (format) {
        case VideoCodecFormatAdapter::VIDEO_DEFAULT:
            return OH_VideoCodecFormat::OH_VIDEO_DEFAULT;
        case VideoCodecFormatAdapter::H264:
            return OH_VideoCodecFormat::OH_H264;
        case VideoCodecFormatAdapter::MPEG4:
            return OH_VideoCodecFormat::OH_MPEG4;
        default:
            break;
    }
    return OH_VideoCodecFormat::OH_VIDEO_CODEC_FORMAT_BUTT;
}

OH_ContainerFormatType GetOHContainerFormatType(const ContainerFormatTypeAdapter& type)
{
    switch (type) {
        case ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE:
            return OH_ContainerFormatType::CFT_MPEG_4A;
        default:
            break;
    }
    return OH_ContainerFormatType::CFT_MPEG_4;
}

void SetAudioInfo(std::shared_ptr<AudioInfoAdapter> audio_info) {
    if (!audio_info) {
        WVLOG_E("SetAudioInfo audio_info is null");
        return;
    }

    if (audio_info->GetMicCapInfo()) {
        avConfig_.audioInfo.micCapInfo.audioSampleRate = audio_info->GetMicCapInfo()->GetAudioSampleRate();
        avConfig_.audioInfo.micCapInfo.audioChannels = audio_info->GetMicCapInfo()->GetAudioChannels();
        avConfig_.audioInfo.micCapInfo.audioSource =
            GetOHAudioCaptureSourceType(audio_info->GetMicCapInfo()->GetAudioSource());
    }

    if (audio_info->GetInnerCapInfo()) {
        avConfig_.audioInfo.innerCapInfo.audioSampleRate =
            audio_info->GetInnerCapInfo()->GetAudioSampleRate();
        avConfig_.audioInfo.innerCapInfo.audioChannels = audio_info->GetInnerCapInfo()->GetAudioChannels();
        avConfig_.audioInfo.innerCapInfo.audioSource =
            GetOHAudioCaptureSourceType(audio_info->GetInnerCapInfo()->GetAudioSource());
    }

    if (audio_info->GetAudioEncInfo()) {
        avConfig_.audioInfo.audioEncInfo.audioBitrate = audio_info->GetAudioEncInfo()->GetAudioBitrate();
        avConfig_.audioInfo.audioEncInfo.audioCodecformat =
            GetOHAudioCodecFormat(audio_info->GetAudioEncInfo()->GetAudioCodecformat());
    }
}

void SetVideoInfo(std::shared_ptr<VideoInfoAdapter> video_info) {
    if (!video_info) {
        WVLOG_E("SetVideoInfo video_info is null");
        return;
    }

    if (video_info->GetVideoCapInfo()) {
        avConfig_.videoInfo.videoCapInfo.displayId = video_info->GetVideoCapInfo()->GetDisplayId();
        avConfig_.videoInfo.videoCapInfo.videoFrameWidth =
            video_info->GetVideoCapInfo()->GetVideoFrameWidth();
        avConfig_.videoInfo.videoCapInfo.videoFrameHeight =
            video_info->GetVideoCapInfo()->GetVideoFrameHeight();
        avConfig_.videoInfo.videoCapInfo.videoSource =
            GetOHVideoSourceType(video_info->GetVideoCapInfo()->GetVideoSourceType());
    }

    if (video_info->GetVideoEncInfo()) {
        avConfig_.videoInfo.videoEncInfo.videoCodec =
            GetOHVideoCodecFormat(video_info->GetVideoEncInfo()->GetVideoCodecFormat());
        avConfig_.videoInfo.videoEncInfo.videoBitrate = video_info->GetVideoEncInfo()->GetVideoBitrate();
        avConfig_.videoInfo.videoEncInfo.videoFrameRate = video_info->GetVideoEncInfo()->GetVideoFrameRate();
    }
}

OH_AVScreenCaptureConfig ConvertScreenCaptureConfig(const std::shared_ptr<ScreenCaptureConfigAdapter> config)
{
    if (!config) {
        WVLOG_E("ConvertScreenCaptureConfig config is null");
        return avConfig_;
    }

    avConfig_.captureMode = GetOHCaptureMode(config->GetCaptureMode());
    avConfig_.dataType = GetOHDataType(config->GetDataType());
    SetAudioInfo(config->GetAudioInfo());
    SetVideoInfo(config->GetVideoInfo());

    if (config->GetDataType() == DataTypeAdapter::CAPTURE_FILE_DATA_TYPE && config->GetRecorderInfo()) {
        const std::string& sourceUrl = config->GetRecorderInfo()->GetUrl();
        avConfig_.recorderInfo.urlLen = static_cast<uint32_t>(sourceUrl.length() + 1);
        avConfig_.recorderInfo.url = new char[avConfig_.recorderInfo.urlLen];
        errno_t err = strncpy_s(
            avConfig_.recorderInfo.url,
            avConfig_.recorderInfo.urlLen,
            sourceUrl.c_str(),
            sourceUrl.length()
        );
        if (err != 0) {
            delete[] avConfig_.recorderInfo.url;
            avConfig_.recorderInfo.url = nullptr;
            avConfig_.recorderInfo.urlLen = 0;
            WVLOG_E("ConvertScreenCaptureConfig strncpy_c recorderInfo is error");
        } else {
            avConfig_.recorderInfo.url[avConfig_.recorderInfo.urlLen - 1] = '\0';
        }
        avConfig_.recorderInfo.fileFormat = GetOHContainerFormatType(config->GetRecorderInfo()->GetFileFormat());
    } else {
        avConfig_.recorderInfo = {};
        avConfig_.recorderInfo.url = nullptr;
    }

    return avConfig_;
}

ScreenCaptureStateCodeAdapter GetScreenCaptureStateCodeAdapter(const OH_AVScreenCaptureStateCode& stateCode)
{
    switch (stateCode) {
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STARTED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_CANCELED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STOPPED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STOPPED_BY_CALL:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE;
        case OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;
        default:
            break;
    }
    return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
}

AudioCaptureSourceTypeAdapter ConvertAudioCaptureSourceType(const OH_AudioCaptureSourceType& type)
{
    switch (type) {
        case OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT:
            return AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
        case OH_AudioCaptureSourceType::OH_MIC:
            return AudioCaptureSourceTypeAdapter::MIC;
        case OH_AudioCaptureSourceType::OH_ALL_PLAYBACK:
            return AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
        case OH_AudioCaptureSourceType::OH_APP_PLAYBACK:
            return AudioCaptureSourceTypeAdapter::APP_PLAYBACK;
        default:
            return AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
    }
}
} // namespace

CallbackSharedWrapper<CallbackInfo> ScreenCaptureAdapterImpl::callback_wrapper_;

void ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnError(
    OH_AVScreenCapture *capture, int32_t errorCode, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    auto callbackInfo = callback_wrapper_.GetCallback(callback_index);
    if (!callbackInfo) {
       WVLOG_E("ScreenCaptureCallbackOnError callbackInfo is nullptr");
       return;
    }
    if (callbackInfo->callback == nullptr) {
        WVLOG_E("callback is null");
        return;
    }
    WVLOG_I("OnError is called, errorCode %{public}d", errorCode);
    callbackInfo->callback->OnError(errorCode);
}

void ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(OH_AVScreenCapture *capture,
    OH_AVBuffer *buffer, OH_AVScreenCaptureBufferType bufferType, int64_t timestamp, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    if (buffer == nullptr) {
        WVLOG_E("OH_AVBuffer is null");
        return;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    auto callbackInfo = callback_wrapper_.GetCallback(callback_index);
    if (!callbackInfo) {
       WVLOG_E("ScreenCaptureCallbackOnBufferAvailable callbackInfo is nullptr");
       return;
    }
    if (callbackInfo->callback == nullptr) {
        WVLOG_E("callback is null");
        return;
    }
    // chromium do not use audio buffer, just use video buffer
    if (bufferType == OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO) {
        OH_NativeBuffer* nativeBuffer = OH_AVBuffer_GetNativeBuffer(buffer);
        OH_NativeBuffer_Config config;
        OH_NativeBuffer_GetConfig(nativeBuffer, &config);
        int32_t ret = OH_NativeBuffer_Unreference(nativeBuffer);
        if (ret != 0) {
            WVLOG_E("release native buffer failed, ret = %{public}d", ret);
        }
        auto surfaceBufferImpl = std::make_shared<OH_SurfaceBufferAdapterImpl>(buffer, config);

        std::unique_lock<std::shared_mutex> lock(surface_map_lock_);
        auto it = bufferAvailableQueueMap_.find(callbackInfo->nweb_id);	
        if (it != bufferAvailableQueueMap_.end()) {
            it->second.push(std::move(surfaceBufferImpl));
        } else {
            std::queue<std::shared_ptr<SurfaceBufferAdapter>> bufferAvailableQueue;
            bufferAvailableQueue.push(std::move(surfaceBufferImpl));
            bufferAvailableQueueMap_[callbackInfo->nweb_id] = bufferAvailableQueue;
        }
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d", bufferType);
        callbackInfo->callback->OnVideoBufferAvailableV2(true, callbackInfo->nweb_id);
    } else if (bufferType == OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER) {
        auto audioBufferImpl = std::make_shared<OH_AudioBufferAdapterImpl>(
            buffer, timestamp, OH_AudioCaptureSourceType::OH_ALL_PLAYBACK);
        
        std::unique_lock<std::shared_mutex> lock(audio_map_lock_);
        auto it = audioBufferAvailableQueueMap_.find(callbackInfo->nweb_id);	
        if (it != audioBufferAvailableQueueMap_.end()) {
            it->second.push(std::move(audioBufferImpl));
        } else {
            std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>> audioBufferAvailableQueue;
            audioBufferAvailableQueue.push(std::move(audioBufferImpl));
            audioBufferAvailableQueueMap_[callbackInfo->nweb_id] = audioBufferAvailableQueue;
        }
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d", bufferType);
        callbackInfo->callback->OnAudioBufferAvailableV2(
            true, ConvertAudioCaptureSourceType(OH_AudioCaptureSourceType::OH_ALL_PLAYBACK), callbackInfo->nweb_id);
    } else if (bufferType == OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_MIC) {
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d will be ignored", bufferType);
    } else {
        WVLOG_E("OnBufferAvailable is called, buffer type = %{public}d is unknown", bufferType);
    }
}

void ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnStateChange(struct OH_AVScreenCapture *capture,
    OH_AVScreenCaptureStateCode stateCode, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    size_t callback_index = reinterpret_cast<size_t>(userData);
    auto callbackInfo = callback_wrapper_.GetCallback(callback_index);
    if (!callbackInfo) {
       WVLOG_E("ScreenCaptureCallbackOnBufferAvailable callbackInfo is nullptr");
       return;
    }
    if (callbackInfo->callback == nullptr) {
        WVLOG_E("callback is null");
        return;
    }
    WVLOG_I("OnStateChange is called, stateCode %{public}d", stateCode);
    callbackInfo->callback->OnStateChangeV2(GetScreenCaptureStateCodeAdapter(stateCode), callbackInfo->nweb_id);
}

ScreenCaptureAdapterImpl::~ScreenCaptureAdapterImpl()
{
    WVLOG_I("ScreenCaptureAdapterImpl::~ScreenCaptureAdapterImpl, nweb_id = %{public}d", nweb_id_);
    {
        std::unique_lock<std::shared_mutex> lock(surface_map_lock_);
        auto video = bufferAvailableQueueMap_.find(nweb_id_);
        if (video != bufferAvailableQueueMap_.end()) {
            bufferAvailableQueueMap_.erase(video);
        }
    }

    {
        std::unique_lock<std::shared_mutex> lock(audio_map_lock_);
        auto audio = audioBufferAvailableQueueMap_.find(nweb_id_);
        if (audio != audioBufferAvailableQueueMap_.end()) {
            audioBufferAvailableQueueMap_.erase(audio);
        }
    }
    Release();
    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
}

int32_t ScreenCaptureAdapterImpl::InitV2(const std::shared_ptr<ScreenCaptureConfigAdapter> config, int nweb_id)
{
    if (screenCapture_) {
        return 0;
    }
    screenCapture_ = OH_AVScreenCapture_Create();
    if (!screenCapture_) {
        WVLOG_E("OH_AVScreenCapture create failed");
        return -1;
    }

    if (!config) {
        WVLOG_E("config is null");
        Release();
        return -1;
    }

    int32_t ret = OH_AVScreenCapture_Init(screenCapture_, ConvertScreenCaptureConfig(config));
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("OH_AVScreenCapture init failed, ret = %{public}d", ret);
        Release();
        return -1;
    }

    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
    std::shared_ptr<CallbackInfo> callback_info = std::make_shared<CallbackInfo>();
    callback_info->nweb_id = nweb_id;
    nweb_id_ = nweb_id;
    callback_index_ = callback_wrapper_.AddCallback(callback_info);
    return 0;
}

void ScreenCaptureAdapterImpl::Release()
{
    if (!screenCapture_) {
        return;
    }
    if (avConfig_.recorderInfo.url != nullptr) {
        delete[] avConfig_.recorderInfo.url;
        avConfig_.recorderInfo.url = nullptr;
     }
    int32_t ret = OH_AVScreenCapture_Release(screenCapture_);
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("OH_AVScreenCapture release failed, ret = %{public}d", ret);
    }
    screenCapture_ = nullptr;
}

int32_t ScreenCaptureAdapterImpl::SetMicrophoneEnable(bool enable)
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = OH_AVScreenCapture_SetMicrophoneEnabled(screenCapture_, enable);
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set microphone enabled failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::StartCapture()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = OH_AVScreenCapture_SetCanvasRotation(screenCapture_, true);
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("start capture SetCanvasRotation failed, ret = %{public}d", ret);
        return -1;
    }
    ret = OH_AVScreenCapture_StartScreenCapture(screenCapture_);
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("start capture failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::StopCapture()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = OH_AVScreenCapture_StopScreenCapture(screenCapture_);
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("stop capture failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback)
{
    auto callback_info = callback_wrapper_.GetCallback(callback_index_);
    if (!callback_info) {
        WVLOG_E("SetCaptureCallback callback_info is nullptr");
        return -1;
    }
    callback_info->callback = std::move(callback);
    if (!screenCapture_ || !callback_info->callback) {
        WVLOG_E("not init or param error");
        return -1;
    }
    int32_t errorRet = OH_AVScreenCapture_SetErrorCallback(screenCapture_,
        ScreenCaptureCallbackOnError, reinterpret_cast<void*>(callback_index_));
    if (errorRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, errorRet = %{public}d", errorRet);
        callback_info->callback = nullptr;
        return -1;
    }
    int32_t dataRet = OH_AVScreenCapture_SetDataCallback(screenCapture_,
        ScreenCaptureCallbackOnBufferAvailable, reinterpret_cast<void*>(callback_index_));
    if (dataRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, dataRet = %{public}d", dataRet);
        callback_info->callback = nullptr;
        return -1;
    }
    int32_t stateRet = OH_AVScreenCapture_SetStateCallback(screenCapture_,
        ScreenCaptureCallbackOnStateChange, reinterpret_cast<void*>(callback_index_));
    if (stateRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, stateRet = %{public}d", stateRet);
        callback_info->callback = nullptr;
        return -1;
    }
    return errorRet + dataRet + stateRet;
}

std::shared_ptr<SurfaceBufferAdapter> ScreenCaptureAdapterImpl::AcquireVideoBuffer()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return nullptr;
    }

    std::unique_lock<std::shared_mutex> lock(surface_map_lock_);
    auto video = bufferAvailableQueueMap_.find(nweb_id_);
    if (video == bufferAvailableQueueMap_.end()) {
        WVLOG_E("bufferAvailableQueue is not found, nwebId=%{public}d", nweb_id_);
        return nullptr;
    }

    if (video->second.empty()) {
        WVLOG_E("bufferAvailableQueue is empty, nwebId=%{public}d", nweb_id_);
        return nullptr;
    }
    auto surfaceBufferImpl = std::move(video->second.front());
    video->second.pop();
    if (!surfaceBufferImpl) {
        WVLOG_E("surfaceBufferImpl is nullptr");
        return nullptr;
    }
    return surfaceBufferImpl;
}

int32_t ScreenCaptureAdapterImpl::ReleaseVideoBuffer()
{
    // do not need manual release buffer, move to ~OH_SurfaceBufferAdapterImpl()
    return 0;
}

int32_t ScreenCaptureAdapterImpl::AcquireAudioBuffer(
    std::shared_ptr<AudioBufferAdapter> audiobuffer, AudioCaptureSourceTypeAdapter type)
{
    if (screenCapture_ == nullptr) {
        WVLOG_E("not init");
        return -1;
    }

    if (audiobuffer == nullptr) {
        WVLOG_E("audiobuffer is nullptr");
        return -1;
    }

    std::unique_lock<std::shared_mutex> lock(audio_map_lock_);
    auto audio = audioBufferAvailableQueueMap_.find(nweb_id_);
    if (audio == audioBufferAvailableQueueMap_.end()) {
        WVLOG_E("audioBufferAvailableQueue is not found, nwebId=%{public}d", nweb_id_);
        return -1;
    }

    if (audio->second.empty()) {
        WVLOG_E("audioBufferAvailableQueue is empty, nwebId=%{public}d", nweb_id_);
        return -1;
    }

    auto audiobufferImpl = std::move(audio->second.front());
    audio->second.pop();
    if (audiobufferImpl->avBuffer_ == nullptr) {
        WVLOG_E("ScreenCaptureAdapterImpl::AcquireAudioBuffer, avBuffer is bullptr");
        return -1;
    }

    audiobuffer->SetBuffer(std::move(audiobufferImpl->avBuffer_));
    audiobuffer->SetLength(audiobufferImpl->length_);
    audiobuffer->SetTimestamp(audiobufferImpl->timestamp_);
    audiobuffer->SetSourcetype(ConvertAudioCaptureSourceType(audiobufferImpl->sourcetype_));
    audiobufferImpl->avBuffer_ = nullptr;

    return 0;
}

int32_t ScreenCaptureAdapterImpl::ReleaseAudioBuffer(AudioCaptureSourceTypeAdapter type)
{
    // do not need manual release buffer, move to ~OH_AudioBufferAdapterImpl()
    return 0;
}

OH_SurfaceBufferAdapterImpl::OH_SurfaceBufferAdapterImpl(OH_AVBuffer* avBuffer,
    OH_NativeBuffer_Config config) : config_(config)
{
    InitSurfaceBufferAdapterImpl(avBuffer);
}

void OH_SurfaceBufferAdapterImpl::InitSurfaceBufferAdapterImpl(OH_AVBuffer* avBuffer)
{
    // avBuffer will be released soon, we must copy in local
    size_ = static_cast<uint32_t>(OH_AVBuffer_GetCapacity(avBuffer));
    if (size_ == 0) {
        WVLOG_E("buffer size = %{public}u is illegal", size_);
        return;
    }
    avBuffer_ = malloc(size_);
    if (avBuffer_ == nullptr) {
        WVLOG_E("buffer malloc failed, size = %{public}d", size_);
        return;
    }
    void* bufferAddr = OH_AVBuffer_GetAddr(avBuffer);
    if (!bufferAddr) {
        WVLOG_E("buffer address not exists");
        ReleaseSurfaceBuffer();
        return;
    }
    if (memcpy_s(avBuffer_, size_, bufferAddr, size_) != EOK) {
        WVLOG_E("OH_SurfaceBufferAdapterImpl memcpy_s is failed");
        ReleaseSurfaceBuffer();
        return;
    }
}

OH_SurfaceBufferAdapterImpl::~OH_SurfaceBufferAdapterImpl()
{
    ReleaseSurfaceBuffer();
}

void OH_SurfaceBufferAdapterImpl::ReleaseSurfaceBuffer()
{
    if (avBuffer_) {
        free(avBuffer_);
        avBuffer_ = nullptr;
    }
}

int32_t OH_SurfaceBufferAdapterImpl::GetFileDescriptor()
{
    return config_.usage;
}

int32_t OH_SurfaceBufferAdapterImpl::GetWidth()
{
    return config_.width;
}

int32_t OH_SurfaceBufferAdapterImpl::GetHeight()
{
    return config_.height;
}

int32_t OH_SurfaceBufferAdapterImpl::GetStride()
{
    return config_.stride;
}

int32_t OH_SurfaceBufferAdapterImpl::GetFormat()
{
    return config_.format;
}

uint32_t OH_SurfaceBufferAdapterImpl::GetSize()
{
    return size_;
}

void* OH_SurfaceBufferAdapterImpl::GetVirAddr()
{
    return avBuffer_;
}

OH_AudioBufferAdapterImpl::OH_AudioBufferAdapterImpl(OH_AVBuffer* avBuffer,
    int64_t timestamp, OH_AudioCaptureSourceType sourcetype) : timestamp_(timestamp), sourcetype_(sourcetype)
{
    InitAudioBufferAdapterImpl(avBuffer);
}

void OH_AudioBufferAdapterImpl::InitAudioBufferAdapterImpl(OH_AVBuffer* avBuffer)
{
    OH_AVCodecBufferAttr info;
    if (!avBuffer) {
        WVLOG_E("InitAudioBufferAdapterImpl avBuffer is nullptr");
        return;
    }
    OH_AVBuffer_GetBufferAttr(avBuffer, &info);
    // avBuffer will be released soon, we must copy in local
    length_ = static_cast<int32_t>(OH_AVBuffer_GetCapacity(avBuffer));
    if (length_ <= 0) {
        WVLOG_E("audio buffer size = %{public}d is illegal", length_);
        return;
    }
    avBuffer_ = (uint8_t*)malloc(length_);
    if (avBuffer_ == nullptr) {
        WVLOG_E("audio buffer malloc failed, size = %{public}d", length_);
        return;
    }
    uint8_t* bufferAddr = OH_AVBuffer_GetAddr(avBuffer);
    if (!bufferAddr) {
        WVLOG_E("audio buffer address not exists");
        ReleaseAudioBuffer();
        return;
    }
    if (memcpy_s(avBuffer_, length_, bufferAddr, length_) != EOK) {
        WVLOG_E("OH_SurfaceBufferAdapterImpl memcpy_s is failed");
        ReleaseAudioBuffer();
        return;
    }
}

OH_AudioBufferAdapterImpl::~OH_AudioBufferAdapterImpl()
{
    ReleaseAudioBuffer();
}

void OH_AudioBufferAdapterImpl::ReleaseAudioBuffer()
{
    if (avBuffer_) {
        free(avBuffer_);
        avBuffer_ = nullptr;
    }
}

uint8_t* OH_AudioBufferAdapterImpl::GetBuffer()
{
    return avBuffer_;
}

int32_t OH_AudioBufferAdapterImpl::GetLength()
{
    return length_;
}

int64_t OH_AudioBufferAdapterImpl::GetTimestamp()
{
    return timestamp_;
}

AudioCaptureSourceTypeAdapter OH_AudioBufferAdapterImpl::GetSourcetype()
{
    return ConvertAudioCaptureSourceType(sourcetype_);
}

void OH_AudioBufferAdapterImpl::SetBuffer(uint8_t* buffer)
{
}

void OH_AudioBufferAdapterImpl::SetLength(int32_t length)
{
}

void OH_AudioBufferAdapterImpl::SetTimestamp(int64_t timestamp)
{
}

void OH_AudioBufferAdapterImpl::SetSourcetype(AudioCaptureSourceTypeAdapter sourcetype)
{
}

} // namespace OHOS::NWeb
