/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "nweb_impl.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

// IS_OHOS begin
#ifdef TRACE_EVENT0
#undef TRACE_EVENT0
#endif
#ifdef TRACE_EVENT1
#undef TRACE_EVENT1
#endif
#define TRACE_EVENT0(...)
#define TRACE_EVENT1(...)
// IS_OHOS end

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/base/report_loss_frame_ext.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "components/web_cache/browser/web_cache_manager.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "arkweb/chromium_ext/components/web_cache/browser/web_cache_manager_utils.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)
#include "base/memory/memory_pressure_listener.h"
#include "base/time/time.h"
#endif  // ARKWEB_PERFORMANCE_MEMORY_THRESHOLD

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"
#include "cef_delegate/nweb_download_handler_delegate.h"
#include "cef/libcef/browser/chrome/chrome_browser_context.h"
#include "arkweb/chromium_ext/content/renderer/host_proxy.h"
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "ndk/arkweb_native_object.h"
#include "nweb_delegate_adapter.h"
#include "nweb_export.h"
#include "nweb_handler.h"
#include "nweb_handler_delegate.h"
#include "nweb_hilog.h"
#include "nweb_hit_test_result_impl.h"
#include "ohos_adapter_helper.h"
#include "res_sched_client_adapter.h"
#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "arkweb/chromium_ext/ui/base/clipboard/ohos/clipboard_ohos.h"
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif
#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
#include "content/public/browser/browsing_data_remover.h"
#include "soc_perf_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
#include "cef_delegate/nweb_application.h"
#include "content/public/browser/network_service_instance.h"
#include "services/network/network_service.h"
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
#include "components/security_state/core/security_state.h"
#endif  // BUILDFLAG(ARKWEB_SECURITY_STATE)

#if BUILDFLAG(ARKWEB_HTTP_DNS)
#include "cef/include/cef_app.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "libcef/browser/browser_context.h"
#include "capi/nweb_prefetch_options.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_config.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_factory.h"
#include "ohos_cef_ext/libcef/browser/predictors/predictor_database.h"
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_COOKIE)
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "content/public/common/content_paths.h"
#endif  // BUILDFLAG(ARKWEB_COOKIE)

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
#include "content/public/browser/network_service_instance.h"
#include "services/network/network_service.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_UA)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_ua_config.h"
#include "ohos_nweb_ex/overrides/ohos_nweb/src/cef_delegate/cloud_control_config/ua_push_config/nweb_safe_browsing_ua_push_config.h"
#endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_engine_cloud_config.h"
#endif
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL) && BUILDFLAG(IS_ARKWEB_EXT)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_engine_global_config.h"
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "cef/libcef/browser/browser_context.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/network_service_instance.h"
#include "content/public/browser/storage_partition.h"
#include "services/network/network_service.h"
#endif

#if BUILDFLAG(ARKWEB_I18N)
#include <regex>

#include "ui/base/ui_base_switches.h"
#endif

#if BUILDFLAG(ARKWEB_ITP)
#include "cef/ohos_cef_ext/libcef/browser/anti_tracking/third_party_cookie_access_policy.h"
#endif

#if defined(OHOS_CLOUD_CONTROL)
#include "cef/libcef/browser/alloy/alloy_browser_context.h"
#endif

#if BUILDFLAG(ARKWEB_SUSPEND_ALL_TIMERS)
#include "arkweb/chromium_ext/content/browser/ohos/content_view_statics_ohos.h"
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
extern bool g_siteIsolationMode;
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "content/public/browser/render_process_host.h"
#endif  // ARKWEB_RENDER_PROCESS_MODE

#if BUILDFLAG(ARKWEB_WARMUP_SERVICEWORKER)
#include "content/public/browser/service_worker_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/origin_util.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "url/gurl.h"
#include "url/origin.h"
#endif  // BUILDFLAG(ARKWEB_WARMUP_SERVICEWORKER)

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
#include "cef/ohos_cef_ext/libcef/browser/ohos_safe_browsing/ohos_url_trust_list_interface.h"
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
extern bool g_siteIsolationMode;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "cef/ohos_cef_ext/libcef/browser/subresource_filter/adblock_list.h"
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "cef/ohos_cef_ext/libcef/browser/net/extra_headers_throttle.h"
#include "cef/ohos_cef_ext/libcef/browser/net/ohos_url_rewrite_controller.h"
#endif

#ifdef OHOS_WEB_LTPO
#include "content/browser/gpu/gpu_process_host.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#endif

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
#include "nweb_resize_helper.h"
#include "ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "cef_delegate/nweb_extension_window_delegate_handler.h"
#include "ohos_cef_ext/libcef/browser/extensions/windows_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "cef/include/cef_request_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "ohos_cef_ext/libcef/browser/offscreen_document_dialog_manager.h"
#include "ohos_cef_ext/libcef/browser/extensions/web_extension_menu_manager.h"
#include "ohos_cef_ext/libcef/browser/extensions/tab_extensions_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "extensions/common/extension.h"
#include "extensions/browser/disable_reason.h"
#include "extensions/browser/uninstall_reason.h"
#include "extensions/browser/ui_util.h"
#include "extensions/browser/extension_registry_info_manager.h"
#include "extensions/browser/extension_system.h"
#include "nweb_extension_action_cef_delegate.h"
#include "nweb_js_dialog_result_impl.h"
#include "chrome/browser/extensions/api/debugger/extension_dev_tools_infobar_delegate.h"
#include "arkweb/chromium_ext/components/crx_file/crx_key_service.h"
#include "cef/ohos_cef_ext/libcef/browser/permission/offscreen_permission_request_handler.h"
#endif // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "components/embedder_support/user_agent_utils.h"
#include "ohos_cef_ext/libcef/browser/alloy/alloy_browser_ua_config.h"
#endif

#if defined(USE_CEF)
#include "cef_delegate/nweb_delegate.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "chrome/common/webui_url_constants.h"
#include "url/url_constants.h"
#include "chrome/browser/extensions/crx_installer.h"
#include "extensions/browser/install/crx_install_error.h"
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
#include "content/browser/gpu/gpu_process_host.h"
#include "services/viz/privileged/mojom/gl/gpu_service.mojom.h"
#include "arkweb/chromium_ext/base/debug/arkweb_dump_info.h"
#endif

#include "net/proxy_resolution/proxy_config_service_ohos.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/arkweb_proxy_config_monitor.h"

#if BUILDFLAG(ARKWEB_EDM_POLICY)
#include "cef/ohos_cef_ext/libcef/browser/policy/browser_policy_handler.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/ohos/nweb_engine_event_logger.h"
#include "base/ohos/nweb_engine_event_logger_code.h"
#include "base/path_service.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "ohos_nweb/src/nweb_web_storage_impl.h"
#include "chrome/browser/browser_process.h"
#include "cef/libcef/browser/prefs/browser_prefs.h"
#include "components/prefs/pref_service.h"
#include "components/os_crypt/sync/os_crypt.h"
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/static/nweb_static_dispatcher.h"
#endif

#include "arkweb/chromium_ext/base/arkweb_report_statistics.h"

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
OnReportStatisticLogFunc
    OHOS::NWeb::NWebImpl::on_report_statistic_log_callback_ = nullptr;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
OnArkWebStaticOffscreenDocumentAlertFunc   OHOS::NWeb::NWebImpl::on_off_screen_alert_callback_ = nullptr;
OnArkWebStaticOffscreenDocumentConfirmFunc OHOS::NWeb::NWebImpl::on_off_screen_confirm_callback_ = nullptr;
OnArkWebStaticOffscreenDocumentPromptFunc  OHOS::NWeb::NWebImpl::on_off_screen_prompt_callback_ = nullptr;

OnArkWebStaticOffscreenDocumentPermissionRequestFunc
    OHOS::NWeb::NWebImpl::on_offscreen_document_permission_request_callback_ = nullptr;
OnArkWebStaticOffscreenDocumentWindowNewFunc
    OHOS::NWeb::NWebImpl::on_off_screen_window_new_callback_ = nullptr;
uint32_t OHOS::NWeb::NWebImpl::off_screen_nweb_id_ = 0;
#endif // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_COOKIE)
std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs>
    OHOS::NWeb::NWebImpl::save_initargs_ = nullptr;
bool OHOS::NWeb::NWebImpl::should_lazy_init_web_engine_ = false;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/chromium_ext/components/viz/host/blankless_data_controller.h"
#include "arkweb/ohos_adapter_ndk/window_manager_adapter/window_manager_adapter_impl.h"
#endif

#include "ohos_nweb/src/capi/nweb_devtools_message_handler.h"
#include "ohos_nweb/src/nweb_advanced_security.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"

#include "capi/nweb_logger_report_event_callback.h"
#include "base/ohos/nweb_engine_event_logger.h"

#include "cef/include/cef_app.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
#include "base/ohos/sys_info_utils_ext.h"
#endif

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
#include "arwkweb/chromium_ext/gpu/config/gpu_finch_feature_ext.h"
#endif

namespace {
uint32_t g_nweb_count = 0;
const uint32_t kSurfaceMaxWidth = 7680;
const uint32_t kSurfaceMaxHeight = 7680;
const float richtextDisplayRatio = 1.0;

#if BUILDFLAG(ARKWEB_NWEB_EX)
bool g_browser_service_api_enabled = false;
std::vector<std::string> g_browser_args = {};
int32_t g_browser_service_sdk_api_level = 0;
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_CLIPBOARD)
bool g_clipboard_site_permission_enabled = false;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
static bool g_angle_config = false;

enum class AngleStatus : int {
  kDefaultAngleStatus,
  kEnableAngle,
  kDisableAngle
};
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
enum class SiteIsolationInitMode{
  INIT,
  PARTIAL,
  STRICT
};
const int32_t ALREADY_SET_ERR = 4;
const int32_t SINGLE_RENDER_SET_STRICT_ERR = 3;
const int32_t ADVANCED_SECURITY_SET_ERR = 2;
const int32_t SITE_ISOLATION_SET_SUCCESS = 1;

bool g_siteIsolationModeInit = false;
SiteIsolationInitMode g_siteIsolationModeInitValue = SiteIsolationInitMode::INIT;
#endif  // BUILDFLAG(ARKWEB_SITE_ISOLATION)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
std::optional<std::string> g_extension_name;

static void HandleExtensionInstallResult(
    OnExtensionInstallCallback callback,
    const std::optional<extensions::CrxInstallError>& error,
    const std::string& extension_id) {
  if (!callback) {
    return;
  }

  if (error.has_value()) {
    std::string error_message = base::UTF16ToUTF8(error->message());
    callback(static_cast<int>(error->type()), error_message.c_str(), nullptr);
  } else {
    callback(0, "Success", extension_id.c_str());
  }
}

static void ConfigureCrxInstaller(
    scoped_refptr<extensions::CrxInstaller> installer) {
  installer->set_install_immediately(true);
}

static void PerformCrxInstallation(const std::string& file_path,
                                   OnExtensionInstallCallback callback,
                                   content::BrowserContext* context,
                                   bool file_exists) {
  if (!file_exists) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "File not found", nullptr);
    }
    return;
  }

  extensions::ExtensionService* service =
      extensions::ExtensionSystem::Get(context)->extension_service();
  if (!service) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Extension service not available", nullptr);
    }
    return;
  }

  Profile* profile = Profile::FromBrowserContext(context);
  auto prompt = std::make_unique<ExtensionInstallPrompt>(profile, nullptr);

  scoped_refptr<extensions::CrxInstaller> installer =
      extensions::CrxInstaller::Create(service, std::move(prompt));

  ConfigureCrxInstaller(installer);

  installer->AddInstallerCallback(
      base::BindOnce([](OnExtensionInstallCallback callback,
                       scoped_refptr<extensions::CrxInstaller> installer,
                       const std::optional<extensions::CrxInstallError>& error) {
        std::string extension_id;
        if (!error.has_value() && installer->extension()) {
          extension_id = installer->extension()->id();
        }
        HandleExtensionInstallResult(callback, error, extension_id);
        crx_file::CrxKeyService::GetInstance()->SetPublisherKeys({});
      }, callback, installer));
  installer->InstallCrx(base::FilePath(file_path));
}

static void ConfigureCrxInstallerV2(
    scoped_refptr<extensions::CrxInstaller> installer,
    const NWebExtensionInstallProperties& options) {
  installer->set_install_immediately(true);

  if (options.silent.has_value() && *options.silent) {
    installer->set_allow_silent_install(true);
    installer->set_grant_permissions(true);
  }
}

static void PerformCrxInstallationV2(const std::string& file_path,
                                     const NWebExtensionInstallProperties& options,
                                     OnExtensionInstallCallback callback,
                                     content::BrowserContext* context,
                                     bool file_exists) {
  if (!file_exists) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "File not found", nullptr);
    }
    return;
  }

  extensions::ExtensionService* service =
      extensions::ExtensionSystem::Get(context)->extension_service();
  if (!service) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Extension service not available", nullptr);
    }
    return;
  }

  if (options.publisherKeys.has_value() && !options.publisherKeys->empty()) {
    OHOS::NWeb::NWebImpl::SetPublisherKeys(*options.publisherKeys);
  }

  Profile* profile = Profile::FromBrowserContext(context);
  auto prompt = std::make_unique<ExtensionInstallPrompt>(profile, nullptr);

  scoped_refptr<extensions::CrxInstaller> installer =
      extensions::CrxInstaller::Create(service, std::move(prompt));

  ConfigureCrxInstallerV2(installer, options);

  installer->AddInstallerCallback(
      base::BindOnce([](OnExtensionInstallCallback callback,
                       scoped_refptr<extensions::CrxInstaller> installer,
                       const std::optional<extensions::CrxInstallError>& error) {
        std::string extension_id;
        if (!error.has_value() && installer->extension()) {
          extension_id = installer->extension()->id();
        }
        HandleExtensionInstallResult(callback, error, extension_id);
        crx_file::CrxKeyService::GetInstance()->SetPublisherKeys({});
      }, callback, installer));
  installer->InstallCrx(base::FilePath(file_path));
}

static void HandleExtensionUninstallResult(
    OnExtensionUninstallCallback callback,
    bool success,
    const std::string& message) {
  if (!callback) {
    return;
  }
  callback(success, message.c_str());
}

#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
// For maximum count of nweb instance
uint32_t g_nweb_max_count = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
static double default_zoom_factor = 1.0;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
const int NWebPlaybackState_NONE = 0;
#endif

#if BUILDFLAG(ARKWEB_CRASHPAD)
static std::string g_crashpad_target_location =
    "/data/storage/el2/base/crashpad";
static std::string g_feedbacklogs_crash_path =
    "/data/storage/el2/base/haps/entry/files/logs/logs/logFile";
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
static std::string kDirEl1 = "/data/storage/el1";
static std::string kDirEl2 = "/data/storage/el2";

void SetPathList(const std::vector<std::string>& module_name,
                 std::map<OHOS::NWeb::NWebImpl::PathType, std::vector<base::FilePath>>& path_lists) {
  for (auto& name : module_name) {
    path_lists[OHOS::NWeb::NWebImpl::PathType::kDirResource].push_back(
        base::FilePath(kDirEl1 + "/bundle/" + name + "/resources/resfile"));
    path_lists[OHOS::NWeb::NWebImpl::PathType::kDirFile].push_back(
        base::FilePath(kDirEl2 + "/base/haps/" + name + "/files"));
    path_lists[OHOS::NWeb::NWebImpl::PathType::kDirCache].push_back(
        base::FilePath(kDirEl2 + "/base/haps/" + name + "/cache"));
    path_lists[OHOS::NWeb::NWebImpl::PathType::kDirTemp].push_back(
        base::FilePath(kDirEl2 + "/base/haps/" + name + "/temp"));
  }
  path_lists[OHOS::NWeb::NWebImpl::PathType::kDirFile].push_back(base::FilePath(kDirEl2 + "/base/files"));
  path_lists[OHOS::NWeb::NWebImpl::PathType::kDirCache].push_back(base::FilePath(kDirEl2 + "/base/cache"));
  path_lists[OHOS::NWeb::NWebImpl::PathType::kDirTemp].push_back(base::FilePath(kDirEl2 + "/base/temp"));
}

void SetExcludedPathList(const std::vector<std::string>& module_name,
                         std::map<OHOS::NWeb::NWebImpl::PathType, std::vector<base::FilePath>>& path_lists) {
  for (auto& name : module_name) {
    path_lists[OHOS::NWeb::NWebImpl::PathType::kDirCache].push_back(
        base::FilePath(kDirEl2 + "/base/haps/" + name + "/cache/web"));
  }
  path_lists[OHOS::NWeb::NWebImpl::PathType::kDirCache].push_back(base::FilePath(kDirEl2 + "/base/cache/web"));
}

void GetExcludedPathList(std::map<OHOS::NWeb::NWebImpl::PathType, std::vector<base::FilePath>>& src_path_lists,
                         std::vector<std::string>& des_path_lists) {
    for (const auto& [path_type, file_paths] : src_path_lists) {
        for (const auto& file_path : file_paths) {
            des_path_lists.push_back(file_path.value());
        }
    }
}

bool CheckRealPath(const base::FilePath& real_path, const std::vector<base::FilePath>& path_list,
                   bool allow_parent, const std::vector<base::FilePath>& excluded_paths = {}) {
  for (const auto& excluded_path : excluded_paths) {
    if (excluded_path == real_path) {
      return false;
    }
  }

  for (const auto& dir : path_list) {
    if ((allow_parent && dir == real_path) || dir.IsParent(real_path)) {
      return true;
    }
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
static bool set_whole_page_drawing = false;
#endif

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
const int32_t SOC_PERF_WEB_DRAG_RESIZE_ID = 10012;
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
const int32_t WEB_RESIZE_CLOSE_DELAY_TIME = 500;
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
// Benchmarking against Windows, the average drag-over interval is 65 milliseconds.
constexpr base::TimeDelta DRAG_OVER_INTERVAL = base::Milliseconds(65);
#endif

using ASHelper = OHOS::NWeb::NWebAdvancedSecurityHelper;

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
std::shared_ptr<NWebLoggerCallback> g_logger_callback;
#endif
bool g_logger_callback_initialized = false;

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
static const int kMigrationBase = 10;
static const int kMigrationMaxCount = 10000;
constexpr base::FilePath::CharType kMigrateKeyFlagFile[] =
    FILE_PATH_LITERAL("migrate/MIGRATE_ASSET_SUCCESS");
#endif
enum class GpuMode {
  UNINITIALIZED = 0,
  FORCE_OOP = 1,
  FORCE_IN_PROCESS = 2
};
static GpuMode oop_gpu_switch = GpuMode::UNINITIALIZED;

bool GetWebOptimizationValue() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetWebOptimizationValue();
}

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
AngleStatus GetANGLEStatus() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  int angle_value = system_properties_adapter.GetIntParameter("web.gpu.angle", 0);
  return static_cast<AngleStatus>(angle_value);
}

bool GetVulkanStatus(const std::list<std::string>& web_engine_args) {
  std::string vulkan_status = "false";
  for (const auto& arg : web_engine_args) {
    if (arg.find("--ohos-enable-vulkan") != std::string::npos) {
      size_t pos = arg.find("=");
      if (pos != std::string::npos) {
        vulkan_status = arg.substr(pos + 1);
      } else {
        vulkan_status = "true";
      }
      break;
    }
  }

  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  std::string cmd_vulkan_enable = system_properties_adapter.GetVulkanStatus();

  if (cmd_vulkan_enable != "None") {
    vulkan_status = cmd_vulkan_enable;
  }

  return vulkan_status != "false";
}

void ApplyANGLEStatus(std::list<std::string>& web_engine_args) {
  if (!base::FeatureList::IsEnable(features::kDefaultANGLE) || base::ohos::IsEmulator()) {
    return;
  }

  AngleStatus angle_status = GetAngleStatus();
  bool vulkan_status = GetVulkanStatus(web_engine_args);
  bool angle_flag = false;
  if (vulkan_status && g_angle_config) {
    angle_flag = true;
  }

  if (angle_status == AngleStatus::kEnableAngle) {
    angle_flag = true;
  } else if (angle_status == AngleStatus::kDisableAngle) {
    angle_flag = false;
  }

  if (angle_flag) {
    LOG(INFO) << "ANGLE enabled";
    web_engine_args.emplace_back("--use-gl=angle");
    web_engine_args.emplace_back("--use-cmd-decoder=passthrough");
  }
}
#endif

static bool IsAdvancedSecurityMode() {
  return ASHelper::Inst().IsSecFeatureEnabled(ASHelper::Feature::SECURE_SHIELD_ENABLED);
}

static std::string GetOOPGPUStatus() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetOOPGPUStatus();
}

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
static std::string GetSiteIsolationMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetSiteIsolationMode();
}

static bool IsMultipleRenderProcess() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (command_line &&
      command_line->HasSwitch(switches::kRendererProcessLimit)) {
    int limit_value = std::stoi(
        command_line->GetSwitchValueASCII(switches::kRendererProcessLimit));
    return (limit_value > 1
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
            && OHOS::NWeb::NWebImpl::GetRenderProcessMode() ==
                   OHOS::NWeb::RenderProcessMode::MULTIPLE_MODE
#endif
    );
  }
  return false;
}

static bool ShouldEnableSiteIsolation() {
  std::string isSiteIsolationMode = GetSiteIsolationMode();

  if (isSiteIsolationMode == "false") {
    return false;
  }

  if (g_siteIsolationModeInit) {
    if (g_siteIsolationModeInitValue == SiteIsolationInitMode::STRICT && !IsMultipleRenderProcess()){
      LOG(ERROR) << "Site isolation mode cannot be strict when single render";
    } else {
      return (g_siteIsolationModeInitValue == SiteIsolationInitMode::STRICT)? true : false;
    }
  }

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (!command_line) {
    LOG(WARNING)
        << "web component is not initialized, unable to enable site isolation";
    return false;
  }

  // for judge PC&&Tablet devices
  bool isIgnoreLockdownMode = (*base::CommandLine::ForCurrentProcess())
                                  .HasSwitch(switches::kIgnoreLockdownMode);

  if (isIgnoreLockdownMode && IsMultipleRenderProcess()) {
    return true;
  }

  if (IsAdvancedSecurityMode() && IsMultipleRenderProcess()) {
    return true;
  }

  return false;
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
static void RemoveBlanklessFrame(std::shared_ptr<OHOS::NWeb::NWebHandler> handle, int32_t lcp_time, bool isAnime)
{
  if (!handle) {
    return;
  }
  if (isAnime) {
    handle->OnRemoveBlanklessFrameWithAnimation(lcp_time);
    return;
  }
  handle->OnRemoveBlanklessFrame(lcp_time);
}
#endif

static std::string GetNetlogMode() {
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetNetlogMode();
}

bool GetIsPopup(std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsPopup() : false;
}

std::string GetDumpPath(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetDumpPath() : "";
}

bool GetIsFrameInfoDump(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsFrameInfoDump() : false;
}

bool GetIsEnhanceSurface(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsEnhanceSurface() : false;
}

bool GetIsMultiRendererProcess(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetIsMultiRendererProcess() : false;
}

std::list<std::string> GetArgsToAdd(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  if (!init_args) {
    std::list<std::string> temp;
    return temp;
  }

  return init_args->GetArgsToAdd();
}

std::list<std::string> GetArgsToDelete(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  if (!init_args) {
    std::list<std::string> temp;
    return temp;
  }

  return init_args->GetArgsToDelete();
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
std::string GetSharedRenderProcessToken(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetSharedRenderProcessToken() : "";
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
float GetVirtualPixelRatioForScrollbar() {
  auto display_manager_adapter =
      OHOS::NWeb::OhosAdapterHelperExt::CreateDisplayMgrAdapter();
  if (display_manager_adapter == nullptr) {
    LOG(ERROR) << "display_manager_adapter is nullptr";
    return -1;
  }
  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (display == nullptr) {
    LOG(ERROR) << "display is nullptr";
    return -1;
  }
  float ratio = display->GetVirtualPixelRatio();
  LOG(DEBUG) << "GetVirtualPixelRatio ratio:" << std::to_string(ratio);
  return ratio;
}
#endif

/**
 * Parse command line flags from a flat buffer, supporting double-quote enclosed strings
 * containing whitespace. argv elements are derived by splitting the buffer on whitepace; double
 * quote characters may enclose tokens containing whitespace; a double-quote literal may be
 * escaped with back-slash. (Otherwise backslash is taken as a literal).
 */
std::vector<std::string> tokenizeQuotedArguments(const std::string& buffer) {
    int max_file_size = 96 * 1024;
    if (buffer.size() > max_file_size) {
        return {};  // Return an empty vector on error
    }
    std::vector<std::string> args;
    std::string arg;
    char currentQuote = '\0';
    const char singleQuote = '\'';
    const char doubleQuote = '"';
    for (char c : buffer) {
        // Detect start or end of quote block.
        if ((currentQuote == '\0' && (c == singleQuote || c == doubleQuote)) || c == currentQuote) {
            if (!arg.empty() && arg.back() == '\\') {
                // Last char was a backslash; treat c as a literal.
                arg.back() = c;
            } else {
                currentQuote = (currentQuote == '\0') ? c : '\0';
            }
        } else if (currentQuote == '\0' && std::isspace(c)) {
            if (!arg.empty()) {
                args.push_back(arg);
                arg.clear();
            }
        } else {
            arg.push_back(c);
        }
    }
    if (!arg.empty()) {
        if (currentQuote != '\0') {
            // If quotes are unbalanced, return empty vector
            return {};
        }
        args.push_back(arg);
    }
    LOG(INFO) << "tokenizeQuotedArguments:: tokenize from ohos-command-line succ.";
    return args;
}

#if BUILDFLAG(ARKWEB_GWP_ASAN)
std::string GetGwpAsanEnable()
{
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  return system_properties_adapter.GetStringParameter("web.gwpasan.enable", "none");
}
#endif

static void UpdateInprocessGpuArg(std::list<std::string>& web_engine_args,
                                  bool xml_gpu) {
  std::string oop_gpu_enable = GetOOPGPUStatus();
  if (oop_gpu_enable == "true") {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(),
                        "--in-process-gpu");
    if (it != web_engine_args.end()) {
      web_engine_args.erase(it);
    }
    return;
  } else if (oop_gpu_enable == "false") {
    return;
  }
  bool enable_oop_gpu_switch = false;
  if (oop_gpu_switch == GpuMode::UNINITIALIZED) {
    enable_oop_gpu_switch = xml_gpu;
  } else if (oop_gpu_switch == GpuMode::FORCE_OOP) {
    enable_oop_gpu_switch = true;
  } else if (oop_gpu_switch == GpuMode::FORCE_IN_PROCESS) {
    enable_oop_gpu_switch = false;
  }

  if (enable_oop_gpu_switch) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(),
                        "--in-process-gpu");
    if (it != web_engine_args.end()) {
      web_engine_args.erase(it);
    } 
  }
}

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
void InitialWebEngineArgs(
    std::list<std::string>& web_engine_args,
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  web_engine_args.clear();

  web_engine_args.emplace_back("/system/bin/web_render");
  web_engine_args.emplace_back("--disable-chrome-login-prompt");
  web_engine_args.emplace_back("--in-process-gpu");
  web_engine_args.emplace_back("--disable-dev-shm-usage");
  web_engine_args.emplace_back("--off-screen-frame-rate=60");
  web_engine_args.emplace_back("--no-unsandboxed-zygote");
  web_engine_args.emplace_back("--no-zygote");
  if (OHOS::NWeb::NWebImpl::GetScrollbarMode() ==
      OHOS::NWeb::ScrollbarMode::FORCE_DISPLAY_SCROLLBAR &&
      OHOS::NWeb::NWebImpl::IsScrollbarModeChanged()) {
    web_engine_args.emplace_back("--disable-features=OverlayScrollbar");
  }
  if (OHOS::NWeb::NWebImpl::GetScrollbarMode() ==
      OHOS::NWeb::ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR &&
      OHOS::NWeb::NWebImpl::IsScrollbarModeChanged()) {
    web_engine_args.emplace_back("--enable-features=UseOzonePlatform,OverlayScrollbar");
  } else {
    web_engine_args.emplace_back("--enable-features=UseOzonePlatform");
  }
  web_engine_args.emplace_back("-ozone-platform=headless");
  web_engine_args.emplace_back("--no-sandbox");
  web_engine_args.emplace_back("--use-mobile-user-agent");
  web_engine_args.emplace_back("--enable-gpu-rasterization");
  web_engine_args.emplace_back("--disable-features=FencedFrames");
  if ((!base::ohos::IsPcDevice() && !base::ohos::IsPcMode()) ||
      base::ohos::IsCompatibleMode()) {
    web_engine_args.emplace_back("--enable-viewport");
  }
  web_engine_args.emplace_back(
      "--browser-subprocess-path=/system/bin/web_render");
  web_engine_args.emplace_back("--zygote-cmd-prefix=/system/bin/web_render");
  web_engine_args.emplace_back("--enable-touch-drag-drop");
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  static float ratio = -1.0f;
  if (ratio < 0) {
    ratio = GetVirtualPixelRatioForScrollbar();
  }
  if (ratio > 0) {
    web_engine_args.emplace_back("--virtual-pixel-ratio=" +
                                 std::to_string(ratio));
  }
#endif
  web_engine_args.emplace_back("--gpu-rasterization-msaa-sample-count=1");
  // enable aggressive domstorage flushing to minimize data loss
  // http://crbug.com/479767
  web_engine_args.emplace_back("--enable-aggressive-domstorage-flushing");
  web_engine_args.emplace_back("--ohos-enable-drdc");
#if BUILDFLAG(ARKWEB_READER_MODE)
  web_engine_args.push_back("--enable-distillability-service");
  web_engine_args.push_back("--enable-dom-distiller");
#endif // ARKWEB_READER_MODE

  std::vector<std::string> modeVector = {"Default", "IncludeSensitive",
                                         "Everything"};
  if (std::find(modeVector.begin(), modeVector.end(), GetNetlogMode()) !=
      modeVector.end()) {
    web_engine_args.emplace_back("--net-log-capture-mode=" + GetNetlogMode());
    web_engine_args.emplace_back(
        "--log-net-log=/data/storage/el2/base/cache/web/netlog.json");
  }

  if (IsAdvancedSecurityMode()) {
    WVLOG_I(
        "In advanced security mode, some HTML5 features will be unavailable, "
        "including "
        "WebAssembly, WebGL, PDF viewer, MathML, speech recognition, etc.");
    web_engine_args.emplace_back("--js-flags=--jitless");

    if (ASHelper::Inst().IsSecFeatureEnabled(ASHelper::Feature::ENABLE_WEBGL)) {
      web_engine_args.emplace_back("--disable-webgl");
      web_engine_args.emplace_back("--disable-webgl2");
    }

    if (ASHelper::Inst().IsSecFeatureEnabled(ASHelper::Feature::ENABLE_PDFVIEWER)) {
      web_engine_args.emplace_back("--disable-pdf-extension");
    }

    if (ASHelper::Inst().IsSecFeatureEnabled(ASHelper::Feature::ENABLE_SPEECHAPI)) {
      web_engine_args.emplace_back(
          "--disable-blink-features=NonAdvancedSecurityMode");
    }

    std::string AdSec = "--advanced_sec_value=" + std::to_string(ASHelper::Inst().GetAdStat());
    web_engine_args.emplace_back(AdSec);

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
    ReportLockdownModeStatus();
#endif
  }

  web_engine_args.emplace_back("--enable-media-stream");
  if (GetIsEnhanceSurface(init_args)) {
    WVLOG_I("is_enhance_surface is true");
    web_engine_args.emplace_back("--ohos-enhance-surface");
  }

  auto args_to_delete = GetArgsToDelete(init_args);
  bool xml_gpu = false;

  for (auto arg : args_to_delete) {
    auto it = std::find(web_engine_args.begin(), web_engine_args.end(), arg);
    if (it != web_engine_args.end()) {
      if (*it == "--in-process-gpu") {
        xml_gpu = true;
        continue;
      }
      web_engine_args.erase(it);
    }
  }

  auto args_to_add = GetArgsToAdd(init_args);
  for (auto arg : args_to_add) {
    web_engine_args.emplace_back(arg);
  }

#if BUILDFLAG(ARKWEB_GWP_ASAN)
  std::string gwpEnable = "--ohos-enable-gwp-asan-type=" + GetGwpAsanEnable();
  web_engine_args.emplace_back(gwpEnable);
#endif

  UpdateInprocessGpuArg(web_engine_args, xml_gpu);

  if (GetIsMultiRendererProcess(init_args)) {
    web_engine_args.emplace_back("--enable-multi-renderer-process");
  }

#if BUILDFLAG(IS_ARKWEB_EXT)
  for (const std::string& arg : g_browser_args) {
    web_engine_args.emplace_back(arg);
  }
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
  ApplyANGLEStatus(web_engine_args);
#endif

  std::string oemmode = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                            .GetSystemPropertiesInstance().GetStringParameter("const.boot.oemmode", "");
  LOG(INFO) << "const.boot.oemmode: " << oemmode;
  base::FilePath ohos_command_line_file(
    FILE_PATH_LITERAL("/data/storage/el1/bundle/arkwebcore/libs/ohos-command-line"));
  if (oemmode == "rd" && base::PathExists(ohos_command_line_file)) {
    std::string ohos_command_line_content;
      if (base::ReadFileToString(ohos_command_line_file, &ohos_command_line_content)) {
        std::vector<std::string> args = tokenizeQuotedArguments(ohos_command_line_content);
          for (auto& arg : args) {
            web_engine_args.emplace_back(arg);
          }
          LOG(INFO) << "ohos connamd line args analysis from ohos-command-line file succ.";
      }
  } else {
    LOG(INFO) << "oemmode is not rd or ohos-command-line does not exist.";
  }
}
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
void MigratePasswordsToPasswordVault() {
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (cache_path.empty()) {
    LOG(INFO) << "[Autofill] cache_path is empty.";
    return;
  }
  if (!g_browser_process->local_state()) {
    LOG(INFO) << "[Autofill] g_browser_process->local_state() is null.";
    return;
  }
  base::FilePath flagFile = cache_path.Append(FILE_PATH_LITERAL(kMigrateKeyFlagFile));
  bool IsFlagFileExist = base::PathExists(flagFile);
  bool migrateReady = g_browser_process->local_state()->GetBoolean(browser_prefs::kMigratePasswordsReady);
  bool migrateVault = g_browser_process->local_state()->GetBoolean(browser_prefs::kMigratePasswordsToPasswordVault);
  LOG(INFO) << "[Autofill] MigratePasswordsReady:" << migrateReady
            << ", MigratePasswordsToPasswordVault:" << migrateVault
            << ", IsFlagFileExist:" << IsFlagFileExist;
  if (migrateReady == true && migrateVault == false && IsFlagFileExist == true) {
    int count = g_browser_process->local_state()->GetInteger(browser_prefs::kMigrationCount);
    LOG(INFO) << "[Autofill] migration count:" << count;
    OSCryptImpl::GetInstance()->SetMigrationCountCurrent(count + 1);
    g_browser_process->local_state()->SetInteger(browser_prefs::kMigrationCount, count + 1);
    g_browser_process->local_state()->CommitPendingWrite();
    if (count <= kMigrationBase || (count % kMigrationBase == 0 && count <= kMigrationMaxCount)) {
      std::shared_ptr<OHOS::NWeb::NWebWebStorageImpl> nweb_web_storage =
          std::make_shared<OHOS::NWeb::NWebWebStorageImpl>();
      nweb_web_storage->MigratePasswords();
    } else if (count > kMigrationMaxCount) {
      LOG(ERROR) << "[Autofill] Migrate passwords over max counts, stop migrate.";
      std::string err_msg = "Migrate passwords over kMigrationMaxCount, stop migrate.";
      base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, base::ohos::kDefaultUrl,
                                    base::ohos::kPasswordManagerError, err_msg);
      g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
      g_browser_process->local_state()->CommitPendingWrite();
    }
  }
}
#endif // ARKWEB_EXT_PASSWORD

typedef void(*ReadDownloadDataCallback)(const char* guid, const void* buffer, const size_t size);
class NWebReadDownloadDataCallback : public CefReadDownloadDataCallback {
 public:
  NWebReadDownloadDataCallback(ReadDownloadDataCallback callback)
  : read_download_data_callback_(callback) { }

  ~NWebReadDownloadDataCallback() = default;

  NO_SANITIZE("cfi")
  void OnReadDownloadDataDone(const CefString& guid,
                              const CefRefPtr<CefBinaryValue>& buffer) override {
    if (!read_download_data_callback_) {
      return;
    }
    if (!buffer) {
      LOG(INFO) << "OnReadDownloadDataDone: buffer is nullptr.";
      read_download_data_callback_(guid.ToString().c_str(), NULL, 0);
      return;
    }
    size_t len = buffer->GetSize();
    std::vector<uint8_t> dataBuffer(len);
    buffer->GetData(&dataBuffer[0], len, 0);
    LOG(INFO) << "OnReadDownloadDataDone: guid: " << guid.ToString() << ", buffer len: " << len;
    read_download_data_callback_(guid.ToString().c_str(), (void *)(&dataBuffer[0]), len);
  }

 private:
  ReadDownloadDataCallback read_download_data_callback_;

  IMPLEMENT_REFCOUNTING(NWebReadDownloadDataCallback);
};
}  // namespace

namespace OHOS::NWeb {

bool NWebImpl::disableWebActivePolicy_ = false;

void* NWebImpl::logger_report_event_callback_ = nullptr;

WebDestroyMode NWebImpl::webDestroyMode_ = WebDestroyMode::NORMAL_MODE;

ScrollbarMode NWebImpl::scrollbarMode_ = ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR;
bool NWebImpl::scrollbarModeChanged_ = false;

// static
std::shared_ptr<NWeb> NWebImpl::CreateNWeb(
    std::shared_ptr<NWebCreateInfo> create_info) {
  if (!create_info) {
    return nullptr;
  }
  static uint32_t current_nweb_id = 0;
#if BUILDFLAG(ARKWEB_PERFORMANCE_TRACE)
  if (current_nweb_id == 0) {
    StartObserveTraceEnable();
  }
#endif
  uint32_t nweb_id = ++current_nweb_id;
  bool is_enhance_surface =
      GetIsEnhanceSurface(create_info->GetEngineInitArgs());
  WVLOG_I(
      "CreateNWeb NWebId: %{public}u, size %{public}u*%{public}u, enhance "
      "surface %{public}d",
      nweb_id, create_info->GetWidth(), create_info->GetHeight(),
      is_enhance_surface);
  TRACE_EVENT1("NWebImpl", "NWebImpl | CreateNWeb", "nweb_id", nweb_id);
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(nweb_id);
  if (nweb == nullptr) {
    WVLOG_E("CreateNWeb create nweb instance failed, because no memory");
    return nullptr;
  }

  if (!nweb->Init(create_info)) {
    WVLOG_E("CreateNWeb init nweb failed");
    return nullptr;
  }

  nweb->AddNWebToMap(nweb_id, nweb);
  ++g_nweb_count;
  WVLOG_E("CreateNWeb NWebId: %{public}u successfully", nweb_id);
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  // Report nweb instance count
  if (g_nweb_count > g_nweb_max_count) {
    g_nweb_max_count = g_nweb_count;
  }
  ReportMultiInstanceStats(nweb_id, g_nweb_count, g_nweb_max_count);
#endif

  return nweb;
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NWebImpl::UpdateAdblockEasyListRules(long adBlockEasyListVersion) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  bool replace_switch = OHOS::adblock::AdBlockConfig::GetInstance()
                            ->GetUserEasylistReplaceSwitch();
  if (replace_switch) {
    WVLOG_D("[adblock] replace switch is true, not update cloud easylist");
    return;
  }

  return nweb_delegate_->UpdateAdblockEasyListRules(adBlockEasyListVersion);
}
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
// static
bool NWebImpl::InitializeICUStatic(
    std::shared_ptr<NWebEngineInitArgs> init_args) {
  if (NWebApplication::GetDefault()->HasInitializedCef()) {
    return true;
  }
  WVLOG_I("will initialize icu.");
  (void)init_args;
#if !BUILDFLAG(IS_NACL)
  static bool g_init_icu = false;
  if (!g_init_icu) {
    std::list<std::string> web_engine_args;
    InitialWebEngineArgs(web_engine_args, init_args);
    uint32_t argc = web_engine_args.size();
    const char** argv = new const char*[argc];
    uint32_t i = 0;
    for (auto it = web_engine_args.begin(); i < argc; ++i, ++it) {
      argv[i] = it->c_str();
    }
    base::CommandLine::Init(argc, argv);
    content::RegisterPathProvider();
    if (!base::i18n::InitializeICU()) {
      WVLOG_E("initialize icu failed.");
      delete[] argv;
      return false;
    }
    g_init_icu = true;
    delete[] argv;
  }
#endif
  return true;
}
#endif  // BUILDFLAG(ARKWEB_COOKIE)

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
// static
void NWebImpl::InitializeWebEngine(
    std::shared_ptr<NWebEngineInitArgs> init_args) {
  LOG(INFO) << "NWebImpl::InitializeWebEngine, begin";
  std::list<std::string> web_engine_args;
  InitialWebEngineArgs(web_engine_args, init_args);

  uint32_t argc = web_engine_args.size();
  const char** argv = new const char*[argc];
  uint32_t i = 0;
  for (auto it = web_engine_args.begin(); i < argc; ++i, ++it) {
    argv[i] = it->c_str();
  }

  CefMainArgs mainargs(argc, const_cast<char**>(argv));
  CefSettings settings;
  settings.windowless_rendering_enabled = true;
  settings.log_severity = LOGSEVERITY_INFO;
  settings.multi_threaded_message_loop = false;

#if BUILDFLAG(ARKWEB_COOKIE)
  bool excludable_devices =
      base::ohos::IsTabletDevice() || base::ohos::IsPcDevice();
  settings.persist_session_cookies = !excludable_devices;
#endif  // BUILDFLAG(ARKWEB_COOKIE)

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif
  NWebApplication::GetDefault()->InitializeCef(mainargs, settings);
  content::GetNetworkService();

#if BUILDFLAG(ARKWEB_COOKIE)
  should_lazy_init_web_engine_ = false;
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebExPassword)) {
    LOG(INFO) << "[Autofill] Migrate passwords to passwordVault start.";
    MigratePasswordsToPasswordVault();
  }
#endif
}
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
static constexpr int32_t SOC_PERF_LOADURL_CONFIG_ID = 10070;
static constexpr int32_t SOC_PERF_MOUSEWHEEL_CONFIG_ID = 10071;
#endif

// For NWebEx
typedef std::unordered_map<int32_t, std::weak_ptr<NWebImpl>> NWebMap;

base::LazyInstance<NWebMap>::DestructorAtExit g_nweb_map =
    LAZY_INSTANCE_INITIALIZER;
base::Lock OHOS::NWeb::NWebImpl::nweb_map_lock_;

void NWebImpl::AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb) {
  if (nweb) {
    base::AutoLock lock_scope(nweb_map_lock_);
    std::weak_ptr<NWebImpl> nweb_weak(nweb);
    g_nweb_map.Get().emplace(id, nweb_weak);
  }
}

NWebImpl* NWebImpl::FromID(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    auto nweb = it->second.lock();
    if (nweb) {
      return nweb.get();
    }
  }
  return nullptr;
}

std::shared_ptr<NWebImpl> NWebImpl::GetNWebSharedPtr(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    if (auto nweb = it->second.lock()) {
      return nweb;
    }
  }
  return nullptr;
}

NWebImpl::NWebImpl(uint32_t id) : nweb_id_(id) {
  drag_over_timer_ = std::make_unique<base::RetainingOneShotTimer>(
      FROM_HERE, DRAG_OVER_INTERVAL,
      base::BindRepeating(&NWebImpl::SendDragOverEvent,
                          weak_factory_.GetWeakPtr()));
  drag_over_event_.action = DelegateDragAction::DRAG_OVER;
  drag_over_event_.x = 0;
  drag_over_event_.y = 0;
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                        nweb_id_);
}

NWebImpl::~NWebImpl() {
  ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT,
                                        nweb_id_);
  ReportLossFrame::GetInstance()->Reset();
  ReportLossFrame::GetInstance()->SetScrollState(ScrollMode::STOP);
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  if (base::ohos::BlanklessController::CheckGlobalProperty()) {
    base::ohos::BlanklessController::GetInstance().Clear(nweb_id_);
  }
#endif
  base::AutoLock lock_scope(nweb_map_lock_);
  g_nweb_map.Get().erase(nweb_id_);
}

bool NWebImpl::Init(std::shared_ptr<NWebCreateInfo> create_info) {
  output_handler_ = NWebOutputHandler::Create(
      create_info->GetWidth(), create_info->GetHeight(),
      [create_info](const char* buffer, uint32_t width,
                    uint32_t height) -> bool {
        std::shared_ptr<NWebOutputFrameCallback> callback =
            create_info->GetOutputFrameCallback();
        if (!callback) {
          return false;
        }
        return callback->Handle(buffer, width, height);
      });
  if (output_handler_ == nullptr) {
    return false;
  }

  incognito_mode_ = create_info->GetIsIncognitoMode();

  output_handler_->SetNWebId(nweb_id_);

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  if (incognito_mode_) {
    ReportOpenPrivateMode();
  }
#endif

  ProcessInitArgs(create_info->GetEngineInitArgs());
  if (!InitWebEngine(create_info)) {
    WVLOG_E("Init nweb failed, web engine init failed");
    return false;
  }

  input_handler_ = NWebInputHandler::Create(nweb_delegate_);
  if (input_handler_ == nullptr) {
    WVLOG_E("Init nweb failed, input_handler_ is nullptr");
    return false;
  }

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
  g_siteIsolationMode = ShouldEnableSiteIsolation();
  OHOS::NWeb::ResSchedClientAdapter::ReportSiteIsolationMode(
      g_siteIsolationMode);
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportSiteIsolationMode(
    std::to_string(g_siteIsolationMode));
#endif
#endif

  if (!g_logger_callback_initialized) {
    g_logger_callback_initialized = true;
    base::ohos::SetUploadCallback(UploadCallback);
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
      base::ohos::SetReportStatisticTaskRunner();
    }
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    NWebHandlerDelegate::RegisterLoggerCallback(g_logger_callback);
#endif
  }
#if BUILDFLAG(ARKWEB_EXT_UA)
  static std::once_flag flag;
  static const int kDefaultConfigId = -1;
  std::call_once(flag, []() {
    OHOS::NWeb::NWebSafeBrowsingUAPushConfig::GetInstance().EnableUAPushConfig(
        kDefaultConfigId, true);
  });
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  if (base::debug::ArkWebDumpInfo::GetInstance().IsDumpEnabled()) {
    std::string dumpNWebInfo = base::StringPrintf("NWeb Init, nweb_id = %u", nweb_id_);
    base::debug::ArkWebDumpInfo::GetInstance().FormatAndWriteNWebDumpInfo(dumpNWebInfo);
  }
#endif
  return true;
}

// static
void NWebImpl::SetLoggerReportEventCallback(void* callback) {
  logger_report_event_callback_ = callback;
}

// static
NO_SANITIZE("cfi") void NWebImpl::UploadCallback(const std::string& module,
                                                 const std::string& resource,
                                                 const std::string& errorCode,
                                                 const std::string& errorMsg) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebStaticDispatcher::OnLoggerReportEvent(module, resource, errorCode, errorMsg);
#endif

  if (logger_report_event_callback_ == nullptr) {
    return;
  }
  (static_cast<NWebLoggerReportEventCallback *>(logger_report_event_callback_))
      ->OnUploadCallback(module, resource, errorCode, errorMsg);
}

void NWebImpl::OnDestroy() {
  WVLOG_I("NWebImpl::OnDestroy, nweb_id = %{public}u", nweb_id_);

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::KEY_TASK);

  if (destroyCallback_ != nullptr) {
    WVLOG_I("NWebImpl::OnDestroy destroyCallback_ webName_ is %{public}s",
            webName_.c_str());
    (destroyCallback_)(webName_.c_str());
    destroyCallback_ = nullptr;
  } else if (nativeDestroyCallback_) {
    nativeDestroyCallback_();
  }

  if (g_nweb_count == 0) {
    return;
  }
  // TRACE_EVENT1("NWebImpl", "NWebImpl | DestoryNWeb", "nweb_id", nweb_id_);

#if BUILDFLAG(ARKWEB_ITP)
  EnableIntelligentTrackingPrevention(false);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  EnableAdsBlock(false);
#endif

  bool is_close_all = (--g_nweb_count) == 0 ? true : false;
  WVLOG_D("NWebImpl::OnDestroy, nweb_id = %{public}u, number = %{public}u",
          nweb_id_, g_nweb_count);
#ifdef OHOS_WEB_LTPO
  if (is_close_all) {
    if (auto* host = GpuProcessHost::Get()) {
      if (auto* host_impl = host->gpu_host()) {
        host_impl->SetVisible(false);
      }
    }
  }
#endif

  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->OnDestroy(is_close_all);
    nweb_delegate_ = nullptr;
  } else {
    WVLOG_W("NWebImpl::OnDestroy, nweb_delegate_ is nullptr");
  }

  if (input_handler_ != nullptr) {
    input_handler_->OnDestroy();
    input_handler_ = nullptr;
  } else {
    WVLOG_W("NWebImpl::OnDestroy, input_handler_ is nullptr");
  }

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  // Report nweb instance count
  ReportMultiInstanceStats(nweb_id_, g_nweb_count, g_nweb_max_count);
#endif
  NWebConnectNativeManager::GetInstance()->UnRegisterNWebHandler(nweb_id_);

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  if (base::debug::ArkWebDumpInfo::GetInstance().IsDumpEnabled()) {
    std::string dumpNWebInfo = base::StringPrintf("NWeb destroy, nweb_id = %u", nweb_id_);
    base::debug::ArkWebDumpInfo::GetInstance().FormatAndWriteNWebDumpInfo(dumpNWebInfo);
  }
#endif
}

void NWebImpl::ProcessInitArgs(std::shared_ptr<NWebEngineInitArgs> init_args) {
  std::string dump_path = GetDumpPath(init_args);
  if (!dump_path.empty() && output_handler_ != nullptr) {
    output_handler_->SetDumpPath(dump_path);
  }
  bool frame_info_dump = GetIsFrameInfoDump(init_args);
  if (frame_info_dump && output_handler_ != nullptr) {
    output_handler_->SetFrameInfoDump(frame_info_dump);
  }

  InitWebEngineArgs(init_args);
}

bool NWebImpl::SetVirtualDeviceRatio() {
  if (fabs(device_pixel_ratio_) < 1e-15) {
    auto display_manager_adapter =
        OHOS::NWeb::OhosAdapterHelperExt::CreateDisplayMgrAdapter();
    if (display_manager_adapter == nullptr) {
      WVLOG_E("display_manager_adapter is nullptr.");
      return false;
    }
    std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
        display_manager_adapter->GetDefaultDisplay();
    if (display == nullptr) {
      WVLOG_E("display is nullptr.");
      return false;
    }
    if (is_richtext_value_) {
      // Created a richtext component
      device_pixel_ratio_ = richtextDisplayRatio;
    } else {
      device_pixel_ratio_ = display->GetVirtualPixelRatio();
    }
    if (device_pixel_ratio_ <= 0) {
      WVLOG_E("invalid ratio.");
      return false;
    }
    WVLOG_I("GetVirtualPixelRatio ratio: %{public}f", device_pixel_ratio_);
  }
  return true;
}

void NWebImpl::SetNwebDelegateForTest(
    std::shared_ptr<NWebEngineInitArgs> init_args) {
  ProcessInitArgs(init_args);
  uint32_t argc = web_engine_args_.size();
  const char** argv = new const char*[argc];
  uint32_t i = 0;
  for (auto it = web_engine_args_.begin(); i < argc; ++i, ++it) {
    argv[i] = it->c_str();
  }
  nweb_delegate_ = std::make_shared<NWebDelegate>(argc, argv);
  g_nweb_count = 1;
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  if (g_nweb_count > g_nweb_max_count) {
    g_nweb_max_count = g_nweb_count;
  }
#endif
}

bool NWebImpl::InitWebEngine(std::shared_ptr<NWebCreateInfo> create_info) {
  if (output_handler_ == nullptr) {
    WVLOG_E("Init web engine failed, NWeb output handler is not ready");
    return false;
  }

  if (web_engine_args_.empty()) {
    WVLOG_E("Init web engine failed, args is empty");
    return false;
  }

  uint32_t argc = web_engine_args_.size();
  const char** argv = new const char*[argc];
  uint32_t i = 0;
  for (auto it = web_engine_args_.begin(); i < argc; ++i, ++it) {
    argv[i] = it->c_str();
    if (!strncmp(argv[i],
                 "--init-richtext-data=", strlen("--init-richtext-data="))) {
      is_richtext_value_ = true;
    }
  }

  std::shared_ptr<NWebEngineInitArgs> init_args =
      create_info->GetEngineInitArgs();
  is_enhance_surface_ = GetIsEnhanceSurface(init_args);
  if (is_enhance_surface_) {
    window_ = create_info->GetEnhanceSurfaceInfo();
  } else {
    window_ = output_handler_->GetNativeWindowFromSurface(
        create_info->GetProducerSurface());
  }

  if (window_ == nullptr) {
    WVLOG_E("Init web engine failed, get native window from surface failed");
    delete[] argv;
    return false;
  }

  int32_t ret = OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
                    .NativeWindowSetBufferGeometry(window_, create_info->GetWidth(),
                                                   create_info->GetHeight());

  if (ret == OHOS::NWeb::GSErrorCode::GSERROR_OK) {
    WVLOG_I(
        "Init web engine, native window opt for emulator in init, result = "
        "%{public}d",
        ret);
  } else {
    WVLOG_W(
        "Init web engine, native window opt for emulator in init failed, "
        "result = %{public}d",
        ret);
  }

  bool is_popup = GetIsPopup(init_args);
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  std::string shared_render_process_token =
      GetSharedRenderProcessToken(init_args);
#endif
  WVLOG_D("nweb create_info.init_args.is_popup: %{public}d", is_popup);
  nweb_delegate_ = NWebDelegateAdapter::CreateNWebDelegate(
      argc, argv, is_enhance_surface_, window_, is_popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
      ,
      nweb_id_
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
      ,
      create_info->GetIsIncognitoMode()
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
          ,
      shared_render_process_token
#endif
  );
  WVLOG_D("nweb create_info.incognito_mode: %{public}d",
          create_info->GetIsIncognitoMode());
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("Init web engine failed, create nweb delegate failed");
    delete[] argv;
    return false;
  }

#if BUILDFLAG(ARKWEB_COOKIE)
  should_lazy_init_web_engine_ = false;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  nweb_delegate_->SetIsRichText(is_richtext_value_);
#endif

  if (!SetVirtualDeviceRatio()) {
    WVLOG_E("Init web engine failed, set virtual device ratio failed");
    delete[] argv;
    return false;
  }

  uint32_t width, height;
  output_handler_->GetWindowInfo(width, height);
  nweb_delegate_->Resize(width, height);

  inputmethod_handler_ = new NWebInputMethodHandler();
  if (!inputmethod_handler_) {
    delete[] argv;
    WVLOG_E("Init web engine failed, inputmethod_handler_ is nullptr");
    return false;
  }
  nweb_delegate_->SetInputMethodClient(inputmethod_handler_);
  nweb_delegate_->SetNWebDelegateInterface(nweb_delegate_);
  inputmethod_handler_->SetVirtualDeviceRatio(device_pixel_ratio_);

  nweb_delegate_->SetNWebId(nweb_id_);
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  if (set_whole_page_drawing) {
    nweb_delegate_->SetWholePageDrawing();
  }
#endif

#if BUILDFLAG(ARKWEB_I18N)
  UpdateAcceptLanguageInternal();
#endif
  delete[] argv;
  bool isReady = nweb_delegate_->IsReady();
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  if (isReady && base::ohos::BlanklessController::CheckGlobalProperty()) {
    static bool initInstance = false;
    if (!initInstance) {
      base::ThreadPool::PostTask(FROM_HERE,
        {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN, base::TaskPriority::USER_BLOCKING},
        base::BindOnce([]() {
          base::ohos::BlanklessDataController::GetInstance().CreateTaskManager();
          WVLOG_D("BlanklessDataController instance init");
      }));
      initInstance = true;
    }
  }
#endif
  LOG(INFO) << "NWebImpl::InitWebEngine, isReady:" << isReady;
  return isReady;
}

void NWebImpl::InitWebEngineArgs(
    std::shared_ptr<NWebEngineInitArgs> init_args) {
  InitialWebEngineArgs(web_engine_args_, init_args);
}

void NWebImpl::PutDownloadCallback(
    std::shared_ptr<NWebDownloadCallback> downloadListener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set download callback failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterDownLoadListener(downloadListener);
}

void NWebImpl::SetNWebHandler(std::shared_ptr<NWebHandler> client) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set nweb handler failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }

  nweb_handle_ = client;
  nweb_delegate_->RegisterNWebHandler(client);
  NWebConnectNativeManager::GetInstance()->RegisterNWebHandler(nweb_id_, client);
  client->SetNWeb(shared_from_this());
}

#if BUILDFLAG(ARKWEB_AI)
void NWebImpl::SetNWebAgentHandler(
    std::shared_ptr<NWebAgentHandler> agent_handler) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set nweb agent handler failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterNWebAgentHandler(agent_handler);
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
void NWebImpl::DisableBoost(uint32_t nweb_id) {
  std::shared_ptr<NWebImpl> newb_impl = GetNWebSharedPtr(nweb_id);
  if (newb_impl) {
      newb_impl->ResizeTime_--;
    if (newb_impl->ResizeTime_ <= 0) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateSocPerfClientAdapter()
        ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, false);
      newb_impl->ResizeTime_ = 0;
    }
  }
}
#endif

void NWebImpl::Resize(uint32_t width, uint32_t height, bool isKeyboard) {
  if (is_pause_) {
    LOG(INFO) << "web kernel in pause state, don't resize, store pending size, "
                 "width = "
              << width << ", height = " << height
              << ", isKeyboard = " << isKeyboard << ", nweb_id = " << nweb_id_;
    ReSizeType temp_size;
    temp_size.width_ = width;
    temp_size.height_ = height;
    temp_size.is_keyboard_ = isKeyboard;
    pending_size_ = temp_size;
    return;
  }

  if (input_handler_ == nullptr || output_handler_ == nullptr) {
    return;
  }
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      WVLOG_E("size too large in surface mode (%{public}u , %{public}u)", width,
              height);
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resize failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return;
  }
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
    ResizeTime_++;
    content::GetUIThreadTaskRunner({})->PostDelayedTask(
      FROM_HERE, base::BindOnce(&NWebImpl::DisableBoost, nweb_id_),
      base::Milliseconds(WEB_RESIZE_CLOSE_DELAY_TIME));
  }
#endif
  nweb_delegate_->Resize(width, height, isKeyboard);
  output_handler_->Resize(width, height);
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  if (base::debug::ArkWebDumpInfo::GetInstance().IsDumpEnabled()) {
    std::string dumpNWebInfo = base::StringPrintf(
          "NWeb Resize, nweb_id = %u, width = %u, height = %u, isKeyboard = %u",
          nweb_id_, width, height, isKeyboard);
    base::debug::ArkWebDumpInfo::GetInstance().FormatAndWriteNWebDumpInfo(dumpNWebInfo);
  }
#endif
}

void NWebImpl::ResizeVisibleViewport(uint32_t width,
                                     uint32_t height,
                                     bool isKeyboard) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      WVLOG_E("size too large in surface mode (%{public}u , %{public}u)", width,
              height);
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resize failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return;
  }

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  if (nweb_delegate_->GetVisibleViewportAvoidHeight() != 0) {
    WVLOG_I("GetVisibleViewportAvoidHeight is not zero, no resize");
    return;
  }
#endif

  nweb_delegate_->ResizeVisibleViewport(width, height, isKeyboard);
#endif
}

void NWebImpl::SetDrawRect(int x, int y, int width, int height) {
  if (nweb_delegate_) {
    nweb_delegate_->SetDrawRect(x, y, width, height);
  }
}

void NWebImpl::SetDrawMode(int mode) {
  WVLOG_I("NWebImpl::SetDrawMode %{public}d, nweb_id = %{public}u", mode,
          nweb_id_);
  draw_mode_ = mode;
  if (nweb_delegate_) {
    nweb_delegate_->SetDrawMode(mode);
  }
}

bool NWebImpl::GetPendingSizeStatus() {
  if (nweb_delegate_) {
    return nweb_delegate_->GetPendingSizeStatus();
  }
  return false;
}

void NWebImpl::SetFitContentMode(int mode) {
  WVLOG_I("NWebImpl::SetFitContentMode %{public}d, nweb_id = %{public}u", mode,
          nweb_id_);
  if (nweb_delegate_) {
    nweb_delegate_->SetFitContentMode(mode);
  }
}

void NWebImpl::OnTouchPress(int32_t id, double x, double y, bool from_overlay) {
  WVLOG_I(
      "NWebImpl::OnTouchPress id=%{public}d, from_overlay=%{public}d, "
      "nweb_id = %{public}u", id, from_overlay, nweb_id_);
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::CLICK, nweb_id_);
  input_handler_->OnTouchPress(id, x, y, from_overlay);

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_) {
    nweb_delegate_->RefreshAccessibilityManagerClickEvent();
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
}

void NWebImpl::OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) {
  WVLOG_I(
      "NWebImpl::OnTouchRelease id=%{public}d, from_overlay=%{public}d, "
      "nweb_id = %{public}u", id, from_overlay, nweb_id_);
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnTouchRelease(id, x, y, from_overlay);
}

void NWebImpl::OnTouchMove(int32_t id, double x, double y, bool from_overlay) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnTouchMove(id, x, y, from_overlay);
}

void NWebImpl::OnTouchMove(
    const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos,
    bool from_overlay) {
  if (input_handler_ == nullptr) {
    return;
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool nativeEmbedMode = false;
  bool isEnableCustomVideoPlayer = false;
  if (nweb_delegate_) {
    nativeEmbedMode = nweb_delegate_->GetNativeEmbedMode();
    isEnableCustomVideoPlayer = nweb_delegate_->IsEnableCustomVideoPlayer();
  }
  if (nativeEmbedMode || isEnableCustomVideoPlayer) {
    for (const auto& touch : touch_point_infos) {
      std::vector<std::shared_ptr<NWebTouchPointInfo>> single_touch_point_info;
      single_touch_point_info.emplace_back(touch);
      input_handler_->OnTouchMove(single_touch_point_info, from_overlay);
    }
  } else {
    input_handler_->OnTouchMove(touch_point_infos, from_overlay);
  }
#else
  input_handler_->OnTouchMove(touch_point_infos, from_overlay);
#endif
}

void NWebImpl::OnStylusTouchPress(
    std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
    bool from_overlay) {
  if (!stylus_touch_point_info) {
    WVLOG_W(
        "OnStylusTouchPress: invalid touch point info, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  WVLOG_I(
      "NWebImpl::OnStylusTouchPress id=%{public}d, from_overlay=%{public}d, "
      "nweb_id = %{public}u",
      stylus_touch_point_info->GetId(), from_overlay, nweb_id_);

  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::CLICK, nweb_id_);
  input_handler_->OnStylusTouchPress(stylus_touch_point_info, from_overlay);

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_) {
    nweb_delegate_->RefreshAccessibilityManagerClickEvent();
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
}

void NWebImpl::OnStylusTouchRelease(
    std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
    bool from_overlay) {
  if (!stylus_touch_point_info) {
    WVLOG_W(
        "OnStylusTouchRelease: invalid touch point info, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  WVLOG_I(
      "NWebImpl::OnStylusTouchRelease id=%{public}d, from_overlay=%{public}d, "
      "nweb_id = %{public}u",
      stylus_touch_point_info->GetId(), from_overlay, nweb_id_);

  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->OnStylusTouchRelease(stylus_touch_point_info, from_overlay);
}

void NWebImpl::OnStylusTouchMove(
    const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>&
        stylus_touch_point_infos,
    bool from_overlay) {
  if (stylus_touch_point_infos.empty()) {
    WVLOG_W("OnStylusTouchMove: empty touch point list, nweb_id = %{public}u",
            nweb_id_);
    return;
  }

  if (input_handler_ == nullptr) {
    return;
  }

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool nativeEmbedMode = false;
  bool isEnableCustomVideoPlayer = false;
  if (nweb_delegate_) {
    nativeEmbedMode = nweb_delegate_->GetNativeEmbedMode();
    isEnableCustomVideoPlayer = nweb_delegate_->IsEnableCustomVideoPlayer();
  }
  if (nativeEmbedMode || isEnableCustomVideoPlayer) {
    for (const auto& stylus_touch : stylus_touch_point_infos) {
      std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>
          single_stylus_touch;
      single_stylus_touch.emplace_back(stylus_touch);
      input_handler_->OnStylusTouchMove(single_stylus_touch, from_overlay);
    }
  } else {
    input_handler_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay);
  }
#endif
}

void NWebImpl::OnTouchCancel() {
  WVLOG_D("NWebImpl::OnTouchCancel");
  if (input_handler_ == nullptr) {
    return;
  }
  input_handler_->OnTouchCancel();
}

void NWebImpl::OnNavigateBack() {
  if (input_handler_ == nullptr) {
    return;
  }
  input_handler_->OnNavigateBack();
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
}

bool NWebImpl::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  if (input_handler_ == nullptr) {
    return false;
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  return input_handler_->SendKeyEvent(keyCode, keyAction);
}

void NWebImpl::SendTouchpadFlingEvent(double x,
                                      double y,
                                      double vx,
                                      double vy) {
  if (input_handler_ == nullptr) {
    return;
  }

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
}

void NWebImpl::SendMouseWheelEvent(double x,
                                   double y,
                                   double deltaX,
                                   double deltaY) {
  if (input_handler_ == nullptr) {
    return;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::SLIDE);

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

void NWebImpl::SendMouseEvent(int x, int y, int button, int action, int count) {
  if (input_handler_ == nullptr) {
    return;
  }

  if (action == MouseAction::PRESS) {
    ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                       ResSchedSceneAdapter::CLICK, nweb_id_);
  }
  input_handler_->SendMouseEvent(x, y, button, action, count);
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
}

int NWebImpl::Load(const std::string& url) {
  if (nweb_delegate_ == nullptr || output_handler_ == nullptr) {
    return NWEB_ERR;
  }

  if (!output_handler_->IsSizeValid()) {
    WVLOG_E("nweb size is invalid, stop Load");
    return NWEB_ERR;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::LOAD_URL, nweb_id_);

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_LOADURL_CONFIG_ID);
#endif

  int result = nweb_delegate_->Load(url);
  output_handler_->StartRenderOutput();
  return result;
}

bool NWebImpl::IsNavigatebackwardAllowed() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigatebackwardAllowed();
}

bool NWebImpl::IsNavigateForwardAllowed() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsNavigateForwardAllowed();
}

bool NWebImpl::CanNavigateBackOrForward(int numSteps) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->CanNavigateBackOrForward(numSteps);
}

void NWebImpl::NavigateBack() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateBack();
}

void NWebImpl::NavigateForward() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateForward();
}

void NWebImpl::NavigateBackOrForward(int step) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->NavigateBackOrForward(step);
}

void NWebImpl::DeleteNavigateHistory() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->DeleteNavigateHistory();
}

void NWebImpl::ClearSslCache() {
  WVLOG_I("NWebImpl::ClearSslCache");
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearSslCache();
}

void NWebImpl::ClearClientAuthenticationCache() {
  WVLOG_I("NWebImpl::ClearClientAuthenticationCache");
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearClientAuthenticationCache();
}

void NWebImpl::Reload() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Reload();
}

int NWebImpl::Zoom(float zoomFactor) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Zoom(zoomFactor);
}

int NWebImpl::ZoomIn() {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomIn();
}

int NWebImpl::ZoomOut() {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->ZoomOut();
}

void NWebImpl::Stop() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->Stop();
}

#if BUILDFLAG(ARKWEB_PDF)
void NWebImpl::ExecuteCreatePDFExt(
    std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
    std::shared_ptr<NWebArrayBufferValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteCreatePDFExt(pdfConfig, callback);
}
#endif  // BUILDFLAG(ARKWEB_PDF)

void NWebImpl::ExecuteJavaScript(const std::string& code) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code);
}

void NWebImpl::PutBackgroundColor(int color) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PutBackgroundColor(color);
}

void NWebImpl::InitialScale(float scale) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->InitialScale(scale);
}

void NWebImpl::PutOptimizeParserBudgetEnabled(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PutOptimizeParserBudgetEnabled(enable);
}

void NWebImpl::NotifyPopupWindowDisposition(
      CefLifeSpanHandler::WindowOpenDisposition disposition) {
  if (disposition == CEF_WOD_NEW_BACKGROUND_TAB) {
    is_pause_ = true;
  }
}

void NWebImpl::OnPause() {
  if (!GetWebOptimizationValue()) {
    LOG(DEBUG) << "WebOptimization disabled.";
    return;
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  is_pause_ = true;
  nweb_delegate_->OnPause();

#if BUILDFLAG(ARKWEB_SOFTKEYBOARD_AVOID)
  if (keyboardBehaviorMode_ == WebSoftKeyboardBehaviorMode::DISABLE_AUTO_KEYBOARD_ON_ACTIVE) {
    return;
  }
#endif
  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnPause";
    nweb_delegate_->GetCustomKeyboardHandler()->CloseFromWebStateChange(
        WebCustomKeyboardState::FROM_ONPAUSE);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  inputmethod_handler_->HideTextInput(
      nweb_id_, NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE);
}

void NWebImpl::OnContinue() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnContinue();
  if (is_pause_) {
    is_pause_ = false;
    if (pending_size_) {
      LOG(INFO) << "web kernel in continue state, continue resize, need resize"
                << ", nweb_id = " << nweb_id_;
      const auto& size = pending_size_.value();
      Resize(size.width_, size.height_, size.is_keyboard_);
      pending_size_.reset();
    }
  }
#if BUILDFLAG(ARKWEB_SOFTKEYBOARD_AVOID)
  if (keyboardBehaviorMode_ == WebSoftKeyboardBehaviorMode::DISABLE_AUTO_KEYBOARD_ON_ACTIVE) {
    if (nweb_delegate_->IsCustomKeyboard()) {
      auto handler = nweb_delegate_->GetCustomKeyboardHandler();
      if (handler && handler->IsAttached()) {
        nweb_delegate_->OnFocus();
      }
    }
    if (inputmethod_handler_) {
      if (inputmethod_handler_->IsAttached() && !inputmethod_handler_->IsFocusSwitch()) {
        nweb_delegate_->OnFocus();
      }
    }
    return;
  }
#endif
  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnContinue and focus";
    auto handler = nweb_delegate_->GetCustomKeyboardHandler();
    if (handler && handler->AttachFromWebStateChange(
                       WebCustomKeyboardState::FROME_ONCONTINUE)) {
      nweb_delegate_->OnFocus();
    }
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (inputmethod_handler_->Reattach(
          nweb_id_, NWebInputMethodHandler::ReattachType::FROM_CONTINUE)) {
    nweb_delegate_->OnFocus();
  }
}

void NWebImpl::OnDragAttach() {
  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  inputmethod_handler_->Reattach(
      nweb_id_, NWebInputMethodHandler::ReattachType::FROM_ONDRAG);
}

void NWebImpl::WebComponentsBlur() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }

  LOG(INFO) << "NWebImpl::WebComponentsBlur, Gesture back blur on.";
  nweb_delegate_->WebComponentsBlur();
}

void NWebImpl::OnOccluded() {
  if (!GetWebOptimizationValue()) {
    LOG(DEBUG) << "WebOptimization disabled.";
    return;
  }
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnOccluded();
}

void NWebImpl::OnUnoccluded() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnUnoccluded();
}

void NWebImpl::SetEnableLowerFrameRate(bool enabled) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->SetEnableLowerFrameRate(enabled);
}

void NWebImpl::SetEnableHalfFrameRate(bool enabled) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->SetEnableHalfFrameRate(enabled);
}

#if BUILDFLAG(ARKWEB_WEBRTC)
void NWebImpl::StopCameraSession() const {
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetForegroundFlag(
      false);
}

void NWebImpl::RestartCameraSession() const {
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetForegroundFlag(
      true);
  if (!OhosAdapterHelper::GetInstance()
           .GetCameraManagerAdapter()
           .IsExistCaptureTask()) {
    return;
  }
  WVLOG_I("RestartSession");
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().RestartSession();
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

std::shared_ptr<NWebPreference> NWebImpl::GetPreference() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->GetPreference();
}

#if BUILDFLAG(ARKWEB_AI)
std::shared_ptr<NWebAgentManager> NWebImpl::GetAgentManager() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->GetAgentManager();
}
#endif  // BUILDFLAG(ARKWEB_AI)

std::string NWebImpl::Title() {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->Title();
}

void NWebImpl::ExecuteJavaScriptExt(
    const int fd,
    const size_t scriptLength,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
}

#if BUILDFLAG(ARKWEB_MSGPORT)
void NWebImpl::ExecuteJavaScript(
    const std::string& code,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ExecuteJavaScript(code, callback, extention);
}

std::vector<std::string> NWebImpl::CreateWebMessagePorts() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    std::vector<std::string> empty;
    return empty;
  }
  return nweb_delegate_->CreateWebMessagePorts();
}

void NWebImpl::PostWebMessage(const std::string& message,
                              const std::vector<std::string>& ports,
                              const std::string& targetUri) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostWebMessage(message, ports, targetUri);
}

void NWebImpl::ClosePort(const std::string& portHandle) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->ClosePort(portHandle);
}

void NWebImpl::PostPortMessage(const std::string& portHandle,
                               std::shared_ptr<NWebMessage> data) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostPortMessage(portHandle, data);
}

void NWebImpl::PostPortMessageV2(const std::string& portHandle,
                                 std::shared_ptr<NWebRomValue> data) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PostPortMessageV2(portHandle, data);
}

void NWebImpl::SetPortMessageCallback(
    const std::string& portHandle,
    std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->SetPortMessageCallback(portHandle, callback);
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

void NWebImpl::SetAutofillCallback(
    std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->SetAutofillCallback(callback);
}

void NWebImpl::FillAutofillData(std::shared_ptr<NWebMessage> data) {
  LOG(INFO) << "NWebImpl::FillAutofillData";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->FillAutofillData(data);
}

void NWebImpl::FillAutofillDataV2(std::shared_ptr<NWebRomValue> data) {
  LOG(INFO) << "NWebImpl::FillAutofillDataV2";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->FillAutofillDataV2(data);
}

void NWebImpl::FillAutofillDataFromTriggerType(
    std::shared_ptr<NWebRomValue> data, const NWebAutoFillTriggerType& type) {
  LOG(INFO) << "NWebImpl::FillAutofillDataFromTriggerType";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->FillAutofillDataFromTriggerType(data, static_cast<int32_t>(type));
}

void NWebImpl::PutVaultPlainTextCallback(
    std::shared_ptr<NWebVaultPlainTextCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->PutVaultPlainTextCallback(callback);
}

void NWebImpl::OnAutofillCancel(const std::string& fillContent) {
  LOG(INFO) << "NWebImpl::OnAutofillCancel";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI nweb_delegate_ its null");
    return;
  }
  nweb_delegate_->ProcessAutofillCancel(fillContent);
}

uint32_t NWebImpl::GetWebId() {
  return nweb_id_;
}

std::shared_ptr<HitTestResult> NWebImpl::GetHitTestResult() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "get hit test result failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return std::make_shared<HitTestResultImpl>();
  }

  return nweb_delegate_->GetHitTestResult();
}

std::shared_ptr<HitTestResult> NWebImpl::GetLastHitTestResult() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("get last hit test result failed, nweb delegate is nullptr, nweb_id = %{public}u", nweb_id_);
    return std::make_shared<HitTestResultImpl>();
  }

  return nweb_delegate_->GetLastHitTestResult();
}

int NWebImpl::PageLoadProgress() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->PageLoadProgress();
}

int NWebImpl::ContentHeight() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->ContentHeight();
}

float NWebImpl::Scale() {
  if (nweb_delegate_ == nullptr) {
    return 0;
  }
  return nweb_delegate_->Scale();
}

int NWebImpl::Load(
    const std::string& url,
    const std::map<std::string, std::string>& additional_http_headers) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->Load(url, additional_http_headers);
}

int NWebImpl::PostUrl(const std::string& url,
                      const std::vector<char>& postData) {
#if BUILDFLAG(ARKWEB_POST_URL)
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->PostUrl(url, postData);
#endif  // BUILDFLAG(ARKWEB_POST_URL)
}

int NWebImpl::LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                     const std::string& data,
                                     const std::string& mimeType,
                                     const std::string& encoding,
                                     const std::string& historyUrl) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->LoadWithDataAndBaseUrl(baseUrl, data, mimeType,
                                                encoding, historyUrl);
}

int NWebImpl::LoadWithData(const std::string& data,
                           const std::string& mimeType,
                           const std::string& encoding) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->LoadWithData(data, mimeType, encoding);
}

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
int NWebImpl::LoadUrlWithParams(const std::string& url,
                                const LoadUrlType load_type,
                                const std::string& refer,
                                const std::string& headers,
                                const std::string& post_data,
                                const bool allow_https_upgrade,
                                int32_t transition_type) {
  if (nweb_delegate_ == nullptr) {
    return NWEB_ERR;
  }
  return nweb_delegate_->LoadUrlWithParams(url, load_type, refer, headers,
                                           post_data, allow_https_upgrade,
                                           transition_type);
}
#endif

void NWebImpl::RegisterNativeArkJSFunction(
    const char* objName,
    const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeArkJSFunction(objName, callbacks);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeArkJSFunction(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFunc>&& callback,
    bool isAsync,
    const std::string& permission) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeJSProxy(
        objName, methodName, std::move(callback), isAsync, permission);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeArkJSFunctionWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
    bool isAsync,
    const std::string& permission) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeJSProxyWithResult(
        objName, methodName, std::move(callback), isAsync, permission);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeAsyncThreadArkJSFunctionWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
    const std::string& permission) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeAsyncThreadJSProxyWithResult(objName, methodName,
        std::move(callback), permission);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::UnRegisterNativeArkJSFunction(const char* objName) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->UnRegisterNativeArkJSFunction(objName);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeValideCallback(
    const char* webName,
    const NativeArkWebOnValidCallback callback) {
  base::AutoLock lock_scope(state_lock_);
  webName_ = webName;
  validCallback_ = callback;
}
void NWebImpl::RegisterNativeDestroyCallback(
    const char* webName,
    const NativeArkWebOnDestroyCallback callback) {
  base::AutoLock lock_scope(state_lock_);
  webName_ = webName;
  destroyCallback_ = callback;
}

void NWebImpl::RegisterNativeDestroyCallback(
    std::function<void(void)>&& callback) {
  base::AutoLock lock_scope(state_lock_);
  nativeDestroyCallback_ = std::move(callback);
}

void NWebImpl::RegisterNativeLoadStartCallback(
    std::function<void(void)>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeLoadStartCallback(std::move(callback));
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeLoadEndCallback(
    std::function<void(void)>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeLoadEndCallback(std::move(callback));
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterNativeScrollCallback(
    std::function<void(double, double)>&& callback) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->RegisterNativeScrollCallback(std::move(callback));
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
}

void NWebImpl::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const int32_t object_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(
      object_name, method_list, std::vector<std::string>(), object_id, "");
}

void NWebImpl::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, "");
}

void NWebImpl::RegisterArkJSfunctionV2(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id,
    const std::string& permission) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to register ark js function");
    return;
  }
  return nweb_delegate_->RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, permission);
}

void NWebImpl::UnregisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->UnregisterArkJSfunction(object_name, method_list);
}

#if BUILDFLAG(ARKWEB_JSPROXY)
void NWebImpl::JavaScriptOnDocumentStart(const ScriptItems& scriptItems) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->JavaScriptOnDocumentStart(scriptItems);

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    nweb_delegate_->CancelAllPrerendering();
  }
#endif
}

void NWebImpl::JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->JavaScriptOnDocumentEnd(scriptItems);

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    nweb_delegate_->CancelAllPrerendering();
  }
#endif
}

void NWebImpl::JavaScriptOnDocumentStartByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentStartByOrder(scriptItems,
                                                          ScriptRegexItems(),
                                                          scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnDocumentEndByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentEndByOrder(scriptItems,
                                                        ScriptRegexItems(),
                                                        scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnHeadReadyByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnHeadReadyByOrder(scriptItems,
                                                      ScriptRegexItems(),
                                                      scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnDocumentStartByOrderV2(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentStartByOrder(scriptItems,
                                                          scriptRegexItems,
                                                          scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnDocumentEndByOrderV2(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnDocumentEndByOrder(scriptItems,
                                                        scriptRegexItems,
                                                        scriptItemsByOrder);
}

void NWebImpl::JavaScriptOnHeadReadyByOrderV2(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->JavaScriptOnHeadReadyByOrder(scriptItems,
                                                      scriptRegexItems,
                                                      scriptItemsByOrder);
}
#endif

void NWebImpl::CallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string& h5_method_name,
    const std::vector<std::shared_ptr<NWebValue>>& args) {
  if (nweb_delegate_ == nullptr || h5_object_id < 0) {
    WVLOG_E("fail to call h5 function");
    return;
  }
  nweb_delegate_->CallH5Function(routing_id, h5_object_id, h5_method_name,
                                 args);
}

void NWebImpl::CallH5FunctionV2(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string& h5_method_name,
    const std::vector<std::shared_ptr<NWebRomValue>>& args) {
  if (nweb_delegate_ == nullptr || h5_object_id < 0) {
    WVLOG_E("fail to call h5 function");
    return;
  }
  nweb_delegate_->CallH5FunctionV2(routing_id, h5_object_id, h5_method_name,
                                   args);
}

void NWebImpl::SetNWebJavaScriptResultCallBack(
    std::shared_ptr<NWebJavaScriptResultCallBack> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RegisterNWebJavaScriptCallBack(callback);
}

void NWebImpl::OnFocus(const FocusReason& focusReason) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  nweb_delegate_->SetFocusWebId(nweb_id_);
#endif
  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnFocus";
    nweb_delegate_->GetCustomKeyboardHandler()->AttachFromWebStateChange(
        WebCustomKeyboardState::FROME_ONFOCUS);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
  if (nweb_delegate_->OnFocus(focusReason)) {
    inputmethod_handler_->Reattach(
        nweb_id_, NWebInputMethodHandler::ReattachType::FROM_ONFOCUS);
  }
}

void NWebImpl::OnBlur(const BlurReason& blurReason) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->OnBlur();

  if (nweb_delegate_->IsCustomKeyboard()) {
    LOG(INFO) << "WebCustomKeyboard NWebImpl::OnBlur";
    nweb_delegate_->GetCustomKeyboardHandler()->CloseFromWebStateChange(
        WebCustomKeyboardState::FROM_ONBLUR);
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return;
  }
#if BUILDFLAG(ARKWEB_FOCUS)
  if (is_enhance_surface_ &&
      blurReason == OHOS::NWeb::BlurReason::WINDOW_BLUR) {
    return;
  }
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)
  if (blurReason == OHOS::NWeb::BlurReason::CLEAR_FOCUS) {
    inputmethod_handler_->HideTextInputForce();
  } else if (blurReason == OHOS::NWeb::BlurReason::FOCUS_SWITCH) {
    inputmethod_handler_->SetNeedReattachOnfocus();
  } else {
    inputmethod_handler_->HideTextInput(
        nweb_id_, NWebInputMethodClient::HideTextinputType::FROM_ONBLUR);
  }
  inputmethod_handler_->SetFocusStatus(false);
}

void NWebImpl::PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RegisterFindListener(findListener);
}

void NWebImpl::FindAllAsync(const std::string& search_string) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindAllAsync(search_string);
}

void NWebImpl::ClearMatches() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearMatches();
}

void NWebImpl::FindNext(const bool forward) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->FindNext(forward);
}

void NWebImpl::StoreWebArchive(
    const std::string& base_name,
    bool auto_name,
    std::shared_ptr<NWebStringValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->StoreWebArchive(base_name, auto_name, callback);
}

void NWebImpl::SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return;
  }
  DelegateDragEvent event;
  if (dragEvent) {
    event.action = static_cast<DelegateDragAction>(dragEvent->GetAction());
    event.x = dragEvent->GetX();
    event.y = dragEvent->GetY();
    auto drag_op = OHOS::NWeb::NWebDragData::DragOperation::DRAG_OPERATION_COPY;
    auto allowed_drag_op =
        OHOS::NWeb::NWebDragData::DragOperationsMask::DRAG_ALLOW_EVERY;
    if (dragEvent->IsDragOpValid()) {
      drag_op = dragEvent->GetDragOperation();
      allowed_drag_op = dragEvent->GetAllowedDragOperation();
    }
    if (event.action == DelegateDragAction::DRAG_OVER ||
        event.action == DelegateDragAction::DRAG_START ||
        event.action == DelegateDragAction::DRAG_ENTER) {
      drag_over_event_.x = dragEvent->GetX();
      drag_over_event_.y = dragEvent->GetY();
      drag_over_event_.allowed_op = static_cast<CefBrowserHost::DragOperationsMask>(allowed_drag_op);
      drag_over_timer_->Reset();
    } else {
      drag_over_timer_->Stop();
    }

    LOG(DEBUG) << "DragDrop, get event from arkweb, op = "
              << (int)drag_op
              << " , allow_op = " << (int)allowed_drag_op;
    event.op = static_cast<CefBrowserHost::DragOperationsMask>(drag_op);
    event.allowed_op = static_cast<CefBrowserHost::DragOperationsMask>(allowed_drag_op);
  }
  nweb_delegate_->SendDragEvent(event);
}

void NWebImpl::SendDragOverEvent() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return;
  }
  drag_over_timer_->Reset();
  nweb_delegate_->SendDragEvent(drag_over_event_);
}

std::string NWebImpl::GetUrl() {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->GetUrl();
}

void NWebImpl::UpdateLocale(const std::string& language,
                            const std::string& region) {
#if BUILDFLAG(ARKWEB_I18N)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateLocale(language, region);
#endif  // if BUILDFLAG(ARKWEB_I18N)
}

const std::string NWebImpl::GetOriginalUrl() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return std::string();
  }
  return nweb_delegate_->GetOriginalUrl();
}

#if BUILDFLAG(IS_OHOS)
bool NWebImpl::TerminateRenderProcess() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("TerminateRenderProcess failed, nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->TerminateRenderProcess();
}
#endif

#if BUILDFLAG(IS_ARKWEB)
NWebPrintDocumentAdapterAdapterImpl::~NWebPrintDocumentAdapterAdapterImpl() {
  if (ref_) {
    delete ref_;
  }
}

void NWebPrintDocumentAdapterAdapterImpl::OnStartLayoutWrite(
    const std::string& jobId,
    std::shared_ptr<NWebPrintAttributesAdapter> oldAttrs,
    std::shared_ptr<NWebPrintAttributesAdapter> newAttrs, uint32_t fd,
    std::shared_ptr<NWebPrintWriteResultCallbackAdapter> callback) {
  if (ref_) {
    ref_->OnStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, callback);
  }
}

void NWebPrintDocumentAdapterAdapterImpl::OnJobStateChanged(
    const std::string& jobId, uint32_t state) {
  if (ref_) {
    ref_->OnJobStateChanged(jobId, state);
  }
}
#endif

bool NWebImpl::GetFavicon(const void** data,
                          size_t& width,
                          size_t& height,
                          ImageColorType& colorType,
                          ImageAlphaType& alphaType) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->GetFavicon(data, width, height, colorType, alphaType);
}

void NWebImpl::PutNetworkAvailable(bool available) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->PutNetworkAvailable(available);
}

void NWebImpl::HasImages(std::shared_ptr<NWebBoolValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI HasImages nweb_delegate_ is null");
    return;
  }

  nweb_delegate_->GetImages(callback);
}

void NWebImpl::RemoveCache(bool include_disk_files) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("JSAPI RemoveCache nweb_delegate_ is null");
    return;
  }

  nweb_delegate_->RemoveCache(include_disk_files);

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    nweb_delegate_->CancelAllPrerendering();
  }
#endif
}

std::shared_ptr<NWebHistoryList> NWebImpl::GetHistoryList() {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }

  return nweb_delegate_->GetHistoryList();
}

std::vector<uint8_t> NWebImpl::SerializeWebState() {
  if (nweb_delegate_ == nullptr) {
    std::vector<uint8_t> empty;
    return empty;
  }
  return nweb_delegate_->SerializeWebState();
}

bool NWebImpl::RestoreWebState(const std::vector<uint8_t>& state) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->RestoreWebState(state);
}

void NWebImpl::PutReleaseSurfaceCallback(
    std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set release surface callback failed, nweb delegate is nullptr, "
        "nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterReleaseSurfaceListener(releaseSurfaceListener);
}

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
void NWebImpl::PageUp(bool top) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->PageUp(top);
}

void NWebImpl::PageDown(bool bottom) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->PageDown(bottom);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void NWebImpl::GetScrollOffset(float* offset_x, float* offset_y) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  float scroll_offset_x = 0;
  float scroll_offset_y = 0;
  float overscroll_offset_x = 0;
  float overscroll_offset_y = 0;
  float finalOffset_x = 0;
  float finalOffset_y = 0;
  nweb_delegate_->GetScrollOffset(&scroll_offset_x, &scroll_offset_y);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  nweb_delegate_->GetOverScrollOffset(&overscroll_offset_x,
                                      &overscroll_offset_y);
#endif
  finalOffset_x = scroll_offset_x + std::round(overscroll_offset_x);
  finalOffset_y = scroll_offset_y + std::round(overscroll_offset_y);

  if ((nullptr == offset_x) || (nullptr == offset_y)) {
    LOG(ERROR) << "offset_x or offset_y is nullptr";
    return;
  }
  *offset_x = finalOffset_x;
  *offset_y = finalOffset_y;
}

void NWebImpl::GetPageOffset(float* offset_x, float* offset_y) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  float scroll_offset_x = 0;
  float scroll_offset_y = 0;
  nweb_delegate_->GetScrollOffset(&scroll_offset_x, &scroll_offset_y);
  *offset_x = scroll_offset_x;
  *offset_y = scroll_offset_y;
}
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebImpl::ScrollTo(float x, float y) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "ScrollTo nweb_delegate_ is nullptr";
    return;
  }
  return nweb_delegate_->ScrollTo(x, y);
}

void NWebImpl::ScrollBy(float delta_x, float delta_y) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "ScrollBy nweb_delegate_ is nullptr";
    return;
  }
  return nweb_delegate_->ScrollBy(delta_x, delta_y);
}

void NWebImpl::ScrollByRefScreen(float delta_x,
                                 float delta_y,
                                 float vx,
                                 float vy) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "ScrollByRefScreen nweb_delegate_ is nullptr";
    return;
  }
  return nweb_delegate_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

void NWebImpl::SlideScroll(float vx, float vy) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "SlideScroll nweb_delegate_ is nullptr";
    return;
  }
  return nweb_delegate_->SlideScroll(vx, vy);
}

bool NWebImpl::WebSendKeyEvent(int32_t keyCode,
                               int32_t keyAction,
                               const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendKeyEvent input_handler_ is nullptr";
    return false;
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  return input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
}

void NWebImpl::WebSendMouseWheelEvent(
    double x,
    double y,
    double deltaX,
    double deltaY,
    const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendMouseWheelEvent input_handler_ is nullptr";
    return;
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::SLIDE);

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
}

bool NWebImpl::WebSendMouseWheelEventV2(
    double x,
    double y,
    double deltaX,
    double deltaY,
    const std::vector<int32_t>& pressedCodes,
    int32_t source) {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendMouseWheelEventV2 input_handler_ is nullptr";
    return true;
  }

  ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter::WEB_SCENE_ENTER, ResSchedSceneAdapter::SLIDE);

#if defined(OHOS_PERFORMANCE_INC_FREQ)
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigById(SOC_PERF_MOUSEWHEEL_CONFIG_ID);
#endif
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  input_handler_->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);
  return true;
}

void NWebImpl::WebSendTouchpadFlingEvent(
    double x,
    double y,
    double vx,
    double vy,
    const std::vector<int32_t>& pressedCodes) {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendTouchpadFlingEvent input_handler_ is nullptr";
    return;
  }

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
}

void NWebImpl::WebSendCancelFlingEvent() {
  if (input_handler_ == nullptr) {
    LOG(ERROR) << "WebSendCancelFlingEvent input_handler_ is nullptr";
    return;
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  input_handler_->WebSendCancelFlingEvent();
}

bool NWebImpl::SendKeyboardEvent(
    const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
  if (input_handler_ == nullptr) {
    return false;
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  return input_handler_->SendKeyboardEvent(keyboardEvent);
}

bool NWebImpl::ScrollByWithResult(float delta_x, float delta_y) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "ScrollByWithResult nweb_delegate_ is nullptr";
    return false;
  }
  return nweb_delegate_->ScrollByWithResult(delta_x, delta_y);
}

void NWebImpl::ScrollToWithAnime(float x, float y, int32_t duration) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollToWithAnime(x, y, duration);
}

void NWebImpl::ScrollByWithAnime(float delta_x,
                                 float delta_y,
                                 int32_t duration) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->ScrollByWithAnime(delta_x, delta_y, duration);
}

void NWebImpl::WebSendMouseEvent(
    const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) {
  if (!input_handler_ || !mouseEvent) {
    return;
  }

  if (mouseEvent->GetAction() == MouseAction::PRESS) {
    ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                       ResSchedSceneAdapter::CLICK, nweb_id_);
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  ClearBlanklessKey();
#endif
  input_handler_->WebSendMouseEvent(mouseEvent);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

bool NWebImpl::GetCertChainDerData(std::vector<std::string>& certChainData,
                                   bool isSingleCert) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("get cert chain data failed, nweb_delegate_ is null");
    return false;
  }
  return nweb_delegate_->GetCertChainDerData(certChainData, isSingleCert);
}

void NWebImpl::SetScreenOffSet(double x, double y) {
  if (inputmethod_handler_) {
    inputmethod_handler_->SetScreenOffSet(x, y);
  }
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  if (!is_pause_ && nweb_delegate_) {
    nweb_delegate_->SetScreenOffset(x, y);
  }
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)
}

void NWebImpl::SetAudioMuted(bool muted) {
#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  WVLOG_D("SetAudioMuted invoked: %{public}s", (muted ? "true" : "false"));
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioMuted(muted);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void NWebImpl::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (nweb_delegate_) {
    nweb_delegate_->SetShouldFrameSubmissionBeforeDraw(should);
  }
}

std::string NWebImpl::GetCurrentLanguage() {
  if (nweb_delegate_ == nullptr) {
    return "";
  }
  return nweb_delegate_->GetCurrentLanguage();
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

void NWebImpl::SetAudioResumeInterval(int32_t resumeInterval) {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioResumeInterval(resumeInterval);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void NWebImpl::SetNativeInnerWeb(bool isInnerWeb) {
  if (nweb_delegate_) {
    nweb_delegate_->SetNativeInnerWeb(isInnerWeb);
  }
}
#endif

void NWebImpl::SetAudioExclusive(bool audioExclusive) {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioExclusive(audioExclusive);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

void NWebImpl::SetAudioSessionType(int32_t audioSessionType) {
  if (nweb_delegate_) {
    nweb_delegate_->SetAudioSessionType(audioSessionType);
  }
}

void NWebImpl::CloseAllMediaPresentations() {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->CloseAllMediaPresentations();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

void NWebImpl::StopAllMedia() {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->StopAllMedia();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

void NWebImpl::ResumeAllMedia() {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ResumeAllMedia();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

void NWebImpl::PauseAllMedia() {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->PauseAllMedia();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

int NWebImpl::GetMediaPlaybackState() {
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  if (nweb_delegate_ == nullptr) {
    return NWebPlaybackState_NONE;
  }
  return nweb_delegate_->GetMediaPlaybackState();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
}

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
void NWebImpl::RegisterScreenLockFunction(
    int32_t windowId,
    std::shared_ptr<NWebScreenLockCallback> callback) {
  if (nweb_delegate_) {
    nweb_delegate_->SetWakeLockCallback(windowId, callback);
  }
}

void NWebImpl::UnRegisterScreenLockFunction(int32_t windowId) {
  if (nweb_delegate_) {
    nweb_delegate_->SetWakeLockCallback(windowId, nullptr);
  }
}
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_LOCK)

void NWebImpl::NotifyMemoryLevel(int32_t level) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;
  static constexpr int32_t kMemoryLevelModerate = 0;
  static constexpr int32_t kMemoryLevelMax = 3;
  static constexpr base::TimeDelta kNotifyGapTime = base::Seconds(3);
  static MemoryPressureLevel last_memory_level =
      MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE;
  static base::Time last_notify_time;

  if (level >= kMemoryLevelMax) {
    LOG(WARNING) << "The memory level >= kMemoryLevelMax(3), not supported";
    return;
  }

  base::Time now = base::Time::Now();
  MemoryPressureLevel memory_pressure_level;
  if (level == kMemoryLevelModerate) {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE;
  } else {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL;
  }

  if (memory_pressure_level == last_memory_level &&
      (now - last_notify_time < kNotifyGapTime)) {
    LOG(INFO) << "The same memory level has been notified within three seconds";
    return;
  }
  last_memory_level = memory_pressure_level;
  last_notify_time = std::move(now);

  LOG(INFO) << "NWebImpl::NotifyMemoryLevel "
            << (level == kMemoryLevelModerate
                    ? "MEMORY_PRESSURE_LEVEL_MODERATE"
                    : "MEMORY_PRESSURE_LEVEL_CRITICAL");
  base::MemoryPressureListener::NotifyMemoryPressure(memory_pressure_level);
#endif  // ARKWEB_PERFORMANCE_MEMORY_THRESHOLD
}

void NWebImpl::SetIsOfflineWebComponent() {
  WVLOG_D("NWebImpl::SetIsOfflineWebComponent");
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetIsOfflineWebComponent nweb delegate is null");
    return;
  }
  nweb_delegate_->SetIsOfflineWebComponent();
}

void NWebImpl::OnWebviewHide() {
  WVLOG_D("NWebImpl::OnWebviewHide");
}

void NWebImpl::OnWebviewShow() {
  WVLOG_D("NWebImpl::OnWebviewShow");
#if BUILDFLAG(ARKWEB_CLOUD_CONTROL)
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    content::BrowserContext* browser_context =
        cef_browser_context->AsBrowserContext();

    if (browser_context && !browser_context->IsOffTheRecord()) {
      auto context = static_cast<ChromeBrowserContext*>(cef_browser_context);
      if (context) {
        context->OnWebViewShow();
      }
    }
  }
#endif
}

void NWebImpl::OnRenderToBackground() {
  TRACE_EVENT0("base", "OnRenderToBackground");
  WVLOG_D("NWebImpl::OnRenderToBackground");
#if BUILDFLAG(ARKWEB_WEBRTC)
  StopCameraSession();
#endif
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnRenderToBackground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnWindowHide();
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  if (base::debug::ArkWebDumpInfo::GetInstance().IsDumpEnabled()) {
    std::string dumpNWebInfo = base::StringPrintf("OnWindowHide, nweb_id = %u", nweb_id_);
    base::debug::ArkWebDumpInfo::GetInstance().FormatAndWriteNWebDumpInfo(dumpNWebInfo);
  }
#endif
}

void NWebImpl::OnRenderToForeground() {
  TRACE_EVENT0("base", "OnRenderToForeground");
  WVLOG_D("NWebImpl::OnRenderToForeground");
#if BUILDFLAG(ARKWEB_WEBRTC)
  RestartCameraSession();
#endif
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnRenderToForeground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnWindowShow();
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  if (base::debug::ArkWebDumpInfo::GetInstance().IsDumpEnabled()) {
    std::string dumpNWebInfo = base::StringPrintf("OnWindowShow, nweb_id = %u", nweb_id_);
    base::debug::ArkWebDumpInfo::GetInstance().FormatAndWriteNWebDumpInfo(dumpNWebInfo);
  }
#endif
}

void NWebImpl::OnOnlineRenderToForeground() {
  TRACE_EVENT0("base", "OnOnlineRenderToForeground");
  WVLOG_D("NWebImpl::OnOnlineRenderToForeground");
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnOnlineRenderToForeground nweb delegate is null");
    return;
  }
  nweb_delegate_->OnOnlineRenderToForeground();
}

void NWebImpl::NotifyForNextTouchEvent() {
  TRACE_EVENT0("base", "NotifyForNextTouchEvent");
  WVLOG_D("NWebImpl::NotifyForNextTouchEvent");
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("NotifyForNextTouchEvent nweb delegate is null");
    return;
  }
  nweb_delegate_->NotifyForNextTouchEvent();
}

bool NWebImpl::IsActivePolicyDisable() {
  return disableWebActivePolicy_;
}

void NWebImpl::DisableWebActivePolicy() {
  WVLOG_I("NWebImpl DisableWebActivePolicy");
  disableWebActivePolicy_ = true;
}

WebDestroyMode NWebImpl::GetWebDestroyMode() {
  return webDestroyMode_;
}

void NWebImpl::SetWebDestroyMode(WebDestroyMode mode) {
  WVLOG_I("NWebImpl set web destroy mode %{public}d", static_cast<int32_t>(mode));
  webDestroyMode_ = mode;
}

bool NWebImpl::IsScrollbarModeChanged() {
  return scrollbarModeChanged_;
}

ScrollbarMode NWebImpl::GetScrollbarMode() {
  return scrollbarMode_;
}

void NWebImpl::SetScrollbarMode(ScrollbarMode mode) {
  WVLOG_I("NWebImpl set web scrollbar mode %{public}d", static_cast<int32_t>(mode));
  if (!base::FeatureList::GetInstance() && !scrollbarModeChanged_) {
    scrollbarModeChanged_ = true;
    scrollbarMode_ = mode;
    LOG(ERROR) << "FeatureList is null and save mode";
    return;
  }
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  std::string enableFeatures;
  std::string disableFeatures;
  base::FeatureList::GetInstance()->GetCommandLineFeatureOverrides(&enableFeatures, &disableFeatures);
  LOG(INFO) << "enable:" << enableFeatures << " disable:" << disableFeatures;
  std::string allFeatures = enableFeatures + disableFeatures;
  if ((allFeatures.find("OverlayScrollbar") == std::string::npos)) {
    if (mode == ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR) {
      enableFeatures = "OverlayScrollbar";
    } else {
      disableFeatures = "OverlayScrollbar";
    }
    base::FeatureList::GetInstance()->InitFromCommandLine(enableFeatures, disableFeatures);
    base::FeatureList::GetInstance()->GetCommandLineFeatureOverrides(&enableFeatures, &disableFeatures);
    LOG(INFO) << "update enable:" << enableFeatures << " disable:" << disableFeatures;
  }
}

void NWebImpl::SetDelayDurationForBackgroundTabFreezing(int64_t delay) {
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetDelayDurationForBackgroundTabFreezing nweb delegate is null");
    return;
  }
  nweb_delegate_->SetDelayDurationForBackgroundTabFreezing(delay);
#endif
}

#if BUILDFLAG(IS_ARKWEB)
void NWebImpl::SetWindowId(uint32_t window_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetWindowId nweb delegate is null");
    return;
  }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  if (base::ohos::BlanklessController::CheckGlobalProperty()) {
    auto& instance = base::ohos::BlanklessController::GetInstance();
    instance.RecordWindowId(nweb_id_, window_id);
  }
#endif
  if (!base::ohos::IsMobileDevice()) {
    nweb_delegate_->SetWindowId(window_id);
  }
  inputmethod_handler_->SetWindowIdForIME(window_id);
}

void NWebImpl::SetFocusWindowId(uint32_t focus_window_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetFocusWindowId nweb delegate is null");
    return;
  }
  if (base::ohos::IsMobileDevice()) {
    nweb_delegate_->SetWindowId(focus_window_id);
  }
}

void NWebImpl::SetToken(void* token) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetToken nweb delegate is null");
    return;
  }
  nweb_delegate_->SetToken(token);
}

void* NWebImpl::CreateWebPrintDocumentAdapter(const std::string& jobName) {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  return nweb_delegate_->CreateWebPrintDocumentAdapter(jobName);
}

std::unique_ptr<OHOS::NWeb::NWebPrintDocumentAdapterAdapter>
    NWebImpl::CreateWebPrintDocumentAdapterV2(const std::string& jobName) {
  if (nweb_delegate_ == nullptr) {
    return nullptr;
  }
  void* adapter = nweb_delegate_->CreateWebPrintDocumentAdapterV2(jobName);
  return std::make_unique<NWebPrintDocumentAdapterAdapterImpl>(
    static_cast<OHOS::NWeb::NWebPrintDocumentAdapterAdapter*>(adapter));
}

void NWebImpl::SetPrintBackground(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetPrintBackground(enable);
}

bool NWebImpl::GetPrintBackground() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetPrintBackground();
}

void NWebImpl::SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) {}

void NWebImpl::PrecompileJavaScript(
    const std::string& url,
    const std::string& script,
    std::shared_ptr<CacheOptions>& cacheOptions,
    std::shared_ptr<NWebMessageValueCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "PrecompileJavaScript: nweb delegate has not init.";
    return;
  }
  nweb_delegate_->PrecompileJavaScript(url, script, cacheOptions, callback);
}

void NWebImpl::InjectOfflineResource(
    const std::string& url,
    const std::string& origin,
    const std::vector<uint8_t>& resource,
    const std::map<std::string, std::string>& responseHeaders,
    const int type) {
  auto manager = web_cache::WebCacheManager::GetInstance();
  manager->webCacheManagerUtils->AddResourceToCache(url, origin, resource, responseHeaders, type);
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebImpl::SetVirtualKeyBoardArg(int32_t width,
                                     int32_t height,
                                     double keyboard) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetVirtualKeyBoardArg nweb delegate is null");
    return;
  }
  nweb_delegate_->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool NWebImpl::ShouldVirtualKeyboardOverlay() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("ShouldVirtualKeyboardOverlay nweb delegate is null");
    return false;
  }
  return nweb_delegate_->ShouldVirtualKeyboardOverlay();
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
std::shared_ptr<NWebDragData> NWebImpl::GetOrCreateDragData() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("nweb_delegate_ is nullptr");
    return nullptr;
  }

  return nweb_delegate_->GetOrCreateDragData();
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

void NWebImpl::PrefetchPage(
    const std::string& url,
    const std::map<std::string, std::string>& additional_http_headers) {
    PrefetchPageV2(url, additional_http_headers, 500, false);
}

void NWebImpl::PrefetchPageV2(
    const std::string& url,
    const std::map<std::string, std::string>& additional_http_headers,
    int32_t min_time_between_prefetches,
    bool ignore_cache_control_no_store) {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  if (nweb_delegate_ == nullptr) {
    return;
  }
  TRACE_EVENT0("NWebImpl", "NWebImpl::PrefetchPage");
  std::string output;
  for (auto& header : additional_http_headers) {
    base::StringAppendF(&output, "%s: %s\r\n", header.first.c_str(),
                        header.second.c_str());
  }
  output.append("\r\n");
  nweb_delegate_->PrefetchPage(PrefetchOptions(url, output,
    min_time_between_prefetches, ignore_cache_control_no_store));
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NWebImpl::CanStoreWebArchive() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  return nweb_delegate_->CanStoreWebArchive();
}

// static
const std::vector<std::string>& NWebImpl::GetCommandLineArgsForNWebEx() {
  return g_browser_args;
}

void NWebImpl::InitBrowserServiceApi(std::vector<std::string>& browser_args,
                                     int32_t api_level) {
  g_browser_args = browser_args;
  g_browser_service_api_enabled = true;
  g_browser_service_sdk_api_level = api_level;
}

bool NWebImpl::GetBrowserServiceApiEnabled() {
  return g_browser_service_api_enabled;
}

int32_t NWebImpl::GetBrowserServiceSdkAPILevel() {
  return g_browser_service_sdk_api_level;
}

void NWebImpl::ReloadOriginalUrl() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->ReloadOriginalUrl();
}

void NWebImpl::SetBrowserUserAgentString(const std::string& user_agent) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetBrowserUserAgentString(user_agent);
}

void NWebImpl::PutArkWebAppClientExtensionCallback(
    std::shared_ptr<ArkWebAppClientExtensionCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterArkWebAppClientExtensionListener(callback);
}

void NWebImpl::PutWebAppClientExtensionCallback(
    std::shared_ptr<NWebAppClientExtensionCallback>
        web_app_client_extension_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterWebAppClientExtensionListener(
      web_app_client_extension_listener);
}

void NWebImpl::RemoveArkWebAppClientExtensionCallback() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->UnRegisterArkWebAppClientExtensionListener();
}

void NWebImpl::RemoveWebAppClientExtensionCallback() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->UnRegisterWebAppClientExtensionListener();
}

void NWebImpl::GetImageFromContextNode() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->GetImageFromContextNode();
}

void NWebImpl::GetImageFromCache(const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->GetImageFromCacheEx(url);
}

void NWebImpl::PutWebExtensionCallback(
    std::shared_ptr<NWebExtensionCallback> web_extension_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web extension callback failed, nweb delegate is nullptr, nweb_id "
        "= %{public}u",
        nweb_id_);
    return;
  }
  WVLOG_I("set web extension, nweb_id = %{public}u", nweb_id_);
  nweb_delegate_->RegisterWebExtensionListener(web_extension_listener);
}

void NWebImpl::RemoveWebExtensionCallback() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->UnRegisterWebExtensionListener();
}

void NWebImpl::RunJavaScriptInFrames(RunJavaScriptParam param,
                                     OnReceiveValueCallback callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }
  if (callback == nullptr) {
    LOG(WARNING) << "NWebImpl::RunJavaScriptInFrames callback is nullptr";
    return;
  }

  nweb_delegate_->RunJavaScriptInFrames(param, callback);
}

void NWebImpl::GetAllFrameInfos(OnReceiveFrameInfosCallback callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }
  if (callback == nullptr) {
    LOG(WARNING) << "NWebImpl::GetAllFrameInfos callback is nullptr";
    return;
  }

  nweb_delegate_->GetAllFrameInfos(callback);
}

void NWebImpl::GetLastJavaScriptProxyCallingFrameInfo(
    OnLastJavaScriptProxyCallingFrameInfoCallback callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "remove web app client extension callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }
  if (callback == nullptr) {
    LOG(WARNING) << "NWebImpl::GetLastJavaScriptProxyCallingFrameInfo callback is nullptr";
    return;
  }

  nweb_delegate_->GetLastJavaScriptProxyCallingFrameInfo(callback);
}

void NWebImpl::OpenDevtools(std::unique_ptr<OpenDevToolsParam> param) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "OpenDevtools failed, no nweb_delegate";
    return;
  }
  int32_t devtools_nweb_id = param->nweb_id;
  NWebImpl* nweb = NWebImpl::FromID(devtools_nweb_id);
  if (!nweb) {
    LOG(WARNING) << "OpenDevtools failed, no nweb";
    return;
  }
  nweb_delegate_->OpenDevtoolsWith(nweb->nweb_delegate_, std::move(param));
}

void NWebImpl::OpenDevtoolsByPb(std::unique_ptr<OpenDevToolsParam> param,
                      OpenDevToolsExtOpt& ext_opt) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "OpenDevtoolsV2 failed, no nweb_delegate";
    return;
  }
  int32_t devtools_nweb_id = param->nweb_id;
  NWebImpl* nweb = NWebImpl::FromID(devtools_nweb_id);
  if (!nweb) {
    LOG(WARNING) << "OpenDevtoolsV2 failed, no nweb";
    return;
  }
  nweb_delegate_->OpenDevtoolsWithByPb(nweb->nweb_delegate_, std::move(param), ext_opt);
}

void NWebImpl::CloseDevtools() {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "CloseDevtools failed, no nweb_delegate";
    return;
  }
  nweb_delegate_->CloseDevtools();
}

void NWebImpl::EnableAutoResize(int32_t min_width, int32_t min_height,
                                int32_t max_width, int32_t max_height) {
  CefSize cef_min_size(min_width, min_height);
  CefSize cef_max_size(max_width, max_height);
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "EnableAutoResize failed, no nweb_delegate";
    return;
  }

  nweb_delegate_->EnableViewAutoResize(cef_min_size, cef_max_size);
}

//  static
void NWebImpl::DisableAutoResize() {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "DisableAutoResize failed, no nweb_delegate";
    return;
  }
  nweb_delegate_->DisableViewAutoResize();
}
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void NWebImpl::EnableVideoAssistant(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "nweb delegate is nullptr when enable video assistant";
    return;
  }
  nweb_delegate_->EnableVideoAssistant(enable);
}

void NWebImpl::ExecuteVideoAssistantFunction(const std::string& cmd_id) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING)
        << "nweb delegate is nullptr when execute video assistant function";
    return;
  }
  nweb_delegate_->ExecuteVideoAssistantFunction(cmd_id);
}

NO_SANITIZE("cfi")
void NWebImpl::OnReportStatisticLog(const std::string& content) {
  if (on_report_statistic_log_callback_) {
    on_report_statistic_log_callback_(content.c_str());
  }
}

void NWebImpl::SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func) {
  on_report_statistic_log_callback_ = func;
  base::ohos::SetOnReportStatisticLogCallback(on_report_statistic_log_callback_);
}

void NWebImpl::CustomWebMediaPlayer(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(WARNING) << "nweb delegate is nullptr when enable custom web media player";
    return;
  }
  nweb_delegate_->CustomWebMediaPlayer(enable);
}

void NWebImpl::WebMediaPlayerControllerPlay() {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerPlay, delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerPlay();
}

void NWebImpl::WebMediaPlayerControllerPause() {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerPause, delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerPause();
}

void NWebImpl::WebMediaPlayerControllerSeek(double time) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerSeek, delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerSeek(time);
}

void NWebImpl::WebMediaPlayerControllerSetMuted(bool muted) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerSetMuted, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerSetMuted(muted);
}

void NWebImpl::WebMediaPlayerControllerSetPlaybackRate(double playback_rate) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerSetPlaybackRate, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerSetPlaybackRate(playback_rate);
}

void NWebImpl::WebMediaPlayerControllerExitFullscreen() {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerExitFullscreen, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerExitFullscreen();
}

void NWebImpl::WebMediaPlayerControllerSetVideoSurface(void* native_window) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerSetVideoSurface, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerSetVideoSurface(native_window);
}

void NWebImpl::WebMediaPlayerControllerDownload() {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerDownload, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerDownload();
}

void NWebImpl::WebMediaPlayerControllerSetVolume(double volume) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerSetVolume, "
                  "delegate is null";
    return;
  }
  nweb_delegate_->WebMediaPlayerControllerSetVolume(volume);
}

double NWebImpl::WebMediaPlayerControllerGetVolume() {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to WebMediaPlayerControllerGetVolume, "
                  "delegate is null";
    return -1.0;
  }
  return nweb_delegate_->WebMediaPlayerControllerGetVolume();
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
content::BrowserContext* NWebImplGetGlobalBrowserContext() {
  CefRequestContextImpl* request_context =
      static_cast<CefRequestContextImpl*>(CefRequestContext::GetGlobalContext().get());
  if (!request_context) {
    LOG(ERROR) << "NWebImpl::GetGlobalBrowserContext request_context is null";
    return nullptr;
  }
  CefBrowserContext* cef_browser_context = request_context->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "NWebImpl::GetGlobalBrowserContext cef_browser_context is null";
    return nullptr;
  }
  return cef_browser_context->AsBrowserContext();
}

// static
void NWebImpl::PutWebExtensionManagerCallback(
    std::shared_ptr<NWebExtensionManagerCallBack> web_extension_manager_listener) {
  WVLOG_I("register web extension manager listener");
  extensions::ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener(web_extension_manager_listener);
}

// static
void NWebImpl::RemoveWebExtensionManagerCallback() {
  WVLOG_I("unreqister web extension manager listener");
  extensions::ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
}

// static
void NWebImpl::UnLoadWebExtension(const std::string& eid) {
  WVLOG_I("NWebImpl::UnLoadWebExtension %{public}s", eid.c_str());
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "NWebImpl::UnLoadWebExtension browser_context is null";
    return;
  }
  const extensions::Extension* current_extension =
      extensions::ExtensionRegistry::Get(browser_context)
      ->GetExtensionById(eid, extensions::ExtensionRegistry::EVERYTHING);
  std::u16string* error = nullptr;
  if (current_extension) {
    if (current_extension->was_installed_by_default()) {
      WVLOG_I("NWebImpl::UnLoadWebExtension RemovedDefaultInstalledExtension");
    }
    if (extensions::ExtensionSystem::Get(browser_context)->extension_service()) {
      bool result = extensions::ExtensionSystem::Get(browser_context)
          ->extension_service()
          ->UninstallExtension(eid, extensions::UNINSTALL_REASON_USER_INITIATED, error);
      WVLOG_I("NWebImpl::UnLoadWebExtension result:%{public}d, error:%{public}s", result, error);
    }
    return;
  }
  WVLOG_I("NWebImpl::UnLoadWebExtension extension not exist!");
}

// static
void NWebImpl::DisableWebExtension(const std::string& eid) {
  WVLOG_I("NWebImpl::DisableWebExtension %{public}s", eid.c_str());
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "NWebImpl::DisableWebExtension browser_context is null";
    return;
  }
  const extensions::Extension* current_extension =
      extensions::ExtensionRegistry::Get(browser_context)
      ->GetExtensionById(eid, extensions::ExtensionRegistry::EVERYTHING);
  if (current_extension) {
    if (current_extension->was_installed_by_default()) {
      WVLOG_I("NWebImpl::DisableWebExtension DisableDefaultInstalledExtension");
    }

    if (extensions::ExtensionSystem::Get(browser_context)->extension_service()) {
      extensions::ExtensionSystem::Get(browser_context)
          ->extension_service()
          ->DisableExtension(eid, extensions::disable_reason::DISABLE_USER_ACTION);
      WVLOG_I("NWebImpl::DisableWebExtension id:%{public}s", eid.c_str());
    }
    return;
  }
  WVLOG_I("NWebImpl::DisableWebExtension extension not exist: id:%{public}s", eid.c_str());
}

// static
void NWebImpl::GetExtensionInfoByTabId(int32_t tabId, std::vector<WebExtensionInfo>& extensionsInfo) {
  WVLOG_I("NWebImpl::GetExtensionInfoByTabId, %{public}d", tabId);
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "NWebImpl::GetExtensionInfoByTabId browser_context is null";
    return;
  }

  const extensions::ExtensionRegistry* extensionRegistry = extensions::ExtensionRegistry::Get(browser_context);
  const extensions::ExtensionSet extensions = extensionRegistry->GenerateInstalledExtensionsSet();
  for (const auto& extension : extensions) {
    LOG(INFO) << "NWebImpl::GetExtensionInfoByTabId id=" << extension->id();
    if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
      continue;
    }
    WebExtensionInfo itemInfo;
    itemInfo.extensionId = extension->id();
    extensions::ExtensionRegistryInfoManager* manager =
                      extensions::ExtensionSystem::Get(browser_context)->GetExtensionRegistryInfoManager();
    if (manager) {
      itemInfo.action = manager->GetExtensionActionInfo(*extension, tabId);
      itemInfo.sidePanel = manager->GetExtensionSidePanelInfo(*extension, tabId);
      itemInfo.contextMenus = manager->GetAllExtensionContextMenus(extension->id());
    }
    extensionsInfo.push_back(itemInfo);
  }
}

void NWebImpl::SetExtensionName(const std::string& extension_name) {
  g_extension_name = extension_name;
}

bool NWebImpl::GetExtensionName(std::string& extension_name) {
  if (g_extension_name.has_value()) {
    extension_name = g_extension_name.value();
    return true;
  } else {
    return false;
  }
}

std::string NWebImpl::GetExtensionVersion(const std::string& extension_id) {
  WVLOG_I("NWebImpl::GetExtensionVersion for extension: %s",
          extension_id.c_str());

  if (extension_id.empty()) {
    return "";
  }

  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    WVLOG_E("Failed to get global browser context");
    return "";
  }

  extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(browser_context);
  if (!registry) {
    WVLOG_E("Failed to get extension registry");
    return "";
  }

  const extensions::Extension* extension = registry->GetExtensionById(
      extension_id, extensions::ExtensionRegistry::ENABLED);

  return extension ? extension->version().GetString() : "";
}

void NWebImpl::InstallExtensionFile(const std::string& file_path,
                                    OnExtensionInstallCallback callback) {
  WVLOG_I("NWebImpl::InstallExtensionFile: %s", file_path.c_str());

  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&NWebImpl::InstallExtensionFile, file_path, callback));
    return;
  }

  if (file_path.empty()) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Invalid file path", nullptr);
    }
    return;
  }

  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    WVLOG_E("Failed to get global browser context");
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Browser context not available", nullptr);
    }
  }

  base::FilePath crx_path(file_path);
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&base::PathExists, crx_path),
      base::BindOnce(&PerformCrxInstallation, file_path, callback, browser_context));
}

void NWebImpl::InstallExtensionFileV2(const std::string& file_path,
                                        const NWebExtensionInstallProperties& options,
                                        OnExtensionInstallCallback callback) {
  WVLOG_I("NWebImpl::InstallExtensionFileV2: %s", file_path.c_str());

  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&NWebImpl::InstallExtensionFileV2, file_path, options, callback));
    return;
  }

  if (file_path.empty()) {
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Invalid file path", nullptr);
    }
    return;
  }

  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    WVLOG_E("Failed to get global browser context");
    if (callback) {
      callback(static_cast<int>(extensions::CrxInstallErrorType::OTHER),
               "Browser context not available", nullptr);
    }
    return;
  }

  base::FilePath crx_path(file_path);
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&base::PathExists, crx_path),
      base::BindOnce(&PerformCrxInstallationV2, file_path, options, callback, browser_context));
}

void NWebImpl::SetPublisherKeys(const std::vector<std::vector<uint8_t>>& keys) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(&NWebImpl::SetPublisherKeys, keys));
    return;
  }
  LOG(INFO) << "NWebImpl::SetPublisherKeys";
  crx_file::CrxKeyService::GetInstance()->SetPublisherKeys(keys);
}

void NWebImpl::UninstallExtension(const std::string& eid,
                                  OnExtensionUninstallCallback callback) {
  WVLOG_I("NWebImpl::UninstallExtension %{public}s", eid.c_str());

  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&NWebImpl::UninstallExtension, eid, callback));
    return;
  }

  if (eid.empty()) {
    HandleExtensionUninstallResult(callback, false, "Invalid extension id");
    return;
  }

  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    HandleExtensionUninstallResult(callback, false, "Browser context not available");
    return;
  }

  const extensions::Extension* current_extension =
      extensions::ExtensionRegistry::Get(browser_context)
          ->GetExtensionById(eid, extensions::ExtensionRegistry::EVERYTHING);

  if (!current_extension) {
    std::string msg = std::string("Failed to find extension with id: ") + eid;
    HandleExtensionUninstallResult(callback, false, msg);
    return;
  }

  auto* service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();

  if (!service) {
    HandleExtensionUninstallResult(callback, false, "Extension service not available");
    return;
  }

  service->RemoveForbidDisplayInSettings(eid);

  std::u16string error;
  const bool ok = service->UninstallExtension(
      eid, extensions::UNINSTALL_REASON_INTERNAL_MANAGEMENT, &error);

  if (!ok) {
    const std::string err =
        error.empty() ? "Uninstall failed" : base::UTF16ToUTF8(error);
    HandleExtensionUninstallResult(callback, false, err);
    return;
  }

  HandleExtensionUninstallResult(callback, true, "");
}

// static
void NWebImpl::WebExtensionSetForbidDisplayInSettings(
    const std::set<std::string>& extension_ids) {
  WVLOG_I("NWebImpl::WebExtensionSetForbidDisplayInSettings");
  content::BrowserContext* browser_context = NWebImplGetGlobalBrowserContext();
  if (!browser_context) {
    WVLOG_E(
        "NWebImpl::WebExtensionSetForbidDisplayInSettings browser_context is "
        "null");
    return;
  }
  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();
  if (!extension_service) {
    WVLOG_E(
        "NWebImpl::WebExtensionSetForbidDisplayInSettings extension_service is "
        "null");
    return;
  }
  extension_service->SetForbidDisplayInSettings(extension_ids);
}

NO_SANITIZE("cfi")
void NWebImpl::OnAlertDialogByJS(const std::string& extensionId,
                                 const std::string& url,
                                 const std::string& message,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool& suppress_message) {
  LOG(INFO) << " func:" << __FUNCTION__;
  std::shared_ptr<NWebJSDialogResult> js_alert_callback =
      std::make_shared<NWebJSDialogResultImpl>(callback);
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    int requestId = dialog_manager->AddAlertRequest(js_alert_callback);
    if (on_off_screen_alert_callback_) {
      on_off_screen_alert_callback_(extensionId.c_str(), url.c_str(),
                                    message.c_str(), requestId);
    }
  }
  suppress_message = false;
}

NO_SANITIZE("cfi")
void NWebImpl::OnConfirmDialogByJS(const std::string& extensionId,
                                   const std::string& url,
                                   const std::string& message,
                                   CefRefPtr<CefJSDialogCallback> callback,
                                   bool& suppress_message) {
  LOG(INFO) << " func:" << __FUNCTION__;
  std::shared_ptr<NWebJSDialogResult> js_confirm_callback =
      std::make_shared<NWebJSDialogResultImpl>(callback);
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    int requestId = dialog_manager->AddConfirmRequest(js_confirm_callback);
    if (on_off_screen_confirm_callback_) {
      on_off_screen_confirm_callback_(extensionId.c_str(), url.c_str(),
                                      message.c_str(), requestId);
    }
  }
  suppress_message = false;
}

NO_SANITIZE("cfi")
void NWebImpl::OnPromptDialogByJS(const std::string& extensionId,
                                  const std::string& url,
                                  const std::string& message,
                                  const std::string& value,
                                  CefRefPtr<CefJSDialogCallback> callback,
                                  bool& suppress_message) {
  LOG(INFO) << " func:" << __FUNCTION__;
  std::shared_ptr<NWebJSDialogResult> js_prompt_callback =
      std::make_shared<NWebJSDialogResultImpl>(callback);
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    int requestId = dialog_manager->AddPromptRequest(js_prompt_callback);
    if (on_off_screen_prompt_callback_) {
      on_off_screen_prompt_callback_(extensionId.c_str(), url.c_str(),
                                     message.c_str(), value.c_str(), requestId);
    }
  }
}

void NWebImpl::SetOnOffscreenDocumentAlertCallback(
    OnArkWebStaticOffscreenDocumentAlertFunc func) {
  LOG(INFO) << " func:" << __FUNCTION__;
  on_off_screen_alert_callback_ = func;
}

void NWebImpl::SetOnOffscreenDocumentConfirmCallback(
    OnArkWebStaticOffscreenDocumentConfirmFunc func) {
  LOG(INFO) << " func:" << __FUNCTION__;
  on_off_screen_confirm_callback_ = func;
}

void NWebImpl::SetOnOffscreenDocumentPromptCallback(
    OnArkWebStaticOffscreenDocumentPromptFunc func) {
  LOG(INFO) << " func:" << __FUNCTION__;
  on_off_screen_prompt_callback_ = func;
}

void NWebImpl::AlertHandle(const int requestId) {
  LOG(INFO) << " func:" << __FUNCTION__;
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    dialog_manager->Alert(requestId);
  }
}

void NWebImpl::ConfirmHandle(const bool type, const int requestId) {
  LOG(INFO) << " func:" << __FUNCTION__;
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    dialog_manager->Confirm(type, requestId);
  }
}

void NWebImpl::PromptHandle(const bool type,
                            const std::string& value,
                            const int requestId) {
  LOG(INFO) << " func:" << __FUNCTION__;
  CefOffScreenDocumentDialogManager* dialog_manager =
      CefOffScreenDocumentDialogManager::GetInstance();
  if (dialog_manager) {
    dialog_manager->Prompt(type, value, requestId);
  }
}

void NWebImpl::SetOnOffscreenDocumentPermissionRequestCallback(
    OnArkWebStaticOffscreenDocumentPermissionRequestFunc func) {
  LOG(INFO) << " func:" << __FUNCTION__;
  on_offscreen_document_permission_request_callback_ = func;
}

NO_SANITIZE("cfi")
void NWebImpl::OnOffscreenDocumentPermissionRequest(
    const std::string& extension_id,
    const std::string& origin_url,
    int resources,
    int request_key) {
  if (on_offscreen_document_permission_request_callback_) {
    LOG(INFO) << " func:" << __FUNCTION__;
    on_offscreen_document_permission_request_callback_(
        extension_id.c_str(), origin_url.c_str(), resources, request_key);
  } else {
    LOG(ERROR) << __FUNCTION__ << " callback is null";
    DenyOffscreenDocumentPermission(resources, request_key);
  }
}

void NWebImpl::GrantOffscreenDocumentPermission(int resources,
                                                int request_key) {
  LOG(INFO) << " func:" << __FUNCTION__;
  OffscreenPermissionRequestHandler::GetInstance()->Grant(resources,
                                                          request_key);
}

void NWebImpl::DenyOffscreenDocumentPermission(int resources, int request_key) {
  LOG(INFO) << " func:" << __FUNCTION__;
  OffscreenPermissionRequestHandler::GetInstance()->Deny(resources,
                                                         request_key);
}

NO_SANITIZE("cfi")
void NWebImpl::OnOffscreenDocumentWindowNewEvent(
    const std::string& extensionId,
    const std::string& originUrl,
    bool isAlert,
    bool isUserTrigger,
    const std::string& targetUrl) {
  LOG(INFO) << " func:" << __FUNCTION__;
  if (on_off_screen_window_new_callback_) {
    on_off_screen_window_new_callback_(
        extensionId.c_str(), originUrl.c_str(),
        isAlert, isUserTrigger, targetUrl.c_str());
  }
}

void NWebImpl::SetOnOffscreenDocumentWindowNewCallback(
    OnArkWebStaticOffscreenDocumentWindowNewFunc func) {
  LOG(INFO) << " func:" << __FUNCTION__;
  on_off_screen_window_new_callback_ = func;
}

void NWebImpl::SetOffscreenNWebId(uint32_t nweb_id) {
  LOG(INFO) << " func:" << __FUNCTION__;
  off_screen_nweb_id_ = nweb_id;
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
void NWebImpl::PutLoggerCallback(
    std::shared_ptr<NWebLoggerCallback> logger_callback) {
  WVLOG_D("put logger callback");
  g_logger_callback = logger_callback;
}

void NWebImpl::RemoveLoggerCallback() {
  WVLOG_D("remove logger callback");
  NWebHandlerDelegate::UnRegisterLoggerCallback();
}
#endif

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
// static
void NWebImpl::SetConnectTimeout(int32_t seconds) {
  content::GetNetworkService()->SetConnectTimeout(seconds);
}

void NWebImpl::SetConnectionTimeout(int32_t timeout) {
  if (g_nweb_count == 0) {
    WVLOG_I(
        "nweb had not initiated. Will set timeout value after network service "
        "initialized.");
    return;
  }

  network::mojom::NetworkService* network_service =
      content::GetNetworkService();
  if (!network_service) {
    WVLOG_I(
        "network_service is nullptr. Will set timeout value after network "
        "service initialized.");
    return;
  }

  content::GetNetworkService()->SetConnectTimeout(timeout);
  WVLOG_I("set connection timeout value in NetHelpers is: %{public}d",
          net_service::NetHelpers::connection_timeout);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_UA)
// static
void NWebImpl::UpdateCloudUAConfig(const std::string& file_path,
                                   const std::string& version) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->UpdateCloudUAConfig(file_path,
                                                                    version);
}

// static
void NWebImpl::UpdateUAListConfig(const std::string& ua_name,
                                  const std::string& ua_string) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->UpdateUAListConfig(ua_name,
                                                                   ua_string);
}

// static
void NWebImpl::SetUAForHosts(const std::string& ua_name,
                             const std::vector<std::string>& hosts) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->SetUAForHosts(ua_name, hosts);
}

// static
std::string NWebImpl::GetUANameConfig(const std::string& host) {
  return nweb_ex::AlloyBrowserUAConfig::GetInstance()->GetUANameConfig(host);
}

// static
void NWebImpl::SetBrowserUA(const std::string& ua_name) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->SetBrowserUA(ua_name);
}

// static
void NWebImpl::EnableGlobalBrowserUAConfig(const bool enable) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->EnableGlobalBrowserUAConfig(enable);
}

void NWebImpl::SetBrowserUAConfigPolicy(int ua_config_policy) {
  nweb_ex::AlloyBrowserUAConfig::GetInstance()->SetBrowserUAConfigPolicy(ua_config_policy);
}
#endif  // ARKWEB_EXT_UA

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT) && BUILDFLAG(IS_ARKWEB_EXT)
// static
void NWebImpl::UpdateBrowserEngineConfig(const std::string& file_path,
                                         const std::string& version) {
  nweb_ex::AlloyBrowserEngineCloudConfig::GetInstance()
      ->UpdateBrowserEngineCloudConfig(file_path, version);
}
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL) && BUILDFLAG(IS_ARKWEB_EXT)
// static
void NWebImpl::UpdateBrowserEngineGlobalConfig(const std::string& file_path,
                                               const std::string& version) {
  nweb_ex::AlloyBrowserEngineGlobalConfig::GetInstance()
      ->UpdateBrowserEngineGlobalConfig(file_path, version);
}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
// static
void NWebImpl::UpdateReaderModeConfig(const std::string& file_path,
                                  const std::string& version) {
  nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->UpdateBrowserReaderModeConfig(file_path, version);
}

// static
void NWebImpl::SetJsFilePath(const std::string& js_type, const std::string& file_path, const std::string& version) {
  nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->SetJsFilePath(js_type, file_path, version);
}

void NWebImpl::Distill(char** guid, const DistillOptions& distill_options, DistillCallback callback) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "NWebImpl::Distill delegate_ is nullptr";
    return;
  }
  if (callback == nullptr) {
    LOG(ERROR) << "NWebImpl::Distill callback is nullptr";
    return;
  }
  std::string guid_str = base::Uuid::GenerateRandomV4().AsLowercaseString();
  size_t len = guid_str.size() + 1;
  *guid = new char[len];
  base::strings::SafeSNPrintf(*guid, len, "%s", guid_str.c_str());
  nweb_delegate_->Distill(guid_str, distill_options, callback);
}

void NWebImpl::AbortDistill() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "NWebImpl::AbortDistill delegate_ is nullptr";
    return;
  }

  nweb_delegate_->AbortDistill();
}
#endif

#if BUILDFLAG(ARKWEB_I18N)
void NWebImpl::UpdateAcceptLanguageInternal() {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(::switches::kLang)) {
    return;
  }
  std::string lang = command_line.GetSwitchValueASCII(::switches::kLang);
  std::regex pattern("-");
  std::smatch match;
  if (std::regex_search(lang, match, pattern)) {
    std::string language = match.prefix();
    std::string region = match.suffix();
    UpdateLocale(language, region);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
std::string NWebImpl::GetSelectedTextFromContextParam() {
  if (nweb_delegate_) {
    return nweb_delegate_->GetSelectedTextFromContextParam();
  }
  return std::string();
}

void NWebImpl::ShowFreeCopyMenu() const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ShowFreeCopyMenu();
}

bool NWebImpl::ShouldShowFreeCopyMenu() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->ShouldShowFreeCopyMenu();
}

#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)

#if BUILDFLAG(ARKWEB_ADBLOCK)
// static
void NWebImpl::SetAdsBlockRules(const std::string& rulesFiles,
                                const bool replace) {
  LOG(DEBUG) << "[adblock] SetAdsBlockRules called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->SetAdsBlockRules(rulesFiles,
                                                                replace);

  ::adblock::AdBlockList::UpdateUserEasyListRules(rulesFiles);
}

// static
void NWebImpl::AddAdsBlockDisallowList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] AddAdsBlockDisallowList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockDisallowList(
      domainSuffixes);
}

// static
void NWebImpl::AddAdsBlockAllowList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] AddAdsBlockAllowList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockAllowList(
      domainSuffixes);
}

// static
void NWebImpl::RemoveAdsBlockDisallowedList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] RemoveAdsBlockDisallowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockDisallowedList(
      domainSuffixes);
}

// static
void NWebImpl::RemoveAdsBlockAllowedList(
    const std::vector<std::string>& domainSuffixes) {
  LOG(DEBUG) << "[adblock] RemoveAdsBlockAllowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockAllowedList(
      domainSuffixes);
}

// static
void NWebImpl::ClearAdsBlockDisallowedList() {
  LOG(DEBUG) << "[adblock] ClearAdsBlockDisallowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockDisallowedList();
}

// static
void NWebImpl::ClearAdsBlockAllowedList() {
  LOG(DEBUG) << "[adblock] ClearAdsBlockAllowedList called from ui";
  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockAllowedList();
}

bool NWebImpl::IsAdsBlockEnabledForCurPage() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  LOG(DEBUG) << "[adblock] IsAdsBlockEnabledForCurPage called from ui";
  return nweb_delegate_->IsAdsBlockEnabledForCurPage();
}

bool NWebImpl::IsAdsBlockEnabled() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  LOG(DEBUG) << "[adblock] IsAdsBlockEnabled called from ui";
  return nweb_delegate_->IsAdsBlockEnabled();
}

void NWebImpl::EnableAdsBlock(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  LOG(DEBUG) << "[adblock] EnableAdsBlock called from ui";
  nweb_delegate_->EnableAdsBlock(enable);
}

void NWebImpl::SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                        int main_frame_tree_node_id) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  LOG(DEBUG) << "[adblock] SetAdBlockEnabledForSite called from ui";
  nweb_delegate_->SetAdBlockEnabledForSite(is_adblock_enabled,
                                           main_frame_tree_node_id);
}

// static
bool NWebImpl::IsAnyNWebAdblockEnabled() {
  NWebMap* map = g_nweb_map.Pointer();

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  bool replace_switch = OHOS::adblock::AdBlockConfig::GetInstance()
                            ->GetUserEasylistReplaceSwitch();

  for (auto it = map->begin(); it != map->end(); it++) {
    auto nweb_weak_ptr = it->second.lock();
    if (nweb_weak_ptr) {
      auto nweb = nweb_weak_ptr.get();

      if (nweb && nweb->IsAdsBlockEnabled() && !replace_switch) {
        return true;
      }
    }
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
bool NWebImpl::GetSavePasswordAutomatically() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetSavePasswordAutomatically();
}

void NWebImpl::SetSavePasswordAutomatically(bool enable) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetSavePasswordAutomatically(enable);
}

bool NWebImpl::GetSavePassword() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetSavePassword();
}

void NWebImpl::SetSavePassword(bool enable) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetSavePassword(enable);
}
void NWebImpl::SaveOrUpdatePassword(bool is_update) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SaveOrUpdatePassword(is_update);
}
#endif  // #if BUILDFLAG(ARKWEB_EXT_PASSWORD)

#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
void NWebImpl::PasswordSuggestionSelected(int list_index) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->PasswordSuggestionSelected(list_index);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
void NWebImpl::SetForceEnableZoom(bool forceEnableZoom) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetForceEnableZoom(forceEnableZoom);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
bool NWebImpl::GetForceEnableZoom() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->GetForceEnableZoom();
}
#endif  // ARKWEB_EXT_FORCE_ZOOM

void NWebImpl::PutWebDownloadDelegateCallback(
    std::shared_ptr<NWebDownloadDelegateCallback>
        web_download_delegate_listener) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web download delegate callback failed, nweb delegate is nullptr, "
        "nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterWebDownloadDelegateListener(
      web_download_delegate_listener);
}

void NWebImpl::StartDownload(const char* url) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "start download failed, nweb delegate is nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->StartDownload(url);
}

void NWebImpl::ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "resume download failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->ResumeDownload(web_download);
}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
NWebDownloadItemState NWebImpl::GetDownloadItemState(long item_id) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "GetDownloadItemState failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }

  return nweb_delegate_->GetDownloadItemState(item_id);
}

// static
NWebDownloadItemState NWebImpl::GetDownloadItemStateByGuid(
    const std::string& guid) {
  LOG(DEBUG) << "get download item state " << guid;
  CefRefPtr<CefDownloadItem> download_item = CefGetDownloadItem(guid);
  if (!download_item) {
    LOG(ERROR) << "GetDownloadItemState failed, for download_item is nullptr, "
               << "guid " << guid;
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }

  return NWebDownloadItem::GetNWebState(download_item);
}
#endif

void NWebImpl::PutAccessibilityEventCallback(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibilityEventListener) {
  // Deprecated due to new accessibility architecture
}

void NWebImpl::PutAccessibilityIdGenerator(
    const AccessibilityIdGenerateFunc accessibilityIdGenerator) {
  // Deprecated due to new accessibility architecture
}

void NWebImpl::ExecuteAction(int64_t accessibilityId, uint32_t action) {
  // Deprecated due to new accessibility architecture
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                          bool isAccessibilityFocus) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetFocusedAccessibilityNodeInfo(
        accessibilityId, isAccessibilityFocus);
  }
#endif
  return nullptr;
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetAccessibilityNodeInfoById(int64_t accessibilityId) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoById(accessibilityId);
  }
#endif
  return nullptr;
}

int64_t NWebImpl::GetWebAccessibilityIdByHtmlElementId(const std::string& htmlElementId) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetWebAccessibilityIdByHtmlElementId(htmlElementId);
  }
#endif
  return -1;
}

bool NWebImpl::GetAccessibilityVisible(int64_t accessibilityId) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityVisible(accessibilityId);
  }
#endif
  return true;
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebImpl::GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                              int32_t direction) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeInfoByFocusMove(accessibilityId,
                                                               direction);
  }
#endif
  return nullptr;
}

void NWebImpl::SetAccessibilityState(bool state) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SetAccessibilityState(state ? STATE_ENABLED
                                                : STATE_DISABLED);
  }
#endif
}

bool NWebImpl::GetAccessibilityNodeRectById(int64_t accessibilityId,
                                            int32_t* width,
                                            int32_t* height,
                                            int32_t* offsetX,
                                            int32_t* offsetY) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->GetAccessibilityNodeRectById(
        accessibilityId, width, height, offsetX, offsetY);
  }
#endif
  return false;
}

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
// static
void NWebImpl::BindToNetwork(int network) {
  net_service::NetHelpers::network = network;
  if (g_nweb_count != 0) {
    network::mojom::NetworkService* network_service =
        content::GetNetworkService();
    if (network_service) {
      network_service->BindDnsToNetwork(network);
      WVLOG_I("bint to network %{public}d", net_service::NetHelpers::network);
      // net::NetworkChangeNotifier::BindToNetwork(network);
    } else {
      WVLOG_E("net_work_service is nullptr");
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
void NWebImpl::SetBrowserZoomLevel(double zoom_factor) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetBrowserZoomLevel(zoom_factor);
}

double NWebImpl::GetBrowserZoomLevel() const {
  if (nweb_delegate_ == nullptr) {
    return default_zoom_factor;
  }
  return nweb_delegate_->GetBrowserZoomLevel();
}

// static
void NWebImpl::SetDefaultBrowserZoomLevel(double zoom_factor) {
  if (g_nweb_count == 0) {
    WVLOG_I("nweb had not initiated try to set default browser zoom level.");
    return;
  }
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    auto browser_context = cef_browser_context->AsProfile();
    if (!browser_context || browser_context->IsOffTheRecord()) {
      LOG(ERROR) << "SetDefaultBrowserZoomLevel null browser_context or "
                    "browser_context is off the record.";
      return;
    }
    if (!browser_context->GetZoomLevelPrefs()) {
      LOG(ERROR) << "GetZoomLevelPrefs is nullptr.";
      return;
    }
    browser_context->GetZoomLevelPrefs()->SetDefaultZoomLevelPref(
        blink::ZoomFactorToZoomLevel(zoom_factor));
    default_zoom_factor = zoom_factor;
  }
}
#endif

#if BUILDFLAG(ARKWEB_CRASHPAD)
void NWebImpl::SetDefaultCrashpadLogPath(const std::string& crashpad_log_path) {
  LOG(INFO) << "g_crashpad_target_location is " << g_crashpad_target_location
            << " crashpad_log_path is " << crashpad_log_path;
  g_crashpad_target_location = crashpad_log_path;
}

const std::string NWebImpl::GetDefaultCrashpadLogPath() {
  return g_crashpad_target_location;
}

void NWebImpl::SetDefaultFeedbacklogsCrashPath(
    const std::string& feedbacklogs_crash_path) {
  LOG(INFO) << "[feedbacklogs] g_feedbacklogs_crash_path is "
            << g_feedbacklogs_crash_path << ",feedbacklogs_crash_path is "
            << feedbacklogs_crash_path;
  g_feedbacklogs_crash_path = feedbacklogs_crash_path;
}

const std::string& NWebImpl::GetDefaultFeedbacklogsCrashPath() {
  return g_feedbacklogs_crash_path;
}

#endif

#if BUILDFLAG(ARKWEB_DISCARD)
bool NWebImpl::Discard() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("Discard failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return false;
  }

  if (!nweb_delegate_->Discard()) {
    return false;
  }

  WVLOG_D("Discard: Notify the bufferq to clean all caches");
  return true;
}

bool NWebImpl::Restore() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("Restore failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return false;
  }

  return nweb_delegate_->Restore();
}
#endif

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
bool NWebImpl::WebExtensionContextMenuIsIframe() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nullptr nweb_delegate_";
    return false;
  }
  return nweb_delegate_->WebExtensionContextMenuIsIframe();
}

void NWebImpl::WebExtensionContextMenuReloadFocusedFrame() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nullptr nweb_delegate_";
    return;
  }
  return nweb_delegate_->WebExtensionContextMenuReloadFocusedFrame();
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void NWebImpl::WebExtensionContextMenuGetFocusedFrameInfo(
    int32_t& frame_id,
    std::string& frame_url) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nullptr nweb_delegate_";
    return;
  }
  nweb_delegate_->WebExtensionContextMenuGetFocusedFrameInfo(frame_id,
                                                             frame_url);
}
#endif

// static
void NWebImpl::ResumeDownloadStatic(
    std::shared_ptr<NWebDownloadItem> web_download) {
  CefResumeDownload(web_download->guid, web_download->url,
                    web_download->full_path, web_download->received_bytes,
                    web_download->total_bytes, web_download->etag,
                    web_download->mime_type, web_download->last_modified,
                    web_download->received_slices);
}

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
// static
void NWebImpl::SetFileRenameOption(const int file_rename_option) {
  WVLOG_I("NWebImpl::SetFileRenameOption: option: %{public}d",
          file_rename_option);
  CefSetFileRenameOption(file_rename_option);
}
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)

void NWebImpl::ReadDownloadData(const std::string& guid,
                                const int32_t read_size,
                                ReadDownloadDataCallback callback) {
  WVLOG_I("NWebImpl::ReadDownloadData: option: %{public}s", guid.c_str());
  CefRefPtr<NWebReadDownloadDataCallback> read_download_data_callback =
      new NWebReadDownloadDataCallback(callback);
  CefReadDownloaData(guid, read_size, read_download_data_callback);
}

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void NWebImpl::UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateBrowserControlsState(constraints, current, animate);
}

void NWebImpl::UpdateBrowserControlsHeight(int height, bool animate) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->UpdateBrowserControlsHeight(height, animate);
}
#endif

bool NWebImpl::GetIsEditTextType() {
  auto hitTest = GetLastHitTestResult();
  if (!hitTest) {
    return false;
  }

  if (inputmethod_handler_ == nullptr) {
    LOG(ERROR) << "inputmethod_handler_ is nullptr.";
    return false;
  }

  if (hitTest->GetType() != HitTestResult::EDIT_TEXT_TYPE) {
    LOG(INFO) << "no hittest edit text, web close keyboard.";
    return false;
  }
  return true;
}

bool NWebImpl::NeedSoftKeyboard() {
  if (inputmethod_handler_) {
    if (inputmethod_handler_->GetIsEditableNode()) {
      return true;
    } else {
      return GetIsEditTextType();
    }
  }
}

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
void NWebImpl::Grant(NWebPermissionRequest* request, int32_t resourse_id) {
  if (!request) {
    return;
  }
  if (!request->access_request) {
    return;
  }
  request->access_request->Agree(resourse_id);
}

void NWebImpl::Deny(NWebPermissionRequest* request) {
  if (!request) {
    return;
  }
  if (!request->access_request) {
    return;
  }
  request->access_request->Refuse();
}

std::string NWebImpl::GetOrigin(NWebPermissionRequest* request) {
  if (!request) {
    return "";
  }
  if (!request->access_request) {
    return "";
  }
  return request->access_request->Origin();
}

int32_t NWebImpl::GetResourceId(NWebPermissionRequest* request) {
  if (!request) {
    return -1;
  }
  if (!request->access_request) {
    return -1;
  }
  return request->access_request->ResourceAcessId();
}

void NWebImpl::GrantV2(int32_t resourse_id, int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GrantV2, delegate is null";
    return;
  }
  nweb_delegate_->PermissionRequestGrant(resourse_id, nweb_request_key);
}

void NWebImpl::DenyV2(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to DenyV2, delegate is null";
    return;
  }
  nweb_delegate_->PermissionRequestDeny(nweb_request_key);
}

void NWebImpl::PermissionRequestDelete(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to PermissionRequestDelete, delegate is null";
    return;
  }
  nweb_delegate_->PermissionRequestDelete(nweb_request_key);
}

std::string NWebImpl::GetOriginV2(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetOriginV2, delegate is null";
    return std::string();
  }
  return nweb_delegate_->PermissionRequestGetOrigin(nweb_request_key);
}

int32_t NWebImpl::GetResourceIdV2(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetResourceIdV2, delegate is null";
    return -1;
  }
  return nweb_delegate_->PermissionRequestGetResourceId(nweb_request_key);
}
#endif  // BUILDFLAG(ARKWEB_EXT_PERMISSION)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
int NWebImpl::InsertBackForwardEntry(int offset, const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return NWebNavigationEntryUpdateResult::ERR_OTHER;
  }
  return nweb_delegate_->InsertBackForwardEntry(offset, url);
}

int NWebImpl::UpdateNavigationEntryUrl(int index, const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return NWebNavigationEntryUpdateResult::ERR_OTHER;
  }
  return nweb_delegate_->UpdateNavigationEntryUrl(index, url);
}

void NWebImpl::ClearForwardList() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ClearForwardList();
}
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

// static
std::shared_ptr<NWeb> NWebImpl::GetNWeb(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = OHOS::NWeb::g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    return it->second.lock();
  }

  return nullptr;
}

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
// static
void NWebImpl::SetWebTag(int32_t nweb_id, const char* web_tag) {
  OHOS::NWeb::NWebImpl* nweb = OHOS::NWeb::NWebImpl::FromID(nweb_id);
  OHOS::NWeb::ArkWebNativeObject::BindWebTagToWebInstance(nweb_id, web_tag);
  if (nweb) {
    nweb->SetWebTag(std::string(web_tag));
    return;
  }

  if (nweb_id == -1) {
    WVLOG_D("fail to find a valid nweb with id:%{public}d Tag:%{public}s",
            nweb_id, web_tag ? web_tag : " ");
  } else {
    WVLOG_E("fail to find a valid nweb with id:%{public}d Tag:%{public}s",
            nweb_id, web_tag ? web_tag : " ");
  }
}
#endif

// static
void NWebImpl::PrepareForPageLoad(const std::string& url,
                                  bool preconnectable,
                                  int32_t num_sockets) {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  WVLOG_I("PrepareForPageLoad start.");
  predictor::PreconnectUrlInfo preconnectUrlInfo;
  preconnectUrlInfo.url = url;
  preconnectUrlInfo.num_sockets = num_sockets;
  preconnectUrlInfo.is_preconnectable = preconnectable;
  predictor::PredictorDatabase::preconnect_url_info_list.emplace_back(
      preconnectUrlInfo);

  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    return;
  }

  CefBrowserContext* context = browser_context_all[0];
  content::BrowserContext* browser_context = context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrepareForPageLoad null browser_context");
    return;
  }

  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    return;
  }

  std::vector<predictor::PreconnectUrlInfo> preconnect_url_infos =
      std::move(predictor::PredictorDatabase::preconnect_url_info_list);
  for (auto& preconnect_url_info : preconnect_url_infos) {
    loading_predictor->PrepareForPageLoad(
        GURL(preconnect_url_info.url), ohos_predictors::HintOrigin::OMNIBOX,
        preconnect_url_info.is_preconnectable, preconnect_url_info.num_sockets);
  }
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
}

// static
void NWebImpl::RemoveAllCache(bool include_disk_files) {
  auto manager = web_cache::WebCacheManager::GetInstance();
  manager->ClearCache();

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    for (std::pair<const int, std::weak_ptr<NWebImpl>>& elem : g_nweb_map.Get()) {
      if (std::shared_ptr nweb_impl = elem.second.lock()) {
        nweb_impl->CancelAllPrerendering();
      }
    }
  }
#endif

  if (!include_disk_files) {
    WVLOG_I("no need remove all disk cache");
    return;
  }
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    return;
  }

  for (const auto& cef_browser_context : browser_context_all) {
    content::BrowserContext* browser_context =
        cef_browser_context->AsBrowserContext();
    if (!browser_context) {
      WVLOG_E("removeAllCache browser_context is null");
      return;
    }

    content::BrowsingDataRemover* remover =
        browser_context->GetBrowsingDataRemover();
    remover->Remove(
        base::Time(), base::Time::Max(),
        content::BrowsingDataRemover::DATA_TYPE_CACHE,
        content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB |
            content::BrowsingDataRemover::ORIGIN_TYPE_PROTECTED_WEB);
  }
  WVLOG_I("remove all of cache is successful");
}

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
bool NWebImpl::IsSafeBrowsingEnabled() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }

  return nweb_delegate_->IsSafeBrowsingEnabled();
}

void NWebImpl::EnableSafeBrowsing(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  return nweb_delegate_->EnableSafeBrowsing(enable);
}

void NWebImpl::EnableSafeBrowsingDetection(bool enable, bool strictMode) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->EnableSafeBrowsingDetection(enable, strictMode);
}

void NWebImpl::OnSafeBrowsingDetectionResult(int code,
                                             int policy,
                                             const std::string& mappingType,
                                             const std::string& url) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->OnSafeBrowsingDetectionResult(code, policy, mappingType,
                                                url);
}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

void NWebImpl::StartCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->StartCamera();
  }
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

void NWebImpl::StopCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->StopCamera();
  }
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

void NWebImpl::CloseCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  if (nweb_delegate_) {
    nweb_delegate_->CloseCamera();
  }
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

void NWebImpl::PauseAllTimers() {
#if BUILDFLAG(ARKWEB_SUSPEND_ALL_TIMERS)
  if (content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()) {
    content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()
        ->PauseWebKitShardTimersFromOHOS();
  } else {
    WVLOG_I("NWebImpl::PauseAllTimers content_view_ is nullptr");
  }
#endif
}

void NWebImpl::ResumeAllTimers() {
#if BUILDFLAG(ARKWEB_SUSPEND_ALL_TIMERS)
  if (content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()) {
    content::SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance()
        ->ResumeWebKitShardTimersFromOHOS();
  } else {
    WVLOG_I("NWebImpl::ResumeAllTimers content_view_ is nullptr");
  }
#endif
}

int NWebImpl::GetSecurityLevel() {
#if BUILDFLAG(IS_OHOS)
  if (nweb_delegate_ == nullptr) {
    return static_cast<int>(security_state::SecurityLevel::NONE);
  }

  return nweb_delegate_->GetSecurityLevel();
#else
  return static_cast<int>(security_state::SecurityLevel::NONE);
#endif
}

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
bool SetExceptionList(ContentSettingsType content_type,
                      const std::vector<std::string>& urls,
                      ContentSetting content_setting) {
  static constexpr unsigned int kMaxUrlExceptions = 10000;
  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    content::BrowserContext* browser_context =
        cef_browser_context->AsBrowserContext();
    if (!browser_context) {
      WVLOG_E("SetUrlExceptionList null browser_context");
      return false;
    }

    HostContentSettingsMap* host_content_settings_map =
        HostContentSettingsMapFactory::GetForProfile(browser_context);
    if (!host_content_settings_map) {
      WVLOG_E("SetUrlExceptionList null host_content_settings_map");
      return false;
    }

    host_content_settings_map->ClearSettingsForOneType(content_type);
    for (unsigned int i = 0; i < urls.size() && kMaxUrlExceptions; ++i) {
      host_content_settings_map->SetContentSettingCustomScope(
          ContentSettingsPattern::FromString(urls[i]),
          ContentSettingsPattern::Wildcard(), content_type, content_setting);
    }

    if (content_type == ContentSettingsType::COOKIES) {
      ContentSettingsForOneType cookies_settings =
          host_content_settings_map->GetSettingsForOneType(content_type);
      browser_context->ForEachLoadedStoragePartition(
          [&](content::StoragePartition* storage_partition) {
            storage_partition->GetCookieManagerForBrowserProcess()
                ->SetContentSettings(ContentSettingsType::COOKIES,
                                     std::move(cookies_settings),
                                     base::NullCallback());
          });
    }
  }

  LOG(INFO) << "SetUrlExceptionList Add "
            << (content_setting == CONTENT_SETTING_BLOCK ? "Block" : "Allow")
            << " exception for content type: " << int32_t(content_type)
            << " with " << urls.size();

  return true;
}

// static
bool NWebImpl::SetExceptionListForJavaScriptEnabled(
    const std::vector<std::string>& urls,
    bool isEnabled) {
  ContentSetting content_setting =
      isEnabled ? CONTENT_SETTING_ALLOW : CONTENT_SETTING_BLOCK;
  return SetExceptionList(ContentSettingsType::JAVASCRIPT, urls,
                          content_setting);
}

// static
bool NWebImpl::SetExceptionListForAcceptCookie(
    const std::vector<std::string>& urls,
    bool isEnabled) {
  ContentSetting content_setting =
      isEnabled ? CONTENT_SETTING_ALLOW : CONTENT_SETTING_BLOCK;
  return SetExceptionList(ContentSettingsType::COOKIES, urls, content_setting);
}
#endif

#if BUILDFLAG(ARKWEB_ITP)
void NWebImpl::EnableIntelligentTrackingPrevention(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  return nweb_delegate_->EnableIntelligentTrackingPrevention(enable);
}

bool NWebImpl::IsIntelligentTrackingPreventionEnabled() const {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->IsIntelligentTrackingPreventionEnabled();
}

// static
bool NWebImpl::IsAnyNWebIntelligentTrackingPreventionEnabled() {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = g_nweb_map.Pointer();
  for (auto it = map->begin(); it != map->end(); it++) {
    auto nweb_weak_ptr = it->second.lock();
    if (nweb_weak_ptr) {
      auto nweb = nweb_weak_ptr.get();
      if (nweb && nweb->IsIntelligentTrackingPreventionEnabled()) {
        return true;
      }
    }
  }
  return false;
}
#endif

std::string NWebImpl::GetSelectInfo() {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  if (inputmethod_handler_ && !inputmethod_handler_->GetSelectInfo().empty()) {
    return inputmethod_handler_->GetSelectInfo();
  }
#endif

#if BUILDFLAG(ARKWEB_AI)
  if (nweb_delegate_) {
    auto ai_select_text = nweb_delegate_->GetDataDetectorSelectText();
    if (!ai_select_text.empty()) {
      return ai_select_text;
    }
  }
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kEnableNwebExFreeCopy)) {
    return GetSelectedTextFromContextParam();
  }
#endif
  return std::string();
}

void NWebImpl::OnCreateNativeMediaPlayer(
    std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set create custome media player callback failed, nweb delegate is "
        "nullptr, nweb_id = %{public}u",
        nweb_id_);
    return;
  }

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  nweb_delegate_->RegisterOnCreateNativeMediaPlayerListener(
      std::move(callback));
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}

#if BUILDFLAG(ARKWEB_ITP)
// static
void NWebImpl::AddIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()
      ->AddITPBypassingList(hosts);
}

// static
void NWebImpl::RemoveIntelligentTrackingPreventionBypassingList(
    const std::vector<std::string>& hosts) {
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()
      ->RemoveITPBypassingList(hosts);
}

// static
void NWebImpl::ClearIntelligentTrackingPreventionBypassingList() {
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()
      ->ClearITPBypassingList();
}
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
// static
std::string NWebImpl::GetDefaultUserAgent() {
  return embedder_support::GetUserAgent();
}
#endif

std::string NWebImpl::GetLastJavascriptProxyCallingFrameUrl() {
#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  if (nweb_delegate_ == nullptr) {
    return "";
  }

  return nweb_delegate_->GetLastJavascriptProxyCallingFrameUrl();
#else
  return "";
#endif
}

int NWebImpl::ScaleGestureChange(double scale, double centerX, double centerY) {
  LOG(DEBUG) << "NWebImpl::ScaleGestureChange scale:" << scale
             << " centerX: " << centerX << " centerY: " << centerY;
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return NWEB_ERR;
  }
  return nweb_delegate_->ScaleGestureChange(scale, centerX, centerY);
}

int NWebImpl::ScaleGestureChangeV2(int type,
                                   double scale,
                                   double originScale,
                                   double centerX,
                                   double centerY) {
  LOG(DEBUG) << "NWebImpl::ScaleGestureChangeV2 type:" << type
             << " scale:" << scale << " originScale: " << originScale
             << " centerX: " << centerX << " centerY: " << centerY;
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "nweb_delegate_ is nullptr.";
    return NWEB_ERR;
  }
  return nweb_delegate_->ScaleGestureChangeV2(type, scale, originScale, centerX,
                                              centerY);
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
// static
void NWebImpl::SetRenderProcessMode(RenderProcessMode mode) {
  LOG(INFO) << "SetRenderProcessMode mode:" << (int)mode;
  content::RenderProcessHost::SetRenderProcessMode(
      static_cast<content::RenderProcessMode>(mode));

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
  g_siteIsolationMode = ShouldEnableSiteIsolation();
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportSiteIsolationMode(std::to_string(g_siteIsolationMode));
#endif
#endif
}

// static
RenderProcessMode NWebImpl::GetRenderProcessMode() {
  return static_cast<RenderProcessMode>(
      content::RenderProcessHost::render_process_mode());
}
#endif  // ARKWEB_RENDER_PROCESS_MODE

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
bool NWebImpl::GetSiteIsolationModeResult() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (!command_line) {
    std::string isSiteIsolationMode = GetSiteIsolationMode();

    if (isSiteIsolationMode == "false") {
      return false;
    }
    bool isMultipleRenderProcess = OHOS::NWeb::NWebImpl::GetRenderProcessMode() ==
                   OHOS::NWeb::RenderProcessMode::MULTIPLE_MODE;

    LOG(INFO) << "isMultipleRenderProcess:" << isMultipleRenderProcess;

    if (g_siteIsolationModeInit) {
      if (g_siteIsolationModeInitValue == SiteIsolationInitMode::STRICT && !isMultipleRenderProcess){
        LOG(ERROR) << "Site isolation mode cannot be strict when single render";
      } else {
        return g_siteIsolationModeInitValue == SiteIsolationInitMode::STRICT;
      }
    }

    // for judge PC&&Tablet devices
    bool isIgnoreLockdownMode = base::ohos::IsTabletDevice() || base::ohos::IsPcDevice();

    if (isIgnoreLockdownMode && isMultipleRenderProcess) {
      return true;
    }
    if (IsAdvancedSecurityMode() && isMultipleRenderProcess) {
      return true;
    }
    return false;
  } else {
    return ShouldEnableSiteIsolation();
  }
}

static void ApplySiteIsolationMode(bool mode){
  std::string rootMode = GetSiteIsolationMode();
  if (rootMode == "false") {
    g_siteIsolationMode = false;
    LOG(WARNING) << "mode forced site isolation to false";
  } else {
    g_siteIsolationMode = mode;
  }
  g_siteIsolationModeInit = true;

  OHOS::NWeb::ResSchedClientAdapter::ReportSiteIsolationMode(
      g_siteIsolationMode);
  ReportSiteIsolationMode(std::to_string(g_siteIsolationMode));
  g_siteIsolationModeInitValue =
    (g_siteIsolationMode == true) ? SiteIsolationInitMode::STRICT : SiteIsolationInitMode::PARTIAL;
  LOG(INFO) << "Final site isolation mode set to:" << g_siteIsolationMode;
}

int32_t NWebImpl::SetSiteIsolationMode(bool mode) {
  LOG(INFO) << "NWeb Impl SetSiteIsolationMode request:" << mode;

  if (g_siteIsolationModeInit) {
      LOG(WARNING) << "Site isolation mode already set by developer";
      return ALREADY_SET_ERR;
  }

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  bool isMultipleRenderProcess = OHOS::NWeb::NWebImpl::GetRenderProcessMode() ==
                OHOS::NWeb::RenderProcessMode::MULTIPLE_MODE;

  if (mode && ((command_line && !IsMultipleRenderProcess()) || (!command_line && !isMultipleRenderProcess))) {
      LOG(WARNING) << "Site isolation mode cannot be strict when single render";
      return SINGLE_RENDER_SET_STRICT_ERR;
  }

  if (IsAdvancedSecurityMode()) {
      LOG(WARNING) << "Cannot change (AdvancedSecurityMode Active)";
      return ADVANCED_SECURITY_SET_ERR;
  }
  ApplySiteIsolationMode(mode);
  return SITE_ISOLATION_SET_SUCCESS;
}

#endif  // ARKWEB_SITE_ISOLATION

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
// static
void NWebImpl::SetWholeWebDrawing() {
  set_whole_page_drawing = true;
}
#endif

// static
void NWebImpl::SetMigrationPasswordReady(const bool migrationReady) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (!base::CommandLine::ForCurrentProcess()) {
    LOG(ERROR) << "[Autofill] InitializeWebEngine is not init.";
    return;
  }
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebExPassword)) {
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsReady, migrationReady);
    g_browser_process->local_state()->CommitPendingWrite();
    LOG(INFO) << "[Autofill] Migrate Passwords Ready:" << migrationReady;
    if (migrationReady == true) {
      MigratePasswordsToPasswordVault();
    }
  }
#endif
}

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
// static
void NWebImpl::UpdateAngleConfig(bool angle_switch) {
  g_angle_config = angle_switch;
}
#endif

#if BUILDFLAG(ARKWEB_GPU)
// static
void NWebImpl::UpdateGpuConfig(bool gpu_switch) {
  const char* switch_status = gpu_switch ? "ON" : "OFF";
  LOG(INFO) << "NWebImpl::SetOopGpuMasterSwitch: " << switch_status;
  if (gpu_switch) {
    oop_gpu_switch = GpuMode::FORCE_OOP;
  } else {
    oop_gpu_switch = GpuMode::FORCE_IN_PROCESS;
  }
}
#endif // BUILDFLAG(ARKWEB_GPU)

}  // namespace OHOS::NWeb

using namespace OHOS::NWeb;

extern "C" OHOS_NWEB_EXPORT void WebDownloadManager_PutDownloadCallback(
    NWebDownloadDelegateCallback* callback) {
  if (!callback) {
    WVLOG_E("invalid callback");
    return;
  }
  WVLOG_I("[WebDownloadManager] put download callback.");
  CefRefPtr<NWebDownloadHandlerDelegate> delegate =
      new NWebDownloadHandlerDelegate(nullptr);
  delegate->RegisterWebDownloadDelegateListener(
      std::make_shared<NWebDownloadDelegateCallback>(*callback));
  CefSetDownloadHandler(delegate);
}

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
void NWebImpl::PrefetchResource(
    const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
    const std::map<std::string, std::string>& additional_http_headers,
    const std::string& cache_key,
    const uint32_t& cache_valid_time) {
  auto it = additional_http_headers.find("Content-Type");
  if (it == additional_http_headers.end() ||
      it->second != "application/x-www-form-urlencoded") {
    WVLOG_E(
        "PrefetchResource post request has no content-type or it is not "
        "supported.");
    return;
  }

  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("PrefetchResource has no browser_context");
    return;
  }

  CefBrowserContext* context = browser_context_all[0];
  content::BrowserContext* browser_context = context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrefetchResource null browser_context");
    return;
  }
  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    WVLOG_E("PrefetchResource no load predictor");
    return;
  }
  std::shared_ptr<ohos_predictors::PreRequestInfo> request_info =
      std::make_shared<ohos_predictors::PreRequestInfo>();
  request_info->url = GURL(pre_args->GetUrl());
  request_info->method = pre_args->GetMethod();
  request_info->request_body = pre_args->GetFormData();

  loading_predictor->PrefetchResource(request_info, additional_http_headers,
                                      cache_key, cache_valid_time);
}

void NWebImpl::ClearPrefetchedResource(
    const std::vector<std::string>& cache_key_list) {
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("PrefetchResource has no browser_context");
    return;
  }

  CefBrowserContext* context = browser_context_all[0];
  content::BrowserContext* browser_context = context->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("PrefetchResource null browser_context");
    return;
  }
  ohos_predictors::LoadingPredictor* loading_predictor =
      ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
          browser_context);
  if (!loading_predictor) {
    WVLOG_E("PrefetchResource no load predictor");
    return;
  }
  loading_predictor->ClearPrefetchedResource(cache_key_list);
}
#endif

// static
void NWebImpl::WarmupServiceWorker(const std::string& url) {
#if BUILDFLAG(ARKWEB_WARMUP_SERVICEWORKER)
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    WVLOG_E("WarmupServiceWorker has no browser_context.");
    return;
  }

  content::BrowserContext* browser_context =
      browser_context_all[0]->AsBrowserContext();
  if (!browser_context) {
    WVLOG_E("WarmupServiceWorker has null browser_context.");
    return;
  }

  content::StoragePartition* storage_partition =
      browser_context->GetDefaultStoragePartition();
  if (!storage_partition) {
    WVLOG_E("WarmupServiceWorker has null storage_partition.");
    return;
  }

  content::ServiceWorkerContext* service_worker_context =
      storage_partition->GetServiceWorkerContext();
  if (!service_worker_context) {
    WVLOG_E("WarmupServiceWorker has null service_worker_context.");
    return;
  }

  if (!content::OriginCanAccessServiceWorkers(GURL(url))) {
    WVLOG_E("Input url can not access service worker.");
    return;
  }

  const blink::StorageKey key =
      blink::StorageKey::CreateFirstParty(url::Origin::Create(GURL(url)));
  if (!service_worker_context->MaybeHasRegistrationForStorageKey(key)) {
    WVLOG_E("WarmupServiceWorker has no registration for storage key.");
    return;
  }

  WVLOG_I("Start to warm up service worker.");
  service_worker_context->WarmUpServiceWorker(GURL(url), key,
                                              base::DoNothing());
#endif  // BUILDFLAG(ARKWEB_WARMUP_SERVICEWORKER)
}

// static
void NWebImpl::SetHostIP(const std::string& hostName,
                         const std::string& address,
                         int32_t aliveTime) {
#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  net_service::NetHelpers::SetHostIP(hostName, address, aliveTime);
  auto it = net_service::NetHelpers::GetHostIP(hostName);
  if (it.size() == 0) {
    WVLOG_E("fail to set host IP.");
    return;
  }

  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    if (!cef_browser_context) {
      WVLOG_E("SetHostIP null browser_context");
      return;
    }
    cef_browser_context->GetNetworkContext()->SetHostIP(hostName, it,
                                                        aliveTime);
  }
  WVLOG_I("Set host IP successfully.");
#endif
}

// static
void NWebImpl::ClearHostIP(const std::string& hostName) {
#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  auto it = net_service::NetHelpers::GetHostIP(hostName);
  if (it.size() == 0) {
    WVLOG_E("NWeb has not set the host IP.");
    return;
  }

  for (const auto& cef_browser_context : CefBrowserContext::GetAll()) {
    if (!cef_browser_context) {
      WVLOG_E("ClearHostIP null browser_context");
      return;
    }
    cef_browser_context->GetNetworkContext()->ClearHostIP(hostName);
  }
  net_service::NetHelpers::ClearHostIP(hostName);
  WVLOG_I("Clear host IP successfully.");
#endif
}

#if BUILDFLAG(ARKWEB_USERAGENT)
void NWebImpl::SetAppCustomUserAgent(const std::string& user_agent) {
  AlloyBrowserUAConfig::GetInstance()->SetAppCustomUserAgent(user_agent);
}

void NWebImpl::SetUserAgentForHosts(const std::string& user_agent,
                                    const std::vector<std::string>& hosts) {
  AlloyBrowserUAConfig::GetInstance()->SetUserAgentForHosts(user_agent, hosts);
}

bool NWebImpl::GetUserAgentClientHintsEnabled() {
  return AlloyBrowserUAConfig::GetInstance()->GetUserAgentClientHintsEnabled();
}

void NWebImpl::SetUserAgentClientHintsEnabled(bool enabled) {
  AlloyBrowserUAConfig::GetInstance()->SetUserAgentClientHintsEnabled(enabled);
}

void NWebImpl::SetUserAgentMetadata(
    const std::string& user_agent,
    std::shared_ptr<NWebUserAgentMetadata> metadata) {
  if (!nweb_delegate_) {
    LOG(WARNING) << kUserAgentMetadataTag
                 << " SetUserAgentMetadata failed, no nweb or delegate";
    return;
  }
  nweb_delegate_->SetUserAgentMetadata(user_agent, metadata);
}

std::shared_ptr<NWebUserAgentMetadata> NWebImpl::GetUserAgentMetadata(
    const std::string& user_agent) {
  if (!nweb_delegate_) {
    LOG(WARNING) << kUserAgentMetadataTag
                 << " GetUserAgentMetadata failed, no nweb or delegate";
    return nullptr;
  }
  return nweb_delegate_->GetUserAgentMetadata(user_agent);
}

#endif

void NWebImpl::SuggestionSelected(int index) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->SuggestionSelected(index);
}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void NWebImpl::PutSpanstringConvertHtmlCallback(
    std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback) {
  ui::ClipboardOHOS::SetConvertHtmlCallback(callback);
  if (GetPreference()) {
    ui::ClipboardOHOS::RegisterCopyOptionCb(base::BindRepeating(
        &NWebPreference::GetCopyOptionMode, GetPreference()));
  }
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_AI)
void NWebImpl::OnTextSelected() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnTextSelected();
}

void NWebImpl::OnDestroyImageAnalyzerOverlay() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnDestroyImageAnalyzerOverlay();
}

void NWebImpl::RunDataDetectorJS() {
  LOG(DEBUG) << "NWebImpl::RunDataDetectorJS";
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->RunDataDetectorJS();
}

void NWebImpl::SetDataDetectorEnable(bool enable) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetDataDetectorEnable(enable);
}

void NWebImpl::OnDataDetectorSelectText() {
  LOG(DEBUG) << "NWebImpl::OnDataDetectorSelectText";
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnDataDetectorSelectText();
}

void NWebImpl::OnDataDetectorCopy(const std::vector<std::string>& recordMix) {
  LOG(DEBUG) << "NWebImpl::OnDataDetectorCopy";
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnDataDetectorCopy(recordMix);
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void NWebImpl::OnSafeInsetsChange(int left, int top, int right, int bottom) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "OnSafeInsetsChange failed, nweb delegate is nullptr, nweb_id = "
        "%{public}u",
        nweb_id_);
    return;
  }
  WVLOG_D(
      "OnSafeInsetsChange nweb_id:%{public}u "
      "%{public}d,%{public}d,%{public}d,%{public}d",
      nweb_id_, left, top, right, bottom);
  nweb_delegate_->OnSafeInsetsChange(left, top, right, bottom);
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool NWebImpl::WebPageSnapshot(const char* id,
                               PixelUnit type,
                               int width,
                               int height,
                               const WebSnapshotCallback callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("WebPageSnapshot failed, nweb delegate is nullptr");
    return false;
  }
  return nweb_delegate_->WebPageSnapshot(id, type, width, height, callback);
}

bool NWebImpl::WebPageSnapshotV2(const char* id,
                                 PixelUnit type,
                                 int width,
                                 int height,
                                 std::shared_ptr<NWebSnapshotCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("WebPageSnapshotV2 failed, nweb delegate is nullptr");
    return false;
  }
  return nweb_delegate_->WebPageSnapshot(
    id, type, width, height,
    [callback](const char* id, bool state, float radio, void* data, int width,
               int height) {
      WVLOG_I("WebPageSnapshotV2 result return to OH");
      callback->OnSnapshotResult(id, state, radio, data, width, height);
    });
}
#endif

#ifdef BUILDFLAG(IS_OHOS)
void NWebImpl::OnConfigurationUpdated(
    std::shared_ptr<NWebSystemConfiguration> configuration) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_I("NWebImpl::OnConfigurationUpdated nweb_delegate_ is nullptr");
    return;
  }
  if (configuration->GetThemeFlags() &
      static_cast<uint8_t>(SystemThemeFlags::THEME_FONT)) {
#if BUILDFLAG(ARKWEB_THEME_FONT)
    for (content::RenderProcessHost::iterator host_iterator =
             content::RenderProcessHost::AllHostsIterator();
         !host_iterator.IsAtEnd(); host_iterator.Advance()) {
      content::RenderProcessHost* host = host_iterator.GetCurrentValue();
      if (host && host->IsInitializedAndNotDead()) {
        host->OnThemeFontChange();
      }
    }
#endif  // ARKWEB_THEME_FONT
  }
}
#endif  //  IS_OHOS

int NWebImpl::SetUrlTrustList(const std::string& urlTrustList) {
  return 0;
}

int NWebImpl::SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
                                        std::string& detailErrMsg) {
#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  if (nweb_delegate_ == nullptr) {
    return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
  }

  return nweb_delegate_->SetUrlTrustListWithErrMsg(urlTrustList,
    true, false, detailErrMsg);
#else
  return -1;
#endif
}

int NWebImpl::SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
    bool allowOpaqueOrigin, bool supportWildcard, std::string& detailErrMsg) {
#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  if (nweb_delegate_ == nullptr) {
    return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
  }

  return nweb_delegate_->SetUrlTrustListWithErrMsg(urlTrustList,
    allowOpaqueOrigin, supportWildcard, detailErrMsg);
#else
  return -1;
#endif
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebImpl::SetPathAllowingUniversalAccess(
    const std::vector<std::string>& pathList,
    const std::vector<std::string>& moduleName,
    std::string& errorPath) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  if (pathList.empty()) {
    LOG(INFO) << "SetPathAllowingUniversalAccess empty";
    nweb_delegate_->SetPathAllowingUniversalAccess(pathList, {});
    return;
  }

  std::map<PathType, std::vector<base::FilePath>> path_lists;
  SetPathList(moduleName, path_lists);

  std::map<PathType, std::vector<base::FilePath>> excluded_path_list;
  SetExcludedPathList(moduleName, excluded_path_list);

  std::vector<std::string> excludedPathList;
  GetExcludedPathList(excluded_path_list, excludedPathList);

  for (auto& p : pathList) {
    base::FilePath path(p);
    auto real_path =
        base::MakeAbsoluteFilePathNoResolveSymbolicLinks(path).value_or(
            base::FilePath());
    if (real_path.empty()) {
      errorPath = p;
      return;
    }

    if (CheckRealPath(real_path, path_lists[NWebImpl::PathType::kDirResource], true) ||
        CheckRealPath(real_path, path_lists[NWebImpl::PathType::kDirFile], false) ||
        CheckRealPath(real_path, path_lists[NWebImpl::PathType::kDirCache], true,
                      excluded_path_list[NWebImpl::PathType::kDirCache]) ||
        CheckRealPath(real_path, path_lists[NWebImpl::PathType::kDirTemp], true)) {
      continue;
    }

    errorPath = p;
    return;
  }
  nweb_delegate_->SetPathAllowingUniversalAccess(pathList, excludedPathList);
}

int NWebImpl::PrerenderPage(const std::string& url,
                            const std::string& additional_headers) {
  if (nweb_delegate_ == nullptr) {
    return ARKWEB_INIT_ERROR;
  }
  return nweb_delegate_->PrerenderPage(url, additional_headers);
}

void NWebImpl::SetExtraHeadersMap(const std::string& url,
                                  const std::string& additional_headers) {
  throttle::ExtraHeadersThrottle::SetExtraHeaders(GURL(url), additional_headers);
}

void NWebImpl::CancelAllPrerendering() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->CancelAllPrerendering();
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
void NWebImpl::DisallowSandboxFileAccessFromFileUrl(bool disallow) const {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->DisallowSandboxFileAccessFromFileUrl(disallow);
}
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

void NWebImpl::PerformAction(
    int64_t accessibilityId,
    uint32_t action,
    const std::map<std::string, std::string>& actionArguments) {
  // Deprecated due to new accessibility architecture
}

bool NWebImpl::PerformActionV2(
    int64_t accessibilityId,
    uint32_t action,
    const std::map<std::string, std::string>& actionArguments) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    return nweb_delegate_->ExecuteAction(accessibilityId, action, actionArguments);
  }
#endif
  return false;
}

void NWebImpl::SendAccessibilityHoverEvent(int32_t x, int32_t y) {
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SendAccessibilityHoverEvent(x, y, true);
  }
}

void NWebImpl::SendAccessibilityHoverEventV2(int32_t x,
                                             int32_t y,
                                             bool isHoverEnter) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SendAccessibilityHoverEvent(x, y, isHoverEnter);
  }
#endif
}

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
void NWebImpl::EnableMixedContentAutoUpgrades(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "EnableMixedContentAutoUpgrades failed,"
                  "for nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->EnableMixedContentAutoUpgrades(enable);
}

bool NWebImpl::IsMixedContentAutoUpgradesEnabled() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "IsMixedContentAutoUpgradesEnabled failed"
                  "for nweb_delegate_ is nullptr.";
    return false;
  }

  return nweb_delegate_->IsMixedContentAutoUpgradesEnabled();
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void NWebImpl::SetUsageScenario(int32_t usage_scenario) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "SetUsageScenario failed"
                  "for nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->SetUsageScenario(usage_scenario);
}

int32_t NWebImpl::GetUsageScenario() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "GetUsageScenario failed"
                  "for nweb_delegate_ is nullptr.";
    return 0;
  }
  return nweb_delegate_->GetUsageScenario();
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void NWebImpl::WebExtensionTabRemoved(int tab_id, bool isWindowClosing, int windowId) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabRemoved(tab_id, isWindowClosing, windowId);
}

void NWebImpl::WebExtensionTabUpdated(
    int tab_id,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
    std::unique_ptr<NWebExtensionTab> tab) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabUpdated(tab_id, std::move(changeInfo), std::move(tab));
}

void NWebImpl::WebExtensionTabAttached(
    int tab_id,
    std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabAttached(tab_id, std::move(attachInfo));
}

void NWebImpl::WebExtensionTabDetached(
    int tab_id,
    std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabDetached(tab_id, std::move(detachInfo));
}

void NWebImpl::WebExtensionTabMoved(
    int32_t tab_id,
    std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabMoved(tab_id, std::move(moveInfo));
}

void NWebImpl::WebExtensionTabReplaced(int32_t addedTabId,
                                       int32_t removedTabId) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionTabReplaced(addedTabId, removedTabId);
}

void NWebImpl::WebExtensionSetViewType(int32_t type) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebExtensionSetViewType(type);
}
#endif  // ARKWEB_ARKWEB_EXTENSIONS

void NWebImpl::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) {
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "fail to set back forward cache options. nweb_delegate is nullptr.");
    return;
  }

  nweb_delegate_->SetBackForwardCacheOptions(size, timeToLive);
#endif
}

void NWebImpl::SetMediaResumeFromBFCachePage(bool resume) {
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("fail to set media resume from bfcache page. nweb_delegate is nullptr.");
    return;
  }
  WVLOG_I("NWebImpl SetMediaResumeFromBFCachePage value: %{public}d.", resume);
  nweb_delegate_->SetMediaResumeFromBFCachePage(resume);
#endif // BUILDFLAG(ARKWEB_BFCACHE)
}

#if BUILDFLAG(IS_ARKWEB)
void NWebImpl::EnableAppLinking(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "EnableAppLinking failed"
                  "for nweb_delegate_ is nullptr.";
    return;
  }
  nweb_delegate_->EnableAppLinking(enable);
}
#endif // BUILDFLAG(IS_ARKWEB)

void NWebImpl::TrimMemoryByPressureLevel(int32_t memoryLevel) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;
  static constexpr int32_t kMemoryLevelModerate = 0;
  static constexpr base::TimeDelta kNotifyGapTime = base::Seconds(3);
  static MemoryPressureLevel last_memory_level =
      MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE;
  static base::Time last_notify_time;

  base::Time now = base::Time::Now();
  MemoryPressureLevel memory_pressure_level;
  if (memoryLevel == kMemoryLevelModerate) {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE;
  } else {
    memory_pressure_level = MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL;
  }

  if (memory_pressure_level == last_memory_level &&
      (now - last_notify_time < kNotifyGapTime)) {
    LOG(INFO) << "The same memory level has been notified within three seconds";
    return;
  }
  last_memory_level = memory_pressure_level;
  last_notify_time = std::move(now);

  LOG(INFO) << "NWebImpl::NotifyMemoryLevel "
            << (memoryLevel == kMemoryLevelModerate
                    ? "MEMORY_PRESSURE_LEVEL_MODERATE"
                    : "MEMORY_PRESSURE_LEVEL_CRITICAL");
  base::MemoryPressureListener::NotifyMemoryPressure(memory_pressure_level);
#endif  // ARKWEB_PERFORMANCE_MEMORY_THRESHOLD
}

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
void NWebImpl::EnableMediaNetworkTrafficPrompt(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "EnableMediaNetworkTrafficPrompt failed, no nweb_delegate_"
               << ", nweb_id_[" << nweb_id_ << "]";
    return;
  }
  LOG(INFO) << "NWebImpl::EnableMediaNetworkTrafficPrompt(" << enable
            << "), nweb_id_[" << nweb_id_ << "]";
  nweb_delegate_->EnableMediaNetworkTrafficPrompt(enable);
}
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(IS_ARKWEB)
void NWebImpl::SetSurfaceDensity(const double& density) {
  device_pixel_ratio_ = density;
  if (!inputmethod_handler_) {
    WVLOG_E("inputmethod_handler_ is nullptr");
    return;
  }
  inputmethod_handler_->SetVirtualDeviceRatio(device_pixel_ratio_);
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetVirtualDeviceRatio failed,nweb_delegate is nullptr.");
    return;
  }
  nweb_delegate_->SetSurfaceDensity(density);
}
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
void NWebImpl::getTotalSize(float size) {
  totalSize_ = size;
}

float NWebImpl::DumpGpuInfo() {
  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  host->gpu_service()->DumpGpuInfo(
      base::BindOnce(&NWebImpl::getTotalSize, weak_factory_.GetWeakPtr()));
  return totalSize_;
}
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void NWebImpl::SetTransformHint(uint32_t rotation) {
  if (nweb_delegate_) {
    nweb_delegate_->SetTransformHint(rotation);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void NWebImpl::SetPopupSurface(void* popupSurface) {
  uint32_t width, height;
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetPopupSurface failed,nweb_delegate is nullptr.");
    return;
  }

  if (output_handler_ == nullptr) {
    WVLOG_E("SetPopupSurface failed, NWeb output handler is not ready");
    return;
  }
  output_handler_->GetWindowInfo(width, height);
  void* popup_window = nullptr;
  popup_window = output_handler_->GetNativeWindowFromSurface(popupSurface);

  int32_t ret = OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
                    .NativeWindowSetBufferGeometry(
                        reinterpret_cast<void*>(popup_window), width, height);
  if (ret == OHOS::NWeb::GSErrorCode::GSERROR_OK) {
    WVLOG_I("popup window opt for emulator in init, result = %{public}d", ret);
  } else {
    WVLOG_W("popup window opt for emulator in init failed, result = %{public}d",
            ret);
  }
  nweb_delegate_->SetPopupSurface(popup_window);
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void NWebImpl::MaximizeResize() {
  if (nweb_delegate_) {
    nweb_delegate_->MaximizeResize();
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
void NWebImpl::DragResize(uint32_t width,
                          uint32_t height,
                          uint32_t pre_height,
                          uint32_t pre_width) {
  LOG(DEBUG) << "===== start drag resize =====";
  bool drag_bigger_height = false;
  bool drag_bigger_width = false;
  if (input_handler_ == nullptr || output_handler_ == nullptr) {
    return;
  }
  LOG(DEBUG) << "====== drag resize orign width ======= " << width
             << "=== drag resize orign height ===" << height;
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetDragResizeStart(true);
  if (pre_height > 0) {
    drag_bigger_height = true;
  }
  if (pre_width > 0) {
    drag_bigger_width = true;
  }
  if (drag_bigger_height) {
    height = static_cast<uint32_t>(OHOS::NWeb::NWebResizeHelper::GetInstance().GetResizeAdjustValue(
        height, pre_height, true));
  }
  if (drag_bigger_width) {
    width = static_cast<uint32_t>(OHOS::NWeb::NWebResizeHelper::GetInstance().GetResizeAdjustValue(
        width, pre_width, false));
  }
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetResizeHeightAndWidth(height,
                                                                      width);
  LOG(DEBUG) << "====== drag resize real width ======= " << width
             << "=== drag resize real height ===" << height;
  if (width > kSurfaceMaxWidth || height > kSurfaceMaxHeight) {
    if (draw_mode_ == 0) {
      OHOS::NWeb::NWebResizeHelper::GetInstance().RefreshParam();
      WVLOG_E("size too large in surface mode (%{public}u , %{public}u)", width,
              height);
      return;
    };
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("resize failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return;
  }
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(SOC_PERF_WEB_DRAG_RESIZE_ID, true);
  nweb_delegate_->Resize(width, height, false);
  output_handler_->Resize(width, height);
}

bool NWebImpl::IsNWebEx() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx)) {
    LOG(DEBUG) << "====== IsNWebEx is true ======= ";
    return true;
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void NWebImpl::StopScreenCapture(int32_t nweb_id, const char* session_id) {
  if (session_id == nullptr) {
    WVLOG_E("StopScreenCapture session_id is null");
    return;
  }
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("StopScreenCapture nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->StopScreenCapture(nweb_id, session_id);
}

void NWebImpl::SetScreenCapturePickerShow() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("SetScreenCapturePickerShow nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->SetScreenCapturePickerShow();
}

void NWebImpl::DisableSessionReuse() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("DisableSessionReuse nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->DisableSessionReuse();
}

void NWebImpl::PutWebScreenCaptureDelegateCallback(
    std::shared_ptr<NWebScreenCaptureDelegateCallback> callback) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E(
        "set web screen capture delegate callback failed, nweb delegate is "
        "nullptr, "
        "nweb_id = %{public}u",
        nweb_id_);
    return;
  }

  nweb_delegate_->RegisterScreenCaptureDelegateListener(callback);
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
bool NWebImpl::NeedToFireBeforeUnloadOrUnloadEvents() {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->NeedToFireBeforeUnloadOrUnloadEvents();
}

void NWebImpl::DispatchBeforeUnload() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->DispatchBeforeUnload();
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

void NWebImpl::SetProxyOverride(
    const std::vector<std::string>& proxyUrls,
    const std::vector<std::string>& proxySchemeFilters,
    const std::vector<std::string>& bypassRules,
    const bool& reverseBypass,
    std::shared_ptr<NWebProxyChangedCallback> callback) {
  std::vector<net::ProxyConfigServiceOHOS::ProxyOverrideRule> proxyRules;
  size_t size = proxySchemeFilters.size();
  DCHECK(proxySchemeFilters.size() == proxyUrls.size());
  proxyRules.reserve(size);
  for (size_t i = 0; i < size; i++) {
    proxyRules.emplace_back(proxySchemeFilters[i], proxyUrls[i]);
  }
  NWEB::ProxyConfigMonitor::GetInstance()->SetProxyOverride(proxyRules, bypassRules, reverseBypass,
      base::BindOnce([](std::shared_ptr<NWebProxyChangedCallback> napiCallback) {napiCallback->OnChanged();},
                     std::move(callback)));
}

void NWebImpl::RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback) {
  NWEB::ProxyConfigMonitor::GetInstance()->ClearProxyOverride(base::BindOnce(
        [](std::shared_ptr<NWebProxyChangedCallback> napiCallback) {napiCallback->OnChanged();},
          std::move(callback)));
}

#if BUILDFLAG(ARKWEB_EDM_POLICY)
// static
void NWebImpl::SetEnterprisePolicy(const std::string& policy, int version) {
  policy::BrowserPolicyHandler::GetInstance()->SetPolicyAndNotify(policy,
                                                                  version);
}
#endif

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
void NWebImpl::RegisterNativeJavaScriptProxy(const std::string& objName,
                                             const std::vector<std::string>& methodName,
                                             std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data,
                                             bool isAsync,
                                             const std::string& permission) {
  if (nweb_delegate_ != nullptr) {
    int len = data->GetSize();
    std::vector<NativeJSProxyCallbackFunc> callback;
    for (int i = 0; i < len; i++) {
      auto lambda = [number = i, calls = data](std::vector<std::vector<uint8_t>> param,
                                               std::vector<size_t> size) -> char* {
        std::vector<std::string> paramVector;
        for (auto paramArray: param) {
          std::string param_str = "";
          for (uint8_t param_int: paramArray) {
            param_str += static_cast<char>(static_cast<unsigned char>(param_int));
          }
          paramVector.push_back(param_str);
        }
        calls->OnHandle(number, paramVector);
        return nullptr;
      };
      callback.push_back(std::move(lambda));
    }
    LOG(INFO) << "NWebImpl::RegisterNativeJavaScriptProxy " << objName;
    nweb_delegate_->RegisterNativeJSProxy(objName, methodName, std::move(callback), isAsync, permission);
  } else {
    LOG(ERROR) << "nweb_delegate_ is nullptr";
  }
  return ;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool NWebImpl::SetFocusByPosition(float x, float y) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->SetFocusByPosition(x, y);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SOFTKEYBOARD_AVOID)
void NWebImpl::SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode) {
  keyboardBehaviorMode_ = mode;
  LOG(DEBUG)
      << "NWebImpl::SetSoftKeyboardBehaviorMode keyboardBehaviorMode_: "
      << static_cast<int>(keyboardBehaviorMode_);
}
#endif

#if BUILDFLAG(ARKWEB_AI_WRITE)
int NWebImpl::GetSelectStartIndex()
{
  if (inputmethod_handler_) {
    return inputmethod_handler_->GetSelectStartIndex();
  }
  return 0;
}

int NWebImpl::GetSelectEndIndex()
{
  if (inputmethod_handler_) {
    return inputmethod_handler_->GetSelectEndIndex();
  }
  return 0;
}

std::string NWebImpl::GetAllTextInfo()
{
  if (inputmethod_handler_ && !inputmethod_handler_->GetAllTextInfo().empty()) {
    return inputmethod_handler_->GetAllTextInfo();
  }
  return "";
}
#endif // ARKWEB_AI_WRITE
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
void NWebImpl::SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                                      double borderRadiusTopRight,
                                      double borderRadiusBottomLeft,
                                      double borderRadiusBottomRight) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SetBorderRadiusFromWeb(
      borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomLeft,
      borderRadiusBottomRight);
}
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PIP)
void NWebImpl::SetPipNativeWindow(int delegate_id,
                                  int child_id,
                                  int frame_routing_id,
                                  void* window) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SetPipNativeWindow(delegate_id, child_id,
                                       frame_routing_id, window);
  }
}

void NWebImpl::SendPipEvent(int delegate_id,
                            int child_id,
                            int frame_routing_id,
                            int event) {
  if (nweb_delegate_ != nullptr) {
    nweb_delegate_->SendPipEvent(delegate_id, child_id,
                                 frame_routing_id, event);
  }
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void NWebImpl::SetBlanklessLoadingKey(const std::string& key) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  blankless_key_ = base::ohos::BlanklessController::ConvertToBlanklessKey(key);
  nweb_delegate_->SetBlanklessLoadingKey(nweb_id_, blankless_key_);
}

void NWebImpl::SetPrivacyStatus(bool isPrivate) {
  is_private_ = isPrivate;
  if (isPrivate) {
    ClearBlanklessKey();
  }
}

int32_t NWebImpl::GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime) {
  MarkUserEnableBlankless();
  if (!base::ohos::BlanklessController::CheckGlobalProperty() ||
      !nweb_delegate_ || !similarity || !loadingTime || !CheckNetAvailable()) {
    if (similarity) {
      *similarity = 0;
    }
    if (loadingTime) {
      *loadingTime = 0;
    }
    return 0;  // SUCCESS
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  auto window_id = instance.GetWindowIdByNWebId(nweb_id_);
  uint64_t blankless_key = base::ohos::BlanklessController::ConvertToBlanklessKey(key);
  auto status_code = instance.RecordKey(nweb_id_, blankless_key);
  LOG(DEBUG) << "blankless GetBlanklessInfoWithKey nweb_id: " << nweb_id_
             << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code);
  auto& databaseInstance = base::ohos::BlanklessDataController::GetInstance();
  auto is_private = OHOS::NWeb::WindowManagerAdapterImpl::GetWindowPrivacyMode(window_id);
  if (is_private) {
    LOG(DEBUG) << "blankless this is a private window: "<< window_id;
    databaseInstance.ClearSnapshot(blankless_key);
    databaseInstance.ClearSnapshotDataItem({blankless_key});
  }
  if (status_code != base::ohos::BlanklessController::StatusCode::DUMPED ||
      databaseInstance.GetBlanklessLoadingCacheCapacity() == 0 || is_private) {
    *similarity = 0;
    *loadingTime = 0;
    LOG(DEBUG) << "blankless GetBlanklessInfoWithKey nweb_id: " << nweb_id_
               << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code)
               << ", capacity: " << databaseInstance.GetBlanklessLoadingCacheCapacity();
  } else {
    blankless_key_ = blankless_key;
    auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
    base::ohos::BlanklessController::GetInstance().RecordSystemTime(nweb_id_, blankless_key_, system_time);
    nweb_delegate_->SetBlanklessLoadingKey(nweb_id_, blankless_key);
    OHOS::NWeb::SnapshotDataItem dataItem = databaseInstance.GetSnapshotDataItem(blankless_key, GetPreferenceHash());
    *similarity = dataItem.historySimilarity;
    *loadingTime = dataItem.lcpTime;
    LOG(DEBUG) << "blankless GetBlanklessInfoWithKey nweb_id: " << nweb_id_
               << ", blankless_key: " << blankless_key << ", similarity: " << dataItem.historySimilarity
               << ", loadingTime: " << dataItem.lcpTime;
  }
  return 0;   // SUCCESS
}

int32_t NWebImpl::SetBlanklessLoadingWithKey(const std::string& key, bool isStart) {
  MarkUserEnableBlankless();
  if (!base::ohos::BlanklessController::CheckGlobalProperty() || !CheckNetAvailable()) {
    return -5;  // ERR_SIGNIFICANT_CHANGE
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  uint64_t blankless_key = base::ohos::BlanklessController::ConvertToBlanklessKey(key);
  auto status_code = instance.MatchKey(nweb_id_, blankless_key);
  LOG(DEBUG) << "blankless SetBlanklessLoadingWithKey nweb_id: " << nweb_id_
             << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code);
  if (status_code == base::ohos::BlanklessController::StatusCode::KEY_NOT_MATCH) {
    return -4;    // ERR_KEY_NOT_MATCH
  }
  auto& databaseInstance = base::ohos::BlanklessDataController::GetInstance();
  auto window_id = instance.GetWindowIdByNWebId(nweb_id_);
  auto is_private = OHOS::NWeb::WindowManagerAdapterImpl::GetWindowPrivacyMode(window_id);
  if (is_private) {
    LOG(DEBUG) << "blankless this is a private window: "<< window_id;
    databaseInstance.ClearSnapshot(blankless_key);
    databaseInstance.ClearSnapshotDataItem({blankless_key});
    return -5;
  }
  if (status_code != base::ohos::BlanklessController::StatusCode::INSERTED ||
      databaseInstance.GetBlanklessLoadingCacheCapacity() == 0) {
    LOG(DEBUG) << "blankless SetBlanklessLoadingWithKey nweb_id: " << nweb_id_
               << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code)
               << ", capacity: " << databaseInstance.GetBlanklessLoadingCacheCapacity()
               << ", isStart: " << isStart;
    return (isStart ? -5 : 0);  // ERR_SIGNIFICANT_CHANGE(true) or SUCCESS(false)
  }
  if (isStart) {
    OHOS::NWeb::SnapshotDataItem dataItem = databaseInstance.GetSnapshotDataItem(blankless_key, GetPreferenceHash());
    if (dataItem.historySimilarity < 0.33) {
      LOG(DEBUG) << "blankless SetBlanklessLoadingWithKey similarity < 0.33";
      return -5;    // ERR_SIGNIFICANT_CHANGE
    }
    CallBlanklessFrameFunc(blankless_key, dataItem);
  }
  return 0;   // SUCCESS
}

bool NWebImpl::TriggerBlanklessForUrl(const std::string& url) {
  if (!base::ohos::BlanklessController::CheckGlobalProperty() || !nweb_delegate_ ||
      !CheckNetAvailable() || IsUserEnableBlankless()) {
    return false;
  }
#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
  auto app_blankless_key = base::ohos::BlanklessController::INVALID_BLANKLESS_KEY;
  if (base::ohos::BlanklessController::GetInstance().CheckEnableForAppUrl(url, app_blankless_key)) {
    return ProcessBlanklessForUrl(app_blankless_key, true);
  }
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
  auto sys_blankless_key = base::ohos::BlanklessController::INVALID_BLANKLESS_KEY;
  if (base::ohos::BlanklessController::GetInstance().CheckEnableForSysUrl(url, sys_blankless_key)) {
    return ProcessBlanklessForUrl(sys_blankless_key);
  }
  return false;
}

bool NWebImpl::ProcessBlanklessForUrl(uint64_t blanklessKey, bool isAnime) {
  blankless_key_ = blanklessKey;
  auto& instance = base::ohos::BlanklessController::GetInstance();
  auto window_id = instance.GetWindowIdByNWebId(nweb_id_);
  auto& databaseInstance = base::ohos::BlanklessDataController::GetInstance();
  auto is_private = OHOS::NWeb::WindowManagerAdapterImpl::GetWindowPrivacyMode(window_id);
  if (is_private) {
    LOG(DEBUG) << "blankless this is a private window: "<< window_id;
    databaseInstance.ClearSnapshot(blankless_key_);
    databaseInstance.ClearSnapshotDataItem({blankless_key_});
    return false;
  }
  auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
  base::ohos::BlanklessController::GetInstance().RecordSystemTime(nweb_id_, blankless_key_, system_time);
  nweb_delegate_->SetBlanklessLoadingKey(nweb_id_, blankless_key_);
  OHOS::NWeb::SnapshotDataItem dataItem = databaseInstance.GetSnapshotDataItem(blankless_key_, GetPreferenceHash());
  CallBlanklessFrameFuncForWhiteList(blankless_key_, dataItem, isAnime);
  return true;
}

void NWebImpl::SetVisibility(bool isVisible) {
  if (!base::ohos::BlanklessController::CheckGlobalProperty()) {
    return;
  }
  is_visible_ = isVisible;
  if (!isVisible || !nweb_handle_) {
    return;
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  if (!IsUserEnableBlankless()){
    uint64_t recorded_time = instance.GetSystemTime(nweb_id_, blankless_key_);
    auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
    int32_t corrected_time = static_cast<int32_t>(static_cast<uint64_t>(system_time) - recorded_time);
    int32_t lcp_time = instance.FireFrameInsertCallback(nweb_id_, blankless_key_);
    int32_t lifetime = lcp_time - (corrected_time);
    if (corrected_time < 0  || lifetime < base::ohos::BlanklessController::MINIMUM_FRAME_LIFETIME) { // 40 ms
      return;
    }
    nweb_handle_->OnRemoveBlanklessFrame(lifetime);
    instance.RegisterFrameRemoveCallback(nweb_id_, blankless_key_, [handle = this->nweb_handle_](){
      handle->OnRemoveBlanklessFrame(0);
    });
    return;
  }
  int32_t lcp_time = instance.FireFrameInsertCallback(nweb_id_, blankless_key_);
  if (lcp_time > 0) {
    nweb_handle_->OnRemoveBlanklessFrame(lcp_time);
    instance.RegisterFrameRemoveCallback(nweb_id_, blankless_key_, [handle = this->nweb_handle_](){
      handle->OnRemoveBlanklessFrame(0);
    });
  }
}

int32_t NWebImpl::SetBlanklessLoadingParams(const std::string& key, bool enable, int32_t duration,
  int64_t expirationTime, std::shared_ptr<NWebBlanklessCallback> callback) {
  MarkUserEnableBlankless();
  if (!base::ohos::BlanklessController::CheckGlobalProperty() || !CheckNetAvailable()) {
    return -5;  // ERR_SIGNIFICANT_CHANGE
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  uint64_t blankless_key = base::ohos::BlanklessController::ConvertToBlanklessKey(key);
  auto status_code = instance.MatchKey(nweb_id_, blankless_key);
  LOG(DEBUG) << "blankless SetBlanklessLoadingParams nweb_id: " << nweb_id_
             << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code);
  if (status_code == base::ohos::BlanklessController::StatusCode::KEY_NOT_MATCH) {
    return -4;    // ERR_KEY_NOT_MATCH
  }
  if (callback) {
    blankless_callback_ = callback;
  }
  string_key_ = key;
  auto& databaseInstance = base::ohos::BlanklessDataController::GetInstance();
  auto window_id = instance.GetWindowIdByNWebId(nweb_id_);
  auto is_private = OHOS::NWeb::WindowManagerAdapterImpl::GetWindowPrivacyMode(window_id);
  if (is_private) {
    LOG(DEBUG) << "blankless this is a private window: "<< window_id;
    databaseInstance.ClearSnapshot(blankless_key);
    databaseInstance.ClearSnapshotDataItem({blankless_key});
    ExecuteBlanklessCallback(key, 1, std::string("this is a private window")); // 1 LOADING_FAILED
    return -5;
  }
  if (expirationTime != 0) {
    databaseInstance.InsertExpirationInfo(static_cast<int64_t>(blankless_key), expirationTime * 1000); // ms->μs
  }
  if (status_code != base::ohos::BlanklessController::StatusCode::INSERTED ||
      databaseInstance.GetBlanklessLoadingCacheCapacity() == 0) {
    LOG(DEBUG) << "blankless SetBlanklessLoadingParams nweb_id: " << nweb_id_
               << ", blankless_key: " << blankless_key << ", status: " << static_cast<int>(status_code)
               << ", capacity: " << databaseInstance.GetBlanklessLoadingCacheCapacity()
               << ", enable: " << enable;
    return (enable ? -5 : 0);  // ERR_SIGNIFICANT_CHANGE(true) or SUCCESS(false)
  }
  if (enable) {
    OHOS::NWeb::SnapshotDataItem dataItem = databaseInstance.GetSnapshotDataItem(blankless_key, GetPreferenceHash());
    if (dataItem.historySimilarity < 0.33) {
      LOG(DEBUG) << "blankless SetBlanklessLoadingParams similarity < 0.33";
      return -5;    // ERR_SIGNIFICANT_CHANGE
    }
    CallBlanklessFrameFuncV2(blankless_key, dataItem, duration);
  }
  return 0;   // SUCCESS
}

void NWebImpl::CallExecuteBlanklessCallback(int32_t state, const std::string& reason) {
  ExecuteBlanklessCallback(string_key_, state, reason);
}

void NWebImpl::ClearBlanklessKey() {
  if (nweb_delegate_ == nullptr || blankless_key_ == base::ohos::BlanklessController::INVALID_BLANKLESS_KEY) {
    return;
  }
  nweb_delegate_->SetBlanklessLoadingKey(nweb_id_, base::ohos::BlanklessController::INVALID_BLANKLESS_KEY);
  blankless_key_ = base::ohos::BlanklessController::INVALID_BLANKLESS_KEY;
  ResetUserEnableBlankless();
}

bool NWebImpl::CheckNetAvailable() {
  auto netConnectAdapter = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateNetConnectAdapter();
  if (!netConnectAdapter) {
    LOG(ERROR) << "blankless net_connect_adapter is nullptr";
    return false;
  }
  NetConnectType type = NetConnectType::CONNECTION_UNKNOWN;
  NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
  netConnectAdapter->GetDefaultNetConnect(type, subtype);
  if (type == NetConnectType::CONNECTION_UNKNOWN) {
    LOG(DEBUG) << "blankless net not available";
    return false;
  }
  return true;
}

void NWebImpl::CallBlanklessFrameFunc(uint64_t blankless_key, SnapshotDataItem& dataItem, bool isAnime) {
  std::string file = isAnime ? dataItem.wholePath : dataItem.staticPath;
  if (nweb_handle_ == nullptr || dataItem.lcpTime == INT32_MAX || dataItem.lcpTime <= 0 || file.empty()) {
    return;
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
  uint64_t recorded_time = instance.GetSystemTime(nweb_id_, blankless_key_);
  int32_t corrected_time = static_cast<int32_t>(static_cast<uint64_t>(system_time) - recorded_time);
  if (corrected_time < 0 || corrected_time >= dataItem.lcpTime ||
      dataItem.lcpTime - corrected_time < base::ohos::BlanklessController::MINIMUM_FRAME_LIFETIME) { // 40 ms
    LOG(DEBUG) << "blankless CallBlanklessFrameFunc corrected time error " <<
      corrected_time << " " << dataItem.lcpTime;
    return;
  }
  int32_t lcp_time = base::ohos::BlanklessController::MAXIMUM_FRAME_LIFETIME;
  if (dataItem.lcpTime >= base::ohos::BlanklessController::A_STANDARD) {
    lcp_time = std::min(dataItem.lcpTime, base::ohos::BlanklessController::MAXIMUM_FRAME_LIFETIME);  // 2000 ms
  }
  LOG(DEBUG) << "blankless OnRemoveBlanklessFrame Delay Time: " << lcp_time;
  if (is_visible_) {
    nweb_handle_->OnInsertBlanklessFrameWithSize(file, dataItem.width, dataItem.height);
    RemoveBlanklessFrame(nweb_handle_, lcp_time, isAnime);
    instance.RegisterFrameRemoveCallback(nweb_id_, blankless_key_, [handle = this->nweb_handle_, isAnime](){
      RemoveBlanklessFrame(handle, 0, isAnime);
    });
  } else {
    instance.RegisterFrameInsertCallback(nweb_id_, blankless_key_,
        [handle = this->nweb_handle_, file, width = dataItem.width, height = dataItem.height](){
          handle->OnInsertBlanklessFrameWithSize(file, width, height);
        }, lcp_time);
  }
}

void NWebImpl::CallBlanklessFrameFuncV2(uint64_t blankless_key, SnapshotDataItem& dataItem,
                                        int32_t duration, bool isAnime) {
  std::string file = isAnime ? dataItem.wholePath : dataItem.staticPath;
  if (nweb_handle_ == nullptr || dataItem.lcpTime == INT32_MAX || dataItem.lcpTime <= 0 || file.empty()) {
    ExecuteBlanklessCallback(string_key_, 1, std::string("invalid frame insertion file")); // 1 LOADING_FAILED
    return;
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
  uint64_t recorded_time = instance.GetSystemTime(nweb_id_, blankless_key_);
  int32_t corrected_time = static_cast<int32_t>(static_cast<uint64_t>(system_time) - recorded_time);
  if (corrected_time < 0 || corrected_time >= dataItem.lcpTime ||
      dataItem.lcpTime - corrected_time < base::ohos::BlanklessController::MINIMUM_FRAME_LIFETIME) { // 40 ms
    LOG(DEBUG) << "blankless CallBlanklessFrameFuncV2 corrected time error " <<
      corrected_time << " " << dataItem.lcpTime;
    ExecuteBlanklessCallback(string_key_, 1, std::string("invalid frame insertion file")); // 1 LOADING_FAILED
    return;
  }
  int32_t lcp_time = base::ohos::BlanklessController::MAXIMUM_FRAME_LIFETIME;
  if (duration != 0) {
    lcp_time = duration;
  } else {
    if (dataItem.lcpTime >= base::ohos::BlanklessController::A_STANDARD) {
      lcp_time = std::min(dataItem.lcpTime, base::ohos::BlanklessController::MAXIMUM_FRAME_LIFETIME);  // 2000 ms
    }
  }
  LOG(DEBUG) << "blankless CallBlanklessFrameFuncV2 OnRemoveBlanklessFrame Delay Time: " << lcp_time;
  if (is_visible_) {
    nweb_handle_->OnInsertBlanklessFrameWithSize(file, dataItem.width, dataItem.height);
    RemoveBlanklessFrame(nweb_handle_, lcp_time, isAnime);
  } else {
    instance.RegisterFrameInsertCallback(nweb_id_, blankless_key_,
        [handle = this->nweb_handle_, file, width = dataItem.width, height = dataItem.height](){
          handle->OnInsertBlanklessFrameWithSize(file, width, height);
        }, lcp_time);
  }
}

void NWebImpl::CallBlanklessFrameFuncForWhiteList(uint64_t blankless_key, SnapshotDataItem& dataItem, bool isAnime) {
  std::string file = isAnime ? dataItem.wholePath : dataItem.staticPath;
  if (nweb_handle_ == nullptr || dataItem.lcpTime == INT32_MAX || dataItem.lcpTime <= 0 || file.empty()) {
    return;
  }
  auto& instance = base::ohos::BlanklessController::GetInstance();
  if (dataItem.lcpTime < base::ohos::BlanklessController::MINIMUM_FRAME_LIFETIME) { // 40 ms
    LOG(DEBUG) << "blankless CallBlanklessFrameFuncForWhiteList lcpTime invalid " << dataItem.lcpTime;
    return;
  }
  int32_t lcp_time = std::min(dataItem.lcpTime, base::ohos::BlanklessController::MAXIMUM_FRAME_LIFETIME);  // 2000 ms
  LOG(DEBUG) << "blankless OnRemoveBlanklessFrame Delay Time: " << lcp_time;
  if (is_visible_) {
    nweb_handle_->OnInsertBlanklessFrameWithSize(file, dataItem.width, dataItem.height);
    RemoveBlanklessFrame(nweb_handle_, lcp_time, isAnime);
    instance.RegisterFrameRemoveCallback(nweb_id_, blankless_key_, [handle = this->nweb_handle_, isAnime](){
      RemoveBlanklessFrame(handle, 0, isAnime);
    });
  } else {
    instance.RegisterFrameInsertCallback(nweb_id_, blankless_key_,
        [handle = this->nweb_handle_, file, width = dataItem.width, height = dataItem.height](){
          handle->OnInsertBlanklessFrameWithSize(file, width, height);
        }, lcp_time);
  }
}

void NWebImpl::ExecuteBlanklessCallback(const std::string& key, int32_t state, const std::string& reason) {
  if (blankless_callback_) {
    auto current_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds();
    blankless_callback_->OnReceiveValue(key, state, static_cast<int64_t>(current_time), reason);
  }
}

int64_t NWebImpl::GetPreferenceHash() {
  if (!nweb_delegate_) {
    return base::ohos::BlanklessDataController::INVALID_PREF_HASH;
  }
  return nweb_delegate_->GetPreferenceHash();
}

// static
int64_t NWebImpl::GetPreferenceHashByNwebId(int32_t nweb_id) {
  base::AutoLock lock_scope(nweb_map_lock_);
  NWebMap* map = OHOS::NWeb::g_nweb_map.Pointer();
  if (auto it = map->find(nweb_id); it != map->end()) {
    auto nweb = it->second.lock();
    if (nweb) {
      return nweb->GetPreferenceHash();
    }
  }

  return base::ohos::BlanklessDataController::INVALID_PREF_HASH;
}
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
void NWebImpl::AvoidVisibleViewportBottom(int32_t avoidHeight) {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("AvoidVisibleViewportBottom failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return;
  }
  nweb_delegate_->AvoidVisibleViewportBottom(avoidHeight);
}

int32_t NWebImpl::GetVisibleViewportAvoidHeight() {
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("GetVisibleViewportAvoidHeight failed, nweb delegate is nullptr, nweb_id = %{public}u",
            nweb_id_);
    return;
  }
  return nweb_delegate_->GetVisibleViewportAvoidHeight();
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void NWebImpl::UpdateSingleHandleVisible(bool isVisible) {
  if (nweb_delegate_) {
    nweb_delegate_->UpdateSingleHandleVisible(isVisible);
  }
}

void NWebImpl::SetTouchHandleExistState(bool touchHandleExist) {
  if (nweb_delegate_) {
    nweb_delegate_->SetTouchHandleExistState(touchHandleExist);
  }
}

void NWebImpl::SetViewportScaleState() {
  if (nweb_delegate_) {
    nweb_delegate_->SetViewportScaleState(true);
  }
}
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
void NWebImpl::SetErrorPageEnabled(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "SetErrorPageEnabled: nweb delegate has not init.";
    return;
  }
  nweb_delegate_->SetErrorPageEnabled(enable);
}

bool NWebImpl::GetErrorPageEnabled() {
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "GetErrorPageEnabled: nweb delegate has not init.";
    return false;
  }
  return nweb_delegate_->GetErrorPageEnabled();
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebImpl::EnablePrivateNetworkAccess(bool enable) {
  net_service::NetHelpers::SetPrivateNetworkAccess(enable);
}

bool NWebImpl::IsPrivateNetworkAccessEnabled() {
  LOG(DEBUG) << "PrivateNetworkAccess is "
             << (net_service::NetHelpers::GetPrivateNetworkAccess() ? "enable" : "false");
  return net_service::NetHelpers::GetPrivateNetworkAccess();
}
#endif

#if BUILDFLAG(ARKWEB_BGTASK)
void NWebImpl::OnBrowserForeground() {
  LOG(INFO) << "NWebImpl::OnBrowserForeground.";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnBrowserForeground nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->OnBrowserForeground();
}

void NWebImpl::OnBrowserBackground() {
  LOG(INFO) << "NWebImpl::OnBrowserBackground.";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("OnBrowserBackground nweb_delegate_ is null");
    return;
  }
  nweb_delegate_->OnBrowserBackground();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
void NWebImpl::EnableHttpsUpgrades(bool enable) {
  if (nweb_delegate_ == nullptr) {
    LOG_FEEDBACK(WARNING, kHttpsUpgrades)
        << "EnableHttpsUpgrades message:nwebDelegateIsNull";
    return;
  }
  nweb_delegate_->EnableHttpsUpgrades(enable);
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void NWebImpl::SetBlankScreenDetectionConfig(
    bool enable,
    const std::vector<double>& detectionTiming,
    const std::vector<int32_t>& detectionMethods,
    int32_t contentfulNodesCountThreshold) {
  LOG(INFO) << "NWebImpl::SetBlankScreenDetectionConfig.";
  if (nweb_delegate_ == nullptr) {
    LOG(ERROR) << "SetBlankScreenDetectionConfig nweb_delegate_ is null";
    return;
  }
  nweb_delegate_->SetBlankScreenDetectionConfig(
      enable, detectionTiming, detectionMethods, contentfulNodesCountThreshold);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
void NWebImpl::SetSocketIdleTimeout(int32_t timeout) {
  if (!NWebApplication::GetDefault()->HasInitializedCef()) {
    WVLOG_I(
        "Web had not initiated. Will set socket idle timeout value after"
        "network_service initialized.");
    net_service::NetHelpers::SetSocketIdleTimeout(timeout);
    return;
  }

  network::mojom::NetworkService* network_service =
      content::GetNetworkService();
  if (!network_service) {
    WVLOG_I(
        "network_service is nullptr. Will set socket idle timeout value after"
        "network_service initialized.");
    return;
  }

  content::GetNetworkService()->SetSocketIdleTimeout(timeout);
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void NWebImpl::SetClipboardSitePermissionEnabled(bool enable) {
  g_clipboard_site_permission_enabled = enable;
}

bool NWebImpl::IsClipboardSitePermissionEnabled() {
  return g_clipboard_site_permission_enabled;
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_WEBRTC)
void NWebImpl::ResumeMicrophone() {
  if (nweb_delegate_) {
    nweb_delegate_->ResumeMicrophone();
  }
}

void NWebImpl::StopMicrophone() {
  if (nweb_delegate_) {
    nweb_delegate_->StopMicrophone();
  }
}

void NWebImpl::PauseMicrophone() {
  if (nweb_delegate_) {
    nweb_delegate_->PauseMicrophone();
  }
}
#endif

void NWebImpl::StopFling() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->StopFling();
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebImpl::EnableRewriteUrlForNavigation(bool enable) {
  OhosUrlRewriteController::EnableRewriteUrl(enable);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
std::map<std::string, std::string> NWebImpl::GetRequestHeader(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetRequestHeader, delegate is null";
    return {};
  }
  return nweb_delegate_->ResourceRequestGetRequestHeader(nweb_request_key);
}
 
std::string NWebImpl::GetRequestUrl(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetRequestUrl, delegate is null";
    return std::string();
  }
  return nweb_delegate_->ResourceRequestGetRequestUrl(nweb_request_key);
}
 
bool NWebImpl::IsRequestGesture(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to IsRequestGesture, delegate is null";
    return false;
  }
  return nweb_delegate_->ResourceRequestIsRequestGesture(nweb_request_key);
}
 
bool NWebImpl::IsMainFrame(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to IsMainFrame, delegate is null";
    return false;
  }
  return nweb_delegate_->ResourceRequestIsMainFrame(nweb_request_key);
}
 
bool NWebImpl::IsRedirect(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to IsRedirect, delegate is null";
    return false;
  }
  return nweb_delegate_->ResourceRequestIsRedirect(nweb_request_key);
}
 
std::string NWebImpl::GetRequestMethod(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetRequestMethod, delegate is null";
    return std::string();
  }
  return nweb_delegate_->ResourceRequestGetRequestMethod(nweb_request_key);
}
 
int32_t NWebImpl::GetPageTransition(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetPageTransition, delegate is null";
    return -1;
  }
  return nweb_delegate_->ResourceRequestGetPageTransition(nweb_request_key);
}
 
int32_t NWebImpl::GetRequestType(int32_t nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetRequestType, delegate is null";
    return -1;
  }
  return nweb_delegate_->ResourceRequestGetRequestType(nweb_request_key);
}
 
 
std::string NWebImpl::GetMimeType(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetMimeType, delegate is null";
    return std::string();
  }
  return nweb_delegate_->ResourceResponseGetMimeType(nweb_response_key);
}
 
std::string NWebImpl::GetEncoding(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetEncoding, delegate is null";
    return std::string();
  }
  return nweb_delegate_->ResourceResponseGetEncoding(nweb_response_key);
}
 
int32_t NWebImpl::GetStatusCode(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetStatusCode, delegate is null";
    return -1;
  }
  return nweb_delegate_->ResourceResponseGetStatusCode(nweb_response_key);
}
 
std::string NWebImpl::GetReasonPhrase(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetReasonPhrase, delegate is null";
    return std::string();
  }
  return nweb_delegate_->ResourceResponseGetReasonPhrase(nweb_response_key);
}
 
std::map<std::string, std::string> NWebImpl::GetResponseHeader(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetResponseHeader, delegate is null";
    return {};
  }
  return nweb_delegate_->ResourceResponseGetResponseHeader(nweb_response_key);
}
 
bool NWebImpl::GetIsFromNetwork(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to GetIsFromNetwork, delegate is null";
    return false;
  }
  return nweb_delegate_->ResourceResponseGetIsFromNetwork(nweb_response_key);
}
 
void NWebImpl::ResourceRequestDelete(int nweb_request_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to ResourceRequestDelete, delegate is null";
    return;
  }
  nweb_delegate_->ResourceRequestDelete(nweb_request_key);
}
 
void NWebImpl::ResourceResponseDelete(int nweb_response_key) {
  if (!nweb_delegate_) {
    LOG(ERROR) << "failed to ResourceResponseDelete, delegate is null";
    return;
  }
  nweb_delegate_->ResourceResponseDelete(nweb_response_key);
}
int32_t NWebImpl::GetLastCommittedEntryPageTransition() {
  LOG(INFO) << "NWebImpl::GetLastCommittedEntryPageTransition.";
  if (nweb_delegate_ == nullptr) {
    WVLOG_E("GetLastCommittedEntryPageTransition nweb_delegate_ is null");
    return -1;
  }
  return nweb_delegate_->GetLastCommittedEntryPageTransition();
}
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
void NWebImpl::LibraryLoaded(std::shared_ptr<NWebEngineInitArgs> init_args,
                             bool lazy) {
  if (NWebApplication::GetDefault()->HasInitializedCef()) {
    return;
  }
  save_initargs_ = init_args;
  should_lazy_init_web_engine_ = lazy;
}

bool NWebImpl::ShouldLazyInitWebEngine() {
  return should_lazy_init_web_engine_;
}

std::shared_ptr<NWebEngineInitArgs> NWebImpl::GetSaveInitargs() {
  return save_initargs_;
}
#endif

void NWebImpl::ReloadIgnoreCache() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->ReloadIgnoreCache();
}
