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

#include "content/browser/webauth/authenticator_common_impl.h"

#include "content/browser/webauth/utils.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/web_authentication_delegate.h"
#include "content/public/common/content_client.h"

namespace content {

namespace {

WebAuthenticationDelegate* GetWebAuthenticationDelegate() {
  return GetContentClient()->browser()->GetWebAuthenticationDelegate();
}

void UpdateCapabilities(
    blink::mojom::Authenticator::GetClientCapabilitiesCallback callback,
    std::vector<blink::mojom::WebAuthnClientCapabilityPtr> result,
    std::optional<bool> conditional_get,
    std::optional<bool> is_uvpaa_override) {
  if (!conditional_get && is_uvpaa_override) {
    conditional_get = *is_uvpaa_override;
  }
  if (conditional_get) {
    for (auto& item : result) {
      if (item->name ==
            client_capabilities::kConditionalGet) {
        item->supported = *conditional_get;
        break;
      }
    }
  }
  if (is_uvpaa_override) {
    for (auto& item : result) {
      if (item->name ==
            client_capabilities::kUserVerifyingPlatformAuthenticator) {
        item->supported = *is_uvpaa_override;
        break;
      }
    }
  }
  std::move(callback).Run(std::move(result));
}

} // namespace

void AuthenticatorCommonImpl::GetClientCapabilitiesExt(
    url::Origin caller_origin,
    blink::mojom::Authenticator::GetClientCapabilitiesCallback callback) {
  GetClientCapabilitiesFromOhosWebAuthnApi(base::BindOnce([](
      url::Origin caller_origin,
      base::WeakPtr<AuthenticatorCommonImpl> thiz,
      blink::mojom::Authenticator::GetClientCapabilitiesCallback callback,
      std::vector<blink::mojom::WebAuthnClientCapabilityPtr> result) {
        if (!thiz) {
          std::move(callback).Run(std::move(result));
          return;
        }

        // See `IsUvpaaAvailableInternal`. Proxy handles request first.
        WebAuthenticationRequestProxy* proxy =
            thiz->GetWebAuthnRequestProxyIfActive(caller_origin);
        std::optional<bool> conditional_get;
        if (proxy) {
          conditional_get = false;
        }
        auto update_capabilities_callback = base::BindOnce(&UpdateCapabilities,
            std::move(callback), std::move(result), conditional_get);
        if (proxy) {
          proxy->SignalIsUvpaaRequest(base::BindOnce([](
            base::OnceCallback<void(std::optional<bool>)> callback,
            bool available) {
              std::move(callback).Run(available);
            }, std::move(update_capabilities_callback)));
          return;
        }

        WebAuthenticationDelegate* delegate = GetWebAuthenticationDelegate();
        if (!delegate) {
          std::move(callback).Run(std::move(result));
          return;
        }
        delegate->IsUserVerifyingPlatformAuthenticatorAvailableOverride(
          thiz->GetRenderFrameHost(), std::move(update_capabilities_callback));
    }, caller_origin, weak_factory_.GetWeakPtr(), std::move(callback)));
}

} // namespace content
