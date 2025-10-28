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

namespace content {
void PermissionServiceImpl::HasPermissionAsync(
    blink::mojom::PermissionDescriptorPtr permission,
    PermissionStatusCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool is_browser = base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx);
  if (!is_browser) {
    std::move(callback).Run(GetPermissionStatus(permission));
    return;
  }
  auto type = blink::PermissionDescriptorToPermissionType(permission);
  if (!type) {
    ReceivedBadMessage();
    std::move(callback).Run(PermissionStatus::DENIED);
    return;
  }
  if (type != PermissionType::CLIPBOARD_READ_WRITE &&
      type != PermissionType::CLIPBOARD_SANITIZED_WRITE) {
    std::move(callback).Run(GetPermissionStatus(permission));
    return;
  }
  LOG(DEBUG) << "HasPermissionAsync type: " << blink::GetPermissionString(*type);
  if (PermissionUtil::IsDomainOverride(permission) &&
      context_->render_frame_host()) {
    BrowserContext* browser_context = context_->GetBrowserContext();
    if (browser_context &&
        PermissionUtil::ValidateDomainOverride(
            {type.value()}, context_->render_frame_host(), permission)) {
      std::move(callback).Run(PermissionControllerImpl::FromBrowserContext(browser_context)
          ->GetPermissionStatusForEmbeddedRequester(
              *type, context_->render_frame_host(),
              PermissionUtil::ExtractDomainOverride(permission)));
      return;
    }
  }
  BrowserContext* browser_context = context_->GetBrowserContext();
  if (!browser_context) {
    std::move(callback).Run(PermissionStatus::DENIED);
    return;
  }

  if (context_->render_frame_host()) {
    browser_context->GetPermissionController()->GetPermissionStatusAsync(*type,
        /*isFromDocument=*/true, context_->render_frame_host(),
        /*origin=*/context_->render_frame_host()->GetLastCommittedOrigin(), std::move(callback));
    return;
  }

  browser_context->GetPermissionController()->GetPermissionStatusAsync(*type,
      /*isFromDocument=*/false, context_->render_process_host(),
      /*origin=*/origin_, std::move(callback));
#else
  std::move(callback).Run(GetPermissionStatus(permission));
#endif  // ARKWEB_NWEB_EX
}

void PermissionServiceImpl::RequestPermissionSync(
    blink::mojom::PermissionDescriptorPtr permission,
    bool user_gesture,
    PermissionStatusCallback callback) {
  LOG(DEBUG) << "RequestPermissionSync user_gesture: " << user_gesture;
  std::vector<PermissionDescriptorPtr> permissions;
  permissions.push_back(std::move(permission));
  RequestPermissions(std::move(permissions), user_gesture,
                     base::BindOnce(&PermissionRequestResponseCallbackWrapper,
                                    std::move(callback)));
}
}   // namespace content
