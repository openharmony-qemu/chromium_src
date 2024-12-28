// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_INFO_PARSER_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_INFO_PARSER_H

#include "base/values.h"
#include "net/base/page_res_request_info.h"

namespace ohos_prp_preload {

class DiskCacheInfoParser {
 public:
  static void ParseResReqPreloadInfoForPreconnect(
      const std::shared_ptr<PRRequestInfo>& info,
      base::Value::Dict& dict);
  static bool ParseJsonForPreconnect(
      const base::Value& json,
      const std::shared_ptr<PRRequestInfo>& info);
  static void ParseResReqPreloadInfoForPreload(
      const std::shared_ptr<PRRequestInfo>& info,
      base::Value::Dict& dict);
  static bool ParseJsonForPreload(const base::Value& json,
                                  const std::shared_ptr<PRRequestInfo>& info);
  static void SetAcceptedStreamTypesToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetAcceptedStreamTypesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetAllowCredentialsToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetAllowCredentialsFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetCacheInfoToJson(const std::shared_ptr<PRRequestInfo>& info,
                                 const std::string& param_name,
                                 base::Value::Dict& dict);
  static bool GetCacheInfoFromJson(const base::Value& json,
                                   const std::string& param_name,
                                   const std::shared_ptr<PRRequestInfo>& info);
  static void SetCacheTypeToJson(const CacheInfo& cache_info,
                                 const std::string& param_name,
                                 base::Value::Dict& dict);
  static bool GetCacheTypeFromJson(const base::Value& json,
                                   const std::string& param_name,
                                   const std::shared_ptr<PRRequestInfo>& info);
  static void SetETagToJson(const CacheInfo& cache_info,
                            const std::string& param_name,
                            base::Value::Dict& dict);
  static bool GetETagFromJson(const base::Value& json,
                              const std::string& param_name,
                              const std::shared_ptr<PRRequestInfo>& info);
  static void SetFreshnessLifeTimesToJson(const CacheInfo& cache_info,
                                          const std::string& param_name,
                                          base::Value::Dict& dict);
  static bool GetFreshnessLifeTimesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetLastModifiedToJson(const CacheInfo& cache_info,
                                    const std::string& param_name,
                                    base::Value::Dict& dict);
  static bool GetLastModifiedFromJson(const base::Value& json,
                                      const std::string& param_name,
                                      const std::shared_ptr<PRRequestInfo>& info);
  static void SetCookieSettingOverridesToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetCookieSettingOverridesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetCorbDetachableToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetCorbDetachableFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetDoNotPromptForLoginToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetDoNotPromptForLoginFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetDynamicHeaderKeysToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetDynamicHeaderKeysFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetExtraRequestHeadersToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetExtraRequestHeadersFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetFirstPartyUrlPolicyToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetFirstPartyUrlPolicyFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetForceIgnoreSiteForCookiesToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetForceIgnoreSiteForCookiesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetForceIgnoreTopFramePartyForCookiesToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetForceIgnoreTopFramePartyForCookiesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetForceMainFrameForSameSiteCookiesToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetForceMainFrameForSameSiteCookiesFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetHasStorageAccessToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetHasStorageAccessFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetIdempotencyToJson(const std::shared_ptr<PRRequestInfo>& info,
                                   const std::string& param_name,
                                   base::Value::Dict& dict);
  static bool GetIdempotencyFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetInitiatorToJson(const std::shared_ptr<PRRequestInfo>& info,
                                 const std::string& param_name,
                                 base::Value::Dict& dict);
  static bool GetInitiatorFromJson(const base::Value& json,
                                   const std::string& param_name,
                                   const std::shared_ptr<PRRequestInfo>& info);
  static void SetIsForWebsocketsToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetIsForWebsocketsFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetKeepaliveToJson(const std::shared_ptr<PRRequestInfo>& info,
                                 const std::string& param_name,
                                 base::Value::Dict& dict);
  static bool GetKeepaliveFromJson(const base::Value& json,
                                   const std::string& param_name,
                                   const std::shared_ptr<PRRequestInfo>& info);
  static void SetLoadFlagsToJson(const std::shared_ptr<PRRequestInfo>& info,
                                 const std::string& param_name,
                                 base::Value::Dict& dict);
  static bool GetLoadFlagsFromJson(const base::Value& json,
                                   const std::string& param_name,
                                   const std::shared_ptr<PRRequestInfo>& info);
  static void SetMethodToJson(const std::shared_ptr<PRRequestInfo>& info,
                              const std::string& param_name,
                              base::Value::Dict& dict);
  static bool GetMethodFromJson(const base::Value& json,
                                const std::string& param_name,
                                const std::shared_ptr<PRRequestInfo>& info);
  static void SetReferrerToJson(const std::shared_ptr<PRRequestInfo>& info,
                                const std::string& param_name,
                                base::Value::Dict& dict);
  static bool GetReferrerFromJson(const base::Value& json,
                                  const std::string& param_name,
                                  const std::shared_ptr<PRRequestInfo>& info);
  static void SetReferrerPolicyToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetReferrerPolicyFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestCredentialsModeToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetRequestCredentialsModeFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestDestinationToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetRequestDestinationFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestEndTimeToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetRequestEndTimeFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetPreloadFlagsToJson(const std::shared_ptr<PRRequestInfo>& info,
                                    const std::string& param_name,
                                    base::Value::Dict& dict);
  static bool GetPreloadFlagsFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestInfoTypeToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetRequestInfoTypeFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestModeToJson(const std::shared_ptr<PRRequestInfo>& info,
                                   const std::string& param_name,
                                   base::Value::Dict& dict);
  static bool GetRequestModeFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetRequestStartTimeToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetRequestStartTimeFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetResourceTypeToJson(const std::shared_ptr<PRRequestInfo>& info,
                                  const std::string& param_name,
                                  base::Value::Dict& dict);
  static bool GetResourceTypeFromJson(const base::Value& json,
                                    const std::string& param_name,
                                    const std::shared_ptr<PRRequestInfo>& info);
  static void SetSecureDnsPolicyToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetSecureDnsPolicyFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetSendClientCertsToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetSendClientCertsFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetUpgradeIfInsecureToJson(
      const std::shared_ptr<PRRequestInfo>& info,
      const std::string& param_name,
      base::Value::Dict& dict);
  static bool GetUpgradeIfInsecureFromJson(
      const base::Value& json,
      const std::string& param_name,
      const std::shared_ptr<PRRequestInfo>& info);
  static void SetUrlToJson(const std::shared_ptr<PRRequestInfo>& info,
                           const std::string& param_name,
                           base::Value::Dict& dict);
  static bool GetUrlFromJson(const base::Value& json,
                             const std::string& param_name,
                             const std::shared_ptr<PRRequestInfo>& info);

 private:
  DiskCacheInfoParser() = default;
  ~DiskCacheInfoParser() = default;
};
}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_INFO_PARSER_H