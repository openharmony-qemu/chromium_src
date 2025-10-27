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

#if BUILDFLAG(ARKWEB_COOKIE)
#include "chrome/browser/profiles/profile_manager.h"
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
void ChromeBrowserMainExtraPartsCef::PreProfileInit() {
  CefRequestContextSettings settings;
  CefContext::Get()->PopulateGlobalRequestContextSettings(&settings);
  g_browser_process->profile_manager()->SetPersistSessionCookies(settings.persist_session_cookies);
}
#endif
