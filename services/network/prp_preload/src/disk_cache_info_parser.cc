// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/disk_cache_info_parser.h"

namespace ohos_prp_preload {
using SetParamToJsonFunc = void (*)(const std::shared_ptr<PRRequestInfo>& info,
                                    const std::string& param_name,
                                    base::Value::Dict& dict);
using GetParamFromJsonFunc =
    bool (*)(const base::Value& json,
             const std::string& param_name,
             const std::shared_ptr<PRRequestInfo>& info);
struct ParseFunc {
  SetParamToJsonFunc set_to_json_func_;
  GetParamFromJsonFunc get_from_json_func_;
};

const std::unordered_map<std::string, ParseFunc>
  PRECONNECT_PARSE_FUNC_MAP = {
    { "allow_credentials",
      { &DiskCacheInfoParser::SetAllowCredentialsToJson,
        &DiskCacheInfoParser::GetAllowCredentialsFromJson } },
    { "cache_info",
      { &DiskCacheInfoParser::SetCacheInfoToJson,
        &DiskCacheInfoParser::GetCacheInfoFromJson } },
    { "url",
      { &DiskCacheInfoParser::SetUrlToJson,
        &DiskCacheInfoParser::GetUrlFromJson } },
};

const std::unordered_map<std::string, ParseFunc>
  PRELOAD_PARSE_FUNC_MAP = {
    { "accepted_stream_types",
      { &DiskCacheInfoParser::SetAcceptedStreamTypesToJson,
        &DiskCacheInfoParser::GetAcceptedStreamTypesFromJson } },
    { "cookie_setting_overrides",
      { &DiskCacheInfoParser::SetCookieSettingOverridesToJson,
        &DiskCacheInfoParser::GetCookieSettingOverridesFromJson } },
    { "corb_detachable",
      { &DiskCacheInfoParser::SetCorbDetachableToJson,
        &DiskCacheInfoParser::GetCorbDetachableFromJson } },
    { "do_not_prompt_for_login",
      { &DiskCacheInfoParser::SetDoNotPromptForLoginToJson,
        &DiskCacheInfoParser::GetDoNotPromptForLoginFromJson } },
    { "dynamic_header_keys",
      { &DiskCacheInfoParser::SetDynamicHeaderKeysToJson,
        &DiskCacheInfoParser::GetDynamicHeaderKeysFromJson } },
    { "extra_request_headers",
      { &DiskCacheInfoParser::SetExtraRequestHeadersToJson,
        &DiskCacheInfoParser::GetExtraRequestHeadersFromJson } },
    { "first_party_url_policy",
      { &DiskCacheInfoParser::SetFirstPartyUrlPolicyToJson,
        &DiskCacheInfoParser::GetFirstPartyUrlPolicyFromJson } },
    { "force_ignore_site_for_cookies",
      { &DiskCacheInfoParser::SetForceIgnoreSiteForCookiesToJson,
        &DiskCacheInfoParser::GetForceIgnoreSiteForCookiesFromJson } },
    { "force_ignore_top_frame_party_for_cookies",
      { &DiskCacheInfoParser::SetForceIgnoreTopFramePartyForCookiesToJson,
        &DiskCacheInfoParser::GetForceIgnoreTopFramePartyForCookiesFromJson } },
    { "force_main_frame_for_same_site_cookies",
      { &DiskCacheInfoParser::SetForceMainFrameForSameSiteCookiesToJson,
        &DiskCacheInfoParser::GetForceMainFrameForSameSiteCookiesFromJson } },
    { "has_storage_access",
      { &DiskCacheInfoParser::SetHasStorageAccessToJson,
        &DiskCacheInfoParser::GetHasStorageAccessFromJson } },
    { "idempotency",
      { &DiskCacheInfoParser::SetIdempotencyToJson,
        &DiskCacheInfoParser::GetIdempotencyFromJson } },
    { "initiator",
      { &DiskCacheInfoParser::SetInitiatorToJson,
        &DiskCacheInfoParser::GetInitiatorFromJson } },
    { "is_for_websockets",
      { &DiskCacheInfoParser::SetIsForWebsocketsToJson,
        &DiskCacheInfoParser::GetIsForWebsocketsFromJson } },
    { "keepalive",
      { &DiskCacheInfoParser::SetKeepaliveToJson,
        &DiskCacheInfoParser::GetKeepaliveFromJson } },
    { "load_flags",
      { &DiskCacheInfoParser::SetLoadFlagsToJson,
        &DiskCacheInfoParser::GetLoadFlagsFromJson } },
    { "method",
      { &DiskCacheInfoParser::SetMethodToJson,
        &DiskCacheInfoParser::GetMethodFromJson } },
    { "preload_flag",
      { &DiskCacheInfoParser::SetPreloadFlagsToJson,
        &DiskCacheInfoParser::GetPreloadFlagsFromJson } },
    { "referrer",
      { &DiskCacheInfoParser::SetReferrerToJson,
        &DiskCacheInfoParser::GetReferrerFromJson } },
    { "referrer_policy",
      { &DiskCacheInfoParser::SetReferrerPolicyToJson,
        &DiskCacheInfoParser::GetReferrerPolicyFromJson } },
    { "request_credentials_mode",
      { &DiskCacheInfoParser::SetRequestCredentialsModeToJson,
        &DiskCacheInfoParser::GetRequestCredentialsModeFromJson } },
    { "request_destination",
      { &DiskCacheInfoParser::SetRequestDestinationToJson,
        &DiskCacheInfoParser::GetRequestDestinationFromJson } },
    { "request_end_time",
      { &DiskCacheInfoParser::SetRequestEndTimeToJson,
        &DiskCacheInfoParser::GetRequestEndTimeFromJson } },
    { "request_info_type",
      { &DiskCacheInfoParser::SetRequestInfoTypeToJson,
        &DiskCacheInfoParser::GetRequestInfoTypeFromJson } },
    { "request_mode",
      { &DiskCacheInfoParser::SetRequestModeToJson,
        &DiskCacheInfoParser::GetRequestModeFromJson } },
    { "request_start_time",
      { &DiskCacheInfoParser::SetRequestStartTimeToJson,
        &DiskCacheInfoParser::GetRequestStartTimeFromJson } },
    { "resource_type",
      { &DiskCacheInfoParser::SetResourceTypeToJson,
        &DiskCacheInfoParser::GetResourceTypeFromJson } },
    { "secure_dns_policy",
      { &DiskCacheInfoParser::SetSecureDnsPolicyToJson,
        &DiskCacheInfoParser::GetSecureDnsPolicyFromJson } },
    { "send_client_certs",
      { &DiskCacheInfoParser::SetSendClientCertsToJson,
        &DiskCacheInfoParser::GetSendClientCertsFromJson } },
    { "upgrade_if_insecure",
      { &DiskCacheInfoParser::SetUpgradeIfInsecureToJson,
        &DiskCacheInfoParser::GetUpgradeIfInsecureFromJson } },
};

void DiskCacheInfoParser::ParseResReqPreloadInfoForPreconnect(
    const std::shared_ptr<PRRequestInfo>& info,
    base::Value::Dict& dict) {
  for (const auto& it : PRECONNECT_PARSE_FUNC_MAP) {
    it.second.set_to_json_func_(info, it.first, dict);
  }
}

bool DiskCacheInfoParser::ParseJsonForPreconnect(
    const base::Value& json,
    const std::shared_ptr<PRRequestInfo>& info) {
  for (const auto& it : PRECONNECT_PARSE_FUNC_MAP) {
    if (!it.second.get_from_json_func_(json, it.first, info)) {
      return false;
    }
  }
  return true;
}

void DiskCacheInfoParser::ParseResReqPreloadInfoForPreload(
    const std::shared_ptr<PRRequestInfo>& info,
    base::Value::Dict& dict) {
  for (const auto& it : PRELOAD_PARSE_FUNC_MAP) {
    it.second.set_to_json_func_(info, it.first, dict);
  }
}

bool DiskCacheInfoParser::ParseJsonForPreload(
    const base::Value& json,
    const std::shared_ptr<PRRequestInfo>& info) {
  for (const auto& it : PRELOAD_PARSE_FUNC_MAP) {
    if (!it.second.get_from_json_func_(json, it.first, info)) {
      return false;
    }
  }
  return true;
}

void DiskCacheInfoParser::SetAcceptedStreamTypesToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  if (info->accepted_stream_types().has_value()) {
    base::Value::List accepted_stream_types_list;
    for (const auto& accepted_stream_type :
         info->accepted_stream_types().value()) {
      accepted_stream_types_list.Append(static_cast<int>(accepted_stream_type));
    }
    dict.Set(param_name, std::move(accepted_stream_types_list));
  }
}

bool DiskCacheInfoParser::GetAcceptedStreamTypesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const base::Value::List* accepted_stream_types_list =
      json.GetDict().FindList(param_name);
  if (accepted_stream_types_list != nullptr) {
    std::vector<net::SourceStream::SourceType> accepted_stream_types;
    for (const auto& accepted_stream_type : *accepted_stream_types_list) {
      accepted_stream_types.push_back(
          static_cast<net::SourceStream::SourceType>(
              accepted_stream_type.GetInt()));
    }
    if (accepted_stream_types.size()) {
      info->set_accepted_stream_types(accepted_stream_types);
    }
  }
  // accepted_stream_types_list can be null, always return true
  return true;
}

void DiskCacheInfoParser::SetAllowCredentialsToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->allow_credentials());
}

bool DiskCacheInfoParser::GetAllowCredentialsFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> allow_credentials =
      json.GetDict().FindBool(param_name);
  if (!allow_credentials.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetAllowCredentialsFromJson allow_credentials is invalid";
    return false;
  }
  info->set_allow_credentials(allow_credentials.value());
  return true;
}
void DiskCacheInfoParser::SetCacheInfoToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  using SetCacheInfoParamToJsonFunc =
      void (*)(const CacheInfo& cache_info, const std::string& param_name,
               base::Value::Dict& dict);
  static const std::unordered_map<std::string, SetCacheInfoParamToJsonFunc>
    SET_CACHE_INFO_PARAM_TO_JSON_FUNC_MAP = {
      { "cache_type", &DiskCacheInfoParser::SetCacheTypeToJson },
      { "e_tag", &DiskCacheInfoParser::SetETagToJson },
      { "freshness_life_times",
        &DiskCacheInfoParser::SetFreshnessLifeTimesToJson },
      { "last_modified", &DiskCacheInfoParser::SetLastModifiedToJson },
      };
  auto cache_info = info->cache_info();
  for (const auto& it : SET_CACHE_INFO_PARAM_TO_JSON_FUNC_MAP) {
    it.second(cache_info, it.first, dict);
  }
}

bool DiskCacheInfoParser::GetCacheInfoFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  using GetCacheInfoParamFromJsonFunc =
      bool (*)(const base::Value& json, const std::string& param_name,
               const std::shared_ptr<PRRequestInfo>& info);
  static const std::unordered_map<std::string, GetCacheInfoParamFromJsonFunc>
    GET_CACHE_INFO_PARAM_FROM_JSON_FUNC_MAP = {
      { "cache_type", &DiskCacheInfoParser::GetCacheTypeFromJson },
      { "e_tag", &DiskCacheInfoParser::GetETagFromJson },
      { "freshness_life_times",
        &DiskCacheInfoParser::GetFreshnessLifeTimesFromJson },
      { "last_modified", &DiskCacheInfoParser::GetLastModifiedFromJson },
      };
  for (const auto& it : GET_CACHE_INFO_PARAM_FROM_JSON_FUNC_MAP) {
    if (!it.second(json, it.first, info)) {
      return false;
    }
  }
  return true;
}

void DiskCacheInfoParser::SetCacheTypeToJson(
    const CacheInfo& cache_info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(cache_info.cache_type_));
}

bool DiskCacheInfoParser::GetCacheTypeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> cache_type = json.GetDict().FindInt(param_name);
  if (!cache_type.has_value() ||
      cache_type.value() <
          static_cast<int>(PRRequestCacheType::NEGOTIATION_CACHE) ||
      cache_type.value() >
          static_cast<int>(PRRequestCacheType::DISABLE_CACHE)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetCacheTypeFromJson " <<
                    "cache_type is invalid";
    return false;
  }
  info->set_cache_type(static_cast<PRRequestCacheType>(cache_type.value()));
  return true;
}

void DiskCacheInfoParser::SetETagToJson(
    const CacheInfo& cache_info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, cache_info.e_tag_);
}

bool DiskCacheInfoParser::GetETagFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* e_tag = json.GetDict().FindString(param_name);
  if (!e_tag) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetETagFromJson e_tag is invalid";
    return false;
  }
  info->set_e_tag(*e_tag);
  return true;
}

void DiskCacheInfoParser::SetFreshnessLifeTimesToJson(
    const CacheInfo& cache_info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, std::to_string(cache_info.freshness_life_times_));
}

bool DiskCacheInfoParser::GetFreshnessLifeTimesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* freshness_life_times_str =
      json.GetDict().FindString(param_name);
  if (!freshness_life_times_str || freshness_life_times_str->empty()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetFreshnessLifeTimesFromJson " <<
                    "fresness_life_times_str is none";
    return false;
  }
  char* end = nullptr;
  errno = 0;  // system global variables
  int64_t freshness_life_times =
      std::strtoll(freshness_life_times_str->c_str(), &end, 10);
  if (errno != 0 || !end || *end) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetFreshnessLifeTimesFromJson " <<
                    "freshness_life_timesr is invalid";
    return false;
  }

  info->set_freshness_life_times(freshness_life_times);
  return true;
}

void DiskCacheInfoParser::SetLastModifiedToJson(
    const CacheInfo& cache_info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, cache_info.last_modified_);
}

bool DiskCacheInfoParser::GetLastModifiedFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* last_modified = json.GetDict().FindString(param_name);
  if (!last_modified) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetLastModifiedFromJson last_modified is invalid";
    return false;
  }
  info->set_last_modified(*last_modified);
  return true;
}

void DiskCacheInfoParser::SetCookieSettingOverridesToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  base::Value::List cookie_setting_overrides_list;
  for (auto cookie_setting_override : info->cookie_setting_overrides()) {
    cookie_setting_overrides_list.Append(
        static_cast<int>(cookie_setting_override));
  }
  dict.Set(param_name, std::move(cookie_setting_overrides_list));
}

bool DiskCacheInfoParser::GetCookieSettingOverridesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const base::Value::List* cookie_setting_overrides_list =
      json.GetDict().FindList(param_name);
  net::CookieSettingOverrides cookie_setting_overrides;
  if (!cookie_setting_overrides_list) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetCookieSettingOverridesFromJson " <<
                    "cookie_setting_overrides is invalid";
    return false;
  }
  for (const auto& cookie_setting_override : *cookie_setting_overrides_list) {
    cookie_setting_overrides.Put(static_cast<net::CookieSettingOverride>(
        cookie_setting_override.GetInt()));
  }
  info->set_cookie_setting_overrides(cookie_setting_overrides);
  return true;
}

void DiskCacheInfoParser::SetCorbDetachableToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->corb_detachable());
}

bool DiskCacheInfoParser::GetCorbDetachableFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> corb_detachable =
      json.GetDict().FindBool(param_name);
  if (!corb_detachable.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetCorbDetachableFromJson corb_detachable is invalid";
    return false;
  }
  info->set_corb_detachable(corb_detachable.value());
  return true;
}

void DiskCacheInfoParser::SetDoNotPromptForLoginToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->do_not_prompt_for_login());
}

bool DiskCacheInfoParser::GetDoNotPromptForLoginFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> do_not_prompt_for_login =
      json.GetDict().FindBool(param_name);
  if (!do_not_prompt_for_login.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetDoNotPromptForLoginFromJson do_not_prompt_for_login is invalid";
    return false;
  }
  info->set_do_not_prompt_for_login(do_not_prompt_for_login.value());
  return true;
}

void DiskCacheInfoParser::SetDynamicHeaderKeysToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  base::Value::List key_list;
  for (const auto& key : info->dynamic_header_keys()) {
    key_list.Append(key);
  }
  dict.Set(param_name, std::move(key_list));
}

bool DiskCacheInfoParser::GetDynamicHeaderKeysFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const base::Value::List* dynamic_header_key_list =
      json.GetDict().FindList(param_name);
  if (!dynamic_header_key_list) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetDynamicHeaderKeysFromJson dynamic_header_keys is invalid";
    return false;
  }
  std::set<std::string> dynamic_header_keys;
  for (const auto& dynamic_header_key : *dynamic_header_key_list) {
    (void)dynamic_header_keys.emplace(dynamic_header_key.GetString());
  }
  if (!dynamic_header_keys.empty()) {
    info->set_dynamic_header_keys(dynamic_header_keys);
  }
  return true;
}

void DiskCacheInfoParser::SetExtraRequestHeadersToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  net::HttpRequestHeaders::Iterator it(info->extra_request_headers());
  base::Value::Dict extra_request_headers_dict;
  while (it.GetNext()) {
    extra_request_headers_dict.Set(it.name(), it.value());
  };
  dict.Set(param_name, std::move(extra_request_headers_dict));
}

bool DiskCacheInfoParser::GetExtraRequestHeadersFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const base::Value::Dict* extra_request_headers_dict =
      json.GetDict().FindDict(param_name);
  net::HttpRequestHeaders extra_request_headers;
  if (!extra_request_headers_dict) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetExtraRequestHeadersFromJson extra_request_headers " <<
                    "is invalid";
    return false;
  }
  for (std::pair<const std::string&, const base::Value&> extra_request_header :
       *extra_request_headers_dict) {
    extra_request_headers.SetHeader(extra_request_header.first,
                                    extra_request_header.second.GetString());
  }
  info->set_extra_request_headers(std::move(extra_request_headers));
  return true;
}
void DiskCacheInfoParser::SetFirstPartyUrlPolicyToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->first_party_url_policy()));
}

bool DiskCacheInfoParser::GetFirstPartyUrlPolicyFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> first_party_url_policy =
      json.GetDict().FindInt(param_name);
  if (!first_party_url_policy.has_value() ||
      first_party_url_policy.value() <
          static_cast<int>(
              net::RedirectInfo::FirstPartyURLPolicy::NEVER_CHANGE_URL) ||
      first_party_url_policy.value() >
          static_cast<int>(
              net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetFirstPartyUrlPolicyFromJson first_party_url_policy " <<
                    "is invalid";
    return false;
  }
  info->set_first_party_url_policy(
      static_cast<net::RedirectInfo::FirstPartyURLPolicy>(
          first_party_url_policy.value()));
  return true;
}

void DiskCacheInfoParser::SetForceIgnoreSiteForCookiesToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->force_ignore_site_for_cookies());
}

bool DiskCacheInfoParser::GetForceIgnoreSiteForCookiesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> force_ignore_site_for_cookies =
      json.GetDict().FindBool(param_name);
  if (!force_ignore_site_for_cookies.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
        "GetForceIgnoreSiteForCookiesFromJson force_ignore_site_for_cookies " <<
        "is invalid";
    return false;
  }
  info->set_force_ignore_site_for_cookies(
      force_ignore_site_for_cookies.value());
  return true;
}

void DiskCacheInfoParser::SetForceIgnoreTopFramePartyForCookiesToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->force_ignore_top_frame_party_for_cookies());
}

bool DiskCacheInfoParser::GetForceIgnoreTopFramePartyForCookiesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> force_ignore_top_frame_party_for_cookies =
      json.GetDict().FindBool(param_name);
  if (!force_ignore_top_frame_party_for_cookies.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetForceIgnoreTopFramePartyForCookiesFromJson " <<
                    "force_ignore_top_frame_party_for_cookies " <<
                    "is invalid";
    return false;
  }
  info->set_force_ignore_top_frame_party_for_cookies(
      force_ignore_top_frame_party_for_cookies.value());
  return true;
}

void DiskCacheInfoParser::SetForceMainFrameForSameSiteCookiesToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->force_main_frame_for_same_site_cookies());
}

bool DiskCacheInfoParser::GetForceMainFrameForSameSiteCookiesFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> force_main_frame_for_same_site_cookies =
      json.GetDict().FindBool(param_name);
  if (!force_main_frame_for_same_site_cookies.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetForceMainFrameForSameSiteCookiesFromJson " <<
                    "force_main_frame_for_same_site_cookies " <<
                    "is invalid";
    return false;
  }
  info->set_force_main_frame_for_same_site_cookies(
      force_main_frame_for_same_site_cookies.value());
  return true;
}
void DiskCacheInfoParser::SetHasStorageAccessToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->has_storage_access());
}

bool DiskCacheInfoParser::GetHasStorageAccessFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> has_storage_access =
      json.GetDict().FindBool(param_name);
  if (!has_storage_access.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetHasStorageAccessFromJson " <<
                    "has_storage_access " <<
                    "is invalid";
    return false;
  }
  info->set_has_storage_access(has_storage_access.value());
  return true;
}

void DiskCacheInfoParser::SetIdempotencyToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->idempotency()));
}

bool DiskCacheInfoParser::GetIdempotencyFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> idempotency = json.GetDict().FindInt(param_name);
  if (!idempotency.has_value() ||
      idempotency.value() < net::Idempotency::DEFAULT_IDEMPOTENCY ||
      idempotency.value() > net::Idempotency::NOT_IDEMPOTENT) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetIdempotencyFromJson " <<
                    "idempotency " <<
                    "is invalid";
    return false;
  }
  info->set_idempotency(static_cast<net::Idempotency>(idempotency.value()));
  return true;
}

void DiskCacheInfoParser::SetInitiatorToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  std::string initiator = info->initiator().has_value()
                              ? info->initiator().value().GetURL().spec()
                              : "";
  dict.Set(param_name, initiator);
}

bool DiskCacheInfoParser::GetInitiatorFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* initiator = json.GetDict().FindString(param_name);
  if (!initiator) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetInitiatorFromJson " <<
                    "initiator " <<
                    "is invalid";
    return false;
  }
  info->set_initiator(url::Origin::Create(GURL(*initiator)));
  return true;
}

void DiskCacheInfoParser::SetIsForWebsocketsToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->is_for_websockets());
}

bool DiskCacheInfoParser::GetIsForWebsocketsFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> is_for_websockets =
      json.GetDict().FindBool(param_name);
  if (!is_for_websockets.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetIsForWebsocketsFromJson " <<
                    "is_for_websockets " <<
                    "is invalid";
    return false;
  }

  info->set_is_for_websockets(is_for_websockets.value());
  return true;
}

void DiskCacheInfoParser::SetKeepaliveToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->keepalive());
}

bool DiskCacheInfoParser::GetKeepaliveFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> keepalive = json.GetDict().FindBool(param_name);
  if (!keepalive.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetKeepaliveFromJson " <<
                    "keepalive " <<
                    "is invalid";
    return false;
  }
  info->set_keepalive(keepalive.value());
  return true;
}

void DiskCacheInfoParser::SetLoadFlagsToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->load_flags());
}
bool DiskCacheInfoParser::GetLoadFlagsFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> load_flags = json.GetDict().FindInt(param_name);
  if (!load_flags.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetLoadFlagsFromJson " <<
                    "load_flags " <<
                    "is invalid";
    return false;
  }
  info->set_load_flags(load_flags.value());
  return true;
}

void DiskCacheInfoParser::SetMethodToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->method());
}

bool DiskCacheInfoParser::GetMethodFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* method = json.GetDict().FindString(param_name);
  if (!method) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetMethodFromJson method " <<
                    "is invalid";
    return false;
  }
  info->set_method(*method);
  return true;
}

void DiskCacheInfoParser::SetPreloadFlagsToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->preload_flag()));
}

bool DiskCacheInfoParser::GetPreloadFlagsFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> preload_flag = json.GetDict().FindInt(param_name);
  if (!preload_flag.has_value() ||
      preload_flag.value() < static_cast<int>(PRRequestFlags::PRPP_FLAGS_NONE) ||
      preload_flag.value() > static_cast<int>(PRRequestFlags::PRPP_FLAGS_UNSUPPORT)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetPreloadFlagsFromJson preload_flag " <<
                    "is invalid";
    return false;
  }
  info->set_preload_flag(static_cast<PRRequestFlags>(preload_flag.value()));
  return true;
}

void DiskCacheInfoParser::SetReferrerToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->referrer());
}

bool DiskCacheInfoParser::GetReferrerFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* referrer = json.GetDict().FindString(param_name);
  if (!referrer) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetReferrerFromJson referrer " <<
                    "is invalid";
    return false;
  }
  info->set_referrer(*referrer);
  return true;
}

void DiskCacheInfoParser::SetReferrerPolicyToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->referrer_policy()));
}

bool DiskCacheInfoParser::GetReferrerPolicyFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> referrer_policy =
      json.GetDict().FindInt(param_name);
  if (!referrer_policy.has_value() ||
      referrer_policy.value() <
          static_cast<int>(net::ReferrerPolicy::
                                CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE) ||
      referrer_policy.value() > static_cast<int>(net::ReferrerPolicy::MAX)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetReferrerPolicyFromJson referrer_policy " <<
                    "is invalid";
    return false;
  }
  info->set_referrer_policy(
      static_cast<net::ReferrerPolicy>(referrer_policy.value()));
  return true;
}

void DiskCacheInfoParser::SetRequestCredentialsModeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->request_credentials_mode());
}

bool DiskCacheInfoParser::GetRequestCredentialsModeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> request_credentials_mode =
      json.GetDict().FindInt(param_name);
  if (!request_credentials_mode.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestCredentialsModeFromJson request_credentials_mode " <<
                    "is invalid";
    return false;
  }
  info->set_request_credentials_mode(request_credentials_mode.value());
  return true;
}

void DiskCacheInfoParser::SetRequestDestinationToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->request_destination());
}

bool DiskCacheInfoParser::GetRequestDestinationFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> request_destination =
      json.GetDict().FindInt(param_name);
  if (!request_destination.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestDestinationFromJson request_destination " <<
                    "is invalid";
    return false;
  }
  info->set_request_destination(request_destination.value());
  return true;
}

void DiskCacheInfoParser::SetRequestEndTimeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, std::to_string(info->request_end_time()));
}

bool DiskCacheInfoParser::GetRequestEndTimeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* request_end_time_str =
      json.GetDict().FindString(param_name);
  if (!request_end_time_str || request_end_time_str->empty()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestEndTimeFromJson request_end_time " <<
                    "is none";
    return false;
  }

  char* end = nullptr;
  errno = 0;  // system global variables
  int64_t request_end_time =
      std::strtoll(request_end_time_str->c_str(), &end, 10);
  if (errno != 0 || !end || *end) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestEndTimeFromJson request_end_time " <<
                    "is invalid";
    return false;
  }

  info->set_request_end_time(request_end_time);
  return true;
}

void DiskCacheInfoParser::SetRequestInfoTypeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->type()));
}

bool DiskCacheInfoParser::GetRequestInfoTypeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> type = json.GetDict().FindInt(param_name);
  if (!type.has_value() ||
      type.value() < static_cast<int>(PRRequestInfoType::TYPE_DEFAULT) ||
      type.value() > static_cast<int>(PRRequestInfoType::TYPE_PAGE_PREFLIGHT)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestInfoTypeFromJson type is invalid";
    return false;
  }
  info->set_type(static_cast<PRRequestInfoType>(type.value()));
  return true;
}

void DiskCacheInfoParser::SetRequestModeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set("request_mode", info->request_mode());
}
bool DiskCacheInfoParser::GetRequestModeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> request_mode = json.GetDict().FindInt(param_name);
  if (!request_mode.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetRequestModeFromJson " <<
                    "request_mode is invalid";
    return false;
  }
  info->set_request_mode(request_mode.value());
  return true;
}

void DiskCacheInfoParser::SetRequestStartTimeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, std::to_string(info->request_start_time()));
}

bool DiskCacheInfoParser::GetRequestStartTimeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* request_start_time_str =
      json.GetDict().FindString(param_name);
  if (!request_start_time_str || request_start_time_str->empty()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestStartTimeFromJson request_start_time " <<
                    "is none";
    return false;
  }

  char* end = nullptr;
  errno = 0;  // system global variables
  int64_t request_start_time =
      std::strtoll(request_start_time_str->c_str(), &end, 10);
  if (errno != 0 || !end || *end) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetRequestStartTimeFromJson request_start_time " <<
                    "is invalid";
    return false;
  }

  info->set_request_start_time(request_start_time);
  return true;
}

void DiskCacheInfoParser::SetResourceTypeToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->resource_type());
}

bool DiskCacheInfoParser::GetResourceTypeFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> resource_type = json.GetDict().FindInt(param_name);
  if (!resource_type.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetResourceTypeFromJson " <<
                    "resource_type is invalid";
    return false;
  }
  info->set_resource_type(resource_type.value());
  return true;
}

void DiskCacheInfoParser::SetSecureDnsPolicyToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, static_cast<int>(info->secure_dns_policy()));
}

bool DiskCacheInfoParser::GetSecureDnsPolicyFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<int> secure_dns_policy =
      json.GetDict().FindInt(param_name);
  if (!secure_dns_policy.has_value() ||
      secure_dns_policy.value() <
          static_cast<int>(net::SecureDnsPolicy::kAllow) ||
      secure_dns_policy.value() >
          static_cast<int>(net::SecureDnsPolicy::kBootstrap)) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetSecureDnsPolicyFromJson secure_dns_policy " <<
                    "is invalid";
    return false;
  }
  info->set_secure_dns_policy(
      static_cast<net::SecureDnsPolicy>(secure_dns_policy.value()));
  return true;
}

void DiskCacheInfoParser::SetSendClientCertsToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->send_client_certs());
}

bool DiskCacheInfoParser::GetSendClientCertsFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> send_client_certs =
      json.GetDict().FindBool(param_name);
  if (!send_client_certs.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetSendClientCertsFromJson send_client_certs " <<
                    "is invalid";
    return false;
  }
  info->set_send_client_certs(send_client_certs.value());
  return true;
}

void DiskCacheInfoParser::SetUpgradeIfInsecureToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  dict.Set(param_name, info->upgrade_if_insecure());
}
bool DiskCacheInfoParser::GetUpgradeIfInsecureFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const absl::optional<bool> upgrade_if_insecure =
      json.GetDict().FindBool(param_name);
  if (!upgrade_if_insecure.has_value()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::" <<
                    "GetUpgradeIfInsecureFromJson upgrade_if_insecure " <<
                    "is invalid";
    return false;
  }
  info->set_upgrade_if_insecure(upgrade_if_insecure.value());
  return true;
}

void DiskCacheInfoParser::SetUrlToJson(
    const std::shared_ptr<PRRequestInfo>& info,
    const std::string& param_name,
    base::Value::Dict& dict) {
  if ((info->type() == PRRequestInfoType::TYPE_PAGE_PREFLIGHT) &&
      !info->url().spec().starts_with(PRPP_PREFLIGHT_PREFIX)) {
    dict.Set(param_name,
             std::string(PRPP_PREFLIGHT_PREFIX) + info->url().spec());
  } else {
    dict.Set(param_name, info->url().spec());
  }
}

bool DiskCacheInfoParser::GetUrlFromJson(
    const base::Value& json,
    const std::string& param_name,
    const std::shared_ptr<PRRequestInfo>& info) {
  const std::string* url = json.GetDict().FindString(param_name);
  if (!url || url->empty()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheInfoParser::GetUrlFromJson " <<
                    "url is invalid";
    return false;
  }

  info->set_url(GURL(*url));
  return true;
}

}  // namespace ohos_prp_preload