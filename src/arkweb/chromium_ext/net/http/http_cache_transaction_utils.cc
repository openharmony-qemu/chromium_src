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
#include "arkweb/chromium_ext/net/http/http_cache_transaction_utils.h"

#include "base/logging.h"
#include "net/http/http_cache.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/net/base/page_res_request_info.h"
#endif

namespace net {

HttpTransactionUtils::HttpTransactionUtils(HttpCache::Transaction* http_cache_transaction)
{
  this->http_cache_transaction_ = http_cache_transaction;
}

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void HttpTransactionUtils::UpdateCacheInfo(const HttpResponseInfo& response)
{
  if (http_cache_transaction_->preload_info_ == nullptr || response.headers == nullptr) {
    return;
  }

  ohos_prp_preload::PRRequestCacheType cache_type =
    ohos_prp_preload::PRRequestCacheType::DISABLE_CACHE;
  int64_t freshness_life_times = 0;
  base::TimeDelta freshnessLifetimes = response.headers->
      GetFreshnessLifetimes(response.response_time).freshness;
  if (!freshnessLifetimes.is_zero()) {
    freshness_life_times = (response.response_time +
                            freshnessLifetimes -
                            response.headers
                              ->GetCurrentAge(response.request_time,
                                              response.response_time,
                                              response.response_time)).ToInternalValue();
    cache_type = ohos_prp_preload::PRRequestCacheType::FORCE_CACHE;
  } else if (response.headers->HasHeader("etag") ||
             response.headers->HasHeader("last-modified")) {
    cache_type = ohos_prp_preload::PRRequestCacheType::NEGOTIATION_CACHE;
  }

  std::string e_tag;
  response.headers->EnumerateHeader(nullptr, "etag", &e_tag);

  std::string last_modified;
  response.headers->EnumerateHeader(nullptr, "last-modified", &last_modified);

  http_cache_transaction_->preload_info_->set_cache_info(cache_type, freshness_life_times, e_tag, last_modified);
}
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
int HttpTransactionUtils::RestartWithSecureDnsOnly(
    CompletionOnceCallback& callback)
{
  // Ensure that we only have one asynchronous call at a time.
  DCHECK(http_cache_transaction_->callback_.is_null());

  if (!http_cache_transaction_->cache_.get()) {
    return ERR_UNEXPECTED;
  }

  int rv = RestartNetworkRequestWithSecureDnsOnly();
  if (rv == ERR_IO_PENDING) {
    http_cache_transaction_->callback_ = std::move(callback);
  }

  return rv;
}

int HttpTransactionUtils::RestartNetworkRequestWithSecureDnsOnly()
{
  DCHECK(
      (http_cache_transaction_->mode_ & HttpCache::Transaction::Mode::WRITE) ||
      (http_cache_transaction_->mode_ == HttpCache::Transaction::Mode::NONE));
  DCHECK(http_cache_transaction_->network_trans_.get());
  DCHECK_EQ(HttpCache::Transaction::State::STATE_NONE,
            http_cache_transaction_->next_state_);

  http_cache_transaction_->next_state_ =
      HttpCache::Transaction::State::STATE_SEND_REQUEST_COMPLETE;
  if (http_cache_transaction_->request_ !=
          http_cache_transaction_->initial_request_ &&
      http_cache_transaction_->custom_request_) {
    http_cache_transaction_->custom_request_->secure_dns_only = true;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO)
        << "RestartNetworkRequestWithSecureDnsOnly secure_dns_only "
        << http_cache_transaction_->request_->secure_dns_only << ", url: "
        << url::LogUtils::ConvertUrlWithMask(http_cache_transaction_->request_->url.spec());
#endif
  }
  int rv = http_cache_transaction_->network_trans_->RestartWithSecureDnsOnly(
      http_cache_transaction_->io_callback_);
  if (rv != ERR_IO_PENDING) {
    return http_cache_transaction_->DoLoop(rv);
  }
  return rv;
}
#endif
}
