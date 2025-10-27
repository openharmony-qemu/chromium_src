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

#include "arkweb/chromium_ext/base/time/time_utils.h"

#include "base/strings/stringprintf.h"

namespace base {

#if BUILDFLAG(ARKWEB_PRECOMPILE)
std::string TimeUtils::ToUTCString(const base::Time& time)
{
  Time::Exploded exploded;
  time.UTCExplode(&exploded);
  std::string time_string =
      StringPrintf("%04d-%02d-%02d %02d:%02d:%02d.%03d UTC", exploded.year,
                   exploded.month, exploded.day_of_month, exploded.hour,
                   exploded.minute, exploded.second, exploded.millisecond);
  return time_string;
}
#endif

}  // namespace base
