// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstring>
#include <string>

#include "cef/include/cef_callback.h"
#include "cef/include/cef_request.h"
#include "cef/include/cef_resource_handler.h"
#include "cef/include/cef_response.h"
#include "cef/include/internal/cef_string.h"
#include "nweb_url_resource_response.h"
#include "ohos_nweb/src/cef_delegate/nweb_resource_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_url_resource_response_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace OHOS::NWeb {

class MockCefCallback : public CefCallback {
 public:
  MockCefCallback() = default;
  ~MockCefCallback() override = default;

  MOCK_METHOD0(Continue, void());
  MOCK_METHOD0(Cancel, void());

  IMPLEMENT_REFCOUNTING(MockCefCallback);
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size) {
  bool result = false;

  std::string strData = std::string(reinterpret_cast<const char*>(data), size);
  std::shared_ptr<NWebUrlResourceResponseImpl>
      nweb_url_resource_response_impl_ptr =
          std::make_shared<NWebUrlResourceResponseImpl>();
  NWebResourceHandler nweb_resource_handler =
      NWebResourceHandler(nweb_url_resource_response_impl_ptr, strData);
  nweb_url_resource_response_impl_ptr->PutResponseDataStatus(false);

  CefRefPtr<CefRequest> cef_request_ptr = CefRequest::Create();
  bool handle_request = false;
  nweb_resource_handler.Open(cef_request_ptr, handle_request,
                             new MockCefCallback());

  char buffer[100];
  int bytes_read = 0;
  nweb_resource_handler.Read(buffer, sizeof(buffer), bytes_read, nullptr);

  int64 response_length = 0;
  CefString cef_string = "";
  nweb_resource_handler.GetResponseHeaders(CefResponse::Create(),
                                           response_length, cef_string);

  return result;
}
}  // namespace OHOS::NWeb

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  /* Run your code on data */
  OHOS::NWeb::DoSomethingInterestingWithMyAPI(data, size);
  return 0;
}
 