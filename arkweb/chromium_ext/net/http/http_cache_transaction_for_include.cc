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

namespace net {

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
int HttpCache::Transaction::RestartWithFallbackProxy(
    CompletionOnceCallback callback) {
  // Ensure that we only have one asynchronous call at a time.
  DCHECK(callback_.is_null());

  if (!cache_.get()) {
    return ERR_UNEXPECTED;
  }

  int rv = RestartNetworkRequestWithFallbackProxy();
  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  }

  return rv;
}

int HttpCache::Transaction::RestartNetworkRequestWithFallbackProxy() {
  DCHECK((mode_ & WRITE) || mode_ == NONE);
  DCHECK(network_trans_.get());
  DCHECK_EQ(STATE_NONE, next_state_);

  next_state_ = STATE_SEND_REQUEST_COMPLETE;
  if (request_ != initial_request_ && custom_request_) {
    custom_request_->secure_dns_only = false;
    custom_request_->retry_with_fallback_proxy = true;
    LOG(DEBUG)
        << "RestartNetworkRequestWithFallbackProxy retry_with_fallback_proxy "
        << request_->retry_with_fallback_proxy << ", url "
        << url::LogUtils::ConvertUrlWithMask(request_->url.spec());
  }
  int rv = network_trans_->RestartWithFallbackProxy(io_callback_);
  if (rv != ERR_IO_PENDING) {
    return DoLoop(rv);
  }
  return rv;
}

int HttpCache::Transaction::RestartWithDirect(CompletionOnceCallback callback) {
  // Ensure that we only have one asynchronous call at a time.
  DCHECK(callback_.is_null());

  if (!cache_.get()) {
    return ERR_UNEXPECTED;
  }

  int rv = RestartNetworkRequestWithDirect();
  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  }

  return rv;
}

int HttpCache::Transaction::RestartNetworkRequestWithDirect() {
  DCHECK((mode_ & WRITE) || mode_ == NONE);
  DCHECK(network_trans_.get());
  DCHECK_EQ(STATE_NONE, next_state_);

  next_state_ = STATE_SEND_REQUEST_COMPLETE;
  if (request_ != initial_request_ && custom_request_) {
    custom_request_->secure_dns_only = false;
    custom_request_->retry_with_fallback_proxy = false;
    custom_request_->load_flags |= LOAD_BYPASS_PROXY;
    LOG(DEBUG)
        << "RestartNetworkRequestWithDirect load_flags with LOAD_BYPASS_PROXY"
        << ", url " << url::LogUtils::ConvertUrlWithMask(request_->url.spec());
  }
  int rv = network_trans_->RestartWithDirect(io_callback_);
  if (rv != ERR_IO_PENDING) {
    return DoLoop(rv);
  }
  return rv;
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}
