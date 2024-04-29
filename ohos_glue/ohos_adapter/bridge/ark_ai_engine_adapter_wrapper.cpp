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

#include "ohos_adapter/bridge/ark_ai_engine_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkAiEngineAdapterWrapper::ArkAiEngineAdapterWrapper(
    ArkWebRefPtr<ArkAiEngineAdapter> ref)
    : ctocpp_(ref) {
}

std::vector<int8_t>
ArkAiEngineAdapterWrapper::GetWordSelection(const std::string &text,
                                            int8_t offset) {
  if (!ctocpp_) {
    return {-1, -1};
  }
  ArkWebString ark_text = ArkWebStringClassToStruct(text);
  ArkWebCharVector ark_result = ctocpp_->GetWordSelection(ark_text, offset);
  std::vector<int8_t> result =
      ArkWebBasicVectorStructToClass<int8_t, ArkWebCharVector>(ark_result);

  ArkWebBasicVectorStructRelease(ark_result);
  ArkWebStringStructRelease(ark_text);
  return result;
}

} // namespace OHOS::ArkWeb
