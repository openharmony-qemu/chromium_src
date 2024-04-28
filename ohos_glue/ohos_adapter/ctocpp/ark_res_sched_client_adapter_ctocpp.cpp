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

#include "ohos_adapter/ctocpp/ark_res_sched_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"

namespace OHOS::ArkWeb {

using ArkResSchedClientAdapterReportKeyThreadFunc =
    bool (*)(int32_t statusAdapter, pid_t pid, pid_t tid, int32_t roleAdapter);
static ArkResSchedClientAdapterReportKeyThreadFunc
    ark_res_sched_client_adapter_report_key_thread = nullptr;

using ArkResSchedClientAdapterReportWindowStatusFunc = bool (*)(
    int32_t statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId);
static ArkResSchedClientAdapterReportWindowStatusFunc
    ark_res_sched_client_adapter_report_window_status = nullptr;

using ArkResSchedClientAdapterReportSceneFunc = bool (*)(int32_t statusAdapter,
                                                         int32_t sceneAdapter,
                                                         int32_t nwebId);
static ArkResSchedClientAdapterReportSceneFunc
    ark_res_sched_client_adapter_report_scene = nullptr;

using ArkResSchedClientAdapterReportAudioDataFunc =
    bool (*)(int32_t statusAdapter, pid_t pid, pid_t tid);
static ArkResSchedClientAdapterReportAudioDataFunc
    ark_res_sched_client_adapter_report_audio_data = nullptr;

using ArkResSchedClientAdapterReportWindowIdFunc = void (*)(int32_t windowId,
                                                            int32_t nwebId);
static ArkResSchedClientAdapterReportWindowIdFunc
    ark_res_sched_client_adapter_report_window_id = nullptr;

using ArkResSchedClientAdapterReportNWebInitFunc =
    void (*)(int32_t statusAdapter, int32_t nweb_id);
static ArkResSchedClientAdapterReportNWebInitFunc
    ark_res_sched_client_adapter_report_nweb_init = nullptr;

using ArkResSchedClientAdapterReportRenderProcessStatusFunc =
    void (*)(int32_t statusAdapter, pid_t pid);
static ArkResSchedClientAdapterReportRenderProcessStatusFunc
    ark_res_sched_client_adapter_report_render_process_status = nullptr;

using ArkResSchedClientAdapterReportScreenCaptureFunc =
    bool (*)(int32_t statusAdapter, pid_t pid);
static ArkResSchedClientAdapterReportScreenCaptureFunc
    ark_res_sched_client_adapter_report_screen_capture = nullptr;

using ArkResSchedClientAdapterReportVideoPlayingFunc =
    bool (*)(int32_t statusAdapter, pid_t pid);
static ArkResSchedClientAdapterReportVideoPlayingFunc
    ark_res_sched_client_adapter_report_video_playing = nullptr;

using ArkResSchedClientAdapterReportProcessInUseFunc = void (*)(pid_t pid);
static ArkResSchedClientAdapterReportProcessInUseFunc
    ark_res_sched_client_adapter_report_process_in_use = nullptr;

using ArkResSchedClientAdapterReportSiteIsolationModeFunc = void (*)(bool mode);
static ArkResSchedClientAdapterReportSiteIsolationModeFunc
    ark_res_sched_client_adapter_report_site_isolation_mode = nullptr;

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportKeyThread(int32_t statusAdapter, pid_t pid,
                                               pid_t tid, int32_t roleAdapter) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_key_thread) {
    ark_res_sched_client_adapter_report_key_thread =
        reinterpret_cast<ArkResSchedClientAdapterReportKeyThreadFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_key_thread_static"));
    if (!ark_res_sched_client_adapter_report_key_thread) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_key_thread(statusAdapter, pid, tid,
                                                        roleAdapter);
}

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportWindowStatus(int32_t statusAdapter,
                                                  pid_t pid, uint32_t windowId,
                                                  int32_t nwebId) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_window_status) {
    ark_res_sched_client_adapter_report_window_status =
        reinterpret_cast<ArkResSchedClientAdapterReportWindowStatusFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_window_status_static"));
    if (!ark_res_sched_client_adapter_report_window_status) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_window_status(statusAdapter, pid,
                                                           windowId, nwebId);
}

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportScene(int32_t statusAdapter,
                                           int32_t sceneAdapter,
                                           int32_t nwebId) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_scene) {
    ark_res_sched_client_adapter_report_scene =
        reinterpret_cast<ArkResSchedClientAdapterReportSceneFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_scene_static"));
    if (!ark_res_sched_client_adapter_report_scene) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_scene(statusAdapter, sceneAdapter,
                                                   nwebId);
}

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportAudioData(int32_t statusAdapter, pid_t pid,
                                               pid_t tid) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_audio_data) {
    ark_res_sched_client_adapter_report_audio_data =
        reinterpret_cast<ArkResSchedClientAdapterReportAudioDataFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_audio_data_static"));
    if (!ark_res_sched_client_adapter_report_audio_data) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_audio_data(statusAdapter, pid,
                                                        tid);
}

ARK_WEB_NO_SANITIZE
void ArkResSchedClientAdapter::ReportWindowId(int32_t windowId,
                                              int32_t nwebId) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_window_id) {
    ark_res_sched_client_adapter_report_window_id =
        reinterpret_cast<ArkResSchedClientAdapterReportWindowIdFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_window_id_static"));
    if (!ark_res_sched_client_adapter_report_window_id) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return;
    }
  }

  // Execute
  ark_res_sched_client_adapter_report_window_id(windowId, nwebId);
}

ARK_WEB_NO_SANITIZE
void ArkResSchedClientAdapter::ReportNWebInit(int32_t statusAdapter,
                                              int32_t nweb_id) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_nweb_init) {
    ark_res_sched_client_adapter_report_nweb_init =
        reinterpret_cast<ArkResSchedClientAdapterReportNWebInitFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_nweb_init_static"));
    if (!ark_res_sched_client_adapter_report_nweb_init) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return;
    }
  }

  // Execute
  ark_res_sched_client_adapter_report_nweb_init(statusAdapter, nweb_id);
}

ARK_WEB_NO_SANITIZE
void ArkResSchedClientAdapter::ReportRenderProcessStatus(int32_t statusAdapter,
                                                         pid_t pid) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_render_process_status) {
    ark_res_sched_client_adapter_report_render_process_status =
        reinterpret_cast<ArkResSchedClientAdapterReportRenderProcessStatusFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_render_process_status_"
                "static"));
    if (!ark_res_sched_client_adapter_report_render_process_status) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return;
    }
  }

  // Execute
  ark_res_sched_client_adapter_report_render_process_status(statusAdapter, pid);
}

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportScreenCapture(int32_t statusAdapter,
                                                   pid_t pid) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_screen_capture) {
    ark_res_sched_client_adapter_report_screen_capture =
        reinterpret_cast<ArkResSchedClientAdapterReportScreenCaptureFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_screen_capture_static"));
    if (!ark_res_sched_client_adapter_report_screen_capture) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_screen_capture(statusAdapter, pid);
}

ARK_WEB_NO_SANITIZE
bool ArkResSchedClientAdapter::ReportVideoPlaying(int32_t statusAdapter,
                                                  pid_t pid) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_video_playing) {
    ark_res_sched_client_adapter_report_video_playing =
        reinterpret_cast<ArkResSchedClientAdapterReportVideoPlayingFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_video_playing_static"));
    if (!ark_res_sched_client_adapter_report_video_playing) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return false;
    }
  }

  // Execute
  return ark_res_sched_client_adapter_report_video_playing(statusAdapter, pid);
}

ARK_WEB_NO_SANITIZE
void ArkResSchedClientAdapter::ReportProcessInUse(pid_t pid) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_process_in_use) {
    ark_res_sched_client_adapter_report_process_in_use =
        reinterpret_cast<ArkResSchedClientAdapterReportProcessInUseFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_res_sched_client_adapter_report_process_in_use_static"));
    if (!ark_res_sched_client_adapter_report_process_in_use) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return;
    }
  }

  // Execute
  ark_res_sched_client_adapter_report_process_in_use(pid);
}

ARK_WEB_NO_SANITIZE
void ArkResSchedClientAdapter::ReportSiteIsolationMode(bool mode) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_res_sched_client_adapter_report_site_isolation_mode) {
    ark_res_sched_client_adapter_report_site_isolation_mode = reinterpret_cast<
        ArkResSchedClientAdapterReportSiteIsolationModeFunc>(
        ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
            "ark_res_sched_client_adapter_report_site_isolation_mode_static"));
    if (!ark_res_sched_client_adapter_report_site_isolation_mode) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return;
    }
  }

  // Execute
  ark_res_sched_client_adapter_report_site_isolation_mode(mode);
}

ArkResSchedClientAdapterCToCpp::ArkResSchedClientAdapterCToCpp() {
}

ArkResSchedClientAdapterCToCpp::~ArkResSchedClientAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkResSchedClientAdapterCToCpp, ArkResSchedClientAdapter,
    ark_res_sched_client_adapter_t>::kBridgeType = ARK_RES_SCHED_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb
