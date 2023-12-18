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

#ifndef NWEB_GEOLOCATION_CALLBACK_H
#define NWEB_GEOLOCATION_CALLBACK_H

#include "cef/include/cef_browser.h"
#include "ohos_nweb/include/nweb_geolocation_callback_interface.h"

namespace OHOS::NWeb {
class NWebGeolocationCallback : public NWebGeolocationCallbackInterface {
 public:
  explicit NWebGeolocationCallback(CefRefPtr<CefBrowser> browser);
  ~NWebGeolocationCallback() = default;

  void GeolocationCallbackInvoke(const std::string& origin,
                                 bool allow,
                                 bool retain,
                                 bool incognito) override;

 private:
  CefRefPtr<CefBrowser> browser_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_GEOLOCATION_CALLBACK_H
