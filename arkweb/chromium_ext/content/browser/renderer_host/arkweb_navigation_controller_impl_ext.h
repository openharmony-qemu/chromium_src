// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_ARKWEB_NAVIGATION_CONTROLLER_IMPL_EXT_H_
#define CONTENT_BROWSER_RENDERER_HOST_ARKWEB_NAVIGATION_CONTROLLER_IMPL_EXT_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/raw_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "content/browser/renderer_host/back_forward_cache_impl.h"
#include "content/browser/renderer_host/navigation_controller_delegate.h"
#include "content/browser/renderer_host/navigation_entry_impl.h"
#include "content/browser/renderer_host/navigation_type.h"
#include "content/browser/ssl/ssl_manager.h"
#include "content/common/content_export.h"
#include "content/common/navigation_client.mojom-forward.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/reload_type.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "net/storage_access_api/status.h"
#include "services/network/public/mojom/source_location.mojom-forward.h"
#include "third_party/blink/public/common/scheduler/task_attribution_id.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/public/mojom/navigation/navigation_api_history_entry_arrays.mojom-forward.h"
#include "third_party/blink/public/mojom/navigation/navigation_initiator_activation_and_ad_status.mojom.h"
#include "third_party/blink/public/mojom/navigation/navigation_params.mojom-forward.h"

namespace content {
class FrameTree;

// NavigationControllerImpl is 1:1 with FrameTree. See comments on the base
// class.
class CONTENT_EXPORT ArkWebNavigationControllerImplExt : public NavigationControllerImpl {
 public:
  ArkWebNavigationControllerImplExt(BrowserContext* browser_context,
                           FrameTree& frame_tree,
                           NavigationControllerDelegate* delegate);

  ArkWebNavigationControllerImplExt(const ArkWebNavigationControllerImplExt&) = delete;
  ArkWebNavigationControllerImplExt& operator=(const ArkWebNavigationControllerImplExt&) = delete;

  ~ArkWebNavigationControllerImplExt() override;

  ArkWebNavigationControllerImplExt *AsArkWebNavigationControllerImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void NewEntrySetExtraHeaders(
    RenderFrameHostImpl* rfh,
    const mojom::DidCommitProvisionalLoadParams& params,
    std::unique_ptr<NavigationEntryImpl>& new_entry);
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  const std::string& GetOriginalUrl() override;
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  NavigationEntryUpdateError InsertBackForwardEntry(int index,
                                                    const GURL& url) override;
  NavigationEntryUpdateError UpdateNavigationEntryUrl(int index,
                                                      const GURL& url) override;
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)
};

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_ARKWEB_NAVIGATION_CONTROLLER_IMPL_EXT_H_
