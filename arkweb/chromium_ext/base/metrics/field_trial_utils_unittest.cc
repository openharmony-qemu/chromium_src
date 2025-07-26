/* Copyright (c) 2025 Huawei Device Co., Ltd.
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
src/arkweb/chromium_ext/components/content_settings/core/browser/content_settings_default_provider_for_
 include_unittest.cc
#include "arkweb/chromium_ext/base/metrics/field_trial_utils.h"
#include "base/memory/read_only_shared_memory_region.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace base {

#if BUILDFLAG(IS_ARKWEB)
TEST(FieldTrialListUtilsTest, GetFieldTrialDescriptor_NullGlobal) {
    auto old_instance = FieldTrialList::BackupInstanceForTesting();
    int result = FieldTrialListUtils::GetFieldTrialDescriptor();
    EXPECT_EQ(result, -1);
    FieldTrialList::RestoreInstanceForTesting(old_instance);
}

TEST(FieldTrialListUtilsTest, GetFieldTrialDescriptor_WithGlobal) {
    auto old_instance = FieldTrialList::BackupInstanceForTesting();

    auto instance = std::make_shared<FieldTrialList>();
    FieldTrialListUtils::GetFieldTrialDescriptor();

    FieldTrialList::RestoreInstanceForTesting(old_instance);
}

#endif // BUILDFALG(IS_ARKWEB)

} // namespace base