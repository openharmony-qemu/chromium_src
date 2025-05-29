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
#ifndef NET_HTTP_CACHE_TRANSACTION_UTILS_H_
#define NET_HTTP_CACHE_TRANSACTION_UTILS_H_

#include "arkweb/build/features/features.h"
#include "net/http/http_cache_transaction.h"

namespace net {
class HttpCache;

class HttpTransactionUtils {
 public:
  HttpCache::Transaction* http_cache_transaction_;
  HttpTransactionUtils(HttpCache::Transaction* http_cache_transaction);
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void UpdateCacheInfo(const HttpResponseInfo& response);
#endif
  int RestartWithSecureDnsOnly(CompletionOnceCallback& callback);

 private:
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  int RestartNetworkRequestWithSecureDnsOnly();
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
};
}  // namespace net

#endif
