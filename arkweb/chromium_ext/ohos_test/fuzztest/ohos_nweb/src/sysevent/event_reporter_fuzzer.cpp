/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "arkweb/ohos_nweb/src/sysevent/oh_web_performance_timing.h"

namespace OHOS::NWeb {
namespace {
    constexpr uint32_t MAX_UINT32_SIZE = 255;
    constexpr int32_t MAX_INT32_SIZE = 255;
    constexpr int32_t MIN_INT32_SIZE = -255;
    constexpr int64_t MAX_INT64_SIZE = 9999999999;
    constexpr size_t MAX_STRING_LENGTH = 255;
}

void EventReporterFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportRenderJsFreeze
    int32_t pid = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string packageName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string processName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string freezeMsg = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t uid = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    ReportRenderJsFreeze(pid, packageName, processName, freezeMsg, uid);
    
    // Test ReportRenderProcessTerminate
    bool is_gpu = dataProvider.ConsumeBool();
    pid = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string reason = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int error = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    ReportRenderProcessTerminate(is_gpu, pid, reason, error);
}

void EventReporterFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportPageLoadStats
    int instanceId = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int accessSumCount = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    int accessSuccCount = dataProvider.ConsumeIntegralInRange<int32_t>(0, accessSumCount);
    int accessFailCount = accessSumCount - accessSuccCount;
    ReportPageLoadStats(instanceId, accessSumCount, accessSuccCount, accessFailCount);
    
    // Test ReportMultiInstanceStats
    instanceId = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int nwebCount = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    int nwebMaxCount = dataProvider.ConsumeIntegralInRange<int32_t>(nwebCount, MAX_INT32_SIZE);
    ReportMultiInstanceStats(instanceId, nwebCount, nwebMaxCount);
}

void EventReporterFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportPageLoadErrorInfo
    int instanceId = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int errorCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    uint32_t errorCount = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportPageLoadErrorInfo(instanceId, errorType, errorCode, errorCount, errorDesc);
    
    // Test ReportPageDownLoadErrorInfo
    long downloadId = dataProvider.ConsumeIntegralInRange<long>(0, MAX_INT64_SIZE);
    errorCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    ReportPageDownLoadErrorInfo(downloadId, errorCode);
}

void EventReporterFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportJankStats
    int64_t startTime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    int duration = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    std::vector<uint16_t> jankStats;
    uint32_t vecSize = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    for (uint32_t i = 0; i < vecSize; i++) {
        jankStats.push_back(dataProvider.ConsumeIntegral<uint16_t>());
    }
    int jankStatsVer = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    ReportJankStats(startTime, duration, jankStats, jankStatsVer);
    
    // Test ReportSlideJankStats
    startTime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    int64_t slideDuration = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    int32_t totalAppFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    int32_t totalAppMissedFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, totalAppFrames);
    int64_t maxAppFrametime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    int32_t maxAppSeqMissedFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, totalAppMissedFrames);
    ReportSlideJankStats(
        startTime, slideDuration, totalAppFrames, totalAppMissedFrames, 
        maxAppFrametime, maxAppSeqMissedFrames);
}

void EventReporterFuzz005Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportAudioPlayErrorInfo
    std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int errorCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
    
    // Test ReportVideoPlayErrorInfo
    errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    errorCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportVideoPlayErrorInfo(errorType, errorCode, errorDesc);
    
    // Test ReportWebMediaPlayErrorInfo
    errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    errorCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportWebMediaPlayErrorInfo(errorType, errorCode, errorDesc);
}

void EventReporterFuzz006Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportAudioFrameDropStats
    int frameCount = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    ReportAudioFrameDropStats(frameCount);
    
    // Test ReportVideoFrameDropStats
    uint32_t videoFrameCount = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    uint64_t frameDuration = dataProvider.ConsumeIntegralInRange<uint64_t>(0, MAX_INT64_SIZE);
    ReportVideoFrameDropStats(videoFrameCount, frameDuration);
    
    // Test ReportDragBlank
    int64_t dragBlankDuration = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    ReportDragBlank(dragBlankDuration);
}

void EventReporterFuzz007Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportDuplicateFileUpload
    std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportDuplicateFileUpload(errorDesc);
    
    // Test ReportDragDropStatus
    std::string eventName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t id = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    ReportDragDropStatus(eventName, id);
    
    // Test ReportDragDropInfo
    eventName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    id = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string fragment = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string linkUrl = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string linkHtml = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportDragDropInfo(eventName, id, fragment, linkUrl, linkHtml);
}

void EventReporterFuzz008Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportSiteIsolationMode
    std::string siteIsolationStatus = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportSiteIsolationMode(siteIsolationStatus);
    
    // Test ReportAvSessionStatus
    std::string avSessionStatus = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportAvSessionStatus(avSessionStatus);
}

void EventReporterFuzz009Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportDrmEncryptedPlayback
    std::string mediaType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string drmSystem = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string encryptedAlgo = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportDrmEncryptedPlayback(mediaType, drmSystem, encryptedAlgo);
    
    // Test ReportVideoEncodeFormat
    std::string encodeFormat = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportVideoEncodeFormat(encodeFormat);
    
    // Test ReportFfmpegCodecOperation
    std::string codecType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string format = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportFfmpegCodecOperation(codecType, format);
    
    // Test ReportAudioHardwareDecode
    std::string codecFormat = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportAudioHardwareDecode(codecFormat);
    
    // Test ReportPictureDecode
    std::string pictureType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportPictureDecode(pictureType);
}

void EventReporterFuzz00ATest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportRendererMem
    std::string type = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string pid = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string rss = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string pss = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string jsHeapTotal = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string jsHeapUsed = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string gpuMem = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string url = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportRendererMem(type, pid, rss, pss, jsHeapTotal, jsHeapUsed, gpuMem, url);
}

void EventReporterFuzz00BTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportPageLoadTimeStats and ReportFirstMeaningfulPaintDone
    OhWebPerformanceTiming timing;
    timing.Reset();
    
    // Randomly set some timing values
    timing.navigation_id = dataProvider.ConsumeIntegral<int64_t>();
    timing.navigation_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.redirect_count = dataProvider.ConsumeIntegral<uint32_t>();
    timing.redirect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.redirect_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.fetch_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.worker_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.domain_lookup_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.domain_lookup_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.connect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.secure_connect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.connect_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.request_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.response_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.response_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_interactive = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_content_loaded_event_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_content_loaded_event_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.load_event_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.load_event_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.first_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.first_contentful_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.largest_contentful_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.render_init_block = dataProvider.ConsumeIntegral<int64_t>();
    timing.input_time = dataProvider.ConsumeIntegral<int64_t>();
    timing.is_paint_done = dataProvider.ConsumeBool();
    timing.first_meaningful_paint = dataProvider.ConsumeIntegral<int64_t>();
    
    ReportPageLoadTimeStats(timing);
    ReportFirstMeaningfulPaintDone(timing);
}

void EventReporterFuzz00CTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ReportGpuProcessEvent
    std::string eventContent = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportGpuProcessEvent(CrashType::TIMEOUT, eventContent);
    eventContent = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    ReportGpuProcessEvent(CrashType::MAILBOX_NONEXISTENT, eventContent);
    
    // Test ReportAppfreeze
    int32_t pid = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    std::string packageName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string processName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string freezeMsg = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t uid = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    ReportAppfreeze(pid, packageName, processName, freezeMsg, uid);
}

void EventReporterFuzz00DTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    
    // Test ReportLockdownModeStatus
    ReportLockdownModeStatus();
    
    // Test ReportForceZoomEnable
    ReportForceZoomEnable();
    
    // Test ReportOpenPrivateMode
    ReportOpenPrivateMode();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    EventReporterFuzz001Test(data, size);
    EventReporterFuzz002Test(data, size);
    EventReporterFuzz003Test(data, size);
    EventReporterFuzz004Test(data, size);
    EventReporterFuzz005Test(data, size);
    EventReporterFuzz006Test(data, size);
    EventReporterFuzz007Test(data, size);
    EventReporterFuzz008Test(data, size);
    EventReporterFuzz009Test(data, size);
    EventReporterFuzz00ATest(data, size);
    EventReporterFuzz00BTest(data, size);
    EventReporterFuzz00CTest(data, size);
    EventReporterFuzz00DTest(data, size);
    return 0;
}

}   // namespace OHOS::NWeb