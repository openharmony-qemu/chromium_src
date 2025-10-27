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

#ifndef COMPONENTS_VIZ_CLIENT_FRAME_EVICTION_MANAGER_EXT_H_
#define COMPONENTS_VIZ_CLIENT_FRAME_EVICTION_MANAGER_EXT_H_

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_DISCARD_BG_WEBPAGE)
#include "base/command_line.h"
#include "base/ohos/sys_info_utils_ext.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "base/trace_event/trace_event.h"
#endif

#if BUILDFLAG(ARKWEB_FLING)
constexpr int kOhosFramesMax = 10;
constexpr int kOhosFramesBase = 2;
constexpr int kPhysicalMemoryBlockSize = 256;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_DISCARD_BG_WEBPAGE)
const int kMaxNumberOfSavedFrames = 100000;
#endif

#define UPDATE_MAX_NUMBER_OF_FRAMES(max_number_of_saved_frames_) \
  if (base::CommandLine::ForCurrentProcess()->HasSwitch( \
          switches::kMaxNumberOfSavedFrames)) { \
    max_number_of_saved_frames_ = kMaxNumberOfSavedFrames; \
  }

#endif
