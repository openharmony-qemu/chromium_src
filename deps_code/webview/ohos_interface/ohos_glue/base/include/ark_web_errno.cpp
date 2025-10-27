/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/include/ark_web_errno.h"

#include "base/include/ark_web_log_utils.h"

static thread_local ArkWebInterfaceResult interface_result = RESULT_OK;

void ArkWebSetErrno(ArkWebInterfaceResult result)
{
    interface_result = result;
    ARK_WEB_BASE_DV_LOG("ark web errno is %{public}d", interface_result);
}

ArkWebInterfaceResult ArkWebGetErrno()
{
    return interface_result;
}

ArkWebInterfaceResult ArkWebResetErrno()
{
    ArkWebInterfaceResult result = interface_result;
    interface_result = RESULT_OK;
    return result;
}
