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

#ifndef ARKWEB_BROWSER_INFO_MANAGER_UTILS_H_
#define ARKWEB_BROWSER_INFO_MANAGER_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "cef/include/cef_callback.h"
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "chrome/browser/preloading/prefetch/no_state_prefetch/no_state_prefetch_manager_factory.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_contents.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_manager.h"
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

class CefBrowserInfoManager;

class ArkwebBrowserInfoManagerUtils {
 public:
  friend class CefBrowserInfoManager;
  explicit ArkwebBrowserInfoManagerUtils(
      CefBrowserInfoManager* cef_browser_info_manager);
  ~ArkwebBrowserInfoManagerUtils();

  ArkwebBrowserInfoManagerUtils(const ArkwebBrowserInfoManagerUtils&) = delete;
  ArkwebBrowserInfoManagerUtils& operator=(
      const ArkwebBrowserInfoManagerUtils&) = delete;

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  bool CanCreateWindow(content::RenderFrameHost* opener,
                       const GURL& target_url,
                       WindowOpenDisposition disposition,
                       bool user_gesture,
                       CefRefPtr<CefCallback> callback);
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  bool IsExtensionsOptionsUiFrame(
      const content::GlobalRenderFrameHostToken& global_token);
  bool IsExtensionsOffscreenFrame(
      const content::GlobalRenderFrameHostToken& global_token);
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

 private:
  const raw_ptr<CefBrowserInfoManager> cef_browser_info_manager_;

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  static bool IsPrerendering(
      const content::GlobalRenderFrameHostToken& global_token);
  static void CancelForPrerendering(
      const content::GlobalRenderFrameHostToken& global_token,
      int timeout_id);
#endif
};

#endif  // ARKWEB_BROWSER_INFO_MANAGER_UTILS_H_
