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

#include "arkweb/build/features/features.h"

namespace network {

#if BUILDFLAG(ARKWEB_COOKIE)
void CookieManager::SetCanonicalCookieSync(
    const net::CanonicalCookie& cookie,
    const GURL& source_url,
    const net::CookieOptions& cookie_options,
    SetCanonicalCookieCallback callback) {
  SetCanonicalCookie(cookie, source_url, cookie_options, std::move(callback));
}

void CookieManager::GetCookieListSync(
    const GURL& url,
    const net::CookieOptions& cookie_options,
    const net::CookiePartitionKeyCollection& cookie_partition_key_collection,
    GetCookieListCallback callback) {
  GetCookieList(url, cookie_options, cookie_partition_key_collection,
                std::move(callback));
}

void CookieManager::GetAllCookiesSync(GetAllCookiesCallback callback) {
  GetAllCookies(std::move(callback));
}

void CookieManager::DeleteCookiesSync(mojom::CookieDeletionFilterPtr filter,
                                      DeleteCookiesCallback callback) {
  DeleteCookies(std::move(filter), std::move(callback));
}

void CookieManager::SetAllCookies(const net::CookieList& list,
                                  SetAllCookiesCallback callback) {
  cookie_store_->SetAllCookiesAsync(list, std::move(callback));
}
#endif

}  // namespace network
