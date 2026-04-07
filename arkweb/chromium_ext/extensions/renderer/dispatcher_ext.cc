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
 
#include "base/logging.h"
#include "extensions/common/extension_urls.h"
#include "extensions/renderer/dispatcher.h"
 
namespace extensions {
 
void Dispatcher::SetWebStoreConfig(mojom::WebStoreConfigPtr config_ptr) {
  extension_urls::WebStoreConfig config;
  config.api_url = config_ptr->api_url;
  config.launch_url = config_ptr->launch_url;
  config.update_url = config_ptr->update_url;
  config.verify_url = config_ptr->verify_url;
  config.home_page_url = config_ptr->home_page_url;
  extension_urls::SetWebStoreConfig(config);
}
 
}  // namespace extensions