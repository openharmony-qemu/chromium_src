/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "ohos_nweb/src/cef_delegate/nweb_cookie_manager_delegate.h"

#include <string>

#include "nweb_value_callback.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::NiceMock;

namespace OHOS::NWeb {

class MockNWebStringValueCallback : public NWebStringValueCallback {
 public:
  MockNWebStringValueCallback() = default;
  ~MockNWebStringValueCallback() override = default;

  MOCK_METHOD1(OnReceiveValue, void(const std::string& value));
};

class MockNWebLongValueCallback : public NWebLongValueCallback {
 public:
  MockNWebLongValueCallback() = default;
  ~MockNWebLongValueCallback() override = default;

  MOCK_METHOD1(OnReceiveValue, void(long value));
};

class MockNWebBoolValueCallback : public NWebBoolValueCallback {
 public:
  MockNWebBoolValueCallback() = default;
  ~MockNWebBoolValueCallback() override = default;

  MOCK_METHOD1(OnReceiveValue, void(bool value));
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size) {
  bool result = false;

  std::string strData = std::string(reinterpret_cast<const char*>(data), size);
  std::shared_ptr<NiceMock<MockNWebStringValueCallback>>
      nweb_string_value_callback =
          std::make_shared<NiceMock<MockNWebStringValueCallback>>();
  NWebCookieManagerDelegate nweb_cookie_manager_delegate;

  nweb_cookie_manager_delegate.ReturnCookie(strData,
                                            nweb_string_value_callback);

  bool is_valid = false;
  nweb_cookie_manager_delegate.ReturnCookie(strData, is_valid, true);

  nweb_cookie_manager_delegate.ReturnCookieWithHttpOnly(strData, is_valid, true,
                                                        true);

  std::shared_ptr<NiceMock<MockNWebLongValueCallback>>
      nweb_long_value_callback =
          std::make_shared<NiceMock<MockNWebLongValueCallback>>();
  nweb_cookie_manager_delegate.ConfigCookie(strData, strData,
                                            nweb_long_value_callback);

  std::shared_ptr<NiceMock<MockNWebBoolValueCallback>>
      nweb_bool_value_callback =
          std::make_shared<NiceMock<MockNWebBoolValueCallback>>();
  nweb_cookie_manager_delegate.SetCookie(strData, strData,
                                         nweb_bool_value_callback);

  nweb_cookie_manager_delegate.SetCookie(strData, strData, true);

  nweb_cookie_manager_delegate.SetCookieWithHttpOnly(strData, strData, true,
                                                     true);

  return result;
}
}  // namespace OHOS::NWeb

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  /* Run your code on data */
  OHOS::NWeb::DoSomethingInterestingWithMyAPI(data, size);
  return 0;
}
 