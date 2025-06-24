/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NWEB_COOKIE_MANAGER_IMP_MOCKS_H
#define NWEB_COOKIE_MANAGER_IMP_MOCKS_H

#include "nweb_value_callback.h"
#include "cef_delegate/nweb_default_engine_init_args_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"


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
}  // namespace OHOS::NWeb

#endif  // NWEB_COOKIE_MANAGER_IMP_MOCKS_H