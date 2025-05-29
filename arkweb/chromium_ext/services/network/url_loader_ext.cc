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
#include "arkweb/chromium_ext/services/network/url_loader_ext.h"
#include "base/logging.h"

namespace network {

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
const char* kLoadTimingInfoEvent = "load_timing_info";
const char* kHost = "host";
const char* kErrorCode = "error_code";
const char* kProtocol = "protocol";
const char* kUseQuic = "use_quic";
const char* kSocketReused = "socket_reused";
const char* kDnsDurationMs = "dns_duration_ms";
const char* kConnectDurationMs = "connect_duration_ms";
const char* kSendStartToReceiveHeadersEndMs =
    "send_start_to_receive_headers_end_ms";
const char* kTotalSendBytes = "total_send_bytes";
const char* kTotalRecvBytes = "total_recv_bytes";
const char* kSendStartToOnComplete = "send_start_to_on_complete";
#endif  // BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
std::string BoolToString(bool value)
{
  return value ? "true" : "false";
}

std::string GetProtocol(const GURL& url, const net::HttpResponseInfo& info)
{
  std::string protocol = info.alpn_negotiated_protocol;
  if (protocol.empty() || protocol == "unknown") {
    if (info.was_fetched_via_spdy) {
      protocol = "h2";
    } else if (url.SchemeIsHTTPOrHTTPS()) {
      protocol = "http";
      if (info.headers) {
        if (info.headers->GetHttpVersion() == net::HttpVersion(0, 9)) {
          protocol = "http/0.9";
        } else if (info.headers->GetHttpVersion() == net::HttpVersion(1, 0)) {
          protocol = "http/1.0";
        } else if (info.headers->GetHttpVersion() == net::HttpVersion(1, 1)) {
          protocol = "http/1.1";
        }
      }
    } else {
      protocol = url.scheme();
    }
  }
  return protocol;
}

void ReportUrlQuicInfo(net::URLRequest* url_request, int error_code)
{
  net::LoadTimingInfo load_timing_info;
  url_request->GetLoadTimingInfo(&load_timing_info);
  std::string host = url_request->url().host();
  const net::HttpResponseInfo& response_info = url_request->response_info();
  std::string protocol = GetProtocol(url_request->url(), response_info);
  bool use_quic = false;
  if (response_info.DidUseQuic()) {
    use_quic = true;
  }
  bool socket_reused = load_timing_info.socket_reused;
  int64_t dns_start_ms =
      load_timing_info.connect_timing.domain_lookup_start.since_origin()
          .InMilliseconds();
  int64_t dns_end_ms =
      load_timing_info.connect_timing.domain_lookup_end.since_origin()
          .InMilliseconds();
  int64_t dns_duration_ms = dns_end_ms - dns_start_ms;
  int64_t connect_start_ms =
      load_timing_info.connect_timing.connect_start.since_origin()
          .InMilliseconds();
  int64_t connect_end_ms =
      load_timing_info.connect_timing.connect_end.since_origin()
          .InMilliseconds();
  int64_t connect_duration_ms = connect_end_ms - connect_start_ms;
  int64_t send_start_ms =
      load_timing_info.send_start.since_origin().InMilliseconds();
  int64_t receive_headers_end_ms =
      load_timing_info.receive_headers_end.since_origin().InMilliseconds();
  int64_t send_start_to_receive_headers_end_ms =
      receive_headers_end_ms - send_start_ms;
  int total_send_bytes = url_request->GetTotalSentBytes();
  int total_recv_bytes = url_request->GetTotalReceivedBytes();
  int64_t on_complete_ms =
      base::TimeTicks::Now().since_origin().InMilliseconds();
  int64_t send_start_to_on_complete = on_complete_ms - send_start_ms;

  LOG(INFO) << "event_message: " << kLoadTimingInfoEvent << " " << kErrorCode
            << ":" << error_code << ", " << kProtocol << ":" << protocol << ", "
            << kUseQuic << ":" << use_quic << ", " << kSocketReused << ":"
            << socket_reused << ", " << kDnsDurationMs << ":" << dns_duration_ms
            << ", " << kConnectDurationMs << ":" << connect_duration_ms << ", "
            << kSendStartToReceiveHeadersEndMs << ":"
            << send_start_to_receive_headers_end_ms << ", " << kTotalSendBytes
            << ":" << total_send_bytes << ", " << kTotalRecvBytes << ":"
            << total_recv_bytes << ", " << kSendStartToOnComplete << ":"
            << send_start_to_on_complete << ".";
  LOG(DEBUG) << "event_message: " << kHost << ":" << host << ".";
}
#endif
}  // namespace network
