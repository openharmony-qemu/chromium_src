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

// clang-format off
#if BUILDFLAG(IS_ARKWEB)
#define DO_FIELD_FOR_ALL_FIELDS(...)                           \
  DO_FIELD(request_id_perf_stat_) __VA_ARGS__                  \
  DO_FIELD(method) __VA_ARGS__                                 \
  DO_FIELD(url) __VA_ARGS__                                    \
  DO_FIELD(site_for_cookies) __VA_ARGS__                       \
  DO_FIELD(update_first_party_url_on_redirect) __VA_ARGS__     \
  DO_FIELD(request_initiator) __VA_ARGS__                      \
  DO_FIELD(isolated_world_origin) __VA_ARGS__                  \
  DO_FIELD(navigation_redirect_chain) __VA_ARGS__              \
  DO_FIELD(referrer) __VA_ARGS__                               \
  DO_FIELD(referrer_policy) __VA_ARGS__                        \
  DO_FIELD(headers) __VA_ARGS__                                \
  DO_FIELD(cors_exempt_headers) __VA_ARGS__                    \
  DO_FIELD(load_flags) __VA_ARGS__                             \
  DO_FIELD(resource_type) __VA_ARGS__                          \
  DO_FIELD(priority) __VA_ARGS__                               \
  DO_FIELD(priority_incremental) __VA_ARGS__                   \
  DO_FIELD(cors_preflight_policy) __VA_ARGS__                  \
  DO_FIELD(originated_from_service_worker) __VA_ARGS__         \
  DO_FIELD(skip_service_worker) __VA_ARGS__                    \
  DO_FIELD(corb_detachable) __VA_ARGS__                    \
  DO_FIELD(mode) __VA_ARGS__                                   \
  DO_FIELD(required_ip_address_space) __VA_ARGS__              \
  DO_FIELD(credentials_mode) __VA_ARGS__                       \
  DO_FIELD(redirect_mode) __VA_ARGS__                          \
  DO_FIELD(fetch_integrity) __VA_ARGS__                        \
  DO_FIELD(destination) __VA_ARGS__                            \
  DO_FIELD(original_destination) __VA_ARGS__                   \
  DO_FIELD(request_body) __VA_ARGS__                           \
  DO_FIELD(keepalive) __VA_ARGS__                              \
  DO_FIELD(browsing_topics) __VA_ARGS__                        \
  DO_FIELD(ad_auction_headers) __VA_ARGS__                     \
  DO_FIELD(shared_storage_writable_eligible) __VA_ARGS__       \
  DO_FIELD(has_user_gesture) __VA_ARGS__                       \
  DO_FIELD(enable_load_timing) __VA_ARGS__                     \
  DO_FIELD(enable_upload_progress) __VA_ARGS__                 \
  DO_FIELD(do_not_prompt_for_login) __VA_ARGS__                \
  DO_FIELD(is_outermost_main_frame) __VA_ARGS__                \
  DO_FIELD(transition_type) __VA_ARGS__                        \
  DO_FIELD(previews_state) __VA_ARGS__                         \
  DO_FIELD(upgrade_if_insecure) __VA_ARGS__                    \
  DO_FIELD(is_revalidating) __VA_ARGS__                        \
  DO_FIELD(throttling_profile_id) __VA_ARGS__                  \
  DO_FIELD(custom_proxy_pre_cache_headers) __VA_ARGS__         \
  DO_FIELD(custom_proxy_post_cache_headers) __VA_ARGS__        \
  DO_FIELD(fetch_window_id) __VA_ARGS__                        \
  DO_FIELD(devtools_request_id) __VA_ARGS__                    \
  DO_FIELD(devtools_stack_id) __VA_ARGS__                      \
  DO_FIELD(is_fetch_like_api) __VA_ARGS__                      \
  DO_FIELD(is_fetch_later_api) __VA_ARGS__                     \
  DO_FIELD(is_favicon) __VA_ARGS__                             \
  DO_FIELD(recursive_prefetch_token) __VA_ARGS__               \
  DO_FIELD(trusted_params) __VA_ARGS__                         \
  DO_FIELD(trust_token_params) __VA_ARGS__                     \
  DO_FIELD(web_bundle_token_params) __VA_ARGS__                \
  DO_FIELD(devtools_accepted_stream_types) __VA_ARGS__         \
  DO_FIELD(net_log_create_info) __VA_ARGS__                    \
  DO_FIELD(net_log_reference_info) __VA_ARGS__                 \
  DO_FIELD(target_ip_address_space) __VA_ARGS__                \
  DO_FIELD(storage_access_api_status) __VA_ARGS__              \
  DO_FIELD(attribution_reporting_support) __VA_ARGS__          \
  DO_FIELD(attribution_reporting_eligibility) __VA_ARGS__      \
  DO_FIELD(shared_dictionary_writer_enabled) __VA_ARGS__       \
  DO_FIELD(attribution_reporting_src_token) __VA_ARGS__        \
  DO_FIELD(is_ad_tagged) __VA_ARGS__                           \
  DO_FIELD(prefetch_token) __VA_ARGS__                         \
  DO_FIELD(allow_preload_record) __VA_ARGS__                   \
  DO_FIELD(main_url) __VA_ARGS__                               \
  DO_FIELD(is_preflight) __VA_ARGS__                           \
  DO_FIELD(is_sync_mode) __VA_ARGS__                           \
  DO_FIELD(is_download_request) __VA_ARGS__                    \
  DO_FIELD(is_triggered_by_download) __VA_ARGS__               \
  DO_FIELD(usage_scenario_) __VA_ARGS__                        \
  DO_FIELD(socket_tag)  __VA_ARGS__                            \
  DO_FIELD(disable_web_security) __VA_ARGS__                   \
  DO_FIELD(retry_with_fallback_proxy) __VA_ARGS__              \
  DO_FIELD(original_error_code)
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(IS_ARKWEB)
enum class FieldsForUma {
  kUnknown = 0,
  kMethod = 1,
  kUrl = 2,
  kSiteForCookies = 3,
  kUpdateFirstPartyUrlOnRedirect = 4,
  kRequestInitiator = 5,
  kIsolatedWorldOrigin = 6,
  kNavigationRedirectChain = 7,
  kReferrer = 8,
  kReferrerPolicy = 9,
  kHeaders = 10,
  kCorsExemptHeaders = 11,
  kLoadFlags = 12,
  kResourceType = 13,
  kPriority = 14,
  kPriorityIncremental = 15,
  kCorsPreflightPolicy = 16,
  kOriginatedFromServiceWorker = 17,
  kSkipServiceWorker = 18,
  kCorbDetachable = 19,
  kMode = 20,
  kRequiredIpAddressSpace = 21,
  kCredentialsMode = 22,
  kRedirectMode = 23,
  kFetchIntegrity = 24,
  kDestination = 25,
  kOriginalDestination = 26,
  kRequestBody = 27,
  kKeepalive = 28,
  kBrowsingTopics = 29,
  kAdAuctionHeaders = 30,
  kSharedStorageWritableEligible = 31,
  kHasUserGesture = 32,
  kEnableLoadTiming = 33,
  kEnableUploadProgress = 34,
  kDoNotPromptForLogin = 35,
  kIsOutermostMainFrame = 36,
  kTransitionType = 37,
  kPreviewsState = 38,
  kUpgradeIfInsecure = 39,
  kIsRevalidating = 40,
  kThrottlingProfileId = 41,
  kCustomProxyPreCacheHeaders = 42,
  kCustomProxyPostCacheHeaders = 43,
  kFetchWindowId = 44,
  kDevtoolsRequestId = 45,
  kDevtoolsStackId = 46,
  kIsFetchLikeApi = 47,
  kIsFetchLaterApi = 48,
  kIsFavicon = 49,
  kRecursivePrefetchToken = 50,
  kTrustedParams = 51,
  kTrustTokenParams = 52,
  kWebBundleTokenParams = 53,
  kDevtoolsAcceptedStreamTypes = 54,
  kNetLogCreateInfo = 55,
  kNetLogReferenceInfo = 56,
  kTargetIpAddressSpace = 57,
  kStorageAccessApiStatus = 58,
  kAttributionReportingSupport = 59,
  kAttributionReportingEligibility = 60,
  kSharedDictionaryWriterEnabled = 61,
  kAttributionReportingSrcToken = 62,
  kIsAdTagged = 63,
  kMaxValue = kIsAdTagged,
};
#endif
// clang-format on
