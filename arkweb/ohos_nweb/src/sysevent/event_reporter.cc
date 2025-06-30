/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "event_reporter.h"

#include "oh_web_performance_timing.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

using OHOS::NWeb::HiSysEventAdapter;
using OHOS::NWeb::OhosAdapterHelper;
namespace {
// For page load statistics
constexpr char PAGE_LOAD_STATISTICS[] = "PAGE_LOAD_STATISTICS";
constexpr char CURRENT_INSTANCE_ID[] = "INSTANCE_ID";
constexpr char ACCESS_SUM_COUNT[] = "ACCESS_SUM_COUNT";
constexpr char ACCESS_SUCC_COUNT[] = "ACCESS_SUCC_COUNT";
constexpr char ACCESS_FAIL_COUNT[] = "ACCESS_FAIL_COUNT";
constexpr char ACCESS_FAIL_RATIO[] = "ACCESS_FAIL_RATIO";

// For mutiple instance statistics
constexpr char MULTI_INSTANCE_STATISTICS[] = "MULTI_INSTANCE_STATISTICS";
constexpr char CURRENT_INSTANCE_COUNT[] = "INSTANCE_COUNT";
constexpr char INSTANCE_MAX_COUNT[] = "MAX_COUNT";

// For page load error info
constexpr char PAGE_LOAD_ERROR[] = "PAGE_LOAD_ERROR";
constexpr char ERROR_TYPE[] = "ERROR_TYPE";
constexpr char ERROR_CODE[] = "ERROR_CODE";
constexpr char ERROR_DESC[] = "ERROR_DESC";
constexpr char ERROR_COUNT[] = "ERROR_COUNT";

constexpr char JANK_STATS_APP[] = "JANK_STATS_APP";
constexpr char STARTTIME[] = "STARTTIME";
constexpr char DURATION[] = "DURATION";
constexpr char JANK_STATS[] = "JANK_STATS";
constexpr char JANK_STATS_VER[] = "JANK_STATS_VER";

constexpr char RENDER_JIT_LOCKDOWN[] = "RENDER_JIT_LOCKDOWN";
constexpr char LOCKDOWN_MODE_STATUS[] = "JIT_LOCKDOWN_MODE";

// For force zoom enable
constexpr char FORCE_ENABLE_ZOOM[] = "FORCE_ENABLE_ZOOM";
constexpr char ENABLE_FORCE_ZOOM_STATUS[] = "ENABLE_FORCE_ZOOM_STATUS";

// For open private mode
constexpr char OPEN_PRIVATE_MODE[] = "OPEN_PRIVATE_MODE";
constexpr char OPEN_PRIVATE_STATUS[] = "OPEN_PRIVATE_STATUS";

// For page download error info
constexpr char PAGE_DOWNLOAD_ERROR[] = "PAGE_DOWNLOAD_ERROR";
constexpr char DOWNLOAD_ID[] = "DOWNLOAD_ID";

// For audio/video error info
constexpr char AUDIO_PLAY_ERROR[] = "AUDIO_PLAY_ERROR";
constexpr char VIDEO_PLAY_ERROR[] = "VIDEO_PLAY_ERROR";

// For audio/video frame dropped statistics
constexpr char AUDIO_FRAME_DROP_STATISTICS[] = "AUDIO_FRAME_DROP_STATISTICS";
constexpr char AUDIO_BLANK_FRAME_COUNT[] = "AUDIO_BLANK_FRAME_COUNT";
constexpr char VIDEO_FRAME_DROP_STATISTICS[] = "VIDEO_FRAME_DROP_STATISTICS";
constexpr char VIDEO_FRAME_DROPPED_COUNT[] = "VIDEO_FRAME_DROPPED_COUNT";
constexpr char VIDEO_FRAME_DROPPED_DURATION[] = "VIDEO_FRAME_DROPPED_DURATION";

constexpr char NWEB_ID[] = "NWEB_ID";
constexpr char PLAIN_TEXT[] = "PLAIN_TEXT";
constexpr char LINK_URL[] = "LINK_URL";
constexpr char HTML[] = "HTML";

constexpr char DYNAMIC_FRAME_DROP_STATISTICS[] =
    "DYNAMIC_FRAME_DROP_STATISTICS";
constexpr char TOTAL_APP_FRAMES[] = "TOTAL_APP_FRAMES";
constexpr char TOTAL_APP_MISSED_FRAMES[] = "TOTAL_APP_MISSED_FRAMES";
constexpr char MAX_APP_FRAMETIME[] = "MAX_APP_FRAMETIME";
constexpr char MAX_APP_SEQ_MISSSED_FRAMES[] = "MAX_APP_SEQ_MISSSED_FRAMES";

// For site_isolation
constexpr char SITE_ISOLATION_MODE[] = "SITE_ISOLATION_MODE";
constexpr char SITE_ISOLATION_STATUS[] = "SITE_ISOLATION_STATUS";

constexpr char PAGE_DRAG_BLANK[] = "PAGE_DRAG_BLANK";
constexpr char PAGE_BLANK_TIME[] = "PAGE_BLANK_TIME";

// For web play error info,such as pip/drm
constexpr char WEB_MEDIA_PLAY_ERROR[] = "WEB_MEDIA_PLAY_ERROR";
 
// For av session
constexpr char WEB_AV_SESSION_DISABLE[] = "WEB_AV_SESSION_DISABLE";
constexpr char DISABLE_WEB_AV_SESSION_STATUS[] = "DISABLE_WEB_AV_SESSION_STATUS";

constexpr char RENDER_INIT_BLOCK[] = "RENDER_INIT_BLOCK";
constexpr char BLOCK_TIME[] = "BLOCK_TIME";

}  // namespace

void ReportPageLoadStats(int instanceId,
                         int accessSumCount,
                         int accessSuccCount,
                         int accessFailCount) {
  float failRatio = float(accessFailCount) / accessSumCount;
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_LOAD_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {CURRENT_INSTANCE_ID, std::to_string(instanceId), ACCESS_SUM_COUNT,
       std::to_string(accessSumCount), ACCESS_SUCC_COUNT,
       std::to_string(accessSuccCount), ACCESS_FAIL_COUNT,
       std::to_string(accessFailCount), ACCESS_FAIL_RATIO,
       std::to_string(failRatio)});
}

void ReportMultiInstanceStats(int instanceId, int nwebCount, int nwebMaxCount) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      MULTI_INSTANCE_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {CURRENT_INSTANCE_ID, std::to_string(instanceId), CURRENT_INSTANCE_COUNT,
       std::to_string(nwebCount), INSTANCE_MAX_COUNT,
       std::to_string(nwebMaxCount)});
}

void ReportPageLoadErrorInfo(int instanceId,
                             const std::string errorType,
                             int errorCode,
                             uint32_t errorCount,
                             const std::string errorDesc) {
  std::string error_type = "";
  std::string error_desc = "";
  int error_code = errorCode;
  if (errorType != "") {
    error_type = errorType;
  }
  if (errorDesc != "") {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_LOAD_ERROR, HiSysEventAdapter::EventType::FAULT,
      {CURRENT_INSTANCE_ID, std::to_string(instanceId), ERROR_TYPE, error_type,
       ERROR_CODE, std::to_string(error_code), ERROR_COUNT, std::to_string(errorCount), ERROR_DESC, error_desc});
}

void ReportJankStats(int64_t startTime,
                     int duration,
                     std::vector<uint16_t> jankStats,
                     int jankStatsVer) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      JANK_STATS_APP, HiSysEventAdapter::EventType::STATISTIC,
      {STARTTIME, startTime, DURATION, duration, JANK_STATS, jankStats,
       JANK_STATS_VER, jankStatsVer});
}

void ReportLockdownModeStatus(void) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      RENDER_JIT_LOCKDOWN, HiSysEventAdapter::EventType::BEHAVIOR,
      {LOCKDOWN_MODE_STATUS, "true"});
}

void ReportForceZoomEnable(void) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      FORCE_ENABLE_ZOOM, HiSysEventAdapter::EventType::BEHAVIOR,
      {ENABLE_FORCE_ZOOM_STATUS, "true"});
}

void ReportOpenPrivateMode(void) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      OPEN_PRIVATE_MODE, HiSysEventAdapter::EventType::BEHAVIOR,
      {OPEN_PRIVATE_STATUS, "true"});
}

void ReportPageDownLoadErrorInfo(long downloadId, int errorCode) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_DOWNLOAD_ERROR, HiSysEventAdapter::EventType::FAULT,
      {DOWNLOAD_ID, std::to_string(downloadId), ERROR_CODE,
       std::to_string(errorCode)});
}

void ReportPageLoadTimeStats(OhWebPerformanceTiming loadPageTime) {
  const std::string input = "NAVIGATION_ID" + std::to_string(loadPageTime.navigation_id) +
    "NAVIGATION_START" + std::to_string(loadPageTime.navigation_start) +
    "REDIRECT_COUNT" + std::to_string(loadPageTime.redirect_count) +
    "REDIRECT_START" + std::to_string(loadPageTime.redirect_start) +
    "REDIRECT_END" + std::to_string(loadPageTime.redirect_end) +
    "FETCH_START" + std::to_string(loadPageTime.fetch_start) +
    "WORKER_START" + std::to_string(loadPageTime.worker_start) +
    "DOMAIN_LOOKUP_START" + std::to_string(loadPageTime.domain_lookup_start) +
    "DOMAIN_LOOKUP_END" + std::to_string(loadPageTime.domain_lookup_end) +
    "CONNECT_START" + std::to_string(loadPageTime.connect_start) +
    "SECURE_CONNECT_START" + std::to_string(loadPageTime.secure_connect_start) +
    "CONNECT_END" + std::to_string(loadPageTime.connect_end) +
    "REQUEST_START" + std::to_string(loadPageTime.request_start) +
    "RESPONSE_START" + std::to_string(loadPageTime.response_start) +
    "RESPONSE_END" + std::to_string(loadPageTime.response_end) +
    "DOM_INTERACTIVE" + std::to_string(loadPageTime.dom_interactive) +
    "DOM_CONTENT_LOADED_EVENT_START" + std::to_string(loadPageTime.dom_content_loaded_event_start) +
    "DOM_CONTENT_LOADED_EVENT_END" + std::to_string(loadPageTime.dom_content_loaded_event_end) +
    "LOAD_EVENT_START" + std::to_string(loadPageTime.load_event_start) +
    "LOAD_EVENT_END" + std::to_string(loadPageTime.load_event_end) +
    "FIRST_PAINT" + std::to_string(loadPageTime.first_paint) +
    "FIRST_CONTENTFUL_PAINT" + std::to_string(loadPageTime.first_contentful_paint) +
    "LARGEST_CONTENTFUL_PAINT" + std::to_string(loadPageTime.largest_contentful_paint) +
    "RENDER_INIT_BLOCK" + std::to_string(loadPageTime.render_init_block);

  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      "PAGE_LOAD_TIME", HiSysEventAdapter::EventType::STATISTIC,{input, ""});
}

void ReportAudioPlayErrorInfo(const std::string errorType,
                              int errorCode,
                              const std::string errorDesc) {
  std::string error_type = "";
  std::string error_desc = "";
  int error_code = errorCode;
  if (!errorType.empty()) {
    error_type = errorType;
  }
  if (!errorDesc.empty()) {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      AUDIO_PLAY_ERROR, HiSysEventAdapter::EventType::FAULT,
      {ERROR_TYPE, error_type, ERROR_CODE, std::to_string(error_code),
       ERROR_DESC, error_desc});
}

void ReportVideoPlayErrorInfo(const std::string errorType,
                              int errorCode,
                              const std::string errorDesc) {
  std::string error_type = "";
  std::string error_desc = "";
  int error_code = errorCode;
  if (!errorType.empty()) {
    error_type = errorType;
  }
  if (!errorDesc.empty()) {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      VIDEO_PLAY_ERROR, HiSysEventAdapter::EventType::FAULT,
      {ERROR_TYPE, error_type, ERROR_CODE, std::to_string(error_code),
       ERROR_DESC, error_desc});
}

void ReportAudioFrameDropStats(int frameCount) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      AUDIO_FRAME_DROP_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {AUDIO_BLANK_FRAME_COUNT, std::to_string(frameCount)});
}

void ReportVideoFrameDropStats(uint32_t frameCount, uint64_t frameDuration) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      VIDEO_FRAME_DROP_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {VIDEO_FRAME_DROPPED_COUNT, frameCount, VIDEO_FRAME_DROPPED_DURATION,
       frameDuration});
}
void ReportDragDropStatus(const std::string& eventName, int32_t id) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      eventName, HiSysEventAdapter::EventType::BEHAVIOR,
      {NWEB_ID, std::to_string(id)});
}

void ReportDragDropInfo(const std::string& eventName,
                        int32_t id,
                        const std::string& fragment,
                        const std::string& linkUrl,
                        const std::string& linkHtml) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      eventName, HiSysEventAdapter::EventType::BEHAVIOR,
      {NWEB_ID, std::to_string(id), PLAIN_TEXT, std::to_string(fragment.size()),
       LINK_URL, std::to_string(linkUrl.size()), HTML,
       std::to_string(linkHtml.size())});
}

void ReportSlideJankStats(int64_t startTime,
                          int64_t duration,
                          int32_t totalAppFrames,
                          int32_t totalAppMissedFrames,
                          int64_t maxAppFrametime,
                          int32_t maxAppSeqMissedFrames) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      DYNAMIC_FRAME_DROP_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {STARTTIME, startTime, DURATION, duration, TOTAL_APP_FRAMES,
       totalAppFrames, TOTAL_APP_MISSED_FRAMES, totalAppMissedFrames,
       MAX_APP_FRAMETIME, maxAppFrametime, MAX_APP_SEQ_MISSSED_FRAMES,
       maxAppSeqMissedFrames});
}

void ReportSiteIsolationMode(const std::string site_isolation_status) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      SITE_ISOLATION_MODE, HiSysEventAdapter::EventType::BEHAVIOR,
      {SITE_ISOLATION_STATUS, site_isolation_status});
}

void ReportWebMediaPlayErrorInfo(const std::string& errorType,
                              int errorCode,
                              const std::string& errorDesc) {
  std::string error_type = "";
  std::string error_desc = "";
  int error_code = errorCode;
  if (!errorType.empty()) {
    error_type = errorType;
  }
  if (!errorDesc.empty()) {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      WEB_MEDIA_PLAY_ERROR, HiSysEventAdapter::EventType::FAULT,
      {ERROR_TYPE, error_type, ERROR_CODE, std::to_string(error_code),
       ERROR_DESC, error_desc});
}
 
void ReportAvSessionStatus(const std::string& disable_web_av_session_status) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      WEB_AV_SESSION_DISABLE, HiSysEventAdapter::EventType::STATISTIC,
      {DISABLE_WEB_AV_SESSION_STATUS, disable_web_av_session_status});
}

void ReportDragBlank(int64_t duration) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_DRAG_BLANK, HiSysEventAdapter::EventType::STATISTIC,
      {PAGE_DRAG_BLANK, std::to_string(duration)});
}
