// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/net/base/page_res_request_info.h"

#include "net/url_request/url_request.h"

namespace ohos_prp_preload {
void PRRequestInfo::InitInfoFromUrlRequest(const net::URLRequest& request)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  url_ = request.url();
  allow_credentials_ = request.allow_credentials();
  ad_tagged_ = request.ad_tagged();
  is_for_websockets_ = request.is_for_websockets();
  initiator_ = request.initiator();
  referrer_ = request.referrer();
  method_ = request.method();
  storage_access_status_ = request.storage_access_status();
  extra_request_headers_ = request.extra_request_headers();
  load_flags_ = request.load_flags();
  secure_dns_policy_ = request.secure_dns_policy();
  accepted_stream_types_ = request.accepted_stream_types();
  upgrade_if_insecure_ = request.upgrade_if_insecure();
  send_client_certs_ = request.send_client_certs();
  idempotency_ = request.GetIdempotency();
  force_ignore_site_for_cookies_ = request.force_ignore_site_for_cookies();
  force_main_frame_for_same_site_cookies_ = request.force_main_frame_for_same_site_cookies();
  cookie_setting_overrides_ = request.cookie_setting_overrides();
  referrer_policy_ = request.referrer_policy();
  first_party_url_policy_ = request.first_party_url_policy();
  net_log_ = request.net_log();
}

// for multi-thread read/write scenarios
CacheInfo PRRequestInfo::cache_info() {
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return CacheInfo {cache_type_, freshness_life_times_, e_tag_, last_modified_};
}

void PRRequestInfo::set_cache_info(PRRequestCacheType cache_type,
    int64_t freshness_life_times,
	const std::string& e_tag,
	const std::string& last_modified) {
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  cache_type_ = cache_type;
  freshness_life_times_ = freshness_life_times;
  e_tag_ = e_tag;
  last_modified_ = last_modified;
}

void PRRequestInfo::set_request_end_time(int64_t time)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  req_end_time_ = time;
}

int64_t PRRequestInfo::request_end_time() const
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return req_end_time_ > req_start_time_ ? req_end_time_ : 0;
}

void PRRequestInfo::set_preload_flag(PRRequestFlags flags)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  preload_flags_ = flags;
}

void PRRequestInfo::or_preload_flag(PRRequestFlags flags)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  preload_flags_ = (PRRequestFlags)(preload_flags_ | flags);
}

PRRequestFlags PRRequestInfo::preload_flag() const
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return preload_flags_;
}

void PRRequestInfo::add_preconnect_num(bool add)
{
  if (add) {
    preconnect_num_++;
  }
}

void PRRequestInfo::add_reused_preconnect_num(bool add)
{
  if (add) {
    reused_preconnect_num_++;
  }
}

void PRRequestInfo::compute_limit_num()
{
  if (preconnect_num_ <= 0 || reused_preconnect_num_ <= 0) {
    limit_num_ = 0;
    return;
  }
  limit_num_ = (reused_preconnect_num_ > MAX_PRECONNECT_COUNT) ? MAX_PRECONNECT_COUNT : reused_preconnect_num_;
}

void PRRequestInfo::set_is_preconnect(bool is_preconnect)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  is_preconnect_ = is_preconnect;
}

bool PRRequestInfo::is_preconnect() const
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return is_preconnect_;
}

void PRRequestInfo::set_is_reused_sokcet(bool is_reused_sokcet)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  is_reused_sokcet_ = is_reused_sokcet;
}

bool PRRequestInfo::is_reused_sokcet() const
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return is_reused_sokcet_;
}

net::HttpRequestHeaders PRRequestInfo::extra_request_headers()
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  return extra_request_headers_;
}

void PRRequestInfo::set_extra_request_headers(const net::HttpRequestHeaders& extra_request_headers)
{
  std::lock_guard<std::mutex> cache_info_guard(cache_info_mutex_);
  extra_request_headers_ = extra_request_headers;
}

} // namespace ohos_prp_preload