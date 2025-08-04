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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/bindings/parkable_string_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string_manager.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string.h"

namespace blink {

constexpr size_t kSizeKb = 20;

String MakeLargeString(char c = 'a') {
  Vector<char> data(kSizeKb * 1000, c);
  return String(data).ReleaseImpl();
}

class ParkableStringUtilsTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(ParkableStringUtilsTest, NullImplReturnsNull) {
  scoped_refptr<StringImpl> null_impl;
  auto result = ParkableStringUtils::GetParkableString(std::move(null_impl), true);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ParkableStringUtilsTest, NonParkablePath) {
  auto result = ParkableStringUtils::GetParkableString(
      MakeLargeString().ReleaseImpl(),
      false
  );
  EXPECT_NE(result, nullptr);
}

}  // namespace blink