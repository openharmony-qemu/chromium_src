// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>

#include "nweb_export.h"
#include "nweb_hilog.h"
#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_ohos.h"
#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_ohos.h"

extern "C" OHOS_NWEB_EXPORT bool SetRendererSeccompPolicy() {
#if defined(__arm__) || defined(__aarch64__)
  sandbox::SeccompStarterOhos starter;

  starter.set_policy(std::make_unique<sandbox::BaselinePolicyOhos>());

  starter.StartSandbox();

  if (starter.status() == sandbox::SeccompSandboxStatus::ENGAGED) {
    WVLOG_I("Successfully set seccomp policy.");
    return true;
  }
  return false;
#else
  return true;
#endif
}
 