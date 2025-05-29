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

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
void ResolveContext::NotifyDohStatsInit() {
  if (!is_https_dns_fallback_enabled_) {
    return;
  }

  if (doh_server_stats_.empty()) {
    return;
  }

  for (size_t i = 0; i < doh_server_stats_.size(); i++) {
    ServerStats* stats = &doh_server_stats_[i];
    stats->last_failure_count = 0;
    stats->current_connection_success = true;
    stats->last_failure = base::TimeTicks();
    stats->last_success = base::TimeTicks();
    LOG(INFO) << "Doh server " << i << " stats init successfully.";
    // TODO(ARKWEB)
    // #ifdef OHOS_LOGGER_REPORT
    //     LOG_FEEDBACK(INFO) << "Doh server " << i << " stats init
    //     successfully.";
    // #endif
  }
}
#endif