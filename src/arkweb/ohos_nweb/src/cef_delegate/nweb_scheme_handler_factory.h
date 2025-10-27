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

#ifndef OHOS_NWEB_SRC_NWEB_SCHEME_HANDLER_FACTORY_H
#define OHOS_NWEB_SRC_NWEB_SCHEME_HANDLER_FACTORY_H

#include "base/memory/raw_ptr.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_browser.h"
#include "cef/include/cef_frame.h"
#include "cef/include/cef_request.h"
#include "cef/include/cef_scheme.h"
#include "ohos_nweb/src/ndk/scheme_handler/scheme_handler.h"

namespace OHOS::NWeb {

typedef std::map<std::string, ArkWeb_SchemeHandler*> SchemeHandlerMap;

class NWebSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  explicit NWebSchemeHandlerFactory();

  static CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> GetOrCreateForScheme(
      const std::string& scheme);
  static void ClearSchemeHandlers(const std::string& web_tag);
  static void ClearServiceWorkerSchemeHandler();

  CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& scheme_name,
                                       CefRefPtr<CefRequest> request) override;

  void SetSchemeHandlerForSW(ArkWeb_SchemeHandler* scheme_handler);
  void SetSchemeHandler(const std::string& web_tag,
                        ArkWeb_SchemeHandler* scheme_handler);
  void RemoveSchemeHandler(const std::string& web_tag);
  void RemoveServiceWorkerSchemeHandler();
  void OnRequestStop(const ArkWeb_ResourceRequest* resource_request,
                     const std::string& web_tag,
                     bool from_service_worker);

 private:
  ArkWeb_SchemeHandler* FromTag(const std::string& web_tag);
  std::string GetWebTag(CefRefPtr<CefBrowser> browser);
  mutable base::Lock lock_;
  SchemeHandlerMap scheme_handler_map_;
  raw_ptr<ArkWeb_SchemeHandler> scheme_handler_for_sw_ = nullptr;

  IMPLEMENT_REFCOUNTING(NWebSchemeHandlerFactory);
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_NWEB_SCHEME_HANDLER_FACTORY_H
