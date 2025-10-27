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

#include "ohos_nweb/src/cef_delegate/nweb_cursor_info_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

class MockCefBrowser : public CefBrowser {
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
  CefRefPtr<ArkWebBrowserHostExt> GetHost() override { return nullptr; }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return -1; }
  bool IsSame(CefRefPtr<CefBrowser> that) override { return false; }
  bool IsPopup() override { return false; }
  bool HasDocument() override { return false; }
  CefRefPtr<CefFrame> GetMainFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
};

void NwebCustomKeyboardHandlerImplFuzzTest(FuzzedDataProvider* fdp) {
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  NWebCustomKeyboardHandlerImpl impl(handler);

  std::string text = fdp->ConsumeRandomLengthString(256);
  impl.InsertText(text);

  int32_t length = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
  impl.DeleteForward(length);

  impl.DeleteBackward(length);

  impl.SendFunctionKey(length);

  impl.IsAttached();

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  int32_t input_flags = fdp->ConsumeIntegralInRange<int32_t>(0, 127);
  impl.Attach(browser, true, input_flags);

  int32_t value = fdp->ConsumeIntegralInRange<int32_t>(0, 127);
  impl.AttachFromWebStateChange(static_cast<OHOS::NWeb::WebCustomKeyboardState>(value));
  
  impl.CloseFromWebStateChange(static_cast<OHOS::NWeb::WebCustomKeyboardState>(value));

  impl.Close();
} 

}// namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NwebCustomKeyboardHandlerImplFuzzTest(&fdp);
  
  return 0;
}
