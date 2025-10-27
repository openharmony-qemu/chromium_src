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

#include "content/public/browser/browser_context.h"
#include "content/public/browser/cors_origin_pattern_setter.h"
#include "extensions/browser/extension_util.h"
#include "extensions/browser/network_permissions_updater.h"
#include "extensions/common/extension.h"
#include "libcef/browser/net_service/cookie_manager_impl_ext.h"

namespace extensions {

// static
void NetworkPermissionsUpdater::SetCorsOriginAccessListForExtensionHelper(
    const std::vector<content::BrowserContext*>& browser_contexts,
    const Extension& extension,
    std::vector<network::mojom::CorsOriginPatternPtr> allow_patterns,
    std::vector<network::mojom::CorsOriginPatternPtr> block_patterns,
    base::OnceClosure closure) {
  auto barrier_closure =
      BarrierClosure(browser_contexts.size(), std::move(closure));
  for (content::BrowserContext* browser_context : browser_contexts) {
    // SetCorsOriginAccessListForExtensionHelper should only affect an incognito
    // profile if the extension is actually allowed to run in an incognito
    // profile (not just by the extension manifest, but also by user
    // preferences).
    if (browser_context->IsOffTheRecord()) {
      DCHECK(util::IsIncognitoEnabled(extension.id(), browser_context));
    }

    CefRefPtr<CefCookieManagerImplExt> cookie_manager =
        CefCookieManagerImplExt::GetInstance(browser_context->IsOffTheRecord());
    if (cookie_manager) {
      cookie_manager->SetOriginAccessListForOrigin(extension.origin(),
                                                   mojo::Clone(allow_patterns),
                                                   mojo::Clone(block_patterns));
    }

    content::CorsOriginPatternSetter::Set(
        browser_context, extension.origin(), mojo::Clone(allow_patterns),
        mojo::Clone(block_patterns), barrier_closure);
  }
}

}  // namespace extensions
