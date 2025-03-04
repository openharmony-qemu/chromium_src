// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/check_op.h"
#include "base/command_line.h"
#include "content/renderer/renderer_main_platform_delegate.h"
#include "sandbox/policy/sandbox.h"
#include "sandbox/policy/sandbox_type.h"

namespace content {

RendererMainPlatformDelegate::RendererMainPlatformDelegate(
    const MainFunctionParams& parameters) {}

RendererMainPlatformDelegate::~RendererMainPlatformDelegate() {}

void RendererMainPlatformDelegate::PlatformInitialize() {}

void RendererMainPlatformDelegate::PlatformUninitialize() {}

bool RendererMainPlatformDelegate::EnableSandbox() {
  sandbox::policy::SandboxLinux::Options options;
  sandbox::policy::Sandbox::Initialize(
      sandbox::policy::SandboxTypeFromCommandLine(
          *base::CommandLine::ForCurrentProcess()),
      sandbox::policy::SandboxLinux::PreSandboxHook(), options);

  // about:sandbox uses a value returned from SandboxLinux::GetStatus() before
  // any renderer has been started.
  // Here, we test that the status of SeccompBpf in the renderer is consistent
  // with what SandboxLinux::GetStatus() said we would do.
  auto* ohos_sandbox = sandbox::policy::SandboxLinux::GetInstance();
  if (ohos_sandbox->GetStatus() & sandbox::policy::SandboxLinux::kSeccompBPF) {
    CHECK(ohos_sandbox->seccomp_bpf_started());
  }

  return true;
}

}  // namespace content
