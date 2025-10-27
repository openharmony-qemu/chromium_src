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

#include "nweb_access_query_delegate.h"

#include "base/logging.h"

namespace OHOS::NWeb {

NWebAccessQueryDelegate::NWebAccessQueryDelegate(
    CefRefPtr<CefAccessQuery> query)
    : query_(query) {}

std::string NWebAccessQueryDelegate::Origin() {
  if (query_ != nullptr) {
    return query_->Origin();
  }

  LOG(ERROR) << "NWebAccessQueryDelegate get Origin fail";
  return "";
}

int NWebAccessQueryDelegate::ResourceAcessId() {
  if (query_ != nullptr) {
    return query_->ResourceAcessId();
  }

  LOG(ERROR) << "NWebAccessQueryDelegate get ResourceAcessId fail";
  return -1;
}

void NWebAccessQueryDelegate::ReportQueryResult(int32_t status) {
  if (query_ != nullptr) {
    query_->ReportQueryResult(status);
    return;
  }

  LOG(ERROR) << "NWebAccessQueryDelegate ReportQueryResult fail";
}

}  // namespace OHOS::NWeb
