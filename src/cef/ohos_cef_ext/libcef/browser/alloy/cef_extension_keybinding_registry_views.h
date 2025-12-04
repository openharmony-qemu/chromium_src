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

#ifndef CEF_EXTENSIONS_KEYBINDING_REGISTRY_VIEWS_H_
#define CEF_EXTENSIONS_KEYBINDING_REGISTRY_VIEWS_H_

#include <string>

#include "chrome/browser/extensions/extension_keybinding_registry.h"
#include "ui/base/accelerators/command.h"

// CefExtensionKeybindingRegistryViews is a class that handles specific
// implementation of the Extension Keybinding shortcuts (keyboard accelerators).

class CefExtensionKeybindingRegistryViews
    : public extensions::ExtensionKeybindingRegistry {
 public:
  CefExtensionKeybindingRegistryViews(content::BrowserContext* context,
      raw_ptr<Delegate> delegate);

  CefExtensionKeybindingRegistryViews(
      const CefExtensionKeybindingRegistryViews&) = delete;
  CefExtensionKeybindingRegistryViews& operator=(
      const CefExtensionKeybindingRegistryViews&) = delete;

  ~CefExtensionKeybindingRegistryViews() override {}

  // Overridden from ui::AcceleratorTarget.
  bool AcceleratorPressed(const ui::Accelerator& accelerator);
  bool CanHandleAccelerators() const { return true; }

 private:
  // Overridden from ExtensionKeybindingRegistry:
  bool PopulateCommands(const extensions::Extension* extension,
                        ui::CommandMap* commands) override;
  bool RegisterAccelerator(const ui::Accelerator& accelerator,
                           const extensions::ExtensionId& extension_id,
                           const std::string& command_name) override;
  void OnShortcutHandlingSuspended(bool suspended) override {}

  raw_ptr<content::BrowserContext> browser_context_profile;

  raw_ptr<Delegate> delegate_;
};

#endif  // CEF_EXTENSIONS_KEYBINDING_REGISTRY_VIEWS_H_
