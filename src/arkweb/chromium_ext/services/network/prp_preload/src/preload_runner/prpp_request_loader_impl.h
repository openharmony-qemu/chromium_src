// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_IMPL_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_IMPL_H

#include "arkweb/chromium_ext/services/network/prp_preload/include/preload_runner/prpp_request_loader.h"
#include "base/memory/scoped_refptr.h"
#include "net/base/transport_info.h"
#include "net/url_request/url_request_context.h"

namespace net {
class URLRequestContext;
} // namespace net

namespace network {
class NetworkContext;
} // namespace network

// only run in network thread
namespace ohos_prp_preload {
class PRPPRequestLoaderImpl : public PRPPRequestLoader,
                              public net::URLRequest::Delegate {
 public:
  enum PRPPRecorderMsg {
    MSG_CONNECTED,
    MSG_RAW_REQ_HDR,
    MSG_EARLY_RESP_HDR,
    MSG_RAW_RESP_HDR,
    MSG_RESPONSE_STARTED,
    MSG_RESPONSE_BODY,
    MSG_ERROR,
  };

  PRPPRequestLoaderImpl(const std::string& main_url,
    const std::shared_ptr<PRRequestInfo>& info,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    ResPreloadedCB res_loaded_cb);
  ~PRPPRequestLoaderImpl() = default;
  bool Init(const net::IsolationInfo& isolation_info) override;
  void SetRequestDelegate(net::URLRequest::Delegate* delegate) override;
  bool StartReplay() override;
  void SetRequestHeadersCallback(net::RequestHeadersCallback callback) override;
  void SetResponseHeadersCallback(net::ResponseHeadersCallback callback) override;
  void SetEarlyResponseHeadersCallback(net::ResponseHeadersCallback callback) override;
  void SetResetUrlRequestCallback(ResetUrlRequestCallback callback) override;
  void SetUpdateResRequestInfoCallback(UpdateResRequestInfoCallback callback) override;
  int Read(net::IOBuffer* buf, int max_bytes) override;
  std::shared_ptr<net::URLRequest> GetURLRequest() override { return url_request_; }
  std::shared_ptr<PRRequestInfo> GetPRRequestInfo() override { return prpp_req_info_; }
  void GetLoadTimingInfo(net::LoadTimingInfo* load_timing_info) const override;
  PRPPReqLoaderState GetState() const override { return preload_state_; }
  void ClearLoaderCallback(bool has_devtools_request_id) override;
 private:
  void ProcessOutboundTrustTokenInterceptor();
  void ProcessOutboundAttributionInterceptor();
  void ProcessOutboundSharedStorageInterceptor();
  void ScheduleStart();
  void SetRawRequestHeadersAndNotify(net::HttpRawRequestHeaders headers);
  void SetRawResponseHeaders(scoped_refptr<const net::HttpResponseHeaders> headers);
  void NotifyEarlyResponse(scoped_refptr<const net::HttpResponseHeaders> headers);
  void FinalizeAttributionIfNecessaryAndThenContinueOnResponseStarted();
  void ContinueOnResponseStarted();
  void StartReading();
  void ReadMore();
  void DidRead(int num_bytes, bool completed_synchronously);
  void DoReplay();
  void ProcessMessages();
  void UpdateResRequestInfo(const std::string& key, const std::shared_ptr<PRRequestInfo>& info);
  void InitAndStartUrlRequest(const std::shared_ptr<PRRequestInfo>& info,
    const net::IsolationInfo& isolation_info, bool need_reset_url_request = false);
  void ReInitAndStartUrlRequest();

  // derived from net::URLRequest::Delegate
  int OnConnected(net::URLRequest* url_request,
                  const net::TransportInfo& info,
                  net::CompletionOnceCallback callback) override;
  void OnReceivedRedirect(net::URLRequest* url_request,
                          const net::RedirectInfo& redirect_info,
                          bool* defer_redirect) override;
  void OnAuthRequired(net::URLRequest* request,
                      const net::AuthChallengeInfo& info) override;
  void OnCertificateRequested(net::URLRequest* request,
                              net::SSLCertRequestInfo* info) override;
  void OnSSLCertificateError(net::URLRequest* request,
                             int net_error,
                             const net::SSLInfo& info,
                             bool fatal) override;
  void OnResponseStarted(net::URLRequest* url_request, int net_error) override;
  void OnReadCompleted(net::URLRequest* request, int bytes_read) override;
  void PushFailure(PRPPReqLoaderState state, bool need_do_replay_self = false);
  void ClearMsgList();
  void ClearBodyCache();
  void ClearLoader();

  std::string main_url_;
  std::string sub_url_;
  base::WeakPtr<net::URLRequestContext> url_request_context_;
  ResPreloadedCB res_loaded_cb_;
  raw_ptr<net::URLRequest::Delegate, DanglingUntriaged> delegate_ { nullptr };
  std::shared_ptr<net::URLRequest> url_request_ { nullptr };
  net::HttpRawRequestHeaders raw_req_headers_;
  scoped_refptr<const net::HttpResponseHeaders> early_response_headers_;
  scoped_refptr<const net::HttpResponseHeaders> raw_response_headers_;
  net::TransportInfo transport_info_ {};
  PRPPReqLoaderState preload_state_ { STATE_IDLE };
  std::queue<scoped_refptr<net::GrowableIOBuffer>> body_cache_;
  scoped_refptr<net::GrowableIOBuffer> cur_write_block_;
  int cur_read_offset_ { 0 };
  uint32_t total_size_ { 0 };
  std::queue<PRPPRecorderMsg> rec_msg_list_;
  net::CompletionOnceCallback completion_once_callback_;
  net::RequestHeadersCallback request_headers_callback_;
  net::ResponseHeadersCallback early_response_headers_callback_;
  net::ResponseHeadersCallback response_headers_callback_;
  ResetUrlRequestCallback reset_url_request_callback_;
  UpdateResRequestInfoCallback update_res_request_info_callback_;
  scoped_refptr<net::IOBuffer> out_buf_ { nullptr };
  int out_max_bytes_ { 0 };
  bool need_do_replay_self_ { false };
  std::shared_ptr<PRRequestInfo> prpp_req_info_;
  bool need_update_req_info_ { false };
  net::LoadTimingInfo load_timing_info_;
  uint32_t real_load_flags_ { 0 };
  bool need_continue_read_ { false };

  base::WeakPtrFactory<PRPPRequestLoaderImpl> weak_ptr_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_IMPL_H