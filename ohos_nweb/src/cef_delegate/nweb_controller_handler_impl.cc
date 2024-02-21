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

#include "nweb_controller_handler_impl.h"

using namespace OHOS::NWeb;

void NWebControllerHandlerImpl::SetNWebHandlerById(int32_t nwebId) {
  nwebId_ = nwebId;
}

int32_t NWebControllerHandlerImpl::GetNWebHandlerId() {
  return nwebId_;
}

bool NWebControllerHandlerImpl::IsFrist() {
  return isFrist_;
}

int32_t NWebControllerHandlerImpl::GetId() {
  return id_;
}
