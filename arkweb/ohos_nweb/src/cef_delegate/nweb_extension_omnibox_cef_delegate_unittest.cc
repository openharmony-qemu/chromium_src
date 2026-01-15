/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <atomic>
#include <gtest/gtest.h>
#include <string>

#include "nweb_extension_omnibox_cef_delegate.h"
#include "chrome/browser/profiles/profile.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_omnibox_dispatcher.h"
#endif

using namespace std;

namespace OHOS::NWeb {

class NWebExtensionOmniboxCefDelegateTest : public testing::Test {
protected:
    void SetUp() override {
        auto &delegate = NWebExtensionOmniboxCefDelegate::GetInstance();
        delegate_ = &delegate;
    }

    void TearDown() override {
        delegate_ = nullptr;
    }

    NWebExtensionOmniboxCefDelegate *delegate_;
};

TEST_F(NWebExtensionOmniboxCefDelegateTest, TestGetInstance) {
    auto &instance1 = NWebExtensionOmniboxCefDelegate::GetInstance();
    auto &instance2 = NWebExtensionOmniboxCefDelegate::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(NWebExtensionOmniboxCefDelegateTest, TestOnInputStarted) {
    std::string extension_id = "test";
    EXPECT_NO_FATAL_FAILURE(delegate_->OnInputStarted(extension_id));
}

TEST_F(NWebExtensionOmniboxCefDelegateTest, TestOnInputCancelled) {
    std::string extension_id = "test";
    EXPECT_NO_FATAL_FAILURE(delegate_->OnInputCancelled(extension_id));
}

TEST_F(NWebExtensionOmniboxCefDelegateTest, TestOnDeleteSuggestion) {
    std::string suggestion = "test com";
    std::string extension_id = "test";
    EXPECT_NO_FATAL_FAILURE(delegate_->OnDeleteSuggestion(suggestion, extension_id));
}

TEST_F(NWebExtensionOmniboxCefDelegateTest, TestSetDefaultSuggestionCallback) {
    std::string extension_id = "test";
    OmniboxSuggestResult result;
    result.content = "test example";
    result.description = "example";
    result.deletable = false;

    EXPECT_NO_FATAL_FAILURE(delegate_->SetDefaultSuggestionCallback(extension_id, result));
}

}  // namespace