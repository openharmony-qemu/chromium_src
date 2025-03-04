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

#ifndef NWEB_APPLICATION_H
#define NWEB_APPLICATION_H

#include <string>

#include "arkweb/build/features/features.h"
#include "cef/include/cef_app.h"
#include "nweb_handler_delegate.h"
#include "nweb_preference_delegate.h"
#if BUILDFLAG(ARKWEB_INIT_CALLBACK)
#include "ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
#include "cef/libcef/common/scheme_registrar_impl.h"
#endif

namespace OHOS::NWeb {
namespace switches {
const char kUrl[] = "url";
const char kOffScreenFrameRate[] = "off-screen-frame-rate";
const char kForTest[] = "for-test";
}  // namespace switches

class NWebApplication : public CefApp,
                        public CefBrowserProcessHandler,
                        public CefRenderProcessHandler {
 public:
  NWebApplication();
  ~NWebApplication();

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
  static CefRefPtr<OHOS::NWeb::NWebApplication> GetDefault();
  void InitializeCef(const CefMainArgs& args, const CefSettings& settings);
  bool HasInitializedCef();
  void CreateBrowser(
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
  );
  void RunAfterContextInitialized(base::OnceCallback<void()> context_callback);
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

  /* CefApp methods begine */
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) override;
#endif
  /* CefApp methods end */

  /* CefBrowserProcessHandler methods begin */
  void OnContextInitialized() override;

  CefRefPtr<CefClient> GetDefaultClient() override;
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) override;
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  void OnContextInitializedForIncognitoMode() override;
#endif
  /* CefBrowserProcessHandler methods end */

  /* CefRenderProcessHandler methods begin */
  void OnWebKitInitialized() override;
  /* CefRenderProcessHandler methods begin */

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  bool RegisterCustomSchemes(const std::string& scheme, int options);
#endif

 private:
  void PopulateCreateSettings(CefRefPtr<CefCommandLine> command_line,
                              CefBrowserSettings& browser_settings
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                              ,
                              bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                              ,
                              const std::string& shared_render_process_token
#endif
  );

  void OnContextInitializedInternal(
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
      bool incognito_mode = false
#endif
  );

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::vector<std::string> CustomSchemeCmdLineSplit(std::string str,
                                                    const char split);
#endif

#if BUILDFLAG(ARKWEB_INIT_CALLBACK)
  void RunWebInitedCallback(WebRunInitedCallback* callback);
#endif

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
  base::OnceCallback<void()> complete_callback_;
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  std::map<std::string, int> scheme_registrar_;
#endif

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebApplication);
};  // NWebApplication
}  // namespace OHOS::NWeb
#endif  // NWebApplication
