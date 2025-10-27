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

#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_EXTENSION_ACTION_BROWSER_NOTIFIER_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_EXTENSION_ACTION_BROWSER_NOTIFIER_H_

#include "base/scoped_observation.h"
#include "chrome/browser/extensions/extension_action_dispatcher.h"

namespace extensions {

class ExtensionActionBrowserNotifier
    : public ExtensionActionDispatcher::Observer {
 public:
  static ExtensionActionBrowserNotifier* GetInstance();

  void StartObservingActionDispatcher(content::BrowserContext* browser_content);

 private:
  // ExtensionActionDispatcher::Observer overrides:
  void OnExtensionActionUpdated(
      ExtensionAction* extension_action,
      content::WebContents* web_contents,
      content::BrowserContext* browser_context) override;

  base::ScopedObservation<ExtensionActionDispatcher,
                          ExtensionActionDispatcher::Observer>
      extension_action_dispatcher_observation_{this};
};

}  // namespace extensions

#endif  // CEF_LIBCEF_BROWSER_EXTENSIONS_EXTENSION_ACTION_BROWSER_NOTIFIER_H_
