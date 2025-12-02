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
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#if BUILDFLAG(IS_ARKWEB)
#include "third_party/blink/public/common/messaging/web_message_port.h"
#endif

#if BUILDFLAG(ARKWEB_DATALIST)
#include "components/autofill/core/browser/ui/suggestion.h"
#include "ui/gfx/geometry/rect_f.h"
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
#include "arkweb/chromium_ext/service/device/wake_lock/power_save_blocker/nweb_screen_lock_tracker.h"
#endif //BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "components/dom_distiller/content/common/mojom/distillability_service.mojom.h"
#endif  // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#include "arkweb/ohos_nweb/include/nweb_vault_plain_text_callback.h"
#endif