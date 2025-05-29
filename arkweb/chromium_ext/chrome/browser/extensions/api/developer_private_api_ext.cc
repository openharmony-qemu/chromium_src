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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "base/datashare_uri_utils.h"
#include "extensions/browser/extension_registry_info_manager.h"
#endif // ARKWEB_ARKWEB_EXTENSIONS

namespace extensions {

namespace developer = api::developer_private;

namespace api {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
DeveloperPrivateOpenUrlFunction::
    DeveloperPrivateOpenUrlFunction() = default;

DeveloperPrivateOpenUrlFunction::
    ~DeveloperPrivateOpenUrlFunction() = default;

ExtensionFunction::ResponseAction
DeveloperPrivateOpenUrlFunction::Run() {
  std::optional<developer::OpenUrl::Params> params =
      api::developer_private::OpenUrl::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(params->url);
  return RespondNow(NoArguments());
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

}  // namespace api

}  // namespace extensions