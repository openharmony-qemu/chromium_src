// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/thread_pool_util.h"

#include <algorithm>

#include "base/system/sys_info.h"

#include "base/command_line.h"
#include "base/memory/low_memory_mode_ohos.h"
#include "base/base_switches.h"
#include "base/strings/string_number_conversions.h"
#include "base/system/sys_info.h"

namespace content {

#if BUILDFLAG(IS_OHOS)
int GetThreadNumFromCommandline() {
  bool low_memory_mode = base::GetLowMemoryMode();
  int num_raster_threads = base::SysInfo::NumberOfEfficientProcessors() - 1;
  if (low_memory_mode) {
    num_raster_threads = 4;
  }
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kNumForegroundThreads)) {
    std::string string_value =
        command_line.GetSwitchValueASCII(switches::kNumForegroundThreads);
    base::StringToInt(string_value, &num_raster_threads);
  }
  return num_raster_threads;
}
#endif

size_t GetMinForegroundThreadsInRendererThreadPool() {
  // Assume a busy main thread.
#if BUILDFLAG(IS_OHOS)
  int thread_num = GetThreadNumFromCommandline();
  return static_cast<size_t>(std::max(1, thread_num));
#else
  return static_cast<size_t>(
      std::max(1, base::SysInfo::NumberOfProcessors() - 1));
#endif
}

}  // namespace content
