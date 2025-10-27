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

#ifndef ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_
#define ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_

#include "chrome/browser/devtools/chrome_devtools_manager_delegate.h"

namespace ohos {

class DevToolsManagerDelegate : public ChromeDevToolsManagerDelegate {
 public:
  DevToolsManagerDelegate();

  DevToolsManagerDelegate(const DevToolsManagerDelegate&) = delete;
  DevToolsManagerDelegate& operator=(const DevToolsManagerDelegate&) = delete;

  ~DevToolsManagerDelegate() override;

 private:
  using List = content::DevToolsAgentHost::List;
  List RemoteDebuggingTargets(TargetType target_type) override;
};

} // namespace ohos
#endif // ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_
