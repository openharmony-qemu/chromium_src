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

#include <memory>
#include "content/browser/webauth/utils.h"
#include <gtest/gtest.h>
using namespace testing;

namespace content {

class UtilsTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UtilsTest, Convert001) {
    const blink::mojom::PublicKeyCredentialMediationRequirement mediation =
        blink::mojom::PublicKeyCredentialMediationRequirement::SILENT;
    device::CredentialMediationRequirement temp = device::CredentialMediationRequirement::kSilent;
    auto result = Convert(mediation);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert002) {
    const blink::mojom::PublicKeyCredentialMediationRequirement mediation =
        blink::mojom::PublicKeyCredentialMediationRequirement::OPTIONAL;
    device::CredentialMediationRequirement temp = device::CredentialMediationRequirement::kOptional;
    auto result = Convert(mediation);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert003) {
    const blink::mojom::PublicKeyCredentialMediationRequirement mediation =
        blink::mojom::PublicKeyCredentialMediationRequirement::REQUIRED;
    device::CredentialMediationRequirement temp = device::CredentialMediationRequirement::kRequired;
    auto result = Convert(mediation);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert004) {
    const blink::mojom::PublicKeyCredentialMediationRequirement mediation =
        blink::mojom::PublicKeyCredentialMediationRequirement::CONDITIONAL;
    device::CredentialMediationRequirement temp = device::CredentialMediationRequirement::kConditional;
    auto result = Convert(mediation);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert005) {
    const blink::mojom::PublicKeyCredentialMediationRequirement mediation =
        static_cast<blink::mojom::PublicKeyCredentialMediationRequirement>(-1);
    device::CredentialMediationRequirement temp = device::CredentialMediationRequirement::kSilent;
    auto result = Convert(mediation);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert006) {
    const blink::mojom::Hint hint = blink::mojom::Hint::SECURITY_KEY;
    device::CredentialHint temp = device::CredentialHint::kSecurityKey;
    auto result = Convert(hint);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert007) {
    const blink::mojom::Hint hint = blink::mojom::Hint::CLIENT_DEVICE;
    device::CredentialHint temp = device::CredentialHint::kClientDevice;
    auto result = Convert(hint);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert008) {
    const blink::mojom::Hint hint = blink::mojom::Hint::HYBRID;
    device::CredentialHint temp = device::CredentialHint::kHintHybrid;
    auto result = Convert(hint);
    EXPECT_EQ(result, temp);
}

TEST_F(UtilsTest, Convert009) {
    const blink::mojom::Hint hint = static_cast<blink::mojom::Hint>(-1);
    device::CredentialHint temp = device::CredentialHint::kHintHybrid;
    auto result = Convert(hint);
    EXPECT_EQ(result, temp);
}
} // content
