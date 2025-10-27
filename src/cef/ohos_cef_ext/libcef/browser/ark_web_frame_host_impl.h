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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ARK_WEB_FRAME_HOST_IMPL_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ARK_WEB_FRAME_HOST_IMPL_H_

#include <cstdint>
#include <string>
#include <vector>
 
#include "cef/libcef/common/mojom/cef.mojom.h"
#include "content/public/browser/page_navigator.h"
#include "url/gurl.h"

// IS_OHOS
// kMainFrameId must be -1 to align with renderer expectations.
const int64_t kMainFrameId = -1;
const int64_t kFocusedFrameId = -2;
const int64_t kUnspecifiedFrameId = -3;
const int64_t kInvalidFrameId = -4;

GURL ArkWebFixupFileUrl(const std::string& url);
 
void ArkWebDealWithPostData(const std::string& method,
                            const std::vector<char>& post_data,
                            content::OpenURLParams* params);
void ArkWebDealWithPostUploadData(const std::string& method,
                                  const std::vector<char>& post_data,
                                  cef::mojom::RequestParamsPtr& params);

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ARK_WEB_FRAME_HOST_IMPL_H_
