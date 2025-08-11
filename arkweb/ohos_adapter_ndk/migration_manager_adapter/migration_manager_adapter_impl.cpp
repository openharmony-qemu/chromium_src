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
#include "migration_manager_adapter_impl.h"

#include "nweb_log.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
void MigrationManagerAdapterImpl::SetMigrationParam(
    const std::string& bundleName, const std::string& abilityName, const std::string& token)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

bool MigrationManagerAdapterImpl::SendMigrationRequest(std::shared_ptr<std::string> jsonData)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return false;
}

uint32_t MigrationManagerAdapterImpl::RegisterMigrationListener(std::shared_ptr<MigrationListenerAdapter> listener)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return -1;
}

} // namespace OHOS::NWeb

