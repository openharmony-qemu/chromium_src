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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/editing/selection_controller_for_include.h"

#if BUILDFLAG(IS_OHOS)
#include "third_party/blink/renderer/core/input_type_names.h"
#endif
#if BUILDFLAG(IS_ARKWEB)
#include "third_party/blink/renderer/core/page/chrome_client.h"
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/html/forms/text_control_element.h"
#include "third_party/blink/renderer/core/input/context_menu_allowed_scope.h"
#endif
