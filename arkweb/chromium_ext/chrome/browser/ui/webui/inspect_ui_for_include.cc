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

#if BUILDFLAG(ARKWEB_DEVTOOLS)
bool InspectUI::InspectInclude(const std::string& source_id,
                               const std::string& target_id) {
  LOG(INFO) << " func:" << __func__;
  bool ret = true;
  scoped_refptr<DevToolsAgentHost> target = FindTarget(source_id, target_id);
  if (!target) {
    LOG(INFO) << " func:" << __func__ << "Couldn't find.";
    return ret;
  }
  std::string host_type;
  if (target->GetType() == content::DevToolsAgentHost::kTypePage) {
    host_type = "Pages";
  } else if (target->GetType() == content::DevToolsAgentHost::kTypeSharedWorker) {
    host_type = "Shared workers";
  } else if (target->GetType() == content::DevToolsAgentHost::kTypeServiceWorker) {
    host_type = "Service workers";
  } else if (target->GetType() == content::DevToolsAgentHost::kTypeExtensions) {
    host_type = "Extensions";
  } else {
    host_type = "";
  }
  RequestOpenDevToolsParams params {
    .type = host_type,
    .source_id = source_id,
    .target_id = target_id,
    .extension_id = ""
  };
  if (WebContents* web_contents = target->GetWebContents()) {
    web_contents->GetDelegate()->OnRequestOpenDevTools(&params);
  } else {
    RequestOpenDevToolsHandler::GetInstance()->OnRequestOpenDevTools(params);
  }
  return ret;
}
#endif // ARKWEB_DEVTOOLS