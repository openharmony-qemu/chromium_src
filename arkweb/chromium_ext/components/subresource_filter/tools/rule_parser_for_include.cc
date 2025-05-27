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

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/strings/string_number_conversions.h"
#endif

namespace subresource_filter {

bool ParseExt(size_t& css_separator_pos,
              std::string_view& part,
              const char next_char) {
  constexpr int css_number = 2;
  if (css_separator_pos + css_number < part.size()) {
    // skip "#?#", extended css selector not supported yet
    // and "#?##" should not be considered normal css rule.
    const char next_next_char = part[css_separator_pos + 2];
    if (next_char == '?' && next_next_char == '#') {
      css_separator_pos = std::string_view::npos;
      return false;
    }
    return true;
  }
  return true;
}

}  // namespace subresource_filter
