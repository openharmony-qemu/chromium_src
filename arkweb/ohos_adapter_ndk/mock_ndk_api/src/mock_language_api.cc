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

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_language_api.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_api_define.h"

namespace base::ohos {

IMPLEMENT_MOCK_KEY(LanguageMock,ComputeLanguageByRegion);
  
#ifdef __cplusplus
extern "C" {
#endif

IMPLEMENT_WRAP_FUNCTION(std::string, LanguageMock, ComputeLanguageByRegion, ());
#ifdef __cplusplus
}
#endif
}  // namespace base::ohos
