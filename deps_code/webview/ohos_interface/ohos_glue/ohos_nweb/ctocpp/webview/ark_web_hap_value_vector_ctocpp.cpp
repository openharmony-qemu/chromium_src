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

#include "ohos_nweb/ctocpp/ark_web_hap_value_vector_ctocpp.h"

#include "ohos_nweb/bridge/ark_web_hap_value_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_hap_value_ctocpp.h"

namespace OHOS::ArkWeb {

std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebHapValue>> ArkWebHapValueMapStructToClass(
    const ArkWebHapValueMap& struct_value)
{
    std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebHapValue>> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::string key = ArkWebStringStructToClass(struct_value.key[count]);
            std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value =
                std::make_shared<ArkWebHapValueWrapper>(ArkWebHapValueCToCpp::Invert(struct_value.value[count]));
            class_value[key] = nweb_hap_value;
        }
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebHapValueMapStructRelease(ArkWebHapValueMap& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>> ArkWebHapValueVectorStructToClass(
    const ArkWebHapValueVector& struct_value)
{
    std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value =
                std::make_shared<ArkWebHapValueWrapper>(ArkWebHapValueCToCpp::Invert(struct_value.value[count]));
            class_value.push_back(nweb_hap_value);
        }
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebHapValueVectorStructRelease(ArkWebHapValueVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb
