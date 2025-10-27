/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <unistd.h>
#include "nweb_export.h"
#include "nweb_hilog.h"
#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/sandbox/seccomp-bpf-helpers/seccomp_starter_ohos.h"
#include "arkweb/chromium_ext/sandbox/seccomp-bpf-helpers/baseline_policy_ohos.h"

extern "C" OHOS_NWEB_EXPORT bool SetRendererSeccompPolicy() {
#if BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
  return true;
#else
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
#endif  // BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
}
