/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_ACTION_CEF_DELEGATE_H
#define NWEB_EXTENSION_ACTION_CEF_DELEGATE_H

#include <mutex>

#include "base/values.h"
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"

namespace extensions {
class MockNWebExtensionActionCefDelegate;
}
namespace OHOS::NWeb {

NWebExtensionActionIcon CreateFromImageSkiaReps(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps);

NWebExtensionActionIconV2 CreateNWebIconFromImageSkiaRepsV2(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps);

class NWebExtensionActionCefDelegate {
 public:
  static void RegisterWebExtensionApiListener(
      std::shared_ptr<NWebExtensionActionApiCallback> action_api_listener);
  static void UnRegisterWebExtensionApiListener();
  static NWebExtensionActionCefDelegate* GetInstance();
  static void WebExtensionActionClicked(std::string extension_id,
                                        const NWebExtensionTab* tab);
  static void WebExtensionActionPinnedStateChanged(std::string extensionId,
                                                   bool isPinned);
  static void WebExtensionActionShowPopup(int tabId, std::string extensionId);
  static void WebExtensionSetPopupWindowId(int popupNwebId, int windowId);
  static void WebExtensionErasePopupWindowId(int popupNwebId);

  void OnDisable(const std::string& extensionId,
                 std::optional<int>& tabId,
                 std::optional<std::string> contextType = std::nullopt,
                 std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnEnable(const std::string& extensionId,
                std::optional<int>& tabId,
                std::optional<std::string> contextType = std::nullopt,
                std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnOpenPopup(
      const std::string& extensionId,
      const std::optional<NWebExtensionActionOpenPopupOptions>& options,
      std::optional<std::string> contextType = std::nullopt,
      std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetTitle(const std::string& extensionId,
                  const NWebExtensionActionSetTitleDetails& details,
                  std::optional<std::string> contextType = std::nullopt,
                  std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetPopup(const std::string& extensionId,
                  const NWebExtensionActionSetPopupDetails& details,
                  std::optional<std::string> contextType = std::nullopt,
                  std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetIcon(std::string extension_id,
                 const gfx::Image& icon_image,
                 int32_t tab_id,
                 std::optional<std::string> contextType = std::nullopt,
                 std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetBadgeText(const std::string& extensionId,
                      const NWebExtensionActionSetBadgeTextDetails& details,
                      std::optional<std::string> contextType = std::nullopt,
                      std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetBadgeTextColor(
      const std::string& extensionId,
      const NWebExtensionActionSetBadgeTextColorDetails& details,
      std::optional<std::string> contextType = std::nullopt,
      std::optional<bool> includeIncognitoInfo = std::nullopt);
  void OnSetBadgeBackgroundColor(
      const std::string& extensionId,
      const NWebExtensionActionSetBadgeBackgroundColorDetails& details,
      std::optional<std::string> contextType = std::nullopt,
      std::optional<bool> includeIncognitoInfo = std::nullopt);
  std::optional<NWebExtensionActionUserSettings> OnGetUserSettings(
      const std::string& extensionId);

 private:
  friend class ::extensions::MockNWebExtensionActionCefDelegate;
  NWebExtensionActionCefDelegate() = default;
  ~NWebExtensionActionCefDelegate() = default;
  NWebExtensionActionCefDelegate(const NWebExtensionActionCefDelegate &) = delete;
  NWebExtensionActionCefDelegate &operator=(const NWebExtensionActionCefDelegate &) = delete;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_ACTION_CEF_DELEGATE_H
