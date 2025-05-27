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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_MOJO_ARK_WEB_MOJO_H_
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_MOJO_ARK_WEB_MOJO_H_

#include <string>

namespace blink {

// Bypass all MojoJS interfaces for security reason, expect PDF related ones.
// Pdfium will using MojoJS to view PDF file in ArkWeb.
// Return |true| if the interface should be bypassed.
bool ArkWebBypassInterfacesExceptPdf(const std::string& name);

}  // namespace blink

#endif  // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_MOJO_ARK_WEB_MOJO_H_
