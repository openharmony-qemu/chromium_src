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

#ifndef NWEB_IMF_SELECTION_RANGE_ADAPTER_IMPL_H
#define NWEB_IMF_SELECTION_RANGE_ADAPTER_IMPL_H

#include "imf_adapter.h"

namespace OHOS::NWeb {

class NWebIMFSelectionRangeAdapterImpl : public IMFSelectionRangeAdapter {
 public:
  NWebIMFSelectionRangeAdapterImpl() = default;

  int32_t GetStart() override;

  int32_t GetEnd() override;

  void SetStart(int32_t start);

  void SetEnd(int32_t end);

 private:
  int32_t start_ = -1;
  int32_t end_ = -1;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_IMF_SELECTION_RANGE_ADAPTER_IMPL_H
