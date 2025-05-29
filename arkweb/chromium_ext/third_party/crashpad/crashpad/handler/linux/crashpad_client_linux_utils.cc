// Copyright 2018 The Crashpad Authors
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

#include "client/crashpad_client.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>

#include "base/check_op.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "client/client_argv_handling.h"
#include "third_party/lss/lss.h"
#include "util/file/file_io.h"
#include "util/file/filesystem.h"
#include "util/linux/exception_handler_client.h"
#include "util/linux/exception_information.h"
#include "util/linux/scoped_pr_set_dumpable.h"
#include "util/linux/scoped_pr_set_ptracer.h"
#include "util/linux/socket.h"
#include "util/misc/address_sanitizer.h"
#include "util/misc/from_pointer_cast.h"
#include "util/posix/scoped_mmap.h"
#include "util/posix/signals.h"
#include "util/posix/spawn_subprocess.h"
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crashpad_client_linux_utils.h"

#if BUILDFLAG(ARKWEB_CRASHPAD)
#include "third_party/crashpad/crashpad/util/linux/crashpad_dfx.h"
#endif
using namespace crashpad;


#if BUILDFLAG(ARKWEB_CRASHPAD)
void CrashpadClientUtils::AddCrashpadArguments(std::vector<std::string>& argv, std::string& g_happen_time)
{
  std::string bundle_name = CrashpadDfx::GetProcessBundleName();
  std::string happen_time = CrashpadDfx::GetCurrentTime();
  g_happen_time = happen_time;
  std::string extra_arg_happentime = "--happen-time=" + happen_time;
  std::string extra_arg_bundlename = "--bundle-name=" + bundle_name;
  argv.push_back(extra_arg_happentime);
  argv.push_back(extra_arg_bundlename);
}
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)