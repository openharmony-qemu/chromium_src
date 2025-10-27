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

#include "ohos_nweb/cpptoc/ark_web_hap_value_vector_cpptoc.h"

#include "ohos_nweb/bridge/ark_web_hap_value_impl.h"
#include "ohos_nweb/cpptoc/ark_web_hap_value_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE ArkWebHapValueMap ArkWebHapValueMapClassToStruct(
    const std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebHapValue>>& class_value)
{
    ArkWebHapValueMap struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.value = (ark_web_hap_value_t**)ArkWebMemMalloc(sizeof(ark_web_hap_value_t*) * struct_value.size);
        if ((struct_value.key == nullptr) || (struct_value.value == nullptr)) {
            struct_value.size = 0;
            SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
            SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.key[count] = ArkWebStringClassToStruct(it->first);
            ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value = new ArkWebHapValueImpl(it->second);
            struct_value.value[count] = ArkWebHapValueCppToC::Invert(ark_web_hap_value);
            count++;
        }
    }

    return struct_value;
}

ARK_WEB_NO_SANITIZE ArkWebHapValueVector ArkWebHapValueVectorClassToStruct(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>>& class_value)
{
    ArkWebHapValueVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (ark_web_hap_value_t**)ArkWebMemMalloc(sizeof(ark_web_hap_value_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value = new ArkWebHapValueImpl(*it);
            struct_value.value[count] = ArkWebHapValueCppToC::Invert(ark_web_hap_value);
            count++;
        }
    }

    return struct_value;
}

ARK_WEB_NO_SANITIZE void ArkWebHapValueVectorStructRelease(ArkWebHapValueVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb
 