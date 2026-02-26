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

#include "nweb_application.h"

#include <cstdlib>
#include <thread>

#include "arkweb/build/features/features.h"
#include "base/threading/thread_restrictions.h"
#include "build/build_config.h"
#include "cef/include/wrapper/cef_helpers.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/content_switches.h"
#include "nweb_handler_delegate.h"
#include "nweb_impl.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_INIT_CALLBACK)
#include "cef/include/wrapper/cef_closure_task.h"
#endif

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
#include "base/json/json_writer.h"
#include "base/values.h"
#include "cef/libcef/common/net/scheme_registration.h"
#endif

#if BUILDFLAG(ARKWEB_DOWNLOAD)
#include "nweb_file_writer_cleaner.h"
#endif  // BUILDFLAG(ARKWEB_DOWNLOAD)

#if BUILDFLAG(ARKWEB_NWEB_EX) && BUILDFLAG(ARKWEB_CRASHPAD)
#include "ohos_nweb_ex/overrides/ohos_nweb/src/cef_delegate/custom_crashpad_handler.h"
#endif

namespace {
#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
CefRefPtr<OHOS::NWeb::NWebApplication> g_application = nullptr;
static bool is_initialized = false;
static std::mutex init_mtx;
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
const char kNWebId[] = "nweb_id";
#endif  //  ARKWEB_EX_DOWNLOAD
}  // namespace

class ScopedAllowBlockingForNwebInit : public base::ScopedAllowBlocking {};

namespace OHOS::NWeb {
#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
// static
CefRefPtr<OHOS::NWeb::NWebApplication> NWebApplication::GetDefault() {
  if (!g_application) {
    g_application = new NWebApplication();
  }
  return g_application;
}

NWebApplication::NWebApplication() {
  g_application = this;
}

NWebApplication::~NWebApplication() {
  g_application = nullptr;
}

bool NWebApplication::HasInitializedCef() {
  return is_initialized;
}

void NWebApplication::InitializeCef(const CefMainArgs& mainargs,
                                    const CefSettings& settings) {
#if BUILDFLAG(ARKWEB_DOWNLOAD)
  LOG(INFO) << "NWebApplication::InitializeCef, is_initialized:"
            << is_initialized;
#endif  // BUILDFLAG(ARKWEB_DOWNLOAD)
  if (is_initialized) {
    LOG(INFO) << "has initialized cef.";
    return;
  }
  int exitcode =
      CefExecuteProcess(mainargs, NWebApplication::GetDefault(), NULL);
  if (exitcode >= 0) {
    LOG(INFO) << "CefExecuteProcess returned : " << exitcode;
    return;
  }
  std::unique_lock<std::mutex> lk(init_mtx);
  bool initial_result = CefInitialize(mainargs, settings, NWebApplication::GetDefault(), NULL);
#if BUILDFLAG(ARKWEB_DOWNLOAD)
  {
    // get download Temp directory.
    ScopedAllowBlockingForNwebInit allow_blocking_for_using_path;
    NwebFileWriterCleaner::GetDeletePendingFiles();
  }

#endif  // BUILDFLAG(ARKWEB_DOWNLOAD)
  if (!initial_result) {
    LOG(ERROR) << "CefInitialize failed";
  } else {
    is_initialized = true;
#if BUILDFLAG(ARKWEB_DOWNLOAD)
    // delete download Temp directory.
    NwebFileWriterCleaner::DeleteDownloadTempDir();
#endif  // BUILDFLAG(ARKWEB_DOWNLOAD)
  }
#if BUILDFLAG(ARKWEB_NWEB_EX) && BUILDFLAG(ARKWEB_CRASHPAD)
  OHOS::NWeb::ReportCrashpadFiles(NWebImpl::GetDefaultCrashpadLogPath());
#endif
}
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

/* CefApp methods begin */
CefRefPtr<CefBrowserProcessHandler>
NWebApplication::GetBrowserProcessHandler() {
  return this;
}

CefRefPtr<CefRenderProcessHandler> NWebApplication::GetRenderProcessHandler() {
  return this;
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::vector<std::string> NWebApplication::CustomSchemeCmdLineSplit(
    std::string str,
    const char split) {
  std::istringstream inStream(str);
  std::vector<std::string> ret;
  std::string token;
  while (getline(inStream, token, split)) {
    if (!token.empty()) {
      ret.push_back(token);
      token.clear();
    }
  }
  return ret;
}

void NWebApplication::OnRegisterCustomSchemes(
    CefRawPtr<CefSchemeRegistrar> registrar) {
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(::switches::kOhosCustomScheme)) {
    std::string cmdline_scheme =
        command_line->GetSwitchValue(::switches::kOhosCustomScheme).ToString();
    LOG(INFO) << "cmdline scheme:" << cmdline_scheme;
    std::vector<std::string> schemesInfo =
        CustomSchemeCmdLineSplit(cmdline_scheme, ';');
    for (auto it = schemesInfo.begin(); it != schemesInfo.end(); ++it) {
      int options = 0;
      std::vector<std::string> scheme = CustomSchemeCmdLineSplit(*it, ',');
      if (scheme.size() != 3) {
        break;
      }
      if (scheme[1] == std::string("1")) {
        options = (options | CEF_SCHEME_OPTION_CORS_ENABLED);
      }

      if (scheme[2] == std::string("1")) {
        options = (options | CEF_SCHEME_OPTION_FETCH_ENABLED);
      }
      LOG(INFO) << "scheme name:" << *it << " scheme options:" << options;
      registrar->AddCustomScheme(scheme[0], options);
    }
  }

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  for (auto scheme_info : scheme_registrar_) {
    registrar->AddCustomScheme(scheme_info.first, scheme_info.second);
  }
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)
}
#endif
/* CefApp methods end */

/* CefBrowserProcessHandler methods begin */
void NWebApplication::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();
  LOG(INFO) << "NWebApplication::OnContextInitialized, begin";

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
  if (complete_callback_) {
    LOG(INFO) << "NWebApplication::OnContextInitialized, into callback";
    std::move(complete_callback_).Run();
  }
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_INIT_CALLBACK)
  auto runWebInitedCallback = OhosAdapterHelper::GetInstance()
                                  .GetInitWebAdapter()
                                  ->GetRunWebInitedCallback();
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&NWebApplication::RunWebInitedCallback, this,
                                runWebInitedCallback));
#endif

  OnContextInitializedInternal();
}

#if BUILDFLAG(ARKWEB_INIT_CALLBACK)
NO_SANITIZE("cfi-vcall")
void NWebApplication::RunWebInitedCallback(WebRunInitedCallback* callback) {
  if (callback != nullptr) {
    callback->RunInitedCallback();
    delete callback;
    callback = nullptr;
  } else {
    LOG(ERROR) << "There is no web inited callback to run.";
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebApplication::OnBeforeChildProcessLaunch(
    CefRefPtr<CefCommandLine> command_line) {
  LOG(INFO) << "NWebApplication::OnBeforeChildProcessLaunch";
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  base::Value::Dict schemes_dict;
  for (auto scheme_info : scheme_registrar_) {
    schemes_dict.Set(scheme_info.first, scheme_info.second);
  }
  std::string schemes_json;
  base::JSONWriter::Write(schemes_dict, &schemes_json);
  command_line->AppendSwitchWithValue(::switches::kOhSchemeHandlerCustomScheme,
                                      schemes_json);
#endif
  if (CefCommandLine::GetGlobalCommandLine()->HasSwitch(
          ::switches::kOhosCustomScheme)) {
    command_line->AppendSwitchWithValue(
        ::switches::kOhosCustomScheme,
        CefCommandLine::GetGlobalCommandLine()
            ->GetSwitchValue(::switches::kOhosCustomScheme)
            .ToString());
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  for (const auto& arg : NWebImpl::GetCommandLineArgsForNWebEx()) {
    command_line->AppendSwitch(arg);
  }
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  std::string extension_name;
  if (NWebImpl::GetExtensionName(extension_name)) {
    command_line->AppendSwitchWithValue(::switches::kSetExtensionName,
                                      extension_name);
  }
#endif
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
  if (CefCommandLine::GetGlobalCommandLine()->HasSwitch(
          ::switches::kOhosHapPath)) {
    LOG(INFO) << "hap package is not decompresssed";
    command_line->AppendSwitchWithValue(
        ::switches::kOhosHapPath, CefCommandLine::GetGlobalCommandLine()
                                      ->GetSwitchValue(::switches::kOhosHapPath)
                                      .ToString());
  }
#endif
}
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
void NWebApplication::OnContextInitializedForIncognitoMode() {
  CEF_REQUIRE_UI_THREAD();

  OnContextInitializedInternal(true);
}
#endif
/* CefBrowserProcessHandler methods end */

/* CefRenderProcessHandler methods begin */
void NWebApplication::OnWebKitInitialized() {
  LOG(INFO) << "OnWebKitInitialized";
}
/* CefRenderProcessHandler methods begin */

CefRefPtr<CefClient> NWebApplication::GetDefaultClient() {
  // Called when a new browser window is created via the Chrome runtime UI
  // OHOS not use it.
  return nullptr;
}

void NWebApplication::PopulateCreateSettings(
    CefRefPtr<CefCommandLine> command_line,
    CefBrowserSettings& browser_settings
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
    ,
    bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
    ,
    const std::string& shared_render_process_token
#endif

) {
  if (command_line->HasSwitch(switches::kOffScreenFrameRate)) {
    browser_settings.windowless_frame_rate =
        atoi(command_line->GetSwitchValue(switches::kOffScreenFrameRate)
                 .ToString()
                 .c_str());
  }

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  browser_settings.incognito_mode = incognito_mode;
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  CefString str = CefString(shared_render_process_token);
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.shared_render_process_token), true);
#endif
}

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
void NWebApplication::RunAfterContextInitialized(
    base::OnceCallback<void()> complete_callback) {
  LOG(INFO)
      << "NWebApplication::RunAfterContextInitialized, set complete_callback_";
  complete_callback_ = std::move(complete_callback);
}

void NWebApplication::CreateBrowser(
    std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
    const std::string& url,
    CefRefPtr<NWebHandlerDelegate> handler_delegate,
    void* window
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
    ,
    bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
    ,
    const std::string& shared_render_process_token
#endif
) {
  LOG(INFO) << "NWebApplication::CreateBrowser";
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;
  PopulateCreateSettings(command_line, browser_settings
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                         ,
                         incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                         ,
                         shared_render_process_token

#endif
  );
  preference_delegate->ComputeBrowserSettings(browser_settings);

  if (command_line->HasSwitch(switches::kForTest)) {
    preference_delegate->PutHasInternetPermission(true);
    preference_delegate->PutBlockNetwork(false);
  }

  std::string url_from_command_line;
  url_from_command_line = command_line->GetSwitchValue(switches::kUrl);

  // Information used when creating the native window.
  CefWindowInfo window_info;
  CefWindowHandle handle = kNullWindowHandle;
  window_info.SetAsWindowless(handle);

  // Create the first browser window.
  if (handler_delegate == nullptr) {
    LOG(ERROR) << "NWebApplication::CreateBrowser, handler_delegate is nullptr";
    return;
  }
  CefRefPtr<CefDictionaryValue> extra_info = CefDictionaryValue::Create();
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  int nweb_id = int(handler_delegate->GetNWebId());
  extra_info->SetInt(kNWebId, nweb_id);
#endif
  LOG(INFO) << "NWebApplication::CreateBrowser, 2";

  CefBrowserHost::CreateBrowser(
      window_info, handler_delegate,
      url_from_command_line.empty() ? url : url_from_command_line,
      browser_settings, extra_info, nullptr);
  auto browser = handler_delegate->GetBrowser();
  if (browser && browser->GetHost()) {
    browser->GetHost()->SetNativeWindow(window);
  }
}
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
bool NWebApplication::RegisterCustomSchemes(const std::string& scheme,
                                            int options) {
  LOG(INFO) << "scheme_handler register custom schemes " << scheme;
  if (!HasInitializedCef()) {
    scheme_registrar_[scheme] = options;
    return true;
  } else {
    return false;
  }
}
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

}  // namespace OHOS::NWeb
