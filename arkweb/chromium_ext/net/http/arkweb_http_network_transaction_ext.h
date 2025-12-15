// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_HTTP_ARKWEB_HTTP_NETWORK_TRANSACTION_EXT_H_
#define NET_HTTP_ARKWEB_HTTP_NETWORK_TRANSACTION_EXT_H_

#include <stdint.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/time/time.h"
#include "build/buildflag.h"
#include "crypto/ec_private_key.h"
#include "net/base/completion_once_callback.h"
#include "net/base/completion_repeating_callback.h"
#include "net/base/net_error_details.h"
#include "net/base/net_export.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/request_priority.h"
#include "net/http/http_auth.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_info.h"
#include "net/http/http_stream_factory.h"
#include "net/http/http_stream_request.h"
#include "net/http/http_transaction.h"
#include "net/log/net_log_with_source.h"
#include "net/net_buildflags.h"
#include "net/proxy_resolution/proxy_resolution_service.h"
#include "net/socket/connection_attempts.h"
#include "net/ssl/ssl_config.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_versions.h"
#include "net/websockets/websocket_handshake_stream_base.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace net {

class HttpNetworkSession;

class NET_EXPORT_PRIVATE ArkWebHttpNetworkTransactionExt : public HttpNetworkTransaction {
 public:
  friend class HttpNetworkTransaction;

  ArkWebHttpNetworkTransactionExt(RequestPriority priority, HttpNetworkSession* session);

  ArkWebHttpNetworkTransactionExt(const ArkWebHttpNetworkTransactionExt&) = delete;
  ArkWebHttpNetworkTransactionExt& operator=(const ArkWebHttpNetworkTransactionExt&) = delete;

  ~ArkWebHttpNetworkTransactionExt() override;

  ArkWebHttpNetworkTransactionExt *AsArkWebHttpNetworkTransactionExt() override { return this; }

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  int RestartWithSecureDnsOnly(CompletionOnceCallback callback) override;
#endif

 private:
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  int DoCreateFallbackStreamWithSecureDnsOnly();
  int DoCreateFallbackStreamWithSecureDnsOnlyComplete(int result);
#endif

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
  void StartRecording();
  void StopRecording();
  void ReportTimeout();

  base::RepeatingTimer timer_;
  bool is_recording_;
#endif
};

}  // namespace net

#endif  // NET_HTTP_ARKWEB_HTTP_NETWORK_TRANSACTION_EXT_H_
