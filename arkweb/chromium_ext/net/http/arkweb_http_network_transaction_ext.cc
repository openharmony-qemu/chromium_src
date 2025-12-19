// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40284755): Remove this and spanify to fix the errors.
#pragma allow_unsafe_buffers
#endif

#include "net/http/http_network_transaction.h"
#include "arkweb/chromium_ext/net/http/arkweb_http_network_transaction_ext.h"

#include <set>
#include <utility>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/base64url.h"
#include "base/compiler_specific.h"
#include "base/feature_list.h"
#include "base/format_macros.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/metrics/sparse_histogram.h"
#include "base/notreached.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
#include "base/trace_event/trace_event.h"
#endif
#include "base/values.h"
#include "build/build_config.h"
#include "net/base/auth.h"
#include "net/base/features.h"
#include "net/base/host_port_pair.h"
#include "net/base/io_buffer.h"
#include "net/base/load_flags.h"
#include "net/base/load_timing_info.h"
#include "net/base/net_errors.h"
#include "net/base/proxy_chain.h"
#include "net/base/proxy_server.h"
#include "net/base/transport_info.h"
#include "net/base/upload_data_stream.h"
#include "net/base/url_util.h"
#include "net/cert/cert_status_flags.h"
#include "net/filter/filter_source_stream.h"
#include "net/http/bidirectional_stream_impl.h"
#include "net/http/http_auth.h"
#include "net/http/http_auth_controller.h"
#include "net/http/http_auth_handler.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_basic_stream.h"
#include "net/http/http_chunked_decoder.h"
#include "net/http/http_connection_info.h"
#include "net/http/http_log_util.h"
#include "net/http/http_network_session.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_request_info.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/http/http_server_properties.h"
#include "net/http/http_status_code.h"
#include "net/http/http_stream.h"
#include "net/http/http_stream_factory.h"
#include "net/http/http_stream_pool.h"
#include "net/http/http_util.h"
#include "net/http/transport_security_state.h"
#include "net/http/url_security_manager.h"
#include "net/log/net_log_event_type.h"
#include "net/proxy_resolution/proxy_info.h"
#include "net/socket/client_socket_factory.h"
#include "net/socket/next_proto.h"
#include "net/socket/transport_client_socket_pool.h"
#include "net/spdy/spdy_http_stream.h"
#include "net/spdy/spdy_session.h"
#include "net/spdy/spdy_session_pool.h"
#include "net/ssl/ssl_cert_request_info.h"
#include "net/ssl/ssl_connection_status_flags.h"
#include "net/ssl/ssl_info.h"
#include "net/ssl/ssl_private_key.h"
#include "url/gurl.h"
#include "url/origin.h"
#include "url/scheme_host_port.h"
#include "url/url_canon.h"

#if BUILDFLAG(ENABLE_REPORTING)
#include "net/network_error_logging/network_error_logging_service.h"
#include "net/reporting/reporting_header_parser.h"
#include "net/reporting/reporting_service.h"
#endif  // BUILDFLAG(ENABLE_REPORTING)

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "arkweb/chromium_ext/net/base/log_utils.h"
#endif

namespace net {

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
  constexpr int kTimeoutSeconds = 5;
#endif

ArkWebHttpNetworkTransactionExt::ArkWebHttpNetworkTransactionExt(RequestPriority priority,
                                               HttpNetworkSession* session)
    : HttpNetworkTransaction(priority, session) {}

ArkWebHttpNetworkTransactionExt::~ArkWebHttpNetworkTransactionExt() {
#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
  StopRecording();
#endif
}

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
int ArkWebHttpNetworkTransactionExt::RestartWithSecureDnsOnly(
    CompletionOnceCallback callback) {
  DCHECK(!stream_.get());
  DCHECK(!stream_request_.get());
  DCHECK_EQ(STATE_NONE, next_state_);
  if (!CheckMaxRestarts()) {
    return ERR_TOO_MANY_RETRIES;
  }

  // Reset the other member variables.
  // Note: this is necessary only with SSL renegotiation.
  ResetStateForRestart();
  next_state_ = STATE_CREATE_FALLBACK_STREAM_WITH_SECURE_DNS_ONLY;
  int rv = DoLoop(OK);
  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  }

  // This always returns ERR_IO_PENDING because DoCreateStream() does, but
  // GenerateNetworkErrorLoggingReportIfError() should be called here if any
  // other net::Error can be returned.
  CHECK_EQ(rv, ERR_IO_PENDING);
  return rv;
}

int ArkWebHttpNetworkTransactionExt::DoCreateFallbackStreamWithSecureDnsOnly() {
  response_.network_accessed = true;

  next_state_ = STATE_CREATE_FALLBACK_STREAM_WITH_SECURE_DNS_ONLY_COMPLETE;
  // IP based pooling is only enabled on a retry after 421 Misdirected Request
  // is received. Alternative Services are also disabled in this case (though
  // they can also be disabled when retrying after a QUIC error).
  if (!enable_ip_based_pooling_) {
    DCHECK(!enable_alternative_services_);
  }
  if (ForWebSocketHandshake()) {
    stream_request_ =
        session_->http_stream_factory()->RequestWebSocketHandshakeStream(
            *request_, priority_, /*allowed_bad_certs=*/observed_bad_certs_,
            this, websocket_handshake_stream_base_create_helper_,
            enable_ip_based_pooling_, enable_alternative_services_, net_log_);
  } else {
    stream_request_ = session_->http_stream_factory()->RequestStream(
        *request_, priority_, /*allowed_bad_certs=*/observed_bad_certs_, this,
        enable_ip_based_pooling_, enable_alternative_services_, net_log_);
  }
  CHECK(stream_request_.get());
  return ERR_IO_PENDING;
}

int ArkWebHttpNetworkTransactionExt::DoCreateFallbackStreamWithSecureDnsOnlyComplete(
    int result) {
  CopyConnectionAttemptsFromStreamRequest();
  if (result == OK) {
    next_state_ = STATE_CONNECTED_CALLBACK;
    DCHECK(stream_.get());
  } else if (result == ERR_HTTP_1_1_REQUIRED ||
             result == ERR_PROXY_HTTP_1_1_REQUIRED) {
    return HandleHttp11Required(result);
  }

  // Handle possible client certificate errors that may have occurred if the
  // stream used SSL for one or more of the layers.
  result = HandleSSLClientAuthError(result);

  // At this point we are done with the stream_request_.
  stream_request_.reset();
  return result;
}
#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
int ArkWebHttpNetworkTransactionExt::RestartWithFallbackProxy(
    CompletionOnceCallback callback) {
  DCHECK(!stream_.get());
  DCHECK(!stream_request_.get());
  DCHECK_EQ(STATE_NONE, next_state_);
  if (!CheckMaxRestarts()) {
    return ERR_TOO_MANY_RETRIES;
  }

  // Reset the other member variables.
  // Note: this is necessary only with SSL renegotiation.
  ResetStateForRestart();
  next_state_ = STATE_CREATE_FALLBACK_STREAM_WITH_FALLBACK_PROXY;
  int rv = DoLoop(OK);
  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  }

  // This always returns ERR_IO_PENDING because DoCreateStream() does, but
  // GenerateNetworkErrorLoggingReportIfError() should be called here if any
  // other net::Error can be returned.
  CHECK_EQ(rv, ERR_IO_PENDING);
  return rv;
}

int ArkWebHttpNetworkTransactionExt::DoCreateStreamWithFallbackProxy() {
  response_.network_accessed = true;

  next_state_ = STATE_CREATE_FALLBACK_STREAM_WITH_FALLBACK_PROXY_COMPLETE;
  // IP based pooling is only enabled on a retry after 421 Misdirected Request
  // is received. Alternative Services are also disabled in this case (though
  // they can also be disabled when retrying after a QUIC error).
  if (!enable_ip_based_pooling_) {
    DCHECK(!enable_alternative_services_);
  }

  if (!request_) {
    LOG(INFO) << "DoCreateStreamWithFallbackProxy return ERR_ABORTED for "
                  "request is nullptr";
    return ERR_ABORTED;
  }

  if (ForWebSocketHandshake()) {
    stream_request_ =
        session_->http_stream_factory()->RequestWebSocketHandshakeStream(
            *request_, priority_, /*allowed_bad_certs=*/observed_bad_certs_,
            this, websocket_handshake_stream_base_create_helper_,
            enable_ip_based_pooling_, enable_alternative_services_, net_log_);
  } else {
    stream_request_ = session_->http_stream_factory()->RequestStream(
        *request_, priority_, /*allowed_bad_certs=*/observed_bad_certs_, this,
        enable_ip_based_pooling_, enable_alternative_services_, net_log_);
  }
  CHECK(stream_request_.get());
  return ERR_IO_PENDING;
}

int ArkWebHttpNetworkTransactionExt::DoCreateStreamWithFallbackProxyComplete(
    int result) {
  CopyConnectionAttemptsFromStreamRequest();
  if (result == OK) {
    next_state_ = STATE_CONNECTED_CALLBACK;
    DCHECK(stream_.get());
  } else if (result == ERR_HTTP_1_1_REQUIRED ||
             result == ERR_PROXY_HTTP_1_1_REQUIRED) {
    return HandleHttp11Required(result);
  }

  // Handle possible client certificate errors that may have occurred if the
  // stream used SSL for one or more of the layers.
  result = HandleSSLClientAuthError(result);

  // At this point we are done with the stream_request_.
  stream_request_.reset();
  return result;
}

int ArkWebHttpNetworkTransactionExt::RestartWithDirect(
    CompletionOnceCallback callback) {
  DCHECK(!stream_.get());
  DCHECK(!stream_request_.get());
  DCHECK_EQ(STATE_NONE, next_state_);
  if (!CheckMaxRestarts()) {
    return ERR_TOO_MANY_RETRIES;
  }

  // Reset the other member variables.
  // Note: this is necessary only with SSL renegotiation.
  ResetStateForRestart();
  response_ = HttpResponseInfo();
  next_state_ = STATE_CREATE_STREAM;
  int rv = DoLoop(OK);
  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  }

  // This always returns ERR_IO_PENDING because DoCreateStream() does, but
  // GenerateNetworkErrorLoggingReportIfError() should be called here if any
  // other net::Error can be returned.
  CHECK_EQ(rv, ERR_IO_PENDING);
  return rv;
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
void ArkWebHttpNetworkTransactionExt::StartRecording() {
  if (is_recording_) {
    timer_.Stop();
    timer_.Start(FROM_HERE, base::Seconds(kTimeoutSeconds), this,
                 &ArkWebHttpNetworkTransactionExt::ReportTimeout);
    return;
  }

  is_recording_ = true;
  timer_.Start(FROM_HERE, base::Seconds(kTimeoutSeconds), this,
               &ArkWebHttpNetworkTransactionExt::ReportTimeout);
}

void ArkWebHttpNetworkTransactionExt::StopRecording() {
  if (!is_recording_) {
    return;
  }

  is_recording_ = false;
  timer_.Stop();
}

void ArkWebHttpNetworkTransactionExt::ReportTimeout() {
  base::Value::Dict record;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  const HttpResponseInfo* response_info = GetResponseInfo();
  if (response_info) {
    record.Set("ip", net::LogUtils::AnonymizeIpAddress(response_info->remote_endpoint));
    record.Set("connection_info", net::HttpConnectionInfoToString(response_info->connection_info));
    record.Set("received_body_bytes", base::NumberToString(received_body_bytes_));
  }
#endif

  LOG(INFO) << "INFO: request had no reponse within 5 seconds. url: *** " << record;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "INFO: request had no reponse within 5 seconds. url: "
                     << url::LogUtils::ConvertUrlWithMask(url_.spec())
                     << " " << record;
  if (!session_->is_strict_log_mode()) {
    LOG(URL) << "request had no reponse within 5 seconds. url: " << url_.spec()
             << " " << record;
  }
#endif
}
#endif

}  // namespace net
