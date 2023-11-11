/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_SELECT_POPUP_MENU_CALLBACK_H
#define NWEB_SELECT_POPUP_MENU_CALLBACK_H

#include "cef/include/cef_dialog_handler.h"
#include "nweb_select_popup_menu.h"

namespace OHOS::NWeb {
class NWebSelectPopupMenuCallbackImpl : public NWebSelectPopupMenuCallback {
 public:
  NWebSelectPopupMenuCallbackImpl() = default;
  explicit NWebSelectPopupMenuCallbackImpl(
      CefRefPtr<CefSelectPopupCallback> callback);
  ~NWebSelectPopupMenuCallbackImpl() = default;
  void Continue(const std::vector<int32_t>& indices) override;
  void Cancel() override;

 private:
  CefRefPtr<CefSelectPopupCallback> callback_ = nullptr;
};
}  // namespace OHOS::NWeb
#endif