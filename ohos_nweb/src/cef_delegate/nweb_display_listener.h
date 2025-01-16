/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_DISPLAY_LISTENER_H
#define NWEB_DISPLAY_LISTENER_H

#include <memory>

#include "nweb_impl.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
class DisplayScreenListener : public DisplayListenerAdapter {
 public:
  explicit DisplayScreenListener(std::shared_ptr<NWebDelegateInterface> nweb);
  ~DisplayScreenListener() = default;
  void OnCreate(DisplayId id) override;
  void OnDestroy(DisplayId id) override;
  void OnChange(DisplayId id) override;

 private:
  std::weak_ptr<NWebDelegateInterface> nweb_;
  std::unique_ptr<DisplayManagerAdapter> display_manager_adapter_;
};

class FoldStatusScreenListener : public FoldStatusListenerAdapter {
 public:
  explicit FoldStatusScreenListener(std::shared_ptr<NWebDelegateInterface> nweb);
  ~FoldStatusScreenListener() = default;
  void OnFoldStatusChanged(FoldStatus foldStatus) override;

 private:
  std::weak_ptr<NWebDelegateInterface> nweb_;
  std::unique_ptr<DisplayManagerAdapter> foldstatus_manager_adapter_;
};
}  // namespace OHOS::NWeb
#endif
