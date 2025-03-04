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

#ifndef NWEB_ACCESS_REQUEST_DELEGATE_H
#define NWEB_ACCESS_REQUEST_DELEGATE_H

#include "arkweb/build/features/features.h"
#include "cef/include/cef_browser.h"
#include "ohos_nweb/include/nweb_access_request.h"

namespace OHOS::NWeb {
class NWebAccessRequestDelegate : public NWebAccessRequest {
 public:
  explicit NWebAccessRequestDelegate(CefRefPtr<CefAccessRequest> request);
#if BUILDFLAG(ARKWEB_WEBRTC)
  ~NWebAccessRequestDelegate();
#else
  ~NWebAccessRequestDelegate() = default;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

  std::string Origin() override;

  int ResourceAcessId() override;

  void Agree(int resourceId) override;

  void Refuse() override;

 private:
  CefRefPtr<CefAccessRequest> request_ = nullptr;
};

#if BUILDFLAG(ARKWEB_WEBRTC)
class NWebScreenCaptureAccessRequestDelegate
    : public NWebScreenCaptureAccessRequest {
 public:
  explicit NWebScreenCaptureAccessRequestDelegate(
      CefRefPtr<CefScreenCaptureAccessRequest> request);
  ~NWebScreenCaptureAccessRequestDelegate();

  std::string Origin() override;

  void Agree(std::shared_ptr<NWebScreenCaptureConfig> config) override;

  void Refuse() override;

 private:
  CefRefPtr<CefScreenCaptureAccessRequest> request_ = nullptr;
};
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}  // namespace OHOS::NWeb

#endif  // NWEB_ACCESS_REQUEST_DELEGATE_H
