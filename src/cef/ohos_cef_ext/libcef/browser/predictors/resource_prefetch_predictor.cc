// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/resource_prefetch_predictor.h"

#include <map>
#include <set>
#include <utility>

#include "base/functional/bind.h"
#include "base/metrics/histogram_macros.h"
#include "base/rand_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "url/origin.h"

using content::BrowserThread;

namespace ohos_predictors {

PreconnectRequest::PreconnectRequest(
    const url::Origin& origin,
    int num_sockets,
    const net::NetworkAnonymizationKey& network_anonymization_key)
    : origin(origin),
      num_sockets(num_sockets),
      network_anonymization_key(network_anonymization_key) {
  DCHECK_GE(num_sockets, 0);
}

PreconnectPrediction::PreconnectPrediction() = default;
PreconnectPrediction::PreconnectPrediction(
    const PreconnectPrediction& prediction) = default;
PreconnectPrediction::~PreconnectPrediction() = default;

}  // namespace ohos_predictors
