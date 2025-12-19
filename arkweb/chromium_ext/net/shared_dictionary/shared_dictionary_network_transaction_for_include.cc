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
int SharedDictionaryNetworkTransaction::RestartWithFallbackProxy(
    CompletionOnceCallback callback) {
  shared_dictionary_used_response_info_.reset();
  return network_transaction_->RestartWithFallbackProxy(
      base::BindOnce(&SharedDictionaryNetworkTransaction::OnStartCompleted,
                     base::Unretained(this), std::move(callback)));
}

int SharedDictionaryNetworkTransaction::RestartWithDirect(
    CompletionOnceCallback callback) {
  shared_dictionary_used_response_info_.reset();
  return network_transaction_->RestartWithDirect(
      base::BindOnce(&SharedDictionaryNetworkTransaction::OnStartCompleted,
                     base::Unretained(this), std::move(callback)));
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}  // namespace net
