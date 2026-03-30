// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_handle.h"

#include <stdint.h>

#include <ostream>

#include "build/build_config.h"
#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "base/check.h"
#include "build/build_config.h"

namespace base {

namespace {
ProcessId g_pid_outside_of_namespace = kNullProcessId;
}  // namespace

std::ostream& operator<<(std::ostream& os, const UniqueProcId& obj) {
  os << obj.GetUnsafeValue();
  return os;
}

UniqueProcId GetUniqueIdForProcess() {
  // Used for logging. Must not use LogMessage or any of the macros that call
  // into it.
  return (g_pid_outside_of_namespace != kNullProcessId)
             ? UniqueProcId(g_pid_outside_of_namespace)
#if BUILDFLAG(ARKWEB_USE_UNIQUE_RENDERER_PROCESS_ID)
             : UniqueProcId(GetCurrentRealPid());
#else
             : UniqueProcId(GetCurrentProcId());
#endif
}

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_AIX) || BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)

void InitUniqueIdForProcessInPidNamespace(ProcessId pid_outside_of_namespace) {
  DCHECK(pid_outside_of_namespace != kNullProcessId);
  g_pid_outside_of_namespace = pid_outside_of_namespace;
}

#endif

}  // namespace base
