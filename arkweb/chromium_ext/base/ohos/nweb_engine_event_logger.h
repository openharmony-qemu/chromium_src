/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_H_
#define BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_H_

#include <stdint.h>
#include <string>
#include <functional>

#include "base/base_export.h"

namespace base {
namespace ohos {

using UploadCallbackFunc = std::function<void(const std::string&,
                                              const std::string&,
                                              const std::string&,
                                              const std::string&)>;

BASE_EXPORT void SetUploadCallback(UploadCallbackFunc callbabck);

BASE_EXPORT void ReportEngineEvent(const std::string& module,
                                   const std::string& resource,
                                   const std::string& error_code,
                                   const std::string& error_msg);

}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_H_