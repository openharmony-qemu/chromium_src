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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

TEST_F(HostResolverManagerDnsTest, CanUseSecureDnsFallback) {
    EXPECT_FALSE(resolver_->CanUseSecureDnsFallback(resolve_context_.get()));

    resolver_->dns_client_ = nullptr;
    EXPECT_FALSE(resolver_->CanUseSecureDnsFallback(resolve_context_.get()));
}

TEST_F(HostResolverManagerDnsTest, SetHttpsDnsFallbackData) {
    auto request_context1 = CreateTestURLRequestContextBuilder()->Build();
    auto request_context2 = CreateTestURLRequestContextBuilder()->Build();
    ResolveContext context1(request_context1.get(), true);
    ResolveContext context2(request_context2.get(), true);

    resolver_->RegisterResolveContext(&context1);
    resolver_->RegisterResolveContext(&context2);
    resolver_->SetHttpsDnsFallbackData(true, "https://dns.example.com");
    EXPECT_FALSE(resolver_->warmup_httpdns_fallback_list_.empty());
    resolver_->DeregisterResolveContext(&context1);
    resolver_->DeregisterResolveContext(&context2);
    EXPECT_TRUE(resolver_->https_dns_fallback_enabled_);
}

TEST_F(HostResolverManagerDnsTest, ReportSecureFallbackDnsResult001) {
    testing::internal::CaptureStderr();
    std::vector<IPEndPoint> cacheAdress = {{{192, 168, 1, 1}, 80}, {{192, 167, 1, 1}, 80}};
    constexpr base::TimeDelta kTtl = base::Seconds(3600);
    HostCache::Entry insecure_entry(OK, cacheAdress, std::set<std::string>(), HostCache::Entry::Source::SOURCE_DNS,
                                    kTtl);
    std::optional<HostCache::Entry> insecure_results(insecure_entry);
    std::vector<IPEndPoint> adress = {{{10, 0, 0, 1}, 443}, {{10, 0, 1, 1}, 443}};
    constexpr base::TimeDelta time = base::Seconds(1200);
    HostCache::Entry secure_fallback_results(OK, adress, std::set<std::string>(), HostCache::Entry::Source::SOURCE_DNS,
                                            time);

    constexpr base::TimeDelta second = base::Seconds(100);
    resolver_->ReportSecureFallbackDnsResult(insecure_results, secure_fallback_results, "example.com", 0, second);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output.find("udp_dns_ip_list=[]"), std::string::npos);
    EXPECT_EQ(log_output.find("ip_list=[]"), std::string::npos);
    EXPECT_EQ(log_output.find("scene=normal DNS"), std::string::npos);
}

TEST_F(HostResolverManagerDnsTest, ReportSecureFallbackDnsResult002) {
    testing::internal::CaptureStderr();
    std::vector<IPEndPoint> cacheAdress = {};
    constexpr base::TimeDelta kTtl = base::Seconds(3600);
    HostCache::Entry insecure_entry(OK, cacheAdress, std::set<std::string>(), HostCache::Entry::Source::SOURCE_DNS,
                                    kTtl);
    std::optional<HostCache::Entry> insecure_results(insecure_entry);
    std::vector<IPEndPoint> adress = {};
    constexpr base::TimeDelta time = base::Seconds(1200);
    HostCache::Entry secure_fallback_results(OK, adress, std::set<std::string>(), HostCache::Entry::Source::SOURCE_DNS,
                                            time);

    constexpr base::TimeDelta second = base::Seconds(100);
    resolver_->ReportSecureFallbackDnsResult(insecure_results, secure_fallback_results, "example.com", 0, second);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output.find("udp_dns_ip_list=[]"), std::string::npos);
    EXPECT_EQ(log_output.find("ip_list=[]"), std::string::npos);
    EXPECT_EQ(log_output.find("scene=normal DNS"), std::string::npos);
}
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(HostResolverManagerDnsTest, SetHttpsDnsFallbackDataOnDnsHijacking) {
    std::vector<std::string> protect_list = {"example.com", "test.org"};
    std::vector<std::string> errorcode_list = {"ERR_NAME_NOT_RESOLVED", "ERR_CONNECTION_TIMED_OUT"};
    
    resolver_->SetHttpsDnsFallbackDataOnDnsHijacking(protect_list, errorcode_list);
    EXPECT_EQ(resolver_->dns_hijacking_protect_list_.size(), 2);
    EXPECT_NE(resolver_->dns_hijacking_protect_list_.find("example.com"), 
              resolver_->dns_hijacking_protect_list_.end());
    EXPECT_NE(resolver_->dns_hijacking_protect_list_.find("test.org"), 
              resolver_->dns_hijacking_protect_list_.end());
    EXPECT_EQ(resolver_->dns_hijacking_errorcode_list_.size(), 2);
    EXPECT_NE(resolver_->dns_hijacking_errorcode_list_.find("ERR_NAME_NOT_RESOLVED"), 
              resolver_->dns_hijacking_errorcode_list_.end());
    EXPECT_NE(resolver_->dns_hijacking_errorcode_list_.find("ERR_CONNECTION_TIMED_OUT"), 
              resolver_->dns_hijacking_errorcode_list_.end());
    resolver_->SetHttpsDnsFallbackDataOnDnsHijacking({}, {});
    EXPECT_TRUE(resolver_->dns_hijacking_protect_list_.empty());
    EXPECT_TRUE(resolver_->dns_hijacking_errorcode_list_.empty());
}

TEST_F(HostResolverManagerDnsTest, IsProtectedDomain) {
    std::vector<std::string> protect_list = {"example.com", "test.org", "sub.domain.com"};
    resolver_->SetHttpsDnsFallbackDataOnDnsHijacking(protect_list, {"ERR_DNS_FAIL"});
    GURL empty_url;
    EXPECT_FALSE(resolver_->IsProtectedDomain(empty_url));
    GURL subdomain("http://sub.domain.com");
    EXPECT_TRUE(resolver_->IsProtectedDomain(subdomain));
    GURL not_match("http://notprotected.com");
    EXPECT_FALSE(resolver_->IsProtectedDomain(not_match));
    resolver_->SetHttpsDnsFallbackDataOnDnsHijacking({}, {});
    GURL test_url("http://example.com");
    EXPECT_FALSE(resolver_->IsProtectedDomain(test_url));
}

TEST_F(HostResolverManagerDnsTest, NeedRetryDnsOnDnsHijack) {
    std::vector<std::string> protect_list = {"example.com", "test.org"};
    std::vector<std::string> error_list = {"ERR_DNS_FAIL", "ERR_CONNECTION_TIMED_OUT"};
    resolver_->SetHttpsDnsFallbackDataOnDnsHijacking(protect_list, error_list);
    GURL protected_url("http://example.com");
    EXPECT_TRUE(resolver_->NeedRetryDnsOnDnsHijack(protected_url, "ERR_DNS_FAIL"));
    EXPECT_FALSE(resolver_->NeedRetryDnsOnDnsHijack(protected_url, "ERR_OTHER_ERROR"));
    GURL non_protected_url("http://notprotected.com");
    EXPECT_FALSE(resolver_->NeedRetryDnsOnDnsHijack(non_protected_url, "ERR_DNS_FAIL"));
}
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)
