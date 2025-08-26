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

#include "arkweb/chromium_ext/chrome/browser/devtools/devtools_manager_delegate.h"

#include <set>

#include "content/public/browser/web_contents.h"

namespace ohos {

DevToolsManagerDelegate::DevToolsManagerDelegate() = default;
DevToolsManagerDelegate::~DevToolsManagerDelegate() = default;

DevToolsManagerDelegate::List DevToolsManagerDelegate::RemoteDebuggingTargets(
    TargetType target_type) {
  auto list = ChromeDevToolsManagerDelegate::RemoteDebuggingTargets(target_type);
  if (target_type != DevToolsManagerDelegate::kTab) {
    return list;
  }

  std::set<content::WebContents*> tab_web_contents;
  List result;

  // find all tab agents.
  for (const auto& agent : list) {
    if (agent->GetType() != content::DevToolsAgentHost::kTypeTab) {
      continue;
    }
    if (content::WebContents* wc = agent->GetWebContents()) {
      tab_web_contents.insert(wc);
    }
    result.push_back(agent);
  }

  // find all other type agents not in tab.
  for (const auto& agent : list) {
    if (agent->GetType() == content::DevToolsAgentHost::kTypeTab) {
      continue;
    }
    if (content::WebContents* wc = agent->GetWebContents()) {
      if (tab_web_contents.find(wc) != tab_web_contents.end()) {
        continue;
      }
    }
    result.push_back(agent);
  }
  return result;
}

} // namespace ohos
