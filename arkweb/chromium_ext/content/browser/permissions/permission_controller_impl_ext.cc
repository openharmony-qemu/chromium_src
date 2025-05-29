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
  if (!render_host) {
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