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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/inspector/ark_web_main_thread_debugger.h"

#include "base/command_line.h"
#include "content/public/common/content_switches.h"

namespace {
constexpr int kMaxMessageSize = 5000;
}

namespace blink {

void ArkWebDealWithMassiveConsoleMessage(WTF::String& message) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx) &&
      message.length() > kMaxMessageSize) {
    message.Truncate(kMaxMessageSize);
  }
}

}  // namespace blink
