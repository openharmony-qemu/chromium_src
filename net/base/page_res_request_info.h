// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_PAGR_RES_REQUEST_INFO_H
#define NET_BASE_PAGR_RES_REQUEST_INFO_H

#include <list>
#include <set>
#include "net/base/idempotency.h"
#include "net/base/load_flags.h"
#include "net/base/network_anonymization_key.h"
#include "net/cookies/cookie_setting_override.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/http/http_request_headers.h"
#include "net/log/net_log_with_source.h"
#include "net/url_request/redirect_info.h"

namespace net {
class URLRequest;
}

namespace ohos_prp_preload {
#define PRPP_PREFLIGHT_PREFIX "preflight+"

// Marker if need to establish connection
enum class PRRequestCacheType {
  NEGOTIATION_CACHE,  // need to establish connection
  FORCE_CACHE,        // only if cache expires, need to establish connection
  DISABLE_CACHE,      // need to establish connection
};

struct CacheInfo {
  PRRequestCacheType cache_type_ { PRRequestCacheType::DISABLE_CACHE };
  int64_t freshness_life_times_ { 0 };
  std::string e_tag_;
  std::string last_modified_;
};

struct LoaderInfo {
  bool corb_detachable_;
  int resource_type_;
  bool keepalive_;
  bool do_not_prompt_for_login_;
  int request_mode_;
  int request_credentials_mode_;
  int request_destination_;
};

// Marker for request info type
enum class PRRequestInfoType {
  TYPE_DEFAULT, // default request
  TYPE_PAGE_ORIGIN, // record page origin
  TYPE_PAGE_PREFLIGHT, // preflight request
};

// Mark if preload request need to be send
enum PRRequestFlags : uint32_t {
  PRPP_FLAGS_NONE, // not visible, do not send preload request
  PRPP_FLAGS_HDR_NOT_MATCH = 1 << 0, // header param is unstable, do not send preload request
  PRPP_FLAGS_VISIBLE = 1 << 1, // visible, precondition for preload
  PRPP_FLAGS_HDR_DYNAMIC = 1 << 2, // header can be predicted, send preload request
  PRPP_FLAGS_URL_DYNAMIC = 1 << 3, // URL is dynamic, do not send preload request
  PRPP_FLAGS_UNSUPPORT = 1 << 4, // unsupport, do not send preload request
};

class PRRequestInfo {
 public:
  PRRequestInfo() = default;
  PRRequestInfo(const GURL& url, bool allow_credentials) :
	url_(url), allow_credentials_(allow_credentials) {}
  void InitInfoFromUrlRequest(const net::URLRequest& request);
  ~PRRequestInfo() = default;

  const GURL& url() const { return url_; }
  void set_url(const GURL& url) { url_ = url; }
  bool allow_credentials() const { return allow_credentials_; }
  void set_allow_credentials(bool allow_credentials) {
	allow_credentials_ = allow_credentials;
  }
  PRRequestCacheType cache_type() const { return cache_type_; }
  void set_cache_type(PRRequestCacheType cache_type) { cache_type_ = cache_type; }
  int64_t freshness_life_times() const { return freshness_life_times_; }
  void set_freshness_life_times(int64_t freshness_life_times) {
	freshness_life_times_ = freshness_life_times;
  }
  const std::string& e_tag() const { return e_tag_; }
  void set_e_tag(const std::string& e_tag) { e_tag_ = e_tag; }
  const std::string& last_modified() const { return last_modified_; }
  void set_last_modified(const std::string& last_modified) {
	last_modified_ = last_modified;
  }

  bool is_for_websockets() const { return is_for_websockets_; }
  void set_is_for_websockets(bool is_for_websockets) { is_for_websockets_ = is_for_websockets; }
  absl::optional<url::Origin> initiator() const { return initiator_; }
  void set_initiator(absl::optional<url::Origin> initiator) { initiator_ = initiator; }
  const std::string& referrer() const { return referrer_; }
  void set_referrer(const std::string& referrer) { referrer_ = referrer; }
  const std::string& method() const { return method_; }
  void set_method(const std::string& method) { method_ = method; }
  bool has_storage_access() const { return has_storage_access_; }
  void set_has_storage_access(bool has_storage_access) { has_storage_access_ = has_storage_access; }
  net::HttpRequestHeaders& extra_request_headers() { return extra_request_headers_; }
  void set_extra_request_headers(const net::HttpRequestHeaders& extra_request_headers) {
	extra_request_headers_ = extra_request_headers;
  }
  int load_flags() const { return load_flags_; }
  void set_load_flags(int load_flags) { load_flags_ = load_flags; }
  net::SecureDnsPolicy secure_dns_policy() const { return secure_dns_policy_; }
  void set_secure_dns_policy(net::SecureDnsPolicy secure_dns_policy) { secure_dns_policy_ = secure_dns_policy; }
  absl::optional<base::flat_set<net::SourceStream::SourceType>> accepted_stream_types() const {
	return accepted_stream_types_;
  }
  void set_accepted_stream_types(absl::optional<base::flat_set<net::SourceStream::SourceType>> accepted_stream_types) {
	accepted_stream_types_ = accepted_stream_types;
  }
  bool upgrade_if_insecure() const { return upgrade_if_insecure_; }
  void set_upgrade_if_insecure(bool upgrade_if_insecure) { upgrade_if_insecure_ = upgrade_if_insecure; }
  bool send_client_certs() const { return send_client_certs_; }
  void set_send_client_certs(bool send_client_certs) { send_client_certs_ = send_client_certs; }
  net::Idempotency idempotency() const { return idempotency_; }
  void set_idempotency(net::Idempotency idempotency) { idempotency_ = idempotency; }
  bool force_ignore_site_for_cookies() const { return force_ignore_site_for_cookies_; }
  void set_force_ignore_site_for_cookies(bool force_ignore_site_for_cookies) {
	force_ignore_site_for_cookies_ = force_ignore_site_for_cookies;
  }
  bool force_main_frame_for_same_site_cookies() const { return force_main_frame_for_same_site_cookies_; }
  void set_force_main_frame_for_same_site_cookies(bool force_main_frame_for_same_site_cookies) {
	force_main_frame_for_same_site_cookies_ = force_main_frame_for_same_site_cookies;
  }
  net::CookieSettingOverrides cookie_setting_overrides() const { return cookie_setting_overrides_; }
  void set_cookie_setting_overrides(net::CookieSettingOverrides cookie_setting_overrides) {
	cookie_setting_overrides_ = cookie_setting_overrides;
  }
  net::ReferrerPolicy referrer_policy() const { return referrer_policy_; }
  void set_referrer_policy(net::ReferrerPolicy referrer_policy) { referrer_policy_ = referrer_policy; }
  net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy() const { return first_party_url_policy_; }
  void set_first_party_url_policy(net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy) {
	first_party_url_policy_ = first_party_url_policy;
  }
  net::NetLogWithSource net_log() const { return net_log_; }
  void set_net_log(net::NetLogWithSource net_log) { net_log_ = net_log; }
  bool force_ignore_top_frame_party_for_cookies() const { return force_ignore_top_frame_party_for_cookies_; }
  void set_force_ignore_top_frame_party_for_cookies(bool force_ignore_top_frame_party_for_cookies) {
	force_ignore_top_frame_party_for_cookies_ = force_ignore_top_frame_party_for_cookies;
  }
  void set_loader_info(const LoaderInfo& loader_info) {
	corb_detachable_ = loader_info.corb_detachable_;
	resource_type_ = loader_info.resource_type_;
	keepalive_ = loader_info.keepalive_;
	do_not_prompt_for_login_ = loader_info.do_not_prompt_for_login_;
	request_mode_ = loader_info.request_mode_;
	request_credentials_mode_ = loader_info.request_credentials_mode_;
	request_destination_ = loader_info.request_destination_;
  }
  bool corb_detachable() const { return corb_detachable_; }
  void set_corb_detachable(bool corb_detachable) { corb_detachable_ = corb_detachable; }
  int resource_type() const { return resource_type_; }
  void set_resource_type(int resource_type) { resource_type_ = resource_type; }
  bool keepalive() const { return keepalive_; }
  void set_keepalive(bool keepalive) { keepalive_ = keepalive; }
  bool do_not_prompt_for_login() const { return do_not_prompt_for_login_; }
  void set_do_not_prompt_for_login(bool do_not_prompt_for_login) { do_not_prompt_for_login_ = do_not_prompt_for_login; }
  int request_mode() const { return request_mode_; }
  void set_request_mode(int request_mode) { request_mode_ = request_mode; }
  int request_credentials_mode() const { return request_credentials_mode_; }
  void set_request_credentials_mode(int request_credentials_mode) {
	request_credentials_mode_ = request_credentials_mode;
  }
  int request_destination() const { return request_destination_; }
  void set_request_destination(int request_destination) { request_destination_ = request_destination; }

  // for multi-thread read/write scenarios
  CacheInfo cache_info();
  void set_cache_info(PRRequestCacheType cache_type,
	  int64_t freshness_life_times,
	  const std::string& e_tag,
	  const std::string& last_modified);

  const PRRequestInfoType& type() const { return type_; }
  void set_type(PRRequestInfoType type) { type_ = type; }
  const std::string& page_origin() const { return page_origin_; }
  void set_page_origin(const std::string& page_origin) { page_origin_ = page_origin; }
  bool only_send_reuse_request() const { return only_send_reuse_request_; }
  void set_only_send_reuse_request(bool only_send_reuse_request) { only_send_reuse_request_ = only_send_reuse_request; }
  const std::set<std::string>& dynamic_header_keys() { return dynamic_header_keys_; }
  void set_dynamic_header_keys(const std::set<std::string>& dynamic_header_keys) {
	dynamic_header_keys_ = dynamic_header_keys;
  }
  const GURL& parent_for_dynamic_header() { return parent_for_dynamic_header_; }
  void set_parent_for_dynamic_header(const GURL& parent_for_dynamic_header) {
	parent_for_dynamic_header_ = parent_for_dynamic_header;
  }
  void set_request_start_time(int64_t time) { req_start_time_ = time; }
  int64_t request_start_time() const { return req_start_time_; }

  // for multi-thread read/write scenarios
  void set_request_end_time(int64_t time);
  int64_t request_end_time() const;
  void set_preload_flag(PRRequestFlags flags);
  void or_preload_flag(PRRequestFlags flags);
  PRRequestFlags preload_flag() const;
 private:
  mutable std::mutex cache_info_mutex_; // lock for update request info after request start
  GURL url_;
  bool allow_credentials_ { false };
  PRRequestCacheType cache_type_ { PRRequestCacheType::DISABLE_CACHE }; // updated by network thread, need to lock
  int64_t freshness_life_times_ { 0 }; // updated by network thread, need to lock
  std::string e_tag_; // updated by network thread, need to lock
  std::string last_modified_; // updated by network thread, need to lock

  bool is_for_websockets_ { false };
  absl::optional<url::Origin> initiator_;
  std::string referrer_;
  std::string method_;
  bool has_storage_access_ { false };
  net::HttpRequestHeaders extra_request_headers_;
  int load_flags_ { net::LOAD_NORMAL };
  net::SecureDnsPolicy secure_dns_policy_ { net::SecureDnsPolicy::kAllow };
  absl::optional<base::flat_set<net::SourceStream::SourceType>>
	accepted_stream_types_;
  bool upgrade_if_insecure_ { false };
  bool send_client_certs_ { true };
  net::Idempotency idempotency_ { net::DEFAULT_IDEMPOTENCY };
  bool force_ignore_site_for_cookies_ { false };
  bool force_main_frame_for_same_site_cookies_ { false };
  net::CookieSettingOverrides cookie_setting_overrides_;
  net::ReferrerPolicy referrer_policy_ { net::ReferrerPolicy::CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE };
  net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy_
    { net::RedirectInfo::FirstPartyURLPolicy::NEVER_CHANGE_URL };
  bool corb_detachable_ { false };
  int resource_type_ { 0 };
  bool keepalive_ { false };
  bool do_not_prompt_for_login_ { false };
  int request_mode_ { 0 };
  int request_credentials_mode_ { 0 };
  int request_destination_ { 0 };
  net::NetLogWithSource net_log_;
  bool force_ignore_top_frame_party_for_cookies_ { false };
  std::string page_origin_;
  PRRequestInfoType type_ { PRRequestInfoType::TYPE_DEFAULT };
  bool only_send_reuse_request_ { false };
  int64_t req_start_time_ { 0 };
  int64_t req_end_time_ { 0 }; // updated by network thread, need to lock
  PRRequestFlags preload_flags_ { PRRequestFlags::PRPP_FLAGS_NONE }; // updated by network thread, need to lock
  GURL parent_for_dynamic_header_;
  std::set<std::string> dynamic_header_keys_;
};

struct PRPPPreconnectInfo {
  GURL origin_url_;
  bool allow_credential_ { false };
  net::NetworkAnonymizationKey net_anonymization_key_;
};

using PRPPPreconnectInfoList = std::list<PRPPPreconnectInfo>;

struct PRPPReqInfoTreeNode {
  PRPPReqInfoTreeNode* parent_ { nullptr };
  std::list<std::shared_ptr<PRPPReqInfoTreeNode>> children_;
  std::shared_ptr<PRRequestInfo> req_info_ { nullptr };
  bool already_preloading_ { false };
};

}  // namespace ohos_prp_preload

#endif  // NET_BASE_PAGR_RES_REQUEST_INFO_H