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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_COMMON_NET_URL_UTIL_EX_H_
#pragma once

#include "cef/include/cef_base.h"

class GURL;

namespace url_util {

GURL FixupGURL(const std::string& url);

}  // namespace url_util

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_COMMON_NET_URL_UTIL_EX_H_
