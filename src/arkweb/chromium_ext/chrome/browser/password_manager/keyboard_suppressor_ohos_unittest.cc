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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "components/autofill/content/browser/content_autofill_client.h"
#define private public
#include "chrome/browser/password_manager/keyboard_suppressor_ohos.h"
#undef private

using namespace testing;

class KeyboardSuppressorOhosTest : public ::testing::Test {};

/**
 * @tc.name: KeyboardSuppressorOhosTest_Suppress_001.
 * @tc.desc: test of Suppress in KeyboardSuppressorOhosTest
 * @tc.type: FUNC.
 */
TEST_F(KeyboardSuppressorOhosTest, KeyboardSuppressorOhosTest_Suppress_001) {
  autofill::ContentAutofillClient* autofill_client = nullptr;
  base::TimeDelta timeout;
  KeyboardSuppressorOhos keyboardSuppressorOhosN(autofill_client, timeout);
  autofill::ContentAutofillDriver* driver = nullptr;
  keyboardSuppressorOhosN.Suppress(driver);
  EXPECT_EQ(keyboardSuppressorOhosN.suppressed_driver_, false);
}

/**
 * @tc.name: KeyboardSuppressorOhosTest_Unsuppress_001.
 * @tc.desc: test of Unsuppress in KeyboardSuppressorOhosTest
 * @tc.type: FUNC.
 */
TEST_F(KeyboardSuppressorOhosTest, KeyboardSuppressorOhosTest_Unsuppress_001) {
  autofill::ContentAutofillClient* autofill_client = nullptr;
  base::TimeDelta timeout;
  KeyboardSuppressorOhos keyboardSuppressorOhos(autofill_client, timeout);
  keyboardSuppressorOhos.Unsuppress();
  EXPECT_EQ(keyboardSuppressorOhos.suppressed_driver_, false);

  keyboardSuppressorOhos.suppressed_driver_ = true;
  keyboardSuppressorOhos.Unsuppress();
  EXPECT_EQ(keyboardSuppressorOhos.suppressed_driver_, false);
}
