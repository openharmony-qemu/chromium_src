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

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include <multimedia/player_framework/native_avscreen_capture.h>
#include <native_buffer/native_buffer.h>

namespace OHOS::NWeb {
std::unordered_map<int, std::queue<std::shared_ptr<SurfaceBufferAdapter>>> bufferAvailableQueueMap_;
std::unordered_map<int, std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>>> audioBufferAvailableQueueMap_;
const int MAX_QUEUE_SIZE = 20;
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

OH_AVScreenCaptureConfig ConvertScreenCaptureConfig(const std::shared_ptr<ScreenCaptureConfigAdapter> config)
{
    OH_AVScreenCaptureConfig avConfig;

    if (!config) {
        WVLOG_I("ConvertScreenCaptureConfig config is null");
        return avConfig;
    }

    avConfig.captureMode = GetOHCaptureMode(config->GetCaptureMode());
    avConfig.dataType = GetOHDataType(config->GetDataType());

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetMicCapInfo()) {
        avConfig.audioInfo.micCapInfo.audioSampleRate = config->GetAudioInfo()->GetMicCapInfo()->GetAudioSampleRate();
        avConfig.audioInfo.micCapInfo.audioChannels = config->GetAudioInfo()->GetMicCapInfo()->GetAudioChannels();
        avConfig.audioInfo.micCapInfo.audioSource =
            GetOHAudioCaptureSourceType(config->GetAudioInfo()->GetMicCapInfo()->GetAudioSource());
    }

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetInnerCapInfo()) {
        avConfig.audioInfo.innerCapInfo.audioSampleRate =
            config->GetAudioInfo()->GetInnerCapInfo()->GetAudioSampleRate();
        avConfig.audioInfo.innerCapInfo.audioChannels = config->GetAudioInfo()->GetInnerCapInfo()->GetAudioChannels();
        avConfig.audioInfo.innerCapInfo.audioSource =
            GetOHAudioCaptureSourceType(config->GetAudioInfo()->GetInnerCapInfo()->GetAudioSource());
    }

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetAudioEncInfo()) {
        avConfig.audioInfo.audioEncInfo.audioBitrate = config->GetAudioInfo()->GetAudioEncInfo()->GetAudioBitrate();
        avConfig.audioInfo.audioEncInfo.audioCodecformat =
            GetOHAudioCodecFormat(config->GetAudioInfo()->GetAudioEncInfo()->GetAudioCodecformat());
    }

    if (config->GetVideoInfo() && config->GetVideoInfo()->GetVideoCapInfo()) {
        avConfig.videoInfo.videoCapInfo.displayId = config->GetVideoInfo()->GetVideoCapInfo()->GetDisplayId();
        avConfig.videoInfo.videoCapInfo.videoFrameWidth =
            config->GetVideoInfo()->GetVideoCapInfo()->GetVideoFrameWidth();
        avConfig.videoInfo.videoCapInfo.videoFrameHeight =
            config->GetVideoInfo()->GetVideoCapInfo()->GetVideoFrameHeight();
        avConfig.videoInfo.videoCapInfo.videoSource =
            GetOHVideoSourceType(config->GetVideoInfo()->GetVideoCapInfo()->GetVideoSourceType());
    }

    if (config->GetVideoInfo() && config->GetVideoInfo()->GetVideoEncInfo()) {
        avConfig.videoInfo.videoEncInfo.videoCodec =
            GetOHVideoCodecFormat(config->GetVideoInfo()->GetVideoEncInfo()->GetVideoCodecFormat());
        avConfig.videoInfo.videoEncInfo.videoBitrate = config->GetVideoInfo()->GetVideoEncInfo()->GetVideoBitrate();
        avConfig.videoInfo.videoEncInfo.videoFrameRate = config->GetVideoInfo()->GetVideoEncInfo()->GetVideoFrameRate();
    }

    if (config->GetDataType() == DataTypeAdapter::CAPTURE_FILE_DATA_TYPE && config->GetRecorderInfo()) {
        avConfig.recorderInfo.url = const_cast<char*>(config->GetRecorderInfo()->GetUrl().c_str());
        avConfig.recorderInfo.fileFormat = GetOHContainerFormatType(config->GetRecorderInfo()->GetFileFormat());
    } else {
        avConfig.recorderInfo = {};
        avConfig.recorderInfo.url = nullptr;
    }

    return avConfig;
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

void ScreenCaptureCallbackOnError(OH_AVScreenCapture *capture, int32_t errorCode, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    WVLOG_I("OnError is called, errorCode %{public}d", errorCode);
    CallbackInfo* callbackInfo = (CallbackInfo*)userData;
    callbackInfo->callback->OnError(errorCode);
}

void PushBufferToVideoQueue(std::shared_ptr<SurfaceBufferAdapter> buffer, int nwebId)
{
    WVLOG_I("%{public}s enter", __FUNCTION__);
    // During the screen capture process, the audio buffer and video buffer are alternately
    // sent to the screen_capture_adapter object. therefore, it is only necessary to check if
    // the video queue exceeds the specified size.
    auto video_it = bufferAvailableQueueMap_.find(nwebId);
    if (video_it == bufferAvailableQueueMap_.end()) {
        std::queue<std::shared_ptr<SurfaceBufferAdapter>> bufferAvailableQueue;
        bufferAvailableQueue.push(std::move(buffer));
        bufferAvailableQueueMap_[nwebId] = bufferAvailableQueue;
        return;
    }
    WVLOG_I("%{public}s enter, video size = %{public}u", __FUNCTION__, video_it->second.size());
    video_it->second.push(std::move(buffer));
    if (video_it->second.size() <= MAX_QUEUE_SIZE) {
        WVLOG_I("%{public}s enter, video size =", __FUNCTION__);
        return;
    }
    video_it->second.pop();

    auto audio_it = audioBufferAvailableQueueMap_.find(nwebId);
    if (!audio_it->second.empty()) {
        audio_it->second.pop();
    }
}

void PushBufferToAudioQueue(std::shared_ptr<OH_AudioBufferAdapterImpl> buffer, int nwebId)
{
    WVLOG_I("%{public}s enter", __FUNCTION__);
    auto it = audioBufferAvailableQueueMap_.find(nwebId);
    if (it == audioBufferAvailableQueueMap_.end()) {
        std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>> bufferAvailableQueue;
        bufferAvailableQueue.push(std::move(buffer));
        audioBufferAvailableQueueMap_[nwebId] = bufferAvailableQueue;
        return;
    }
    it->second.push(std::move(buffer));
    if (it->second.size() <= MAX_QUEUE_SIZE) {
        WVLOG_I("%{public}s enter, video size =", __FUNCTION__);
        return;
    }
    it->second.pop();
}

void ScreenCaptureCallbackOnBufferAvailable(OH_AVScreenCapture *capture, OH_AVBuffer *buffer,
    OH_AVScreenCaptureBufferType bufferType, int64_t timestamp, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    CallbackInfo* callbackInfo = (CallbackInfo*)userData;
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
        auto tmpBuffer = std::make_shared<OH_SurfaceBufferAdapterImpl>(buffer, config);
        PushBufferToVideoQueue(std::move(tmpBuffer), callbackInfo->nweb_id);
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d", bufferType);
        callbackInfo->callback->OnVideoBufferAvailableV2(true, callbackInfo->nweb_id);
    } else if (bufferType == OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER) {
        auto audioBufferImpl = std::make_shared<OH_AudioBufferAdapterImpl>(
            buffer, timestamp, OH_AudioCaptureSourceType::OH_ALL_PLAYBACK);
        PushBufferToAudioQueue(std::move(audioBufferImpl), callbackInfo->nweb_id);
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d", bufferType);
        callbackInfo->callback->OnAudioBufferAvailableV2(
            true, ConvertAudioCaptureSourceType(OH_AudioCaptureSourceType::OH_ALL_PLAYBACK), callbackInfo->nweb_id);
    } else if (bufferType == OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_MIC) {
        WVLOG_D("OnBufferAvailable is called, buffer type = %{public}d will be ignored", bufferType);
    } else {
        WVLOG_E("OnBufferAvailable is called, buffer type = %{public}d is unknown", bufferType);
    }
}

void ScreenCaptureAdapterImpl::ClearBufferQueue(int nwebId) {
    WVLOG_I(" %{public}s enter", __FUNCTION__);
    auto audio_it = audioBufferAvailableQueueMap_.find(nwebId);
    if (audio_it != audioBufferAvailableQueueMap_.end()) {
        std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>>().swap(
            audio_it->second);
    }

    auto video_it = bufferAvailableQueueMap_.find(nwebId);
    if (video_it != bufferAvailableQueueMap_.end()) {
        std::queue<std::shared_ptr<SurfaceBufferAdapter>>().swap(
            video_it->second);
    }
}

void ScreenCaptureCallbackOnStateChange(struct OH_AVScreenCapture *capture,
    OH_AVScreenCaptureStateCode stateCode, void* userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    WVLOG_I("OnStateChange is called, stateCode %{public}d", stateCode);
    CallbackInfo* callbackInfo = (CallbackInfo*)userData;
    if (callbackInfo->callback == nullptr) {
        WVLOG_E("callback is null");
        return;
    }
    callbackInfo->callback->OnStateChangeV2(GetScreenCaptureStateCodeAdapter(stateCode), callbackInfo->nweb_id);
}

void ScreenCaptureCallbackOnDisplaySelected(OH_AVScreenCapture* capture,
                                            uint64_t displayId,
                                            void* userData) {
    WVLOG_I("%{public}s enter", __FUNCTION__);
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    WVLOG_I("OnDisplaySelected is called, displayId %{public}d", displayId);
    CallbackInfo* callbackInfo = static_cast<CallbackInfo*>(userData);
    if (callbackInfo->callback == nullptr) {
        WVLOG_E("callback is null");
        return;
    }
    callbackInfo->callback->OnDisplaySelectedV2(displayId,
                                                callbackInfo->nweb_id);
}

ScreenCaptureAdapterImpl::~ScreenCaptureAdapterImpl()
{
    auto video = bufferAvailableQueueMap_.find(callback_info_.nweb_id);
    if (video != bufferAvailableQueueMap_.end()) {
      bufferAvailableQueueMap_.erase(video);
    }

    auto audio = audioBufferAvailableQueueMap_.find(callback_info_.nweb_id);
    if (audio != audioBufferAvailableQueueMap_.end()) {
      audioBufferAvailableQueueMap_.erase(audio);
    }
    Release();
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
        return -1;
    }

    int32_t ret = OH_AVScreenCapture_Init(screenCapture_, ConvertScreenCaptureConfig(config));
    if (ret != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("OH_AVScreenCapture init failed, ret = %{public}d", ret);
        Release();
        return -1;
    }

    callback_info_.nweb_id = nweb_id;
    return 0;
}

void ScreenCaptureAdapterImpl::Release()
{
    if (!screenCapture_) {
        return;
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
    WVLOG_I("start screen capture[%{public}p] success", screenCapture_);
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
    WVLOG_I("stop screen capture[%{public}p] success", screenCapture_);
    return 0;
}

int32_t ScreenCaptureAdapterImpl::SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback)
{
    callback_info_.callback = std::move(callback);
    if (!screenCapture_ || !callback_info_.callback) {
        WVLOG_E("not init or param error");
        return -1;
    }
    int32_t errorRet = OH_AVScreenCapture_SetErrorCallback(screenCapture_,
        ScreenCaptureCallbackOnError, &callback_info_);
    if (errorRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, errorRet = %{public}d", errorRet);
        callback_info_.callback = nullptr;
        return -1;
    }
    int32_t dataRet = OH_AVScreenCapture_SetDataCallback(screenCapture_,
        ScreenCaptureCallbackOnBufferAvailable, &callback_info_);
    if (dataRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, dataRet = %{public}d", dataRet);
        callback_info_.callback = nullptr;
        return -1;
    }
    int32_t stateRet = OH_AVScreenCapture_SetStateCallback(screenCapture_,
        ScreenCaptureCallbackOnStateChange, &callback_info_);
    if (stateRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, stateRet = %{public}d", stateRet);
        callback_info_.callback = nullptr;
        return -1;
    }
    int32_t DisplayRet = OH_AVScreenCapture_SetDisplayCallback(screenCapture_,
         ScreenCaptureCallbackOnDisplaySelected, &callback_info_);
    if (DisplayRet != OH_AVSCREEN_CAPTURE_ErrCode::AV_SCREEN_CAPTURE_ERR_OK) {
        WVLOG_E("set callback failed, stateRet = %{public}d", DisplayRet);
        callback_info_.callback = nullptr;
        return -1;
    }
    return errorRet + dataRet + stateRet + DisplayRet;
}

std::shared_ptr<SurfaceBufferAdapter> ScreenCaptureAdapterImpl::AcquireVideoBuffer()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return nullptr;
    }

    if (bufferAvailableQueueMap_.find(callback_info_.nweb_id) == bufferAvailableQueueMap_.end()) {
        WVLOG_E("bufferAvailableQueue is not found");
        return nullptr;
    }

    if (bufferAvailableQueueMap_[callback_info_.nweb_id].empty()) {
        WVLOG_E("bufferAvailableQueue is empty");
        return nullptr;
    }
    auto surfaceBufferImpl = std::move(bufferAvailableQueueMap_[callback_info_.nweb_id].front());
    if (!surfaceBufferImpl) {
        WVLOG_E("buffer is nullptr");
        return nullptr;
    }
    bufferAvailableQueueMap_[callback_info_.nweb_id].pop();
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

    if (audioBufferAvailableQueueMap_.find(callback_info_.nweb_id) == audioBufferAvailableQueueMap_.end()) {
        WVLOG_E("audiobuffer is nullptr");
        return -1;
    }

    if (audioBufferAvailableQueueMap_[callback_info_.nweb_id].empty()) {
        WVLOG_E("audioBufferAvailableQueue is empty");
        return -1;
    }

    auto audiobufferImpl = std::move(audioBufferAvailableQueueMap_[callback_info_.nweb_id].front());
    if (audiobufferImpl->avBuffer_ == nullptr) {
        WVLOG_E("ScreenCaptureAdapterImpl::AcquireAudioBuffer, avBuffer is bullptr");
        return -1;
    }

    audiobuffer->SetBuffer(std::move(audiobufferImpl->avBuffer_));
    audiobuffer->SetLength(audiobufferImpl->length_);
    audiobuffer->SetTimestamp(audiobufferImpl->timestamp_);
    audiobuffer->SetSourcetype(ConvertAudioCaptureSourceType(audiobufferImpl->sourcetype_));
    audiobufferImpl->avBuffer_ = nullptr;
    audioBufferAvailableQueueMap_[callback_info_.nweb_id].pop();

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
    // avBuffer will be released soon, we must copy in local
    size_ = static_cast<uint32_t>(OH_AVBuffer_GetCapacity(avBuffer));
    if (size_ <= 0) {
        WVLOG_E("buffer size = %{public}d is illegal", size_);
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
        return;
    }
    memcpy_s(avBuffer_, size_, bufferAddr, size_);
}

OH_SurfaceBufferAdapterImpl::~OH_SurfaceBufferAdapterImpl()
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
    OH_AVCodecBufferAttr info;
    OH_AVBuffer_GetBufferAttr(avBuffer, &info);
    // avBuffer will be released soon, we must copy in local
    length_ = static_cast<uint32_t>(OH_AVBuffer_GetCapacity(avBuffer));
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
        return;
    }
    memcpy_s(avBuffer_, length_, bufferAddr, length_);
}

OH_AudioBufferAdapterImpl::~OH_AudioBufferAdapterImpl()
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
