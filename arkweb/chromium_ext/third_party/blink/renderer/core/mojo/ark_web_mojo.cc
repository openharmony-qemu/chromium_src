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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/mojo/ark_web_mojo.h"

#include "base/strings/string_util.h"

namespace {
const char kExtensionsKeepAlive[] = "extensions.KeepAlive";
const char kExtensionsMimeHandlerMimeHandlerService[] =
    "extensions.mime_handler.MimeHandlerService";
const char kExtensionsMimeHandlerBeforeUnloadControl[] =
    "extensions.mime_handler.BeforeUnloadControl";
}  // namespace

namespace blink {

bool ArkWebBypassInterfacesExceptPdf(const std::string& name) {
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  if (base::EqualsCaseInsensitiveASCII(name.c_str(), kExtensionsKeepAlive) ||
      base::EqualsCaseInsensitiveASCII(
          name.c_str(), kExtensionsMimeHandlerMimeHandlerService) ||
      base::EqualsCaseInsensitiveASCII(
          name.c_str(), kExtensionsMimeHandlerBeforeUnloadControl)) {
    return false;
  }
#endif
  return true;
}

}  // namespace blink
