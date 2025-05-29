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
#include "extension_icon_image_observer.h"
#include <map>

#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/common/content_switches.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"

namespace extensions {

std::map<std::string, ExtensionIconImageObserver*> g_icon_image_observers;

void SetExtensionIconImageObserver(const std::string& extension_id, int32_t tab_id) {
  LOG(INFO) << "Add Icon Image Observer, extension_id=" << extension_id
            << " tab_id=" << tab_id;
  if (g_icon_image_observers.find(extension_id) != g_icon_image_observers.end()) {
    LOG(INFO) << "The observer already exists. extension_id=" << extension_id;
    return;
  }
  ExtensionIconImageObserver* observer = new ExtensionIconImageObserver(extension_id, tab_id);
  g_icon_image_observers[extension_id] = observer;
}

ExtensionIconImageObserver* GetExtensionIconImageObserver(const std::string& extension_id) {
  if (g_icon_image_observers.find(extension_id) != g_icon_image_observers.end()) {
    ExtensionIconImageObserver* observer = g_icon_image_observers[extension_id];
    return observer;
  }
  LOG(INFO) << "Not found observer! extension_id=" << extension_id;
  return nullptr;
}

void DestroyExtensionIconImageObserver(const std::string& extension_id) {
  LOG(INFO) << "Destroy Icon Image Observer, extension_id=" << extension_id;
  if (g_icon_image_observers.find(extension_id) != g_icon_image_observers.end()) {
    ExtensionIconImageObserver* observer = g_icon_image_observers[extension_id];
    g_icon_image_observers.erase(extension_id);
    delete observer;
    LOG(INFO) << "Destroy Icon Image Observer Success.";
  }
}

ExtensionIconImageObserver::ExtensionIconImageObserver(const std::string& extension_id, int32_t tab_id)
    : extension_id_(extension_id), tab_id_(tab_id) {}
ExtensionIconImageObserver::~ExtensionIconImageObserver() = default;

void ExtensionIconImageObserver::OnExtensionIconImageChanged(IconImage* icon_image) {
    LOG(INFO) << "ExtensionIconImageObserver::OnExtensionIconImageChange start";
    gfx::Image icon = icon_image->image();
    OHOS::NWeb::NWebExtensionActionCefDelegate::OnSetIcon(
        extension_id_, icon, tab_id_);
    icon_image->RemoveObserver(this);
}

}