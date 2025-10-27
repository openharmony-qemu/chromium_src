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

#include "libcef/common/net/url_util_ex.h"

#include "base/logging.h"
#include "components/url_formatter/url_fixer.h"

namespace url_util {

GURL FixupGURL(const std::string& url) {
  GURL fixup_url = url_formatter::FixupURL(url, std::string());
  if (fixup_url.is_valid()) {
    return fixup_url;
  }
  return GURL();
}

}  // namespace url_util
