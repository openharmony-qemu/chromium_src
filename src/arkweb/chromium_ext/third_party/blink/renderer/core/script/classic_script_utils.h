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

#ifndef THIRD_PARTY_BLINK_RENDER_SCRIPT_CLASSIC_SCRIPT_UTILS_H_
#define THIRD_PARTY_BLINK_RENDER_SCRIPT_CLASSIC_SCRIPT_UTILS_H_

#include <string>

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/bindings/core/v8/sanitize_script_errors.h"
#include "third_party/blink/renderer/bindings/core/v8/script_source_location_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/loader/resource/script_resource.h"
#include "third_party/blink/renderer/core/script/script.h"

namespace blink {

class ClassicScript;
class ParkableString;
class ScriptFetchOptions;

class ClassicScriptUtils {
public:
  #if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
    static ClassicScript* CreateUnparkScript(
       const String& source_text,
       ScriptSourceLocationType = ScriptSourceLocationType::kUnknown,
       SanitizeScriptErrors = SanitizeScriptErrors::kSanitize
    );
  #endif
};
}

#endif