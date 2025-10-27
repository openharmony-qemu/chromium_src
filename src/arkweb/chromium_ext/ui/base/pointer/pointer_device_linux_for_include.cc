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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_FLING)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace ui {

static int GetPointerTypesForFling(int& available_pointer_types) {
  if (base::ohos::IsPcDevice()) {
    available_pointer_types |= POINTER_TYPE_FINE;
  }
  if (base::ohos::IsTabletDevice() || base::ohos::IsMobileDevice()) {
    available_pointer_types |= POINTER_TYPE_COARSE;
  }
  return available_pointer_types;
}

} // namespace ui
