/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "datashare_adapter_impl.h"

#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>

#include "nweb_log.h"
#include "uri.h"
#include "file_uri.h"
#include "datashare_helper.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"

namespace OHOS::NWeb {

constexpr char MEDIALIBRARY_DATA_URI[] = "datashare:///media";
// static
DatashareAdapterImpl& DatashareAdapterImpl::GetInstance()
{
    static DatashareAdapterImpl instance;
    return instance;
}

std::string DatashareAdapterImpl::GetRealPath(const std::string& uriStr)
{
    Uri uri = Uri(uriStr);
    AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    return fileUri.GetRealPath();
}

int DatashareAdapterImpl::OpenDataShareUriForRead(const std::string& uriStr)
{
    auto sam = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        WVLOG_E("open datashare uri read, system ability manager is null");
        return -1;
    }
    auto remoteObj = sam->GetSystemAbility(OHOS::STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        WVLOG_E("open datashare uri read, remoteObj is null");
        return -1;
    }
    auto dataShareHelper = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI);
    if (dataShareHelper == nullptr) {
        WVLOG_E("open datashare uri read, dataShareHelper is null");
        return -1;
    }
    Uri uri = Uri(uriStr);
    return dataShareHelper->OpenFile(uri, "r");
}

std::string DatashareAdapterImpl::GetFileDisplayName(const std::string& uriStr)
{
    AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    return fileUri.GetName();
}
} // namespace OHOS::NWeb
