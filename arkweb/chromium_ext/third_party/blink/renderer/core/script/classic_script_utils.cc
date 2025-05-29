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

#include <string>

#include "arkweb/chromium_ext/third_party/blink/renderer/core/script/classic_script_utils.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/loader/fetch/script_fetch_options.h"
#include "third_party/blink/renderer/core/script/classic_script.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string.h"

namespace blink {

ClassicScript* ClassicScriptUtils::CreateUnparkScript(
    const String& source_text,
    ScriptSourceLocationType source_location_type,
    SanitizeScriptErrors sanitize_script_errors)
{
    return MakeGarbageCollected<ClassicScript>(
      ParkableString(source_text.Impl()), KURL(), KURL(), ScriptFetchOptions(),
      source_location_type, sanitize_script_errors);
}

}  // namespace blink