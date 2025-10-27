/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CEF_LIBCEF_BROWSER_ADBLOCK_CONTENT_SUBRESOURCE_FILTER_WEB_CONTENTS_HELPER_FACTORY_H_
#define CEF_LIBCEF_BROWSER_ADBLOCK_CONTENT_SUBRESOURCE_FILTER_WEB_CONTENTS_HELPER_FACTORY_H_

namespace content {
class NavigationHandle;
}  // namespace content

// Creates a ContentSubresourceFilterThrottleManager and attaches it to
// |web_contents|, passing it embedder-level state.
void CreateSubresourceFilterWebContentsHelper(
    content::NavigationHandle* navigation_handle);

#endif  // CEF_LIBCEF_BROWSER_ADBLOCK_CONTENT_SUBRESOURCE_FILTER_WEB_CONTENTS_HELPER_FACTORY_H_
