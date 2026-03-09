/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

namespace extensions {
namespace devtools_util {

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void InspectServiceWorkerBackgroundV2(const Extension* extension,
                                      Profile* profile,
                                      DevToolsOpenedByAction opened_by) {
  DCHECK(BackgroundInfo::IsServiceWorkerBased(extension));
  content::DevToolsAgentHost::List targets =
      content::DevToolsAgentHost::GetOrCreateAll();
  for (const scoped_refptr<content::DevToolsAgentHost>& host : targets) {
    if (host->GetBrowserContext() == profile) {
      RequestOpenDevToolsParams params = {
        .type = host->GetType(),
        .source_id = host->GetURL().spec(),
        .target_id = host->GetId(),
        .extension_id = extension->id()
      };
      RequestOpenDevToolsHandler::GetInstance()->OnRequestOpenDevTools(params);
      break;
    }
  }
}

void OnRequestOpenDevTools(const scoped_refptr<content::DevToolsAgentHost>& host,
                           const Extension* extension) {
  content::WebContents* web_contents = host->GetWebContents();
  RequestOpenDevToolsParams params = {
    .type = host->GetType(),
    .source_id = host->GetURL().spec(),
    .target_id = host->GetId(),
    .extension_id = extension->id()
  };

  if (web_contents && web_contents->GetDelegate()) {
    web_contents->GetDelegate()->OnRequestOpenDevTools(&params);
  } else {
    RequestOpenDevToolsHandler::GetInstance()->OnRequestOpenDevTools(params);
  }
}
#endif // ARKWEB_DEVTOOLS

}  // namespace devtools_util
}  // namespace extensions