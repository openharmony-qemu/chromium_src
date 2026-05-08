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
std::string GetTypeFromHost(const std::string& hostType) {
  std::string host_type;
  if (hostType == content::DevToolsAgentHost::kTypePage) {
    host_type = "Pages";
  } else if (hostType == content::DevToolsAgentHost::kTypeSharedWorker) {
    host_type = "Shared workers";
  } else if (hostType == content::DevToolsAgentHost::kTypeServiceWorker) {
    host_type = "Service workers";
  } else if (hostType == content::DevToolsAgentHost::kTypeExtensions) {
    host_type = "Extensions";
  } else {
    host_type = "Other";
  }
  return host_type;
}
void InspectServiceWorkerBackgroundV2(content::WebContents* web_contents,
                                      const Extension* extension,
                                      Profile* profile,
                                      DevToolsOpenedByAction opened_by) {
  DCHECK(BackgroundInfo::IsServiceWorkerBased(extension));
  content::DevToolsAgentHost::List targets =
      content::DevToolsAgentHost::GetOrCreateAll();
  for (const scoped_refptr<content::DevToolsAgentHost>& host : targets) {
    if (host->GetBrowserContext() != profile) {
      continue;
    }
    RequestOpenDevToolsParams params = {
      .type = GetTypeFromHost(host->GetType()),
      .source_id = host->GetURL().spec(),
      .target_id = host->GetId(),
      .extension_id = extension->id(),
      .panel = ""
    };
    if (host->GetType() == content::DevToolsAgentHost::kTypeServiceWorker ||
        host->GetType() == content::DevToolsAgentHost::kTypeSharedWorker ||
        host->GetURL().spec().find(extension->id()) == std::string::npos) {
      continue;
    }
    content::WebContents* host_web_contents = host->GetWebContents();
    if (web_contents && (host_web_contents == web_contents) && web_contents->GetDelegate()) {
      web_contents->GetDelegate()->OnRequestOpenDevTools(&params);
      break;
    }
  }
}

void OnRequestOpenDevTools(const scoped_refptr<content::DevToolsAgentHost>& host,
                           const Extension* extension) {
  content::WebContents* web_contents = host->GetWebContents();
  RequestOpenDevToolsParams params = {
    .type = GetTypeFromHost(host->GetType()),
    .source_id = host->GetURL().spec(),
    .target_id = host->GetId(),
    .extension_id = extension->id(),
    .panel = ""
  };

  if (web_contents && web_contents->GetDelegate()) {
    web_contents->GetDelegate()->OnRequestOpenDevTools(&params);
  } else {
    RequestOpenDevToolsHandler::GetInstance()->OnRequestOpenDevTools(params);
  }
}

void OnRequestOpenDevToolsWithContext(std::unique_ptr<LazyContextTaskQueue::ContextInfo>& context_info) {
  if (!context_info) {
      return;
  }

  const Extension* extension =
      ExtensionRegistry::Get(context_info->browser_context)
          ->enabled_extensions()
          .GetByID(context_info->extension_id);
  auto web_contents = context_info->web_contents;
  if (!web_contents || !extension) {
    return;
  }
  RequestOpenDevToolsParams params = {
    .type = "Extensions",
    .source_id = web_contents->GetURL().spec(),
    .target_id = "",
    .extension_id = extension->id(),
    .panel = ""
  };

  if (web_contents->GetDelegate()) {
    web_contents->GetDelegate()->OnRequestOpenDevTools(&params);
  }
}
#endif // ARKWEB_DEVTOOLS

}  // namespace devtools_util
}  // namespace extensions