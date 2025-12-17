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

#include "ohos_nweb/src/sysevent/event_reporter.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <string>

namespace {
  constexpr uint8_t MAX_STRING_LENGTH = UINT8_MAX;
  constexpr uint16_t MAX_UINT16_SIZE = UINT16_MAX;
  constexpr int32_t MAX_INT_SIZE = INT32_MAX;
  constexpr uint32_t MAX_UINT32_SIZE = UINT32_MAX;
  constexpr int64_t MAX_INT64_SIZE = INT64_MAX;
  constexpr uint64_t MAX_UINT64_SIZE = UINT64_MAX;
  constexpr uint32_t MAX_VECTOR_SIZE = 10;
}

namespace OHOS {

void ReportPageLoadStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int instanceId = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  int accessSuccCount = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  int accessSumCount = dataProvider.ConsumeIntegralInRange<int>(accessSuccCount, MAX_INT_SIZE);
  int accessFailCount = accessSumCount - accessSuccCount;
  ReportPageLoadStats(instanceId, accessSumCount, accessSuccCount, accessFailCount);
}

void ReportRenderJsFreezeFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int32_t pid = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::string packageName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string processName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string freezeMsg = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int32_t uid = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportRenderJsFreeze(pid, packageName, processName, freezeMsg, uid);
}

void ReportMultiInstanceStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int instanceId = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  int nwebCount = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  int nwebMaxCount = dataProvider.ConsumeIntegralInRange<int>(nwebCount, MAX_INT_SIZE);
  ReportMultiInstanceStats(instanceId, nwebCount, nwebMaxCount);
}

void ReportPageLoadErrorInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int instanceId = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  int errorCode = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  uint32_t errorCount = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
  std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

  /* branch1: type and desc are all empty */
  ReportPageLoadErrorInfo(instanceId, "", errorCode, errorCount, "");

  /* branch2: desc is not empty */
  ReportPageLoadErrorInfo(instanceId, errorType, errorCode, errorCount, "errorDesc");

  /* branch3: type is not empty */
  ReportPageLoadErrorInfo(instanceId, "errorType", errorCode, errorCount, errorDesc);

  /* branch4: type and desc are auto length */
  ReportPageLoadErrorInfo(instanceId, errorType, errorCode, errorCount, errorDesc);
}

void ReportJankStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int64_t startTime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  int duration = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::vector<uint16_t> jankStats;
  int vecSize = dataProvider.ConsumeIntegralInRange<int>(0, MAX_VECTOR_SIZE);
  for (int i = 0; i< vecSize; i++) {
    uint16_t item = dataProvider.ConsumeIntegralInRange<uint16_t>(0, MAX_UINT16_SIZE);
    jankStats.push_back(item);
  }
  int jankStatsVer = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportJankStats(startTime, duration, jankStats, jankStatsVer);
}

void ReportLockdownModeStatusFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  ReportLockdownModeStatus();
}

void ReportPageLoadTimeStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  OhWebPerformanceTiming loadPageTime;
  loadPageTime.navigation_id = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.navigation_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.redirect_count = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
  loadPageTime.redirect_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.redirect_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.fetch_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.worker_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.domain_lookup_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.domain_lookup_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.connect_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.secure_connect_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.connect_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.request_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.response_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.response_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.dom_interactive = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.dom_content_loaded_event_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.dom_content_loaded_event_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.load_event_start = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.load_event_end = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.first_paint = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.first_contentful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.largest_contentful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.render_init_block = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.input_time = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  loadPageTime.is_paint_done = dataProvider.ConsumeBool();
  loadPageTime.first_meaningful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_INT64_SIZE);
  ReportPageLoadTimeStats(loadPageTime);
}

void ReportAudioPlayErrorInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int errorCode = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

  /* branch1: type and desc are all empty */
  ReportAudioPlayErrorInfo("", errorCode, "");

  /* branch2: type is not empty */
  ReportAudioPlayErrorInfo("errorType", errorCode, errorDesc);

  /* branch3: desc is not empty */
  ReportAudioPlayErrorInfo(errorType, errorCode, "errorDesc");

  /* branch4: type and desc are auto length */
  ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
}

void ReportVideoPlayErrorInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int errorCode = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

  /* branch1: type and desc are all empty */
  ReportVideoPlayErrorInfo("", errorCode, "");

  /* branch2: type is not empty */
  ReportVideoPlayErrorInfo("errorType", errorCode, errorDesc);

  /* branch3: desc is not empty */
  ReportVideoPlayErrorInfo(errorType, errorCode, "errorDesc");

  /* branch4: type and desc are auto length */
  ReportVideoPlayErrorInfo(errorType, errorCode, errorDesc);
}

void ReportAudioFrameDropStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int frameCount = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportAudioFrameDropStats(frameCount);
}

void ReportVideoFrameDropStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  uint32_t frameCount = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
  uint64_t frameDuration = dataProvider.ConsumeIntegralInRange<int64_t>(-1, MAX_UINT64_SIZE);
  ReportVideoFrameDropStats(frameCount, frameDuration);
}

void ReportDragDropStatusFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string eventName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int32_t id = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportDragDropStatus(eventName, id);
}

void ReportDragDropInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string eventName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int32_t id = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
  std::string fragment = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string linkUrl = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string linkHtml = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  ReportDragDropInfo(eventName, id, fragment, linkUrl, linkHtml);
}

void ReportForceZoomEnableFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  ReportForceZoomEnable();
}

void ReportOpenPrivateModeFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  ReportOpenPrivateMode();
}

void ReportPageDownLoadErrorInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  long downloadId = dataProvider.ConsumeIntegralInRange<long>(0, MAX_INT_SIZE);
  int errorCode = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportPageDownLoadErrorInfo(downloadId, errorCode);
}

void ReportSlideJankStatsFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int64_t startTime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  int64_t duration = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  int32_t totalAppFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
  int32_t totalAppMissedFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
  int64_t maxAppFrametime = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  int32_t maxAppSeqMissedFrames = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
  ReportSlideJankStats(startTime, duration, totalAppFrames, totalAppMissedFrames, maxAppFrametime,
    maxAppSeqMissedFrames);
}

void ReportSiteIsolationModeFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string siteStatus = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  ReportSiteIsolationMode(siteStatus);
}

void ReportRendererMemFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string type = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string pid = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string rss = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string pss = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string jsHeapTotal = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string jsHeapUsed = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string gpuMem = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  std::string url = std::to_string(dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE));
  ReportRendererMem(type, pid, rss, pss, jsHeapTotal, jsHeapUsed, gpuMem, url);
}

void ReportWebMediaPlayErrorInfoFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string errorType = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int errorCode = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::string errorDesc = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

  /* branch1: type and desc are all empty */
  ReportWebMediaPlayErrorInfo("", errorCode, "");

  /* branch2: type is not empty */
  ReportWebMediaPlayErrorInfo("errorType", errorCode, errorDesc);

  /* branch3: desc is not empty */
  ReportWebMediaPlayErrorInfo(errorType, errorCode, "errorDesc");

  /* branch4: type and desc are auto length */
  ReportWebMediaPlayErrorInfo(errorType, errorCode, errorDesc);
}

void ReportAvSessionStatusFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string avSession = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  ReportAvSessionStatus(avSession);
}

void ReportDragBlankFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int64_t duration = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  ReportDragBlank(duration);
}

void ReportFirstMeaningfulPaintDoneFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  OhWebPerformanceTiming loadPageTime;
  loadPageTime.navigation_id = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.navigation_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.redirect_count = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
  loadPageTime.redirect_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.redirect_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.fetch_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.worker_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.domain_lookup_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.domain_lookup_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.connect_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.secure_connect_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.connect_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.request_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.response_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.response_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.dom_interactive = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.dom_content_loaded_event_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.dom_content_loaded_event_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.load_event_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.load_event_end = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.first_paint = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.first_contentful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.largest_contentful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.render_init_block = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.input_time = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  loadPageTime.is_paint_done = dataProvider.ConsumeBool();
  loadPageTime.first_meaningful_paint = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
  ReportFirstMeaningfulPaintDone(loadPageTime)
}

void ReportGpuProcessEventFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  std::string eventContent = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  /* time out type */
  ReportGpuProcessEvent(CrashType::TIMEOUT, eventContent);

  /* none existent */
  ReportGpuProcessEvent(CrashType::MAILBOX_NONEXISTENT, eventContent);
}

void ReportAppfreezeFuzzTest(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size == 0)) {
    return;
  }
  FuzzedDataProvider dataProvider(data, size);
  int32_t pid = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  std::string packageName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string processName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  std::string freezeMsg = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
  int32_t uid = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
  ReportAppfreeze(pid, packageName, processName, freezeMsg, uid);
}

}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  OHOS::ReportPageLoadStatsFuzzTest(data, size);
  OHOS::ReportMultiInstanceStatsFuzzTest(data, size);
  OHOS::ReportPageLoadErrorInfoFuzzTest(data, size);

  OHOS::ReportJankStatsFuzzTest(data, size);
  OHOS::ReportLockdownModeStatusFuzzTest(data, size);
  OHOS::ReportPageLoadTimeStatsFuzzTest(data, size);

  OHOS::ReportAudioPlayErrorInfoFuzzTest(data, size);
  OHOS::ReportVideoPlayErrorInfoFuzzTest(data, size);
  OHOS::ReportAudioFrameDropStatsFuzzTest(data, size);

  OHOS::ReportVideoFrameDropStatsFuzzTest(data, size);
  OHOS::ReportDragDropStatusFuzzTest(data, size);
  OHOS::ReportDragDropInfoFuzzTest(data, size);

  OHOS::ReportForceZoomEnableFuzzTest(data, size);
  OHOS::ReportOpenPrivateModeFuzzTest(data, size);
  OHOS::ReportPageDownLoadErrorInfoFuzzTest(data, size);

  OHOS::ReportSlideJankStatsFuzzTest(data, size);
  OHOS::ReportSiteIsolationModeFuzzTest(data, size);
  OHOS::ReportRendererMemFuzzTest(data, size);

  OHOS::ReportWebMediaPlayErrorInfoFuzzTest(data, size);
  OHOS::ReportAvSessionStatusFuzzTest(data, size);
  OHOS::ReportDragBlankFuzzTest(data, size);

  OHOS::ReportFirstMeaningfulPaintDoneFuzzTest(data, size);
  OHOS::ReportGpuProcessEventFuzzTest(data, size);

  OHOS::ReportRenderJsFreezeFuzzTest(data, size);
  OHOS::ReportAppfreezeFuzzTest(data, size);
  return 0;
}
