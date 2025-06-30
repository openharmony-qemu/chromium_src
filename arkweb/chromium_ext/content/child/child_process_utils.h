/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef CHILD_PROCESS_UTILS_H_
#define CHILD_PROCESS_UTILS_H_

#include "arkweb/build/features/features.h"

namespace content {
class ChildProcess;
class ChildThreadImpl;

class ChildProcessUtils {
public:
  ChildProcess* childProcess;
  ChildProcessUtils(ChildProcess* impl);
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void ReportIoThreadStatus(bool is_created, bool is_in_browser_process);
  void ReportCompositorKeyThread(bool is_created, bool is_in_browser_process);
#endif
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  void ReportHisyevent(int64_t block_time, const std::string& mode, bool is_in_browser_process);
#endif
};
}
#endif