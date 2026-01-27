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
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_SCRIPT_RESOURCE_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_SCRIPT_RESOURCE_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"

namespace blink {
class ScriptResource;

class ScriptResourceUtils : public GarbageCollected<ScriptResourceUtils> {
public:
	void Trace(Visitor* visitor) const;
	Member<ScriptResource> scriptResource;
	ScriptResourceUtils(ScriptResource* impl);

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
	static ScriptResource* CreateForOfflineResource(const KURL& kurl,
			const KURL& origin_url,
			ResourceResponse response,
			const bool is_module);
#endif
};
}
#endif