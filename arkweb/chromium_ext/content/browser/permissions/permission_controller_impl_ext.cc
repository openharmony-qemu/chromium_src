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

#if BUILDFLAG(ARKWEB_NOTIFICATION)
void PermissionControllerImpl::GetPermissionStatusAsync(
    blink::PermissionType permission,
    bool isFromDocument,
    void* render_host,
    const url::Origin& origin,
    base::OnceCallback<void(blink::mojom::PermissionStatus)> callback) {
  if (isFromDocument && !render_host) {
    LOG(ERROR) << "GetPermissionStatusAsync render_host is null";
    std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
    return;
  }

  std::optional<blink::mojom::PermissionStatus> status =
      permission_overrides_.Get(origin, permission);
  if (status.has_value()) {
    LOG(INFO) << "GetPermissionStatusAsync permission_overrides status="
              << (int)(*status);
    std::move(callback).Run(*status);
    return;
  }

  PermissionControllerDelegate* delegate =
      browser_context_->GetPermissionControllerDelegate();
  if (!delegate) {
    std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
    return;
  }

  if (isFromDocument) {
    RenderFrameHost* render_frame_host = (RenderFrameHost*)render_host;
    if (VerifyContextOfCurrentDocument(permission, render_frame_host).status ==
        blink::mojom::PermissionStatus::DENIED) {
      LOG(INFO)
          << "GetPermissionStatusAsync VerifyContextOfCurrentDocument return";
      std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
      return;
    }
  }

  delegate->GetPermissionStatusAsync(permission, origin.GetURL(),
                                     std::move(callback));
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void PermissionControllerImpl::RequestPermissionsSkipPermissionsPolicy(
    RenderFrameHost* render_frame_host,
    PermissionRequestDescription request_description,
    base::OnceCallback<void(const std::vector<PermissionStatus>&)> callback) {
  if (!IsRequestAllowed(request_description.permissions, render_frame_host,
                        callback, false)) {
    return;
  }

  for (PermissionType permission : request_description.permissions) {
    NotifySchedulerAboutPermissionRequest(render_frame_host, permission);
  }

  request_description.requesting_origin =
      render_frame_host->GetLastCommittedOrigin().GetURL();
  std::vector<std::optional<blink::mojom::PermissionStatus>> override_results =
      OverridePermissions(request_description, render_frame_host,
                          permission_overrides_);

  auto wrapper = base::BindOnce(&MergeOverriddenAndDelegatedResults,
                                std::move(callback), override_results);
  if (request_description.permissions.empty()) {
    std::move(wrapper).Run({});
    return;
  }

  // Use delegate to find statuses of other permissions that have been requested
  // but do not have overrides.
  PermissionControllerDelegate* delegate =
      browser_context_->GetPermissionControllerDelegate();
  if (!delegate) {
    std::move(wrapper).Run(std::vector<PermissionStatus>(
        request_description.permissions.size(), PermissionStatus::DENIED));
    return;
  }

  delegate->RequestPermissionsFromCurrentDocument(
      render_frame_host, request_description, std::move(wrapper));
}

bool PermissionControllerImpl::IsClipboardSitePermissionEnabled() {
  if (!browser_context_) {
    return false;
  }
  PermissionControllerDelegate* delegate =
      browser_context_->GetPermissionControllerDelegate();
  if (!delegate) {
    return false;
  }
  return delegate->IsClipboardSitePermissionEnabled();
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)