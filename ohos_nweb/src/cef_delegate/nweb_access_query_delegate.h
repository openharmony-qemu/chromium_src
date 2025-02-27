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

#ifndef NWEB_ACCESS_QUERY_DELEGATE_H_
#define NWEB_ACCESS_QUERY_DELEGATE_H_

#include <string>
#include <memory>

#include "cef/include/cef_permission_status_query.h"
#include "ohos_nweb/src/nweb_access_query.h"

namespace OHOS::NWeb {

class NWebAccessQueryDelegate : public NWebAccessQuery {
 public:
  NWebAccessQueryDelegate(CefRefPtr<CefAccessQuery>);
  ~NWebAccessQueryDelegate() = default;

  std::string Origin() override;

  int ResourceAcessId() override;

  void ReportQueryResult(int32_t status) override;

 private:
  CefRefPtr<CefAccessQuery> query_ = nullptr;
};

}

#endif // NWEB_ACCESS_QUERY_DELEGATE_H_

