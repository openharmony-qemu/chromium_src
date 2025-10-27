/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

 #include "ohos_nweb/src/cef_delegate/nweb_access_request_delegate.h"

 #include <fuzzer/FuzzedDataProvider.h>
 
 #include <iostream>
 #include <map>
 #include <memory>
 #include <string>
 #include <unordered_map>
 
 #include "base/logging.h"
 #include "cef/include/cef_browser.h"
 #include "gmock/gmock.h"
 #include "gtest/gtest.h"
 #include "include/cef_permission_request.h"
 #include "ohos_nweb/include/nweb_access_request.h"
 
 using namespace OHOS::NWeb;
 
 namespace OHOS::NWeb {
 class MockCefAccessRequest : public CefAccessRequest {
  public:
   IMPLEMENT_REFCOUNTING(MockCefAccessRequest);
   CefString Origin() override { return ""; }
   int ResourceAcessId() override { return -1; }
   void ReportRequestResult(bool allowed) override {}
 };
 
 class MockCefScreenCaptureAccessRequest : public CefScreenCaptureAccessRequest {
  public:
   IMPLEMENT_REFCOUNTING(MockCefScreenCaptureAccessRequest);
 
   CefString Origin() override { return ""; }
   void SetCaptureMode(int mode) override {}
   void SetCaptureSourceId(int id) override {}
   void ReportRequestResult(bool allowed) override {}
 };
 
 class MockNWebScreenCaptureConfig : public NWebScreenCaptureConfig {
  public:
   int GetMode() override { return -1; }
   int GetSourceId() override { return -1; }
 };
 
 void NWebAccessRequestDelegate001FuzzTest(FuzzedDataProvider* fdp) {
   if ((fdp == nullptr) || (fdp->remaining_bytes() < sizeof(int32_t))) {
     return;
   }
 
   CefRefPtr<MockCefAccessRequest> mockRequest = new MockCefAccessRequest();
   CefRefPtr<CefAccessRequest> request =
       fdp->ConsumeBool() ? mockRequest : nullptr;
   NWebAccessRequestDelegate delegate(request);
   delegate.Origin();
   delegate.ResourceAcessId();
 
   if (fdp->ConsumeBool()) {
     int randomResId = fdp->ConsumeIntegral<int>();
     delegate.Agree(randomResId);
   } else {
     delegate.Refuse();
   }
 }
 
 void NWebAccessRequestDelegate002FuzzTest(FuzzedDataProvider* fdp) {
   CefRefPtr<MockCefScreenCaptureAccessRequest> mockScreenRequest =
       new MockCefScreenCaptureAccessRequest();
   CefRefPtr<CefScreenCaptureAccessRequest> screenRequest =
       fdp->ConsumeBool() ? mockScreenRequest : nullptr;
   NWebScreenCaptureAccessRequestDelegate screenDelegate(screenRequest);
   screenDelegate.Origin();
 
   std::shared_ptr<NWebScreenCaptureConfig> config;
   if (fdp->ConsumeBool()) {
     config = std::make_shared<MockNWebScreenCaptureConfig>();
   }
 
   if (fdp->ConsumeBool()) {
     screenDelegate.Agree(config);
   } else {
     screenDelegate.Refuse();
   }
 }
 
 }  // namespace OHOS::NWeb
 
 extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if ((data == nullptr) || (size < sizeof(int32_t))) {
     return 0;
   }
   FuzzedDataProvider fdp(data, size);
   NWebAccessRequestDelegate001FuzzTest(&fdp);
   NWebAccessRequestDelegate002FuzzTest(&fdp);
   return 0;
 }
 