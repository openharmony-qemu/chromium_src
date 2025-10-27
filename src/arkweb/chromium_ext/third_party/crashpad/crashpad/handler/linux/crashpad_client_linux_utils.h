// Copyright 2014 The Crashpad Authors
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

#ifndef CRASHPAD_CLIENT_CRASHPAD_CLIENT_H_UTILS
#define CRASHPAD_CLIENT_CRASHPAD_CLIENT_H_UTILS

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <stdint.h>

#include "arkweb/build/features/features.h"
#include "base/files/file_path.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "util/file/file_io.h"

#if !BUILDFLAG(IS_FUCHSIA)
#include "util/misc/capture_context.h"
#endif  // !BUILDFLAG(IS_FUCHSIA)

#if BUILDFLAG(IS_APPLE)
#include "base/apple/scoped_mach_port.h"
#elif BUILDFLAG(IS_WIN)
#include <windows.h>
#include "util/win/scoped_handle.h"
#elif BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || \
    BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_OHOS)
#include <signal.h>
#include <ucontext.h>
#endif

#if BUILDFLAG(IS_IOS)
#include "client/upload_behavior_ios.h"
#endif

namespace crashpad {

//! \brief The primary interface for an application to have Crashpad monitor
//!     it for crashes.
class CrashpadClientUtils {
 public:
  CrashpadClientUtils();

  CrashpadClientUtils(const CrashpadClientUtils&) = delete;
  CrashpadClientUtils& operator=(const CrashpadClientUtils&) = delete;

  ~CrashpadClientUtils();

#if BUILDFLAG(ARKWEB_CRASHPAD)
  static void AddCrashpadArguments(std::vector<std::string>& argv, std::string& g_happen_time);
#endif

};

}
#endif  // CRASHPAD_CLIENT_LINUX_UTILS_H_