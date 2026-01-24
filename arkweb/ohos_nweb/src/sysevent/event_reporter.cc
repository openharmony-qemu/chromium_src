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
#include "base/logging.h"

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

// For media behavior info
constexpr char DRM_ENCRYPTED_PLAYBACK[] = "DRM_ENCRYPTED_PLAYBACK";
constexpr char MEDIA_TYPE[] = "MEDIA_TYPE";
constexpr char DRM_SYSTEM[] = "DRM_SYSTEM";
constexpr char ENCRYPTION_ALGORITHM[] = "ENCRYPTION_ALGORITHM";

constexpr char VIDEO_ENCODE_FORMAT[] = "VIDEO_ENCODE_FORMAT";
constexpr char ENCODE_FORMAT[] = "ENCODE_FORMAT";

constexpr char FFMPEG_CODEC_OPERATION[] = "FFMPEG_CODEC_OPERATION";
constexpr char CODEC_TYPE[] = "CODEC_TYPE";
constexpr char FORMAT[] = "FORMAT";

constexpr char AUDIO_HARDWARE_DECODE[] = "AUDIO_HARDWARE_DECODE";
constexpr char CODEC_FORMAT[] = "CODEC_FORMAT";

constexpr char PICTURE_DECODE[] = "PICTURE_DECODE";
constexpr char PICTURE_TYPE[] = "PICTURE_TYPE";

// For audio/video error info
constexpr char AUDIO_PLAY_ERROR[] = "AUDIO_PLAY_ERROR";
constexpr char VIDEO_PLAY_ERROR[] = "VIDEO_PLAY_ERROR";

// For audio/video frame dropped statistics
constexpr char AUDIO_FRAME_DROP_STATISTICS[] = "AUDIO_FRAME_DROP_STATISTICS";
constexpr char AUDIO_BLANK_FRAME_COUNT[] = "AUDIO_BLANK_FRAME_COUNT";
constexpr char VIDEO_FRAME_DROP_STATISTICS[] = "VIDEO_FRAME_DROP_STATISTICS";
constexpr char VIDEO_FRAME_DROPPED_COUNT[] = "VIDEO_FRAME_DROPPED_COUNT";
constexpr char VIDEO_FRAME_DROPPED_DURATION[] = "VIDEO_FRAME_DROPPED_DURATION";

// For dumplicate file upload statistics
constexpr char DUPLICATE_FILE_UPLOAD[] = "DUPLICATE_FILE_UPLOAD";

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

// For renderer memory statistics
constexpr char PAGE_MEM_LEAK[] = "PAGE_MEM_LEAK";
constexpr char TYPE[] = "TYPE";
constexpr char PID[] = "PID";
constexpr char RSS[] = "RSS";
constexpr char PSS[] = "PSS";
constexpr char JS_HEAP_TOTAL[] = "JS_HEAP_TOTAL";
constexpr char JS_HEAP_USED[] = "JS_HEAP_USED";
constexpr char GPU_MEM[] = "GPU_MEM";
constexpr char URL[] = "URL";

constexpr char PAGE_DRAG_BLANK[] = "PAGE_DRAG_BLANK";
constexpr char PAGE_BLANK_TIME[] = "PAGE_BLANK_TIME";

constexpr char RENDER_JS_FREEZE[] = "RENDER_JS_FREEZE";

// For web play error info,such as pip/drm
constexpr char WEB_MEDIA_PLAY_ERROR[] = "WEB_MEDIA_PLAY_ERROR";
 
// For av session
constexpr char WEB_AV_SESSION_DISABLE[] = "WEB_AV_SESSION_DISABLE";
constexpr char DISABLE_WEB_AV_SESSION_STATUS[] = "DISABLE_WEB_AV_SESSION_STATUS";

constexpr char RENDER_INIT_BLOCK[] = "RENDER_INIT_BLOCK";
constexpr char BLOCK_TIME[] = "BLOCK_TIME";

const char GPU_DISPLAY_ERROR[] = "GPU_DISPLAY_ERROR";
const char EVENT_TYPE[] = "EVENT_TYPE";
const char EVENT_CONTENT[] = "EVENT_CONTENT";

constexpr char TIMEOUT[] = "TIMEOUT";
constexpr char MAILBOX_NONEXISTENT[] = "MAILBOX_NONEXISTENT";

// For render freeze monitoring
constexpr char PROCESS_FREEZE_WARNING[] = "PROCESS_FREEZE_WARNING";

constexpr char RENDER_PROCESS_TERMINATE[] = "RENDER_PROCESS_TERMINATE";
}  // namespace

void ReportRenderJsFreeze(int32_t pid, const std::string& packageName, const std::string& processName,
                          const std::string& freezeMsg, int32_t uid) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      RENDER_JS_FREEZE, HiSysEventAdapter::EventType::FAULT,
      {
        "PID", pid,
        "PACKAGE_NAME", packageName,
        "PROCESS_NAME", processName,
        "MSG", freezeMsg,
        "UID", uid
      });
}                         

void ReportRenderProcessTerminate(bool is_gpu, const std::string& pid, const std::string& reason) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      RENDER_PROCESS_TERMINATE, HiSysEventAdapter::EventType::FAULT,
      {
        "PROCESS_TYPE", is_gpu ? std::string("gpu") : std::string("render"),
        "PID", pid,
        "REASON", reason
      });
}

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

// LOVC_EXCL_START
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
// LOVC_EXCL_STOP

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
      {AUDIO_BLANK_FRAME_COUNT, frameCount});
}

void ReportVideoFrameDropStats(uint32_t frameCount, uint64_t frameDuration) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      VIDEO_FRAME_DROP_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {VIDEO_FRAME_DROPPED_COUNT, frameCount, VIDEO_FRAME_DROPPED_DURATION,
       frameDuration});
}

void ReportDuplicateFileUpload(const std::string errorDesc) {
  std::string error_desc = "";
  if (!errorDesc.empty()) {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      DUPLICATE_FILE_UPLOAD, HiSysEventAdapter::EventType::STATISTIC,
      {ERROR_DESC, error_desc});
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

void ReportDrmEncryptedPlayback(const std::string& mediaType,
                                const std::string& drmSystem,
                                const std::string& encryptedAlgo) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      DRM_ENCRYPTED_PLAYBACK, HiSysEventAdapter::EventType::BEHAVIOR,
      {MEDIA_TYPE, mediaType,
       DRM_SYSTEM, drmSystem,
       ENCRYPTION_ALGORITHM, encryptedAlgo});
}

void ReportVideoEncodeFormat(const std::string& encodeFormat) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      VIDEO_ENCODE_FORMAT, HiSysEventAdapter::EventType::BEHAVIOR,
      {ENCODE_FORMAT, encodeFormat});
}

void ReportFfmpegCodecOperation(const std::string& codecType, const std::string& format) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      FFMPEG_CODEC_OPERATION, HiSysEventAdapter::EventType::BEHAVIOR,
      {CODEC_TYPE, codecType,
       FORMAT, format});
}

void ReportAudioHardwareDecode(const std::string& codecFormat) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      AUDIO_HARDWARE_DECODE, HiSysEventAdapter::EventType::BEHAVIOR,
      {CODEC_FORMAT, codecFormat});
}

void ReportPictureDecode(const std::string& pictureType) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PICTURE_DECODE, HiSysEventAdapter::EventType::BEHAVIOR,
      {PICTURE_TYPE, pictureType});
}

void ReportRendererMem(const std::string& type,
                       const std::string& pid,
                       const std::string& rss,
                       const std::string& pss,
                       const std::string& js_heap_total,
                       const std::string& js_heap_used,
                       const std::string& gpu_mem,
                       const std::string& url)
{
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_MEM_LEAK, HiSysEventAdapter::EventType::STATISTIC,
      {TYPE, type, PID, pid, RSS, rss, PSS, pss, JS_HEAP_TOTAL, js_heap_total,
       JS_HEAP_USED, js_heap_used, GPU_MEM, gpu_mem, URL, url});
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

void ReportFirstMeaningfulPaintDone(OhWebPerformanceTiming loadPageTime) {
  const std::string input = "NAVIGATION_ID" + std::to_string(loadPageTime.navigation_id) +
    "NAVIGATION_START" + std::to_string(loadPageTime.navigation_start) +
    "REDIRECT_COUNT" + std::to_string(loadPageTime.redirect_count) +
    "INPUT_TIME" + std::to_string(loadPageTime.input_time) +
    "FIRST_PAINT" + std::to_string(loadPageTime.first_paint) +
    "FIRST_CONTENTFUL_PAINT" + std::to_string(loadPageTime.first_contentful_paint) +
    "FIRST_MEANINGFUL_PAINT" + std::to_string(loadPageTime.first_meaningful_paint) +
    "IS_PAINT_DONE" + std::to_string(loadPageTime.is_paint_done);

  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      "FIRST_MEANINGFUL_PAINT_DONE", HiSysEventAdapter::EventType::STATISTIC,{input, ""});
}

void ReportGpuProcessEvent(CrashType type, std::string eventcontent) {
  switch(type) {
    case CrashType::TIMEOUT:
      OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        GPU_DISPLAY_ERROR, HiSysEventAdapter::EventType::STATISTIC,
        {EVENT_TYPE, TIMEOUT, EVENT_CONTENT, eventcontent});
      break;
    case CrashType::MAILBOX_NONEXISTENT:
      OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        GPU_DISPLAY_ERROR, HiSysEventAdapter::EventType::STATISTIC,
        {EVENT_TYPE, MAILBOX_NONEXISTENT, EVENT_CONTENT, eventcontent});
      break;
  }
}
// LOVC_EXCL_START
void ReportAppfreeze(int32_t pid, const std::string& packageName, const std::string& processName,
                     const std::string& freezeMsg, int32_t uid) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PROCESS_FREEZE_WARNING, HiSysEventAdapter::EventType::FAULT,
      {
        "PID", pid,
        "PACKAGE_NAME", packageName,
        "PROCESS_NAME", processName,
        "MSG", freezeMsg,
        "UID", uid
      });
}
// LOVC_EXCL_STOP
