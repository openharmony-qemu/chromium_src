/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_DOCUMENT_LOADER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_DOCUMENT_LOADER_EXT_H_

#include <memory>
#include <optional>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/memory/scoped_refptr.h"
#include "base/time/time.h"
#include "base/unguessable_token.h"
#include "base/uuid.h"
#include "mojo/public/cpp/base/big_buffer.h"
#include "mojo/public/cpp/bindings/shared_remote.h"
#include "net/storage_access_api/status.h"
#include "services/metrics/public/cpp/ukm_source_id.h"
#include "third_party/blink/public/common/fenced_frame/redacted_fenced_frame_config.h"
#include "third_party/blink/public/common/frame/view_transition_state.h"
#include "third_party/blink/public/common/loader/loading_behavior_flag.h"
#include "third_party/blink/public/common/permissions_policy/document_policy.h"
#include "third_party/blink/public/common/permissions_policy/permissions_policy.h"
#include "third_party/blink/public/common/scheduler/task_attribution_id.h"
#include "third_party/blink/public/common/subresource_load_metrics.h"
#include "third_party/blink/public/mojom/fenced_frame/fenced_frame.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/triggering_event_info.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/loader/content_security_notifier.mojom-blink.h"
#include "third_party/blink/public/mojom/loader/mhtml_load_result.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/loader/same_document_navigation_type.mojom-blink.h"
#include "third_party/blink/public/mojom/navigation/navigation_params.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/navigation/navigation_params.mojom-shared.h"
#include "third_party/blink/public/mojom/page/page.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/page_state/page_state.mojom-blink.h"
#include "third_party/blink/public/mojom/permissions/permission.mojom-blink.h"
#include "third_party/blink/public/mojom/runtime_feature_state/runtime_feature.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/service_worker/controller_service_worker_mode.mojom-blink.h"
#include "third_party/blink/public/mojom/timing/resource_timing.mojom-blink-forward.h"
#include "third_party/blink/public/platform/scheduler/web_scoped_virtual_time_pauser.h"
#include "third_party/blink/public/platform/web_navigation_body_loader.h"
#include "third_party/blink/public/web/web_document_loader.h"
#include "third_party/blink/public/web/web_frame_load_type.h"
#include "third_party/blink/public/web/web_history_commit_type.h"
#include "third_party/blink/public/web/web_navigation_params.h"
#include "third_party/blink/public/web/web_navigation_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/weak_identifier_map.h"
#include "third_party/blink/renderer/core/frame/dactyloscoper.h"
#include "third_party/blink/renderer/core/frame/frame_types.h"
#include "third_party/blink/renderer/core/frame/policy_container.h"
#include "third_party/blink/renderer/core/frame/use_counter_impl.h"
#include "third_party/blink/renderer/core/html/parser/parser_synchronization_policy.h"
#include "third_party/blink/renderer/core/loader/document_load_timing.h"
#include "third_party/blink/renderer/core/loader/frame_loader_types.h"
#include "third_party/blink/renderer/core/loader/navigation_policy.h"
#include "third_party/blink/renderer/core/loader/preload_helper.h"
#include "third_party/blink/renderer/core/page/viewport_description.h"
#include "third_party/blink/renderer/core/permissions_policy/policy_helper.h"
#include "third_party/blink/renderer/core/speculation_rules/speculation_rule_set.h"
#include "third_party/blink/renderer/platform/bindings/source_location.h"
#include "third_party/blink/renderer/platform/exported/wrapped_resource_response.h"
#include "third_party/blink/renderer/platform/instrumentation/use_counter.h"
#include "third_party/blink/renderer/platform/loader/fetch/client_hints_preferences.h"
#include "third_party/blink/renderer/platform/loader/fetch/code_cache_host.h"
#include "third_party/blink/renderer/platform/loader/fetch/early_hints_preload_entry.h"
#include "third_party/blink/renderer/platform/loader/fetch/loader_freeze_mode.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_error.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_request.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_remote.h"
#include "third_party/blink/renderer/platform/storage/blink_storage_key.h"
#include "third_party/blink/renderer/platform/weborigin/referrer.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/shared_buffer.h"

namespace blink {

class LocalFrame;
class SubresourceFilter;

class CORE_EXPORT ArkWebDocumentLoaderExt : public DocumentLoader {
 public:
  friend class DocumentLoader;
  ArkWebDocumentLoaderExt(LocalFrame*,
                          WebNavigationType navigation_type,
                          std::unique_ptr<WebNavigationParams> navigation_params,
                          std::unique_ptr<PolicyContainer> policy_container,
                          std::unique_ptr<ExtraData> extra_data);
  ~ArkWebDocumentLoaderExt();
  ArkWebDocumentLoaderExt *AsArkWebDocumentLoaderExt() override { return this; }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  WebDocumentSubresourceFilter* GetWebSubresourceFilter() override;

  void SetUserSubresourceFilter(SubresourceFilter*);
  SubresourceFilter* GetUserSubresourceFilter() const {
    return user_subresource_filter_.Get();
  }

  void SetWebUserSubresourceFilter(WebDocumentSubresourceFilter*) override;

  WebDocumentSubresourceFilter* GetWebUserSubresourceFilter() override;
#endif

#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
  scoped_refptr<const SharedBuffer> OnGetImageFromCache(const WTF::String& url);
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void NotifyLcpForBlankless();
#endif

 protected:
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void UpdateAllowPreloadRecord();
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_DOCUMENT_LOADER_EXT_H_
