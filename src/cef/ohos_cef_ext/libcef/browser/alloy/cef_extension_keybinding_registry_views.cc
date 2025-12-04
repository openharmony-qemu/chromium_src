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

#include "libcef/browser/alloy/cef_extension_keybinding_registry_views.h"

#include "chrome/browser/extensions/extension_commands_global_registry.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/content_accelerators/accelerator_util.h"

CefExtensionKeybindingRegistryViews::CefExtensionKeybindingRegistryViews(
    content::BrowserContext* context,
    raw_ptr<Delegate> delegate)
    : ExtensionKeybindingRegistry(
          context,
          extensions::ExtensionKeybindingRegistry::ALL_EXTENSIONS,
          delegate),
      browser_context_profile(context) {
  Init();
}

// Overridden from ui::AcceleratorTarget.
bool CefExtensionKeybindingRegistryViews::AcceleratorPressed(
    const ui::Accelerator& accelerator) {
  return ExtensionKeybindingRegistry::NotifyEventTargets(accelerator);
}

