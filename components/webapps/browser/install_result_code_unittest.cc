// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/webapps/browser/install_result_code.h"
#if defined(OHOS_UNITTESTS)
#define protected public
#include <ostream>
#undef protected
#endif // OHOS_UNITTESTS
#include "testing/gtest/include/gtest/gtest.h"

namespace webapps {

TEST(InstallResultCodeTest, IsSuccess) {
  // TODO(crbug.com/1296447): Test the rest of the constants.
  EXPECT_TRUE(IsSuccess(InstallResultCode::kSuccessNewInstall));
  EXPECT_TRUE(IsSuccess(InstallResultCode::kSuccessAlreadyInstalled));

  EXPECT_FALSE(IsSuccess(InstallResultCode::kExpectedAppIdCheckFailed));
}

#if defined(OHOS_UNITTESTS)
TEST(InstallResultCodeTest, operator_001) {
  InstallResultCode code =
      InstallResultCode::kCancelledDueToMainFrameNavigation;
  std::ostringstream os_;
  os_ << code;
  EXPECT_EQ(os_.str(), "kCancelledDueToMainFrameNavigation");
}
#endif  // OHOS_UNITTESTS

}  // namespace webapps
