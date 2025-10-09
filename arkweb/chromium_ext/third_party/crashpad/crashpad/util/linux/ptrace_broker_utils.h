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

#ifndef CRASHPAD_UTIL_LINUX_PTRACE_BROKER_UTILS_H_
#define CRASHPAD_UTIL_LINUX_PTRACE_BROKER_UTILS_H_

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

#include "arkweb/build/features/features.h"
#include "util/file/file_io.h"
#include "util/linux/exception_handler_protocol.h"
#include "util/linux/ptrace_connection.h"
#include "util/linux/ptracer.h"
#include "util/linux/ptrace_broker.h"
#include "util/linux/thread_info.h"
#include "util/misc/address_types.h"


namespace crashpad {


class PtraceBrokerUtils {
public:

  PtraceBrokerUtils() = default;
  PtraceBrokerUtils(int sock, pid_t pid, bool is_64_bit);

  PtraceBrokerUtils(const PtraceBrokerUtils&) = delete;
  PtraceBrokerUtils& operator=(const PtraceBrokerUtils&) = delete;

  ~PtraceBrokerUtils();

#if BUILDFLAG(ARKWEB_CRASHPAD)
  static int ConvertRealtidToNstid(int real_tid, PtraceBroker* ptraceBroker);
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)
};

}

#endif  // CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_H_