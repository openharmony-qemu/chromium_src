/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_LANGUAGE_API_H
#define MOCK_LANGUAGE_API_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_api_define.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base::ohos {

class LanguageMock {
 public:
  DECLARE_IMPLEMENT_INSTANCE(LanguageMock)

  DECLARE_MOCK_KEY(ComputeLanguageByRegion);

  MOCK_METHOD(std::string, ComputeLanguageByRegion, (), ());
};

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_REAL_FUNC(std::string, ComputeLanguageByRegion, ());
#ifdef __cplusplus
}
#endif
}  // namespace base::ohos
#endif  // MOCK_LANGUAGE_API_H
