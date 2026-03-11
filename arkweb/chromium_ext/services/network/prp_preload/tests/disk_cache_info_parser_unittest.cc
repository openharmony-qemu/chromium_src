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

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_info_parser.h"

using namespace ohos_prp_preload;

TEST(DiskCacheInfoParserTest, ParseResReqPreloadInfoForPreconnectTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnect(info, dict);
  constexpr int kExpectedDictSize = 6;
  EXPECT_EQ(dict.size(), kExpectedDictSize);
  EXPECT_TRUE(dict.contains("allow_credentials"));
  EXPECT_TRUE(dict.contains("cache_type"));
  EXPECT_TRUE(dict.contains("e_tag"));
  EXPECT_TRUE(dict.contains("freshness_life_times"));
  EXPECT_TRUE(dict.contains("last_modified"));
  EXPECT_TRUE(dict.contains("url"));

  info->set_type(PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  GURL test_url("http://example.com/test");
  info->set_url(test_url);
  DiskCacheInfoParser::SetUrlToJson(info, "url", dict);
  std::string* stored_url = dict.FindString("url");
  ASSERT_NE(stored_url, nullptr);
  EXPECT_TRUE(stored_url->starts_with(PRPP_PREFLIGHT_PREFIX));

  base::Value::Dict dict2;
  dict2.Set("param_name_to_test",
            static_cast<int>(PRRequestCacheType::DISABLE_CACHE));
  auto json = base::Value(std::move(dict2));
  EXPECT_FALSE(
      DiskCacheInfoParser::GetCacheInfoFromJson(json, "cache_info", info));
  EXPECT_FALSE(
      DiskCacheInfoParser::GetCacheTypeFromJson(json, "cache_type", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetETagFromJson(json, "e_tag", info));
}

TEST(DiskCacheInfoParserTest, ParseJsonForPreconnectTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("allow_credentials", true);
  dict.Set("url", "http://example.com/test");
  dict.Set("last_modified", "last_modified_value");
  dict.Set("freshness_life_times", "0");
  dict.Set("e_tag", "e_tag_test");
  dict.Set("cache_type", static_cast<int>(PRRequestCacheType::DISABLE_CACHE));
  base::Value json(std::move(dict));
  auto res = DiskCacheInfoParser::ParseJsonForPreconnect(json, info);
  EXPECT_TRUE(res);

  base::Value::Dict dict2;
  dict2.Set("not_allow_credentials", true);
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(DiskCacheInfoParser::GetAllowCredentialsFromJson(
      json, "allow_credentials", info));
  EXPECT_FALSE(DiskCacheInfoParser::ParseJsonForPreconnect(json, info));
}

TEST(DiskCacheInfoParserTest, ParseResReqPreloadInfoForPreloadTest) {
  auto info = std::make_shared<PRRequestInfo>();
  auto sourceType = std::make_optional(base::flat_set<net::SourceStream::SourceType>(
      {net::SourceStream::SourceType::TYPE_BROTLI}));
  info->set_accepted_stream_types(sourceType);
  std::optional<net::cookie_util::StorageAccessStatus> saStatus =
      net::cookie_util::StorageAccessStatus::kActive;
  info->set_storage_access_status(saStatus);
  info->set_upgrade_if_insecure(true);
  net::CookieSettingOverrides cookie_setting_overrides =
      net::CookieSettingOverrides::All();
  info->set_cookie_setting_overrides(cookie_setting_overrides);
  std::set<std::string> dynamic_header_keys{"a", "b", "c"};
  info->set_dynamic_header_keys(dynamic_header_keys);
  net::HttpRequestHeaders headers;
  headers.SetHeader("key_test", "value_test");
  info->set_extra_request_headers(headers);

  base::Value::Dict dict;
  DiskCacheInfoParser::ParseResReqPreloadInfoForPreload(info, dict);
  constexpr int kPRreloadParseFuncMapSize = 30;
  EXPECT_EQ(dict.size(), kPRreloadParseFuncMapSize);
  EXPECT_TRUE(dict.contains("storage_access_status"));
  EXPECT_TRUE(dict.contains("accepted_stream_types"));
  EXPECT_TRUE(dict.contains("ad_tagged"));
  EXPECT_TRUE(dict.contains("upgrade_if_insecure"));
  auto res = dict.FindBool("upgrade_if_insecure");
  EXPECT_TRUE(res);

  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::ParseJsonForPreload(json, info));

  base::Value::Dict dict2;
  dict2.Set("not_ad_tagged", true);
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(
      DiskCacheInfoParser::GetAdTaggedFromJson(json, "ad_tagged", info));
  EXPECT_FALSE(DiskCacheInfoParser::ParseJsonForPreload(json, info));
}

TEST(DiskCacheInfoParserTest, ParseResReqPreloadInfoForPreconnectLimitTest) {
  auto info = std::make_shared<PRRequestInfo>();
  GURL test_url("http://example.com/test");
  info->set_url(test_url);

  base::Value::Dict dict;
  DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnectLimit(info, dict);
  constexpr int kPreconnectLimitParseFuncMapSize = 4;
  EXPECT_EQ(dict.size(), kPreconnectLimitParseFuncMapSize);
  EXPECT_TRUE(dict.contains("limit_num"));
  EXPECT_TRUE(dict.contains("allow_credentials"));
  EXPECT_TRUE(dict.contains("request_info_type"));
  EXPECT_TRUE(dict.contains("url"));

  auto stringPointer = dict.FindString("limit_num");
  if (stringPointer != nullptr) {
    EXPECT_EQ(*stringPointer, "0");
  }

  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::ParseJsonForPreconnectLimit(json, info));

  base::Value::Dict dict2;
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(DiskCacheInfoParser::ParseJsonForPreconnectLimit(json, info));
}

TEST(DiskCacheInfoParserTest, SetAndGetPageIndexToJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  info->set_page_index(0);

  base::Value::Dict dict;
  DiskCacheInfoParser::SetPageIndexToJson(info, "page_index", dict);
  constexpr int kDictSize = 1;
  EXPECT_EQ(dict.size(), kDictSize);

  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetPageIndexFromJson(json, "page_index",
                                                        info));

  base::Value::Dict dict2;
  dict2.Set("page_index", "");
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(DiskCacheInfoParser::GetPageIndexFromJson(json, "page_index",
                                                         info));

  base::Value::Dict dict3;
  dict3.Set("page_index", "123abc");
  json = base::Value(std::move(dict3));
  EXPECT_FALSE(DiskCacheInfoParser::GetPageIndexFromJson(json, "page_index",
                                                         info));
}

TEST(DiskCacheInfoParserTest, SetPageOriginToJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  info->set_page_origin("test");

  base::Value::Dict dict;
  DiskCacheInfoParser::SetPageOriginToJson(info, "page_origin", dict);
  EXPECT_TRUE(dict.contains("page_origin"));

  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetPageOriginFromJson(json, "page_origin",
                                                         info));
  EXPECT_FALSE(DiskCacheInfoParser::GetPageOriginFromJson(json,
                                                          "not_page_origin",
                                                          info));
}

TEST(DiskCacheInfoParserTest, GetFreshnessLifeTimesFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("not_freshness_life_times", "0");
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetFreshnessLifeTimesFromJson(
      json, "freshness_life_times", info));

  base::Value::Dict dict2;
  dict2.Set("freshness_life_times", "123abc");
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(DiskCacheInfoParser::GetFreshnessLifeTimesFromJson(
      json, "freshness_life_times", info));
}

TEST(DiskCacheInfoParserTest, GetCookieSettingOverridesFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value::List empty_list;
  dict.Set("not_cookie_setting_overrides", std::move(empty_list));
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetCookieSettingOverridesFromJson(
      json, "cookie_setting_overrides", info));
}

TEST(DiskCacheInfoParserTest, GetDoNotPromptForLoginFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetDoNotPromptForLoginFromJson(
      json, "do_not_prompt_for_login", info));
}

TEST(DiskCacheInfoParserTest, GetDynamicHeaderKeysFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetDynamicHeaderKeysFromJson(
      json, "dynamic_header_keys", info));
}

TEST(DiskCacheInfoParserTest, GetExtraRequestHeadersFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetExtraRequestHeadersFromJson(
      json, "extra_request_headers", info));
}

TEST(DiskCacheInfoParserTest, GetFirstPartyUrlPolicyFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetFirstPartyUrlPolicyFromJson(
      json, "first_party_url_policy", info));
}

TEST(DiskCacheInfoParserTest, GetOptionalValueFailFromJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetForceIgnoreSiteForCookiesFromJson(
      json, "force_ignore_site_for_cookies", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetForceMainFrameForSameSiteCookiesFromJson(
      json, "force_main_frame_for_same_site_cookies", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetIdempotencyFromJson(json, "idempotency",
                                                           info));
  EXPECT_FALSE(DiskCacheInfoParser::GetInitiatorFromJson(json, "initiator",
                                                         info));
  EXPECT_FALSE(DiskCacheInfoParser::GetIsForWebsocketsFromJson(
      json, "is_for_websockets", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetKeepaliveFromJson(json, "keepalive",
                                                         info));
  EXPECT_FALSE(DiskCacheInfoParser::GetLoadFlagsFromJson(json, "load_flags",
                                                         info));
  EXPECT_FALSE(DiskCacheInfoParser::GetMethodFromJson(json, "method", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetLimitNumFromJson(json, "limit_num",
                                                        info));

  base::Value::Dict dict2;
  dict2.Set("limit_num", "123abc");
  json = base::Value(std::move(dict2));
  EXPECT_FALSE(DiskCacheInfoParser::GetLimitNumFromJson(json, "limit_num",
                                                        info));
  EXPECT_FALSE(DiskCacheInfoParser::GetPreloadFlagsFromJson(json,
                                                            "preload_flag",
                                                            info));
  EXPECT_FALSE(DiskCacheInfoParser::GetPreloadSeqNumFromJson(json,
                                                             "preload_seq_num",
                                                             info));
  EXPECT_FALSE(DiskCacheInfoParser::GetReferrerFromJson(json, "referrer",
                                                        info));
  EXPECT_FALSE(DiskCacheInfoParser::GetReferrerPolicyFromJson(
      json, "referrer_policy", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestCredentialsModeFromJson(
      json, "request_credentials_mode", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestDestinationFromJson(
      json, "request_destination", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestEndTimeFromJson(
      json, "request_end_time", info));

  base::Value::Dict dict3;
  dict3.Set("request_end_time", "123abc");
  json = base::Value(std::move(dict3));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestEndTimeFromJson(
      json, "request_end_time", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestInfoTypeFromJson(
      json, "request_info_type", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestModeFromJson(json,
                                                           "request_mode",
                                                           info));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestStartTimeFromJson(
      json, "request_start_time", info));
  dict3.Set("request_start_time", "123abc");
  json = base::Value(std::move(dict3));
  EXPECT_FALSE(DiskCacheInfoParser::GetRequestStartTimeFromJson(
      json, "request_start_time", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetResourceTypeFromJson(json,
                                                            "resource_type",
                                                            info));
  EXPECT_FALSE(DiskCacheInfoParser::GetSecureDnsPolicyFromJson(
      json, "secure_dns_policy", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetSendClientCertsFromJson(
      json, "send_client_certs", info));
  EXPECT_TRUE(DiskCacheInfoParser::GetStorageAccessStatusFromJson(
      json, "storage_access_status", info));

  constexpr int kOutOfBoundsValue = -1;
  dict3.Set("storage_access_status", kOutOfBoundsValue);
  json = base::Value(std::move(dict3));
  EXPECT_FALSE(DiskCacheInfoParser::GetStorageAccessStatusFromJson(
      json, "storage_access_status", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetUpgradeIfInsecureFromJson(
      json, "upgrade_if_insecure", info));
  EXPECT_FALSE(DiskCacheInfoParser::GetUrlFromJson(json, "url", info));
}

TEST(DiskCacheInfoParserTest, SetInitiatorToJsonTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  std::optional<url::Origin> opt_origin =
      url::Origin::Create(GURL("http://example.com/test"));
  info->set_initiator(opt_origin);
  DiskCacheInfoParser::SetInitiatorToJson(info, "initiator", dict);
  const std::string* initiator = dict.FindString("initiator");
  if (initiator != nullptr) {
    EXPECT_EQ(*initiator, "http://example.com/");
  }
  EXPECT_TRUE(dict.contains("initiator"));
}

TEST(DiskCacheInfoParserTest, SetInitiatorToJsonNoValueTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  std::optional<url::Origin> opt_origin;
  if (info->initiator()) {
    dict.Set("initiator", info->initiator()->Serialize());
  }
  DiskCacheInfoParser::SetInitiatorToJson(info, "initiator", dict);
  const std::string* initiator = dict.FindString("initiator");
  EXPECT_NE(initiator, nullptr);
}

TEST(DiskCacheInfoParserTest, SetAcceptedStreamTypesToJsonNoValueTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  DiskCacheInfoParser::SetAcceptedStreamTypesToJson(info, "accepted_stream_types", dict);
  EXPECT_FALSE(dict.contains("accepted_stream_types"));
}

TEST(DiskCacheInfoParserTest, SetStorageAccessStatusToJsonNoValueTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  DiskCacheInfoParser::SetStorageAccessStatusToJson(info, "storage_access_status", dict);
  EXPECT_FALSE(dict.contains("storage_access_status"));
}

TEST(DiskCacheInfoParserTest, SetUrlToJsonPreflightTest) {
  auto info = std::make_shared<PRRequestInfo>();
  GURL test_url("http://example.com/test");
  info->set_url(test_url);
  info->set_type(PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  
  base::Value::Dict dict;
  DiskCacheInfoParser::SetUrlToJson(info, "url", dict);
  const std::string* url = dict.FindString("url");
  ASSERT_NE(url, nullptr);
  EXPECT_TRUE(url->starts_with(PRPP_PREFLIGHT_PREFIX));
}

TEST(DiskCacheInfoParserTest, SetUrlToJsonNormalTest) {
  auto info = std::make_shared<PRRequestInfo>();
  GURL test_url("http://example.com/test");
  info->set_url(test_url);
  info->set_type(PRRequestInfoType::TYPE_PAGE_ORIGIN);
  
  base::Value::Dict dict;
  DiskCacheInfoParser::SetUrlToJson(info, "url", dict);
  const std::string* url = dict.FindString("url");
  ASSERT_NE(url, nullptr);
  EXPECT_FALSE(url->starts_with(PRPP_PREFLIGHT_PREFIX));
}

TEST(DiskCacheInfoParserTest, GetAcceptedStreamTypesFromJsonEmptyTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value::List empty_list;
  dict.Set("accepted_stream_types", std::move(empty_list));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetAcceptedStreamTypesFromJson(json, "accepted_stream_types", info));
}

TEST(DiskCacheInfoParserTest, GetDynamicHeaderKeysFromJsonEmptyTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value::List empty_list;
  dict.Set("dynamic_header_keys", std::move(empty_list));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetDynamicHeaderKeysFromJson(json, "dynamic_header_keys", info));
}

TEST(DiskCacheInfoParserTest, GetStorageAccessStatusFromJsonNullTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetStorageAccessStatusFromJson(json, "storage_access_status", info));
}

TEST(DiskCacheInfoParserTest, GetStorageAccessStatusFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("storage_access_status", static_cast<int>(net::cookie_util::StorageAccessStatus::kActive));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetStorageAccessStatusFromJson(json, "storage_access_status", info));
}

TEST(DiskCacheInfoParserTest, GetCacheTypeFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("cache_type", static_cast<int>(PRRequestCacheType::FORCE_CACHE));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetCacheTypeFromJson(json, "cache_type", info));
}

TEST(DiskCacheInfoParserTest, GetFirstPartyUrlPolicyFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("first_party_url_policy", static_cast<int>(net::RedirectInfo::FirstPartyURLPolicy::NEVER_CHANGE_URL));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetFirstPartyUrlPolicyFromJson(json, "first_party_url_policy", info));
}

TEST(DiskCacheInfoParserTest, GetIdempotencyFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("idempotency", static_cast<int>(net::Idempotency::IDEMPOTENT));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetIdempotencyFromJson(json, "idempotency", info));
}

TEST(DiskCacheInfoParserTest, GetReferrerPolicyFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("referrer_policy", static_cast<int>(net::ReferrerPolicy::CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetReferrerPolicyFromJson(json, "referrer_policy", info));
}

TEST(DiskCacheInfoParserTest, GetSecureDnsPolicyFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("secure_dns_policy", static_cast<int>(net::SecureDnsPolicy::kAllow));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetSecureDnsPolicyFromJson(json, "secure_dns_policy", info));
}

TEST(DiskCacheInfoParserTest, GetPreloadFlagsFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("preload_flag", static_cast<int>(PRRequestFlags::PRPP_FLAGS_VISIBLE));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetPreloadFlagsFromJson(json, "preload_flag", info));
}

TEST(DiskCacheInfoParserTest, GetRequestInfoTypeFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("request_info_type", static_cast<int>(PRRequestInfoType::TYPE_PAGE_ORIGIN));
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetRequestInfoTypeFromJson(json, "request_info_type", info));
}

TEST(DiskCacheInfoParserTest, GetFreshnessLifeTimesFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("freshness_life_times", "123456789");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetFreshnessLifeTimesFromJson(json, "freshness_life_times", info));
}

TEST(DiskCacheInfoParserTest, GetLimitNumFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("limit_num", "100");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetLimitNumFromJson(json, "limit_num", info));
}

TEST(DiskCacheInfoParserTest, GetRequestEndTimeFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("request_end_time", "123456789");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetRequestEndTimeFromJson(json, "request_end_time", info));
}

TEST(DiskCacheInfoParserTest, GetRequestStartTimeFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("request_start_time", "123456789");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetRequestStartTimeFromJson(json, "request_start_time", info));
}

TEST(DiskCacheInfoParserTest, GetPageIndexFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("page_index", "100");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetPageIndexFromJson(json, "page_index", info));
}

TEST(DiskCacheInfoParserTest, GetUrlFromJsonValidTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("url", "http://example.com/test");
  base::Value json(std::move(dict));
  EXPECT_TRUE(DiskCacheInfoParser::GetUrlFromJson(json, "url", info));
}

TEST(DiskCacheInfoParserTest, GetUrlFromJsonEmptyTest) {
  auto info = std::make_shared<PRRequestInfo>();
  base::Value::Dict dict;
  dict.Set("url", "");
  base::Value json(std::move(dict));
  EXPECT_FALSE(DiskCacheInfoParser::GetUrlFromJson(json, "url", info));
}