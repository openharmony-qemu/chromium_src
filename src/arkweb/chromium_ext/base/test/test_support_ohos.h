// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_TEST_SUPPORT_OHOS_H_
#define BASE_TEST_TEST_SUPPORT_OHOS_H_

#include "base/test/test_suite.h"
#include "base/base_export.h"

namespace base {
void RegisterPathProviderForOhosTest();

// Init the message loop for tests in ohos
BASE_EXPORT void InitOhosTestMessageLoop();
}  // namespace base

#endif  // BASE_TEST_TEST_SUPPORT_OHOS_H_
