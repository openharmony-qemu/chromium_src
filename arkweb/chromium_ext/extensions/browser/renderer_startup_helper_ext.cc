/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "arkweb/chromium_ext/extensions/common/mojom/webstore_config.mojom.h"
#include "extensions/browser/renderer_startup_helper.h"
 
namespace extensions {
 
void RendererStartupHelper::SetWebStoreConfig(mojom::Renderer* renderer) {
  if (!renderer) {
    return;
  }
 
  if (!extension_urls::IsWebStoreEnable()) {
    return;
  }
 
  auto config = extension_urls::GetWebStoreConfig();
  mojom::WebStoreConfigPtr configPtr = mojom::WebStoreConfig::New(
      config.api_url, config.launch_url, config.update_url, config.verify_url,
      config.home_page_url);
  renderer->SetWebStoreConfig(std::move(configPtr));
}
 
void RendererStartupHelper::OnWebStoreConfigChanged(
    const extension_urls::WebStoreConfig& config) {
  mojom::WebStoreConfigPtr configPtr = mojom::WebStoreConfig::New(
      config.api_url, config.launch_url, config.update_url, config.verify_url,
      config.home_page_url);
  for (auto& process_entry : process_mojo_map_) {
    content::RenderProcessHost* process = process_entry.first;
    mojom::Renderer* renderer = GetRenderer(process);
    if (renderer) {
      renderer->SetWebStoreConfig(configPtr->Clone());
    }
  }
}
 
}  // namespace extensions