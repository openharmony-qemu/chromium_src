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
#include "cef/include/cef_app.h"
#include "nweb_handler_delegate.h"
#include "nweb_preference_delegate.h"
#include "ohos_adapter_helper.h"

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

  static CefRefPtr<OHOS::NWeb::NWebApplication> GetDefault();
  void InitializeCef(const CefMainArgs& args, const CefSettings& settings);
  bool HasInitializedCef();

  /* CefApp methods begine */
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
  void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
  /* CefApp methods end */

  /* CefBrowserProcessHandler methods begin */
  void OnContextInitialized() override;
  CefRefPtr<CefClient> GetDefaultClient() override;
  void OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) override;
  /* CefBrowserProcessHandler methods end */

  /* CefRenderProcessHandler methods begin */
  void OnWebKitInitialized() override;
  /* CefRenderProcessHandler methods begin */

  void CreateBrowser(
      std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
      const std::string& url,
      CefRefPtr<NWebHandlerDelegate> handler_delegate,
      void* window);

  void RunAfterContextInitialized(base::OnceCallback<void()> context_callback);

 private:
  void PopulateCreateSettings(CefRefPtr<CefCommandLine> command_line,
                              CefBrowserSettings& browser_settings);

  void OnContextInitializedInternal();
  std::vector<std::string> CustomSchemeCmdLineSplit(std::string str,
                                                    const char split);
  void RunWebInitedCallback(WebRunInitedCallback* callback);

  base::OnceCallback<void()> complete_callback_;
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebApplication);
};  // NWebApplication
}  // namespace OHOS::NWeb
#endif  // NWebApplication
