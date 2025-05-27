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
#include "arkweb/ohos_nweb_ex/build/features/features.h"

#if BUILDFLAG(IS_ARKWEB) || BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
#include "arkweb/chromium_ext/content/renderer/ark_web_render_frame_impl.h"
#include "cef/ohos_cef_ext/libcef/renderer/javascript/oh_gin_javascript_bridge_dispatcher.h"
#endif  // BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
