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

#ifndef NWEB_CONTROLLER_HANDLER_IMPL_H
#define NWEB_CONTROLLER_HANDLER_IMPL_H

#include "nweb_controller_handler.h"

namespace OHOS::NWeb {
class NWebControllerHandlerImpl : public NWebControllerHandler {
 public:
  NWebControllerHandlerImpl() = default;
  explicit NWebControllerHandlerImpl(int32_t id, bool isFrist)
      : id_(id), isFrist_(isFrist), nwebId_(-1) {}
  ~NWebControllerHandlerImpl() = default;

  void SetNWebHandlerById(int32_t nwebId) override;
  int32_t GetNWebHandlerId() override;
  bool IsFrist() override;
  int32_t GetId() override;

 private:
  int32_t id_;
  int32_t isFrist_;
  int32_t nwebId_;
};
}  // namespace OHOS::NWeb
#endif
