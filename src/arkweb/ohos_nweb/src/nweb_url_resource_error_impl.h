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

#ifndef NWEB_URL_RESOURCE_ERROR_IMPL
#define NWEB_URL_RESOURCE_ERROR_IMPL

#include "nweb_url_resource_error.h"

namespace OHOS::NWeb {
class UrlResourceErrorImpl : public NWebUrlResourceError {
 public:
  UrlResourceErrorImpl(int error_code, const std::string& error_info)
      : error_code_(error_code), error_info_(error_info) {}
  ~UrlResourceErrorImpl() = default;

  std::string ErrorInfo() override { return error_info_; }
  int ErrorCode() override { return error_code_; }

 private:
  int error_code_;
  std::string error_info_;
};
}  // namespace OHOS::NWeb
#endif  // NWEB_URL_RESOURCE_ERROR_IMPL
