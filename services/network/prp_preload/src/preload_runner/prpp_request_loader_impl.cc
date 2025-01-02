// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on url_loader.cc originally written by
// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/preload_runner/prpp_request_loader_impl.h"

#include "base/logging.h"
#include "services/network/prp_preload/src/page_res_parallel_preload_mgr_impl.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace {
const int32_t CACHE_BLOCK_SIZE = 64 * 1024;
}

namespace ohos_prp_preload {
std::shared_ptr<PRPPRequestLoader> PRPPRequestLoader::CreatePRPPRequestLoader(
    const std::string& main_url,
    const std::shared_ptr<PRRequestInfo>& info,
    const net::IsolationInfo& isolation_info,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    ResPreloadedCB res_loaded_cb)
{
  std::shared_ptr<PRPPRequestLoader> prpp_req_loader = std::make_shared<PRPPRequestLoaderImpl>(
    main_url, info, url_request_context, std::move(res_loaded_cb));
  if (prpp_req_loader->Init(isolation_info)) {
    return prpp_req_loader;
  }
  return nullptr;
}

PRPPRequestLoaderImpl::PRPPRequestLoaderImpl(const std::string& main_url,
    const std::shared_ptr<PRRequestInfo>& info,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    ResPreloadedCB res_loaded_cb)
  : main_url_(main_url), sub_url_(info->url().spec()), url_request_context_(url_request_context),
  res_loaded_cb_(std::move(res_loaded_cb)), prpp_req_info_(info) { }

bool PRPPRequestLoaderImpl::Init(const net::IsolationInfo& isolation_info)
{
  if (!prpp_req_info_ || !url_request_context_.get()) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::Init, invalid args";
    return false;
  }
  real_load_flags_ = prpp_req_info_->load_flags();
  InitAndStartUrlRequest(prpp_req_info_, isolation_info);
  return true;
}

void PRPPRequestLoaderImpl::InitAndStartUrlRequest(const std::shared_ptr<PRRequestInfo>& info,
  const net::IsolationInfo& isolation_info)
{
  if (info->type() == PRRequestInfoType::TYPE_PAGE_PREFLIGHT) {
    sub_url_ = info->url().spec().substr(strlen(PRPP_PREFLIGHT_PREFIX));
  }
  url_request_ = url_request_context_.get()->CreateRequestForPrpp(GURL(sub_url_),
    net::MEDIUM, this, MISSING_TRAFFIC_ANNOTATION, false);
  url_request_->set_method(info->method());
  url_request_->set_force_ignore_site_for_cookies(info->force_ignore_site_for_cookies());
  url_request_->SetURLChain( { GURL(sub_url_) } );
  url_request_->SetReferrer(info->referrer());
  url_request_->set_referrer_policy(info->referrer_policy());
  url_request_->set_upgrade_if_insecure(info->upgrade_if_insecure());
  if (isolation_info.IsEmpty()) {
    url::Origin origin = url::Origin::Create(GURL(sub_url_));
    net::IsolationInfo iso_info = net::IsolationInfo::Create(net::IsolationInfo::RequestType::kOther,
      origin, origin, net::SiteForCookies());
     url_request_->set_site_for_cookies(iso_info.site_for_cookies());
     url_request_->set_isolation_info(iso_info);
  } else {
    url_request_->set_site_for_cookies(isolation_info.site_for_cookies());
    url_request_->set_isolation_info(isolation_info);
  }
  url_request_->set_force_main_frame_for_same_site_cookies(info->force_main_frame_for_same_site_cookies());
  url_request_->SetSecureDnsPolicy(info->secure_dns_policy());
  url_request_->SetExtraRequestHeaders(info->extra_request_headers());

  if (info->accepted_stream_types().has_value()) {
    url_request_->set_accepted_stream_types(info->accepted_stream_types());
  }
  if (info->initiator().has_value()) {
    url_request_->set_initiator(info->initiator());
  }
  url_request_->set_first_party_url_policy(info->first_party_url_policy());
  url_request_->SetLoadFlags(real_load_flags_);
  if (!info->allow_credentials()) {
    url_request_->set_allow_credentials(false);
  }
  url_request_->set_send_client_certs(info->send_client_certs());
  url_request_->SetRequestHeadersCallback(base::BindRepeating(
      &PRPPRequestLoaderImpl::SetRawRequestHeadersAndNotify, base::Unretained(this)));
  url_request_->SetResponseHeadersCallback(base::BindRepeating(
      &PRPPRequestLoaderImpl::SetRawResponseHeaders, base::Unretained(this)));
  url_request_->SetEarlyResponseHeadersCallback(base::BindRepeating(
      &PRPPRequestLoaderImpl::NotifyEarlyResponse, base::Unretained(this)));
  url_request_->set_has_storage_access(info->has_storage_access());
  url_request_->cookie_setting_overrides().PutAll(info->cookie_setting_overrides());
  url_request_->set_update_res_request_info_callback(
    base::BindRepeating(&PRPPRequestLoaderImpl::UpdateResRequestInfo,
    weak_ptr_factory_.GetWeakPtr()));
  url_request_->set_allow_preload_record(true);
  url_request_->set_main_url(GURL(main_url_));
  url_request_->set_preload_info(prpp_req_info_);

  BeginTrustTokenOperationIfNecessaryAndThenScheduleStart();
}

void PRPPRequestLoaderImpl::ReInitAndStartUrlRequest()
{
  preload_state_ = STATE_IDLE;
  ClearLoader();
  InitAndStartUrlRequest(prpp_req_info_, url_request_->isolation_info());
}

void PRPPRequestLoaderImpl::BeginTrustTokenOperationIfNecessaryAndThenScheduleStart()
{
  // only support no trust token params now.
  BeginAttributionIfNecessaryAndThenScheduleStart();
}

void PRPPRequestLoaderImpl::BeginAttributionIfNecessaryAndThenScheduleStart()
{
  // only support !attribution_request_helper_
  ScheduleStart();
}

void PRPPRequestLoaderImpl::ScheduleStart()
{
  // for sub resource start directly.
  url_request_->Start();
}

void PRPPRequestLoaderImpl::SetRawRequestHeadersAndNotify(net::HttpRawRequestHeaders headers)
{
  if (preload_state_ >= STATE_REQ_SENT) {
    LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderImpl::SetRawRequestHeadersAndNotify, invalid state:" <<
      (int32_t)preload_state_;
    return;
  }
  raw_req_headers_ = std::move(headers);
  rec_msg_list_.push(MSG_RAW_REQ_HDR);
  preload_state_ = STATE_REQ_SENT;
  if (need_do_replay_self_) {
    DoReplay();
  }
}

void PRPPRequestLoaderImpl::NotifyEarlyResponse(scoped_refptr<const net::HttpResponseHeaders> headers)
{
  if (preload_state_ >= STATE_ERALY_HEAD_RESPONSED) {
    LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderImpl::NotifyEarlyResponse, invalid state:" <<
      (int32_t)preload_state_;
    return;
  }
  early_response_headers_ = headers;
  rec_msg_list_.push(MSG_EARLY_RESP_HDR);
  preload_state_ = STATE_ERALY_HEAD_RESPONSED;
  if (need_do_replay_self_) {
    DoReplay();
  }
}

void PRPPRequestLoaderImpl::SetRawResponseHeaders(scoped_refptr<const net::HttpResponseHeaders> headers)
{
  if (preload_state_ >= STATE_HEAD_RESPONSED) {
    LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderImpl::SetRawResponseHeaders, invalid state:" <<
      (int32_t)preload_state_;
    return;
  }
  raw_response_headers_ = headers;
  rec_msg_list_.push(MSG_RAW_RESP_HDR);
  preload_state_ = STATE_HEAD_RESPONSED;
  if (need_do_replay_self_) {
    DoReplay();
  }
}

int PRPPRequestLoaderImpl::OnConnected(net::URLRequest* url_request,
                                       const net::TransportInfo& info,
                                       net::CompletionOnceCallback callback)
{
  int ret = net::OK;
  if (preload_state_ >= STATE_CONNECTED) {
    LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderImpl::OnConnected, invalid state:" <<
      (int32_t)preload_state_;
    return ret;
  }
  transport_info_ = info;
  completion_once_callback_ = std::move(callback);
  rec_msg_list_.push(MSG_CONNECTED);
  preload_state_ = STATE_CONNECTED;
  if (need_do_replay_self_) {
    DoReplay();
  }
  return ret;
}

void PRPPRequestLoaderImpl::OnReceivedRedirect(net::URLRequest* url_request,
                                               const net::RedirectInfo& redirect_info,
                                               bool* defer_redirect)
{
  if (!res_loaded_cb_.is_null()) {
    std::move(res_loaded_cb_).Run(this);
  }
  PushFailure(STATE_UNSUPPORT, need_do_replay_self_);
}

void PRPPRequestLoaderImpl::OnAuthRequired(net::URLRequest* request,
                                           const net::AuthChallengeInfo& info)
{
  if (!res_loaded_cb_.is_null()) {
    std::move(res_loaded_cb_).Run(this);
  }
  PushFailure(STATE_UNSUPPORT, need_do_replay_self_);
}

void PRPPRequestLoaderImpl::OnCertificateRequested(net::URLRequest* request,
                                                   net::SSLCertRequestInfo* info)
{
  if (!res_loaded_cb_.is_null()) {
    std::move(res_loaded_cb_).Run(this);
  }
  PushFailure(STATE_UNSUPPORT, need_do_replay_self_);
}

void PRPPRequestLoaderImpl::OnSSLCertificateError(net::URLRequest* request,
                                                  int net_error,
                                                  const net::SSLInfo& info,
                                                  bool fatal)
{
  if (!res_loaded_cb_.is_null()) {
    std::move(res_loaded_cb_).Run(this);
  }
  PushFailure(STATE_UNSUPPORT, need_do_replay_self_);
}

void PRPPRequestLoaderImpl::OnResponseStarted(net::URLRequest* url_request, int net_error)
{
  if ((net_error == net::ERR_CACHE_MISS) &&
      ((real_load_flags_ & net::LOAD_ONLY_FROM_CACHE) == net::LOAD_ONLY_FROM_CACHE)) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::OnResponseStarted, ERR_CACHE_MISS try restart";
    real_load_flags_ &= (~net::LOAD_ONLY_FROM_CACHE);
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&PRPPRequestLoaderImpl::ReInitAndStartUrlRequest, weak_ptr_factory_.GetWeakPtr()));
    return;
  }

  if (preload_state_ >= STATE_RESPONSE_STARTED) {
    LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderImpl::OnResponseStarted, invalid state:" <<
      (int32_t)preload_state_;
    return;
  }

  if (net_error != net::OK) {
    PushFailure(STATE_UNSUPPORT, need_do_replay_self_);
    return;
  }
  preload_state_ = STATE_RESPONSE_STARTED;
  rec_msg_list_.push(MSG_RESPONSE_STARTED);
  if (need_do_replay_self_) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&PRPPRequestLoaderImpl::DoReplay, weak_ptr_factory_.GetWeakPtr()));
  }
  FinalizeAttributionIfNecessaryAndThenContinueOnResponseStarted();
}

void PRPPRequestLoaderImpl::FinalizeAttributionIfNecessaryAndThenContinueOnResponseStarted()
{
  // only support !attribution_request_helper_
  ContinueOnResponseStarted();
}

void PRPPRequestLoaderImpl::ContinueOnResponseStarted()
{
  StartReading();
}

void PRPPRequestLoaderImpl::StartReading()
{
  ReadMore();
}

void PRPPRequestLoaderImpl::ReadMore()
{
  if (!cur_write_block_) {
    cur_write_block_ = base::MakeRefCounted<net::GrowableIOBuffer>();
    if (!cur_write_block_) {
      LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::ReadMore, make cache block failed";
      PushFailure(STATE_ERROR);
      return;
    }
    cur_write_block_->SetCapacity(CACHE_BLOCK_SIZE);
    cur_write_block_->set_offset(0);
    if (!cur_write_block_->StartOfBuffer()) {
      LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::ReadMore, allocate cache block failed";
      PushFailure(STATE_ERROR);
      cur_write_block_ = nullptr;
      return;
    }
  } else if (cur_write_block_->RemainingCapacity() == 0) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::ReadMore, no left cache block";
    PushFailure(STATE_ERROR);
    return;
  }

  int bytes_read = url_request_->Read(cur_write_block_.get(), cur_write_block_->RemainingCapacity());
  if (bytes_read != net::ERR_IO_PENDING) {
    DidRead(bytes_read, true);
  }
}

void PRPPRequestLoaderImpl::DidRead(int num_bytes, bool completed_synchronously)
{
  if (num_bytes > cur_write_block_->RemainingCapacity() || num_bytes < 0) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::DidRead, left cache block not enough";
    PushFailure(STATE_ERROR);
    return;
  }

  if (num_bytes == 0) {
    total_size_ += (cur_write_block_->capacity() - cur_write_block_->RemainingCapacity());
    body_cache_.push(cur_write_block_);
    cur_write_block_ = nullptr;
    preload_state_ = STATE_RESPONSED;
    if (out_buf_ && delegate_) {
        int len = Read(out_buf_.get(), out_max_bytes_);
        delegate_->OnReadCompleted(url_request_.get(), len);
    }
    return;
  }

  cur_write_block_->set_offset(cur_write_block_->offset() + num_bytes);
  if (cur_write_block_->RemainingCapacity() == 0) {
    total_size_ += cur_write_block_->capacity();
    body_cache_.push(cur_write_block_);
    cur_write_block_ = nullptr;
    if (out_buf_ && delegate_) {
        int len = Read(out_buf_.get(), out_max_bytes_);
        delegate_->OnReadCompleted(url_request_.get(), len);
    }
  }
  if (completed_synchronously) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE,
      base::BindOnce(&PRPPRequestLoaderImpl::ReadMore, weak_ptr_factory_.GetWeakPtr()));
  } else {
    ReadMore();
  }
}

void PRPPRequestLoaderImpl::OnReadCompleted(net::URLRequest* request, int bytes_read)
{
  if (!res_loaded_cb_.is_null()) {
    std::move(res_loaded_cb_).Run(this);
  }

  if ((preload_state_ == STATE_UNSUPPORT) || (preload_state_ == STATE_ERROR)) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::OnReadCompleted, invalid state:" <<
      (int32_t)preload_state_;
    return;
  }
  DidRead(bytes_read, false);
}

void PRPPRequestLoaderImpl::SetRequestDelegate(net::URLRequest::Delegate* delegate)
{
  delegate_ = delegate;
}

void PRPPRequestLoaderImpl::SetRequestHeadersCallback(net::RequestHeadersCallback callback)
{
  request_headers_callback_ = std::move(callback);
}

void PRPPRequestLoaderImpl::SetResponseHeadersCallback(net::ResponseHeadersCallback callback)
{
  response_headers_callback_ = std::move(callback);
}

void PRPPRequestLoaderImpl::SetEarlyResponseHeadersCallback(net::ResponseHeadersCallback callback)
{
  early_response_headers_callback_ = std::move(callback);
}

bool PRPPRequestLoaderImpl::StartReplay()
{
  load_timing_info_.request_start_time = base::Time::Now();
  load_timing_info_.request_start = base::TimeTicks::Now();

  if (!delegate_ || (preload_state_ == STATE_UNSUPPORT) || (preload_state_ == STATE_ERROR)) {
    return false;
  }

  prpp_req_info_->or_preload_flag(ohos_prp_preload::PRPP_FLAGS_VISIBLE);
  prpp_req_info_->set_request_start_time(base::Time::Now().ToInternalValue());
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE,
    base::BindOnce(&PRPPRequestLoaderImpl::DoReplay, weak_ptr_factory_.GetWeakPtr()));
  if (need_update_req_info_) {
    PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(main_url_, prpp_req_info_);
  }
  return true;
}

void PRPPRequestLoaderImpl::DoReplay()
{
  if (rec_msg_list_.empty()) {
    if (preload_state_ != STATE_RESPONSED && preload_state_ != STATE_UNSUPPORT &&
        preload_state_ != STATE_ERROR) {
      need_do_replay_self_ = true;
    }
    return;
  }
  if (!delegate_) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::DoReplay, no delegate";
    return;
  }
  PRPPRecorderMsg cur_msg = rec_msg_list_.front();
  rec_msg_list_.pop();
  switch (cur_msg) {
    case MSG_CONNECTED:
      delegate_->OnConnected(url_request_.get(), transport_info_, std::move(completion_once_callback_));
      break;
    case MSG_RAW_REQ_HDR:
      if (!request_headers_callback_.is_null()) {
        request_headers_callback_.Run(std::move(raw_req_headers_));
      }
      break;
    case MSG_EARLY_RESP_HDR:
      if (!early_response_headers_callback_.is_null()) {
        early_response_headers_callback_.Run(early_response_headers_);
      }
      break;
    case MSG_RAW_RESP_HDR:
      if (!response_headers_callback_.is_null()) {
        response_headers_callback_.Run(raw_response_headers_);
      }
      break;
    case MSG_RESPONSE_STARTED:
      need_do_replay_self_ = false;
      delegate_->OnResponseStarted(url_request_.get(), net::OK);
      break;
    case MSG_RESPONSE_BODY:
      break;
    case MSG_ERROR:
    default:
      delegate_->OnResponseStarted(url_request_.get(), PRPP_ERROR);
      break;
  }
  if (!rec_msg_list_.empty() ||
     ((preload_state_ != STATE_RESPONSE_STARTED) && (preload_state_ != STATE_RESPONSED))) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE,
      base::BindOnce(&PRPPRequestLoaderImpl::DoReplay, weak_ptr_factory_.GetWeakPtr()));
  }
}

int PRPPRequestLoaderImpl::Read(net::IOBuffer* buf, int max_bytes)
{
  int ret = net::OK;
  do {
    if (max_bytes <= 0) {
      break;
    }

    if (body_cache_.empty()) {
      if (preload_state_ != STATE_RESPONSED && preload_state_ != STATE_UNSUPPORT &&
          preload_state_ != STATE_ERROR) {
        ret = net::ERR_IO_PENDING;
      }
      break;
    }
    scoped_refptr<net::GrowableIOBuffer> cur_block = body_cache_.front();
    if (!cur_block) {
      LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::Read, cur block is null, cache block size:" <<
        (int)body_cache_.size() << ", max_bytes:" << max_bytes;
      break;
    }
    int block_offset = - cur_block->offset() + cur_read_offset_;
    if (cur_block->offset() <= max_bytes + cur_read_offset_) {
      int len = cur_block->offset() - cur_read_offset_;
      if (memcpy_s(buf->data(), len, cur_block->data() + block_offset, len) != EOK) {
        break;
      }
      body_cache_.pop();
      cur_read_offset_ = 0;
      ret = len;
      break;
    }
    if (memcpy_s(buf->data(), max_bytes, cur_block->data() + block_offset, max_bytes) != EOK) {
      break;
    }
    cur_read_offset_ += max_bytes;
    ret = max_bytes;
  } while (false);

  if (ret == net::ERR_IO_PENDING) {
    out_buf_ = buf;
    out_max_bytes_ = max_bytes;
  } else {
    out_buf_ = nullptr;
    out_max_bytes_ = 0;
  }

  return ret;
}

void PRPPRequestLoaderImpl::UpdateResRequestInfo(const std::string& key, const std::shared_ptr<PRRequestInfo>& info)
{
  if (delegate_) {
    PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(key, info);
    return;
  }
  need_update_req_info_ = true;
}

void PRPPRequestLoaderImpl::PushFailure(PRPPReqLoaderState state, bool need_do_replay_self)
{
  if ((preload_state_ == STATE_UNSUPPORT) || (preload_state_ == STATE_ERROR)) {
    return;
  }
  preload_state_ = state;
  ClearMsgList();
  rec_msg_list_.push(MSG_ERROR);
  if (need_do_replay_self) {
    DoReplay();
  }
}

void PRPPRequestLoaderImpl::ClearMsgList()
{
  std::queue<PRPPRecorderMsg> msg_list;
  rec_msg_list_.swap(msg_list);
}

void PRPPRequestLoaderImpl::ClearBodyCache()
{
  std::queue<scoped_refptr<net::GrowableIOBuffer>> body_cache;
  body_cache_.swap(body_cache);
}

void PRPPRequestLoaderImpl::ClearLoader()
{
  ClearMsgList();
  ClearBodyCache();
  cur_write_block_ = nullptr;
  out_buf_ = nullptr;
  out_max_bytes_ = 0;
  cur_read_offset_ = 0;
  total_size_ = 0;
}

void PRPPRequestLoaderImpl::GetLoadTimingInfo(net::LoadTimingInfo* load_timing_info) const
{
  if (!load_timing_info || !url_request_) {
    LOG(WARNING) << "PRPPreload.PRPPRequestLoaderImpl::GetLoadTimingInfo, load_timing_info invalid";
    return;
  }

  url_request_->GetLoadTimingInfo(load_timing_info);
  load_timing_info->request_start_time = load_timing_info_.request_start_time;
  load_timing_info->request_start = load_timing_info_.request_start;

  if (!load_timing_info->proxy_resolve_start.is_null()) {
    load_timing_info->proxy_resolve_start = base::TimeTicks();
  }
  if (!load_timing_info->proxy_resolve_end.is_null()) {
    load_timing_info->proxy_resolve_end = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.domain_lookup_start.is_null()) {
    load_timing_info->connect_timing.domain_lookup_start = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.domain_lookup_end.is_null()) {
    load_timing_info->connect_timing.domain_lookup_end = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.connect_start.is_null()) {
    load_timing_info->connect_timing.connect_start = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.connect_end.is_null()) {
    load_timing_info->connect_timing.connect_end = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.ssl_start.is_null()) {
    load_timing_info->connect_timing.ssl_start = base::TimeTicks();
  }
  if (!load_timing_info->connect_timing.ssl_end.is_null()) {
    load_timing_info->connect_timing.ssl_end = base::TimeTicks();
  }
  if (load_timing_info->connect_timing.connect_end.is_null()) {
    load_timing_info->socket_reused = true;
  }

  if (!load_timing_info->send_start.is_null() &&
      (load_timing_info->send_start.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->send_start = load_timing_info->request_start;
  }
  if (!load_timing_info->send_end.is_null() &&
      (load_timing_info->send_end.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->send_end = load_timing_info->request_start;
  }
  if (!load_timing_info->receive_headers_start.is_null() &&
      (load_timing_info->receive_headers_start.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->receive_headers_start = load_timing_info->request_start;
  }
    if (!load_timing_info->receive_headers_end.is_null() &&
      (load_timing_info->receive_headers_end.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->receive_headers_end = load_timing_info->request_start;
  }
  if (!load_timing_info->receive_non_informational_headers_start.is_null() &&
      (load_timing_info->receive_non_informational_headers_start.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->receive_non_informational_headers_start =
      load_timing_info->request_start;
  }
  if (!load_timing_info->first_early_hints_time.is_null() &&
      (load_timing_info->first_early_hints_time.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->first_early_hints_time = load_timing_info->request_start;
  }
  if (!load_timing_info->push_start.is_null() &&
      (load_timing_info->push_start.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->push_start = load_timing_info->request_start;
  }
  if (!load_timing_info->push_end.is_null() &&
      (load_timing_info->push_end.ToInternalValue() <
       load_timing_info->request_start.ToInternalValue())) {
    load_timing_info->push_end = load_timing_info->request_start;
  }
}

void PRPPRequestLoaderImpl::ClearLoaderCallback(bool has_devtools_request_id)
{
  SetRequestDelegate(nullptr);
  SetRequestHeadersCallback(net::RequestHeadersCallback());
  if (has_devtools_request_id) {
    SetResponseHeadersCallback(net::ResponseHeadersCallback());
  }
  SetEarlyResponseHeadersCallback(net::ResponseHeadersCallback());
}

}  // namespace ohos_prp_preload