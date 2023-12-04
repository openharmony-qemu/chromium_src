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

#ifndef SRC_OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_FULL_SCREEN_EXIT_HANDLER_IMPL_H_
#define SRC_OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_FULL_SCREEN_EXIT_HANDLER_IMPL_H_

#include "cef/include/cef_browser.h"
#include "nweb_full_screen_exit_handler.h"

namespace OHOS::NWeb {
class NWebFullScreenExitHandlerImpl : public NWebFullScreenExitHandler {
 public:
  explicit NWebFullScreenExitHandlerImpl(CefRefPtr<CefBrowser> browser);
  ~NWebFullScreenExitHandlerImpl() = default;

  void ExitFullScreen() override;

 private:
  CefRefPtr<CefBrowser> browser_;
};
}  // namespace OHOS::NWeb

#endif  // SRC_OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_FULL_SCREEN_EXIT_HANDLER_IMPL_H_
