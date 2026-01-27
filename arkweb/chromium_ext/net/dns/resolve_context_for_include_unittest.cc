/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(ResolveContextTest, NotifyDohStatsInitTest) {
  DnsConfig config = CreateDnsConfig(/*num_servers=*/2, /*num_doh_servers=*/2);
  scoped_refptr<DnsSession> session = CreateDnsSession(config);

  auto request_context = CreateTestURLRequestContextBuilder()->Build();
  ResolveContext context(request_context.get(), /*enable_caching=*/true);
  context.InvalidateCachesAndPerSessionData(session.get(),
                                            false /* network_change */);
  context.NotifyDohStatsInit();

  EXPECT_FALSE(context.is_https_dns_fallback_enabled_);

  context.SetHttpsDnsFallbackEnabled(true);
  EXPECT_TRUE(context.is_https_dns_fallback_enabled_);
  context.NotifyDohStatsInit();
  EXPECT_FALSE(context.doh_server_stats_.empty());
  context.doh_server_stats_.clear();
  EXPECT_NO_FATAL_FAILURE(context.NotifyDohStatsInit());
}
#endif