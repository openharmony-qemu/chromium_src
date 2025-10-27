// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREDICTORS_RESOURCE_PREFETCH_PREDICTOR_H_
#define CEF_LIBCEF_BROWSER_PREDICTORS_RESOURCE_PREFETCH_PREDICTOR_H_

#include <stddef.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "base/gtest_prod_util.h"
#include "base/memory/weak_ptr.h"
#include "base/task/cancelable_task_tracker.h"
#include "base/time/time.h"
#include "components/keyed_service/core/keyed_service.h"
#include "net/base/network_anonymization_key.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_config.h"
#include "ohos_cef_ext/libcef/browser/predictors/navigation_id.h"
#include "url/origin.h"

namespace ohos_predictors {

// Stores all values needed to trigger a preconnect/preresolve job to a single
// origin.
struct PreconnectRequest {
  // |network_anonymization_key| specifies the key that network requests for the
  // preconnected URL are expected to use. If a request is issued with a
  // different key, it may not use the preconnected socket. It has no effect
  // when |num_sockets| == 0.
  PreconnectRequest(
      const url::Origin& origin,
      int num_sockets,
      const net::NetworkAnonymizationKey& network_anonymization_key);

  url::Origin origin;
  // A zero-value means that we need to preresolve a host only.
  int num_sockets = 0;
  bool allow_credentials = true;
  net::NetworkAnonymizationKey network_anonymization_key;
};

// Stores a result of preconnect prediction. The |requests| vector is the main
// result of prediction and other fields are used for histograms reporting.
struct PreconnectPrediction {
  PreconnectPrediction();
  PreconnectPrediction(const PreconnectPrediction& other);
  ~PreconnectPrediction();

  bool is_redirected = false;
  std::string host;
  std::vector<PreconnectRequest> requests;
};

}  // namespace ohos_predictors

#endif  // CEF_LIBCEF_BROWSER_PREDICTORS_RESOURCE_PREFETCH_PREDICTOR_H_
