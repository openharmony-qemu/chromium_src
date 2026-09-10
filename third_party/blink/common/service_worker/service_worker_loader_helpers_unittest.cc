// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/public/common/service_worker/service_worker_loader_helpers.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/mojom/service_worker/dispatch_fetch_event_params.mojom.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_response.mojom.h"

namespace blink {
namespace {

using network::mojom::FetchAPIResponse;
using network::mojom::URLResponseHead;

TEST(ServiceWorkerLoaderHelpersTest, SaveResponseInfo_TimingAllowPassedReset) {
  auto response = mojom::FetchAPIResponse::New();
  response->status_code = 200;
  response->status_text = "OK";
  response->response_type = network::mojom::FetchResponseType::kOpaque;

  auto head = network::mojom::URLResponseHead::New();
  head->timing_allow_passed = true;
  ServiceWorkerLoaderHelpers::SaveResponseInfo(*response, head.get());

  EXPECT_FALSE(head->timing_allow_passed);
}

}  // namespace
}  // namespace blink
