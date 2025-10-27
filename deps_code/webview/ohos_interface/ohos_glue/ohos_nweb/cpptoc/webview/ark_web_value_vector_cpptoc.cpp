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

#include "ohos_nweb/cpptoc/ark_web_value_vector_cpptoc.h"

#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "base/include/ark_web_macros.h"

namespace OHOS::ArkWeb {

ArkWebValue* ArkValueVectorNew(int size)
{
    ArkWebValue* value = nullptr;
    if (size > 0) {
        value = new ArkWebValue[size];
    }

    ARK_WEB_CPPTOC_DV_LOG("address is %{public}ld,size is %{public}d", (long)value, size);

    return value;
}

void ArkValueVectorDelete(void* mem)
{
    ArkWebValue* value = (ArkWebValue*)mem;
    ARK_WEB_CPPTOC_DV_LOG("address is %{public}ld", (long)value);

    if (value) {
        delete[] value;
    }
}

ArkWebValueVector ArkWebValueVectorClassToStruct(const std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>>& class_value)
{
    ArkWebValueVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkValueVectorDelete };
    if (struct_value.size > 0) {
        struct_value.value = ArkValueVectorNew(struct_value.size);

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.value[count].nweb_value = *it;
            count++;
        }
    }

    return struct_value;
}

void ArkWebValueVectorStructRelease(ArkWebValueVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb
