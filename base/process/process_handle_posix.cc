// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_handle.h"

#include <unistd.h>
#if BUILDFLAG(ARKWEB_USE_UNIQUE_RENDERER_PROCESS_ID)
#include <dlfcn.h>
#include <string.h>

#include "base/check.h"
#endif

namespace base {

ProcessId GetCurrentProcId() {
  return getpid();
}

#if BUILDFLAG(ARKWEB_USE_UNIQUE_RENDERER_PROCESS_ID)
NO_SANITIZE("cfi-icall") ProcessId GetCurrentRealPid() {
  using GetProcXid = int (*)(void);
  static GetProcXid getProcPid = nullptr;
  if (getProcPid == nullptr) {
    getProcPid =
        reinterpret_cast<GetProcXid>(dlsym(RTLD_DEFAULT, "getprocpid"));
    CHECK(getProcPid);
  }
  return getProcPid();
}
#endif

ProcessHandle GetCurrentProcessHandle() {
  return GetCurrentProcId();
}

ProcessId GetProcId(ProcessHandle process) {
  return process;
}

}  // namespace base
