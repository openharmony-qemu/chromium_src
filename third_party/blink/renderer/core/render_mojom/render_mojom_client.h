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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_RENDER_MOJOM_RENDER_MOJOM_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_RENDER_MOJOM_RENDER_MOJOM_CLIENT_H_

#include <memory>
#include <shared_mutex>

#include "arkweb/build/features/features.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace blink {

using OHOS::NWeb::OhosAdapterHelper;

// Class here is used for render to transfer something to browser.

class SysPropRenderObserverClientImpl;

// This head file will be used by many objects, in case that compilation sequence is different, the include statement
// for mojom is moved to the source file, and use a class to control the mojom interface class.
class SysPropRenderObserverClientRep {
 public:
  SysPropRenderObserverClientRep();
  ~SysPropRenderObserverClientRep();

  // This will call the Init method for SysPropRenderObserverClientImpl as it is invisble for other object.
  static void Init();

  // This will call the Attach and Detach method for SysPropRenderObserverClientImpl.
  void AttachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback);

  void DetachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback);
 private:
  SysPropRenderObserverClientImpl* impl_;
};

class CORE_EXPORT ResSchedReportClient final {
  public:
    void SendAudioData(uint8_t status, uint32_t pid, uint32_t platform_id);
    void ApplySocByIdEx(uint32_t id, bool tag);
    void ApplySocById(uint32_t id);
    void StartPerformanceBoost();
#if BUILDFLAG(ARKWEB_OHOS_MEM_USAGE_REPORT)
    void ReportMemoryUsage(const String& msg);
#endif  // BUILDFLAG(ARKWEB_OHOS_MEM_USAGE_REPORT)
    ResSchedReportClient() = default;
    ResSchedReportClient(uint32_t id);
  private:
    std::unordered_map<uint32_t, std::shared_mutex> resSchedMutex_;
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_RENDER_MOJOM_RENDER_MOJOM_CLIENT_H_
