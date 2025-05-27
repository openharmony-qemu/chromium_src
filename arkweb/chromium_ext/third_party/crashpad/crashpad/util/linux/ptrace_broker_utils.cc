// Copyright (c) 2025 Huawei Device Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "util/linux/ptrace_broker.h"
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/ptrace_broker_utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <algorithm>
#include <limits.h>
#include <string.h>

#include "base/check_op.h"
#include "base/memory/page_size.h"
#include "base/posix/eintr_wrapper.h"
#include "third_party/lss/lss.h"

#if BUILDFLAG(ARKWEB_CRASHPAD)
#include "base/logging.h"
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/proc_info_ohos.h"
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)

#include "util/linux/scoped_ptrace_attach.h"
#include "util/misc/memory_sanitizer.h"
#include "util/posix/scoped_mmap.h"


using namespace crashpad;


#if BUILDFLAG(ARKWEB_CRASHPAD)
int PtraceBrokerUtils::ConvertRealtidToNstid(int real_tid, PtraceBroker* ptraceBroker)
{
  if (ptraceBroker->is_in_pid_ns_) {
    auto it = ptraceBroker->tid_nstid_map_.find(real_tid);
    if (it != ptraceBroker->tid_nstid_map_.end()) {
      return it->second;
    }
  }
  return real_tid;
}
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)

