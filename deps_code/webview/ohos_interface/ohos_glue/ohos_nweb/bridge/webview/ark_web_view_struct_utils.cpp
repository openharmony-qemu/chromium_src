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

#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDateTime ArkWebDateTimeClassToStruct(const OHOS::NWeb::DateTime& class_value)
{
    ArkWebDateTime struct_value = { .year = class_value.year,
        .month = class_value.month,
        .day = class_value.day,
        .hour = class_value.hour,
        .minute = class_value.minute,
        .second = class_value.second };
    return struct_value;
}

OHOS::NWeb::DateTime ArkWebDateTimeStructToClass(const ArkWebDateTime& struct_value)
{
    OHOS::NWeb::DateTime class_value = { .year = struct_value.year,
        .month = struct_value.month,
        .day = struct_value.day,
        .hour = struct_value.hour,
        .minute = struct_value.minute,
        .second = struct_value.second };
    return class_value;
}

} // namespace OHOS::ArkWeb
