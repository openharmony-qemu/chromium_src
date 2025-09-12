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

#include <gtest/gtest.h>
#include <string>

#include "base/check.h"
#include "build/build_config.h"
#include "build/buildflag.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
namespace subresource_filter {
bool ParseExt(size_t& css_separator_pos,
              std::string_view& part,
              const char next_char);

TEST(ParseExtTest, ReturnsFalseAndNposForExtendedSelector) {
  size_t pos = 1;
  std::string str = "a#?##b";
  std::string_view part(str);
  // next_char = '?', next_next_char = '#'
  bool result = ParseExt(pos, part, '?');
  EXPECT_FALSE(result);
  EXPECT_EQ(pos, std::string_view::npos);
}

TEST(ParseExtTest, ReturnsTrueForNormalSelector) {
  size_t pos = 1;
  std::string str = "a#?ab";
  std::string_view part(str);
  // next_char = '?', next_next_char = 'a'
  bool result = ParseExt(pos, part, '?');
  EXPECT_TRUE(result);
  EXPECT_EQ(pos, 1u);
}

TEST(ParseExtTest, ReturnsTrueWhenNotEnoughLength) {
  size_t pos = 3;
  std::string str = "abc";
  std::string_view part(str);
  // css_separator_pos + 2 >= part.size()
  bool result = ParseExt(pos, part, '#');
  EXPECT_TRUE(result);
  EXPECT_EQ(pos, 3u);
}
}  // namespace subresource_filter
#endif