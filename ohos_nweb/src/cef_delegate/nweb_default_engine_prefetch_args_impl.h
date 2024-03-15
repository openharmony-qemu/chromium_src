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

#ifndef WEB_DEFAULT_ENGINE_PREFETCH_ARGS_IMPL_H
#define WEB_DEFAULT_ENGINE_PREFETCH_ARGS_IMPL_H

#include "nweb.h"

namespace OHOS::NWeb {

class NWebDefaultEnginePrefetchArgsImpl : public NWebEnginePrefetchArgs {
public:
  NWebDefaultEnginePrefetchArgsImpl(const std::string& url,
                             const std::string& method,
                             const std::string& formData)
                             : url_(url), method_(method), form_data_(formData) {}

  ~NWebDefaultEnginePrefetchArgsImpl() = default;

  std::string GetUrl() override;

  std::string GetMethod() override;

  std::string GetFormData() override;

private:
  std::string url_;
  std::string method_;
  std::string form_data_;
};

} // namespace OHOS::NWeb

#endif // WEB_DEFAULT_ENGINE_PREFETCH_ARGS_IMPL_H
