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

#ifndef NWEB_IMF_TEXT_CONFIG_ADAPTER_IMPL_H
#define NWEB_IMF_TEXT_CONFIG_ADAPTER_IMPL_H

#include "imf_adapter.h"

namespace OHOS::NWeb {

class NWebIMFTextConfigAdapterImpl : public IMFTextConfigAdapter {
 public:
  NWebIMFTextConfigAdapterImpl() = default;

  std::shared_ptr<IMFInputAttributeAdapter> GetInputAttribute() override;

  std::shared_ptr<IMFCursorInfoAdapter> GetCursorInfo() override;

  std::shared_ptr<IMFSelectionRangeAdapter> GetSelectionRange() override;

  uint32_t GetWindowId() override;

  void SetInputAttribute(std::shared_ptr<IMFInputAttributeAdapter> attribute);

  void SetCursorInfo(std::shared_ptr<IMFCursorInfoAdapter> info);

  void SetSelectionRange(std::shared_ptr<IMFSelectionRangeAdapter> range);

  void SetWindowId(uint32_t id);

 private:
  std::shared_ptr<IMFInputAttributeAdapter> input_attribute_ = nullptr;
  std::shared_ptr<IMFCursorInfoAdapter> cursor_info_ = nullptr;
  std::shared_ptr<IMFSelectionRangeAdapter> range_ = nullptr;
  uint32_t window_id_ = -1;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_IMF_TEXT_CONFIG_ADAPTER_IMPL_H
