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

#include "init_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "ohos_adapter_helper.h"
#include "screen_capture_adapter_impl.h"

using namespace OHOS::NWeb;

class MockVideoEncInfoAdapter : public VideoEncInfoAdapter {
public:
    MockVideoEncInfoAdapter() = default;

    VideoCodecFormatAdapter GetVideoCodecFormat() override
    {
        return videoCodec;
    }

    int32_t GetVideoBitrate() override
    {
        return videoBitrate;
    }

    int32_t GetVideoFrameRate() override
    {
        return videoFrameRate;
    }

    VideoCodecFormatAdapter videoCodec = VideoCodecFormatAdapter::VIDEO_DEFAULT;
    int32_t videoBitrate = 0;
    int32_t videoFrameRate = 0;
};

class MockVideoCaptureInfoAdapter : public VideoCaptureInfoAdapter {
public:
    MockVideoCaptureInfoAdapter() = default;

    uint64_t GetDisplayId() override
    {
        return displayId;
    }

    std::list<int32_t> GetTaskIDs() override
    {
        return taskIDs;
    }

    int32_t GetVideoFrameWidth() override
    {
        return videoFrameWidth;
    }

    int32_t GetVideoFrameHeight() override
    {
        return videoFrameHeight;
    }

    VideoSourceTypeAdapter GetVideoSourceType() override
    {
        return videoSource;
    }

    uint64_t displayId = 0;
    std::list<int32_t> taskIDs;
    int32_t videoFrameWidth = 0;
    int32_t videoFrameHeight = 0;
    VideoSourceTypeAdapter videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
};

class VideoInfoAdapterMock : public VideoInfoAdapter {
public:
    VideoInfoAdapterMock() = default;

    std::shared_ptr<VideoCaptureInfoAdapter> GetVideoCapInfo()
    {
        return videoCapInfo;
    }

    std::shared_ptr<VideoEncInfoAdapter> GetVideoEncInfo()
    {
        return videoEncInfo;
    }

    std::shared_ptr<MockVideoCaptureInfoAdapter> videoCapInfo = nullptr;
    std::shared_ptr<MockVideoEncInfoAdapter> videoEncInfo = nullptr;
};

class MockAudioCaptureInfoAdapter : public AudioCaptureInfoAdapter {
public:
    MockAudioCaptureInfoAdapter() = default;

    int32_t GetAudioSampleRate() override
    {
        return audioSampleRate;
    }

    int32_t GetAudioChannels() override
    {
        return audioChannels;
    }

    AudioCaptureSourceTypeAdapter GetAudioSource() override
    {
        return audioSource;
    }

    int32_t audioSampleRate;
    int32_t audioChannels;
    AudioCaptureSourceTypeAdapter audioSource = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
};

class MockAudioEncInfoAdapter : public AudioEncInfoAdapter {
public:
    MockAudioEncInfoAdapter() = default;

    int32_t GetAudioBitrate() override
    {
        return audioBitrate;
    }

    AudioCodecFormatAdapter GetAudioCodecformat() override
    {
        return audioCodecformat;
    }

    int32_t audioBitrate = 0;
    AudioCodecFormatAdapter audioCodecformat = AudioCodecFormatAdapter::AUDIO_DEFAULT;
};

class AudioInfoAdapterMock : public AudioInfoAdapter {
public:
    AudioInfoAdapterMock() = default;

    std::shared_ptr<AudioCaptureInfoAdapter> GetMicCapInfo() override
    {
        return micCapInfo;
    }

    std::shared_ptr<AudioCaptureInfoAdapter> GetInnerCapInfo() override
    {
        return innerCapInfo;
    }

    std::shared_ptr<AudioEncInfoAdapter> GetAudioEncInfo() override
    {
        return audioEncInfo;
    }

    std::shared_ptr<MockAudioCaptureInfoAdapter> micCapInfo = nullptr;
    std::shared_ptr<MockAudioCaptureInfoAdapter> innerCapInfo = nullptr;
    std::shared_ptr<MockAudioEncInfoAdapter> audioEncInfo = nullptr;
};

class RecorderInfoAdapterMock : public RecorderInfoAdapter {
public:
    RecorderInfoAdapterMock() = default;

    std::string GetUrl() override
    {
        return url;
    }

    ContainerFormatTypeAdapter GetFileFormat() override
    {
        return fileFormat;
    }

    std::string url = "";
    ContainerFormatTypeAdapter fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
};

class ScreenCaptureConfigAdapterMock : public ScreenCaptureConfigAdapter {
public:
    ScreenCaptureConfigAdapterMock();
    ~ScreenCaptureConfigAdapterMock() override;

    CaptureModeAdapter GetCaptureMode() override;
    DataTypeAdapter GetDataType() override;
    std::shared_ptr<AudioInfoAdapter> GetAudioInfo() override;
    std::shared_ptr<VideoInfoAdapter> GetVideoInfo() override;
    std::shared_ptr<RecorderInfoAdapter> GetRecorderInfo() override;
};

ScreenCaptureConfigAdapterMock::ScreenCaptureConfigAdapterMock() {}

ScreenCaptureConfigAdapterMock::~ScreenCaptureConfigAdapterMock() {}

CaptureModeAdapter ScreenCaptureConfigAdapterMock::GetCaptureMode()
{
    return CaptureModeAdapter::CAPTURE_HOME_SCREEN;
}

DataTypeAdapter ScreenCaptureConfigAdapterMock::GetDataType()
{
    return DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
}

std::shared_ptr<AudioInfoAdapter> ScreenCaptureConfigAdapterMock::GetAudioInfo()
{
    auto audioInfo = std::make_shared<AudioInfoAdapterMock>();
    return audioInfo;
}

std::shared_ptr<VideoInfoAdapter> ScreenCaptureConfigAdapterMock::GetVideoInfo()
{
    auto videoInfo = std::make_shared<VideoInfoAdapterMock>();
    return videoInfo;
}

std::shared_ptr<RecorderInfoAdapter> ScreenCaptureConfigAdapterMock::GetRecorderInfo()
{
    auto recorderInfo = std::make_shared<RecorderInfoAdapterMock>();
    return recorderInfo;
}

namespace OHOS {

bool ApplyInitFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    ScreenCaptureAdapterImpl impl;
    std::shared_ptr<ScreenCaptureConfigAdapter> config = std::make_shared<ScreenCaptureConfigAdapterMock>();
    impl.Init(config);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyInitFuzzTest(data, size);
    return 0;
}