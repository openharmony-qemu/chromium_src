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

#include "nweb_web_storage_origin_impl.h"

namespace OHOS::NWeb {

void NWebWebStorageOriginImpl::SetOrigin(const std::string& origin) {
  origin_ = origin;
}

void NWebWebStorageOriginImpl::SetQuota(long quota) {
  quota_ = quota;
}

void NWebWebStorageOriginImpl::SetUsage(long usage) {
  usage_ = usage;
}

std::string NWebWebStorageOriginImpl::GetOrigin() {
  return origin_;
}

long NWebWebStorageOriginImpl::GetQuota() {
  return quota_;
}

long NWebWebStorageOriginImpl::GetUsage() {
  return usage_;
}

}  // namespace OHOS::NWeb
