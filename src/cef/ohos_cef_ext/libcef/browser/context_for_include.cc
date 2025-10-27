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

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
void CefContext::PopulateGlobalOTRRequestContextSettings(
    CefRequestContextSettings* settings) {
  // This value was already normalized in Initialize.
  CefString(&settings->cache_path) = CefString("");

  settings->persist_session_cookies = false;
  // TODO(ARKWEB) check
  // settings->persist_user_preferences = false;

  CefString(&settings->cookieable_schemes_list) =
      CefString(&settings_.cookieable_schemes_list);
  settings->cookieable_schemes_exclude_defaults =
      settings_.cookieable_schemes_exclude_defaults;
  settings->incognito_mode = true;

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  settings->global_request_context = nullptr;
#endif
}
#endif
