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

#include "nweb_extension_prompt_cef_delegate.h"

#include <map>

#include "base/logging.h"
#include "nweb_icon_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_prompt_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {

static int g_request_id = 0;

static std::map<int, ShowPromptCallbackFunc> g_show_prompt_callbacks;

static std::map<int, GetPromptDataCallbackFunc> g_get_prompt_data_callbacks;

static std::map<int, std::vector<std::string>> g_prompt_extension_ids;

}  // namespace

NWebExtensionPromptCefDelegate& NWebExtensionPromptCefDelegate::GetInstance() {
  static NWebExtensionPromptCefDelegate instance;
  return instance;
}

NWebExtensionPromptData* NWebExtensionPromptCefDelegate::GetPromptData(int id) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (g_get_prompt_data_callbacks.count(id)) {
    LOG(INFO) << "succeed to find callback,id is " << id;
    NWebExtensionPromptData* data =
        (NWebExtensionPromptData*)calloc(1, sizeof(NWebExtensionPromptData));
    if (!data) {
      return nullptr;
    }

    std::move(g_get_prompt_data_callbacks[id]).run(data);
    return data;
  }

  LOG(WARNING) << "failed to find callback,id is " << id;
#endif

  return nullptr;
}

std::vector<std::string> NWebExtensionPromptCefDelegate::GetIdList(int id) {
  auto iter = g_prompt_extension_ids.find(id);
  if (iter == g_prompt_extension_ids.end()) {
    return {};
  }
  return iter->second;
}

void NWebExtensionPromptCefDelegate::FreePromptData(void* addr) {
  NWebExtensionPromptData* data = (NWebExtensionPromptData*)addr;
  if (!data) {
    return;
  }

  if (data->title) {
    free(data->title);
  }

  if (data->abortButtonLabel) {
    free(data->abortButtonLabel);
  }

  if (data->acceptButtonLabel) {
    free(data->acceptButtonLabel);
  }

  if (data->permissionsHeading) {
    free(data->permissionsHeading);
  }

  if (data->permissions) {
    for (uint32_t i = 0; i < data->permissionCount; i++) {
      auto item = &data->permissions[i];
      if (item->detail) {
        free(item->detail);
      }

      if (item->permission) {
        free(item->permission);
      }

      if (item->hideDetailLabel) {
        free(item->hideDetailLabel);
      }

      if (item->showDetailLabel) {
        free(item->showDetailLabel);
      }
    }
    free(data->permissions);
  }

  free(data);
}

bool NWebExtensionPromptCefDelegate::ShowExtensionPrompt(
    PromptType type,
    const std::string& extensionId,
    const gfx::Image& icon_image,
    ShowPromptCallbackFunc showPromptFunc,
    GetPromptDataCallbackFunc getPromptDataFunc) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  NWebExtensionActionIcon* icon = CreateActionIcon(icon_image);
  g_show_prompt_callbacks[id] = showPromptFunc;
  g_get_prompt_data_callbacks[id] = getPromptDataFunc;
  if (NWebExtensionPromptDispatcher::GetInstance().ShowExtensionPrompt(
          id, type, extensionId.c_str(), icon)) {
    LOG(INFO) << "succeed to notify client to show extension prompt,id is "
              << id << ",type is " << type;
    ReleaseActionIcon(icon);
    return;
  }

  LOG(WARNING) << "failed to notify client to show extension prompt,id is "
               << id << ",type is " << type;
  ReleaseActionIcon(icon);

  std::move(g_show_prompt_callbacks[id])
      .run(2, "failed to notify client to show extension prompt");
  g_show_prompt_callbacks.erase(id);
  g_get_prompt_data_callbacks.erase(id);
#endif

  return false;
}

bool NWebExtensionPromptCefDelegate::ShowMultiExtensionUninstallPrompt(
    const std::vector<std::string>& extension_ids,
    ShowPromptCallbackFunc show_prompt_func) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  g_prompt_extension_ids[id] = extension_ids;
  g_show_prompt_callbacks[id] = std::move(show_prompt_func);

  std::string empty_id;
  if (NWebExtensionPromptDispatcher::GetInstance().ShowExtensionPrompt(
      id, PROMPT_MULTI_UNINSTALLATION, empty_id.c_str(), nullptr)) {
    LOG(INFO) << "succeed to call show multi-extensions prompt,id is " << id;
    return true;
  }

  LOG(WARNING) << "failed to call show multi-extension prompt,id is " << id;
  g_prompt_extension_ids.erase(id);
  g_show_prompt_callbacks.erase(id);
#endif
  return false;
}

void NWebExtensionPromptCefDelegate::OnShowExtensionPrompt(int id,
                                                           int action,
                                                           const char* error) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  std::string strErroe = error ? error : "";
  if (g_show_prompt_callbacks.count(id)) {
    LOG(INFO) << "succeed to find callback,id is " << id << ",action is "
              << action << ",error is " << strError;
    std::move(g_show_prompt_callbacks[id]).run(action, strError);
    g_show_prompt_callbacks.erase(id);
    g_get_prompt_data_callbacks.erase(id);
    g_prompt_extension_ids.erase(id);
    return;
  }

  LOG(WARNING) << "failed to find callback,id is " << id << ",action is "
               << action << ",error is " << strError;
#endif
}

}  // namespace OHOS::NWeb
