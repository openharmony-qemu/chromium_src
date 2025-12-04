// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_MANAGER_H_
#define CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_MANAGER_H_

#include <memory>

#include "base/containers/id_map.h"
#include "base/memory/weak_ptr.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "content/public/browser/permission_controller.h"
#include "content/public/browser/permission_controller_delegate.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include <map>
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

namespace content {
class WebContents;
}
#if BUILDFLAG(ARKWEB_WEBRTC)
class AlloyMediaAccessRequest;
#endif // BUILDFLAG(ARKWEB_WEBRTC)

class AlloyPermissionManager : public content::PermissionControllerDelegate {
 public:
  AlloyPermissionManager(const AlloyPermissionManager&) = delete;
  AlloyPermissionManager& operator=(const AlloyPermissionManager&) = delete;

  AlloyPermissionManager();
  ~AlloyPermissionManager() override;

  // PermissionControllerDelegate implementation.
  void RequestPermissions(
      content::RenderFrameHost* render_frame_host,
      const content::PermissionRequestDescription& request_description,
      base::OnceCallback<
          void(const std::vector<content::PermissionResult>&)> callback)
      override;
  content::PermissionStatus GetPermissionStatus(
      const blink::mojom::PermissionDescriptorPtr& permission,
      const GURL& requesting_origin,
      const GURL& embedding_origin) override;
  void ResetPermission(blink::PermissionType permission,
                       const GURL& requesting_origin,
                       const GURL& embedding_origin) override;
  void RequestPermissionsFromCurrentDocument(
      content::RenderFrameHost* render_frame_host,
      const content::PermissionRequestDescription& request_description,
      base::OnceCallback<void(const std::vector<content::PermissionResult>&)> callback)
      override;
  content::PermissionResult GetPermissionResultForOriginWithoutContext(
      const blink::mojom::PermissionDescriptorPtr& permission_descriptor,
      const url::Origin& requesting_origin,
      const url::Origin& embedding_origin) override;
  content::PermissionStatus GetPermissionStatusForCurrentDocument(
      const blink::mojom::PermissionDescriptorPtr& permission_descriptor,
      content::RenderFrameHost* render_frame_host,
      bool should_include_device_status) override;
  content::PermissionResult GetPermissionResultForCurrentDocument(
      const blink::mojom::PermissionDescriptorPtr& permission_descriptor,
      content::RenderFrameHost* render_frame_host,
      bool should_include_device_status) override;
  content::PermissionStatus GetPermissionStatusForWorker(
      const blink::mojom::PermissionDescriptorPtr& permission_descriptor,
      content::RenderProcessHost* render_process_host,
      const GURL& worker_origin) override;
  content::PermissionStatus GetPermissionStatusForEmbeddedRequester(
      const blink::mojom::PermissionDescriptorPtr& permission_descriptor,
      content::RenderFrameHost* render_frame_host,
      const url::Origin& requesting_origin) override;
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetPermissionStatusAsync(
      blink::PermissionType permission,
      const GURL& requesting_origin,
      base::OnceCallback<void(blink::mojom::PermissionStatus)> callback) override;
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
 protected:
  void AbortPermissionRequest(int request_id);
  void AbortPermissionRequests();

 private:
#if BUILDFLAG(ARKWEB_WEBRTC)
  friend class AlloyMediaAccessRequest;
#endif // BUILDFLAG(ARKWEB_WEBRTC)
  class UnhandledRequest;
  using UnhandledRequestsMap = base::IDMap<std::unique_ptr<UnhandledRequest>>;
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  class UnhandledQuery;
  using UnhandledQuerysMap = base::IDMap<std::unique_ptr<UnhandledQuery>>;
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void RequestPermissionByType(const blink::PermissionType permission_type,
                               CefRefPtr<CefBrowserHostBase> browser,
                               UnhandledRequest* pending_request_raw,
                               const int request_id);
  virtual GURL LastCommittedOrigin(content::RenderFrameHost* render_frame_host);
  void AbortPermissionRequestByType(const blink::PermissionType permission_type,
                                    CefRefPtr<CefBrowserHostBase> browser,
                                    const GURL& requesting_origin);
  // The weak pointer to this is used to clean up any information which is
  // stored in the pending request or result cache maps. However, the callback
  // should be run regardless of whether the class is still alive so the method
  // is static.
  static void OnRequestResponseCallBack(
      const base::WeakPtr<AlloyPermissionManager>& manager,
      int request_id,
      blink::PermissionType permission,
      bool allowed);

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static void OnQueryResponseCallBack(
      const base::WeakPtr<AlloyPermissionManager>& manager,
      int query_id,
      blink::PermissionType permission,
      int32_t status);
 
  UnhandledQuerysMap unhandled_querys_;
 
  std::map<GURL, int32_t> notification_permission_;
  std::map<GURL, int32_t> geolocation_permission_;
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

  UnhandledRequestsMap unhandled_requests_;
  base::WeakPtrFactory<AlloyPermissionManager> weak_ptr_factory_{this};
};

#endif  // CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_MANAGER_H_
