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
 
#ifndef NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H
#define NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H
 
#include <string>
#include <mutex>
#include <memory>
 
#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"
#include "ohos_nweb/src/capi/web_extension_side_panel_items.h"

namespace OHOS::NWeb {

using SidePanelOnOpenCallback = base::RepeatingCallback<void(const std::optional<std::string>&)>;
using SidePanelOnSetOptionsCallback = base::RepeatingCallback<void(const std::optional<std::string>&)>;

class NWebExtensionSidePanelCefDelegate {
  public:
    static NWebExtensionSidePanelCefDelegate* GetInstance();
 
    bool HasOnOpenByPbCallback();
    bool HasOnSetOptionsByPbCallback();
    bool OnOpenByPb(ExtensionSidePanelOpenOptions& options,
                    SidePanelOnOpenCallback callback);
    bool OnSetOptionsByPb(ExtensionSidePanelSetOptions& options,
                          SidePanelOnSetOptionsCallback callback);
#if BUILDFLAG(ARKWEB_NWEB_EX)
    void OnOpenCallback(int request_id, const std::optional<std::string>& error);
    void OnSetOptionsCallback(int request_id, const std::optional<std::string>& error);
#endif

  private:
    NWebExtensionSidePanelCefDelegate();
    NWebExtensionSidePanelCefDelegate(const NWebExtensionSidePanelCefDelegate&) = delete;
    NWebExtensionSidePanelCefDelegate& operator=(
        const NWebExtensionSidePanelCefDelegate&) = delete;
};
} // namespace OHOS::NWeb
 
#endif  // NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H
