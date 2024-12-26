// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRECONNECT_RUNNER_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRECONNECT_RUNNER_H

#include "base/memory/weak_ptr.h"
#include "net/base/network_anonymization_key.h"

namespace net {
class URLRequestContext;
}  // namespace net

class GURL;

namespace ohos_prp_preload {
// only run in network thread
class PreconnectRunner {
 public:
  static void PreconnectSocket(const GURL& original_url,
    bool allow_credentials,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    base::WeakPtr<net::URLRequestContext> url_request_context);
  static GURL GetHSTSRedirect(const GURL& original_url,
    base::WeakPtr<net::URLRequestContext> url_request_context);
 private:
  PreconnectRunner() = default;
  ~PreconnectRunner() = default;
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRECONNECT_RUNNER_H