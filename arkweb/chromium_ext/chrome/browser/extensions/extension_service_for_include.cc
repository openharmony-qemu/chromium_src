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
 
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif
 
namespace extensions {
 
namespace {
#if BUILDFLAG(ARKWEB_NWEB_EX)
void NotifyOnInstalledExtensionsLoadedInFileTask() {
  content::GetUIThreadTaskRunner({})
          ->PostTask(FROM_HERE,base::BindOnce(&NWebExtensionManagerDispatcher::OnExtensionInitLoadEndCallBack));
}
 
void NotifyOnInstalledExtensionsLoadedInUITask() {
  GetExtensionFileTaskRunner()->PostTask(
        FROM_HERE, base::BindOnce(&NotifyOnInstalledExtensionsLoadedInFileTask));
}
 
void NotifyOnInstalledExtensionsLoaded() {
  content::GetUIThreadTaskRunner({base::TaskPriority::USER_VISIBLE})
        ->PostTask(FROM_HERE, base::BindOnce(&NotifyOnInstalledExtensionsLoadedInUITask));
} 
#endif 
}  // namespace
}  // namespace extensions