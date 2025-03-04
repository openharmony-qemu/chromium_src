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

#ifndef NWEB_IMF_INPUT_ATTRIBUTE_ADAPTER_IMPL_H
#define NWEB_IMF_INPUT_ATTRIBUTE_ADAPTER_IMPL_H

#include "imf_adapter.h"

namespace OHOS::NWeb {

class NWebIMFInputAttributeAdapterImpl : public IMFInputAttributeAdapter {
 public:
  NWebIMFInputAttributeAdapterImpl() = default;

  int32_t GetInputPattern() override;

  int32_t GetEnterKeyType() override;

  void SetInputPattern(int32_t inputPattern);

  void SetEnterKeyType(int32_t enterKeyType);

 private:
  int32_t input_pattern_ = 0;
  int32_t enterKey_type_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_IMF_INPUT_ATTRIBUTE_ADAPTER_IMPL_H
