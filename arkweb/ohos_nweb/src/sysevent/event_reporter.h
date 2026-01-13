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

#ifndef EVENT_REPORT_H
#define EVENT_REPORT_H

#include <string>
#include <vector>

#include "oh_web_performance_timing.h"

enum class CrashType {
    TIMEOUT,
    MAILBOX_NONEXISTENT
};

void ReportRenderJsFreeze(int32_t pid, const std::string& packageName, const std::string& processName,
                          const std::string& freezeMsg, int32_t uid);

void ReportPageLoadStats(int instanceId,
                         int accessSumCount,
                         int accessSuccCount,
                         int accessFailCount);

void ReportMultiInstanceStats(int instanceId, int nwebCount, int nwebMaxCount);

void ReportPageLoadErrorInfo(int instanceId,
                             const std::string errorType,
                             int errorCode,
                             uint32_t errorCount,
                             const std::string errorDesc);

void ReportJankStats(int64_t startTime,
                     int duration,
                     std::vector<uint16_t> jankStats,
                     int jankStatsVer);

void ReportLockdownModeStatus(void);

void ReportPageLoadTimeStats(OhWebPerformanceTiming loadPageTime);

void ReportAudioPlayErrorInfo(const std::string errorType,
                              int errorCode,
                              const std::string errorDesc);

void ReportVideoPlayErrorInfo(const std::string errorType,
                              int errorCode,
                              const std::string errorDesc);

void ReportAudioFrameDropStats(int frameCount);

void ReportVideoFrameDropStats(uint32_t frameCount, uint64_t frameDuration);

void ReportDumplicateFileUpload(const std::string errorDesc);

void ReportDragDropStatus(const std::string& eventName, int32_t id);

void ReportDragDropInfo(const std::string& eventName,
                        int32_t id,
                        const std::string& fragment,
                        const std::string& linkUrl,
                        const std::string& linkHtml);

void ReportForceZoomEnable(void);

void ReportOpenPrivateMode(void);

void ReportPageDownLoadErrorInfo(long downloadId, int errorCode);

void ReportSlideJankStats(int64_t startTime,
                          int64_t duration,
                          int32_t totalAppFrames,
                          int32_t totalAppMissedFrames,
                          int64_t maxAppFrametime,
                          int32_t maxAppSeqMissedFrames);

void ReportSiteIsolationMode(const std::string site_isolation_status);

void ReportDrmEncryptedPlayback(const std::string& mediaType,
                                const std::string& drmSystem,
                                const std::string& encryptedAlgo);

void ReportVideoEncodeFormat(const std::string& encodeFormat);

void ReportFfmpegCodecOperation(const std::string& codecType, const std::string& format);

void ReportAudioHardwareDecode(const std::string& codecFormat);

void ReportPictureDecode(const std::string& pictureType);

void ReportRendererMem(const std::string& type,
                       const std::string& pid,
                       const std::string& rss,
                       const std::string& pss,
                       const std::string& js_heap_total,
                       const std::string& js_heap_used,
                       const std::string& gpu_mem,
                       const std::string& url);

void ReportWebMediaPlayErrorInfo(const std::string& errorType,
                              int errorCode,
                              const std::string& errorDesc);

void ReportAvSessionStatus(const std::string& disable_web_av_session_status);

void ReportDragBlank(int64_t duration);

void ReportFirstMeaningfulPaintDone(OhWebPerformanceTiming loadPageTime);

void ReportGpuProcessEvent(CrashType type, std::string eventContent);

void ReportAppfreeze(int32_t pid, const std::string& packageName, const std::string& processName,
                     const std::string& freezeMsg, int32_t uid);
#endif
