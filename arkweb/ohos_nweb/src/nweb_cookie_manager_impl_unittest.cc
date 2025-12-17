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
#include "ohos_nweb/src/cef_delegate/nweb_cookie_manager_delegate.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#define private public
#define protected public
#include "ohos_nweb/src/nweb_cookie_manager_impl.h"
#undef private
#undef protected

using namespace OHOS::NWeb;
using namespace testing;

class NWebCookieManagerImplTestAccessor {
 public:
  static void SetDelegate(std::shared_ptr<NWebCookieManagerImpl> manager, 
                          std::shared_ptr<NWebCookieManagerDelegate> delegate) {
    manager->delegate_ = delegate;
  }
  static std::shared_ptr<void> GetDelegate(std::shared_ptr<NWebCookieManagerImpl> manager) {
    return std::static_pointer_cast<void>(manager->delegate_);
  }
};

class TestNWebLongValueCallback : public NWebLongValueCallback {
 public:
  void OnReceiveValue(long value) override {}
};

class TestNWebBoolValueCallback : public NWebBoolValueCallback {
 public:
  void OnReceiveValue(bool value) override {}
};

class TestNWebStringValueCallback : public NWebStringValueCallback {
 public:
  void OnReceiveValue(const std::string& value) override {}
};

class MockNWebCookieManagerDelegate : public NWebCookieManagerDelegate {
 public:
  MOCK_METHOD(bool, IsAcceptCookieAllowed, (), (override));
  MOCK_METHOD(void, PutAcceptCookieEnabled, (bool accept), (override));
  MOCK_METHOD(bool, IsThirdPartyCookieAllowed, (), (override));
  MOCK_METHOD(void, PutAcceptThirdPartyCookieEnabled, (bool accept), (override));
  MOCK_METHOD(bool, IsFileURLSchemeCookiesAllowed, (), (override));
  MOCK_METHOD(void, PutAcceptFileURLSchemeCookiesEnabled, (bool allow), (override));
  MOCK_METHOD(void, ConfigCookie, (const std::string& url, const std::string& value, bool incognito_mode,
              bool includeHttpOnly, std::shared_ptr<NWebLongValueCallback> callback), (override));
  MOCK_METHOD(void, SetCookie, (const std::string& url, const std::string& value,
              std::shared_ptr<NWebBoolValueCallback> callback), (override));
  MOCK_METHOD(int, SetCookie, (const std::string& url, const std::string& value, bool incognito_mode,
              bool includeHttpOnly), (override));
  MOCK_METHOD(void, ReturnCookie, (const std::string& url, bool incognito_mode,
              std::shared_ptr<NWebStringValueCallback> callback), (override));
  MOCK_METHOD(std::string, ReturnCookie, (const std::string& url, bool& is_valid, bool incognito_mode), (override));
  MOCK_METHOD(std::string, ReturnCookieWithHttpOnly, (const std::string& url, bool& is_valid,
              bool incognito_mode, bool includeHttpOnly), (override));
  MOCK_METHOD(void, ExistCookies, (std::shared_ptr<NWebBoolValueCallback> callback), (override));
  MOCK_METHOD(bool, ExistCookies, (bool incognito_mode), (override));
  MOCK_METHOD(void, Store, (std::shared_ptr<NWebBoolValueCallback> callback), (override));
  MOCK_METHOD(bool, Store, (), (override));
  MOCK_METHOD(void, DeleteSessionCookies, (std::shared_ptr<NWebBoolValueCallback> callback), (override));
  MOCK_METHOD(void, DeleteCookieEntirely, (std::shared_ptr<NWebBoolValueCallback> callback,
              bool incognito_mode), (override));
  MOCK_METHOD(int, SetCookieWithHttpOnly, (const std::string& url, const std::string& value,
              bool incognito_mode, bool includeHttpOnly), (override));
};

class NWebCookieManagerImplTest : public Test {
protected:
  void SetUp() override {
    cookie_manager_ = std::make_shared<NWebCookieManagerImpl>();
  }
  void TearDown() override {
    if (cookie_manager_ && cookie_manager_->delegate_) {
      cookie_manager_->delegate_.reset();
    }
    cookie_manager_.reset();
  }
  std::shared_ptr<NWebCookieManagerImpl> cookie_manager_;
};

TEST_F(NWebCookieManagerImplTest, IsAcceptCookieAllowed_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_FALSE(cookie_manager_->IsAcceptCookieAllowed());
}

TEST_F(NWebCookieManagerImplTest, IsAcceptCookieAllowed_DelegateReturnsTrue) {
  ASSERT_NE(nullptr, cookie_manager_);
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, IsAcceptCookieAllowed()).WillOnce(Return(true));
  EXPECT_TRUE(cookie_manager_->IsAcceptCookieAllowed());
}

TEST_F(NWebCookieManagerImplTest, PutAcceptCookieEnabled_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  cookie_manager_->PutAcceptCookieEnabled(true);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, PutAcceptCookieEnabled_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, PutAcceptCookieEnabled(true)).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->PutAcceptCookieEnabled(true));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, IsThirdPartyCookieAllowed_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_FALSE(cookie_manager_->IsThirdPartyCookieAllowed());
}

TEST_F(NWebCookieManagerImplTest, IsThirdPartyCookieAllowed_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, IsThirdPartyCookieAllowed()).WillOnce(Return(true));
  EXPECT_TRUE(cookie_manager_->IsThirdPartyCookieAllowed());
}

TEST_F(NWebCookieManagerImplTest, PutAcceptThirdPartyCookieEnabled_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  cookie_manager_->PutAcceptThirdPartyCookieEnabled(true);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, PutAcceptThirdPartyCookieEnabled_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, PutAcceptThirdPartyCookieEnabled(true)).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->PutAcceptThirdPartyCookieEnabled(true));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, IsFileURLSchemeCookiesAllowed_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_FALSE(cookie_manager_->IsFileURLSchemeCookiesAllowed());
}

TEST_F(NWebCookieManagerImplTest, IsFileURLSchemeCookiesAllowed_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, IsFileURLSchemeCookiesAllowed()).WillOnce(Return(true));
  EXPECT_TRUE(cookie_manager_->IsFileURLSchemeCookiesAllowed());
}

TEST_F(NWebCookieManagerImplTest, PutAcceptFileURLSchemeCookiesEnabled_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  cookie_manager_->PutAcceptFileURLSchemeCookiesEnabled(true);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, PutAcceptFileURLSchemeCookiesEnabled_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, PutAcceptFileURLSchemeCookiesEnabled(true)).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->PutAcceptFileURLSchemeCookiesEnabled(true));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ConfigCookie_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebLongValueCallback>();
  cookie_manager_->ConfigCookie("http://example.com", "cookie=value", callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ConfigCookie_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebLongValueCallback>();
  EXPECT_CALL(*mock_delegate, 
              ConfigCookie(::testing::StrEq("http://example.com"), 
                           ::testing::StrEq("cookie=value"), 
                           false, 
                           false, 
                           ::testing::Eq(std::static_pointer_cast<NWebLongValueCallback>(callback)))).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->ConfigCookie("http://example.com", "cookie=value", callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, SetCookie_WithCallback_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  cookie_manager_->SetCookie("http://example.com", "cookie=value", callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, SetCookie_WithCallback_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  EXPECT_CALL(*mock_delegate, 
              SetCookie(::testing::StrEq("http://example.com"), 
                        ::testing::StrEq("cookie=value"), 
                        ::testing::Eq(std::static_pointer_cast<NWebBoolValueCallback>(callback)))).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->SetCookie("http://example.com", "cookie=value", callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, SetCookie_WithIncognitoMode_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookie("http://example.com", "cookie=value", true));
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookie("http://example.com", "cookie=value", false));
}

TEST_F(NWebCookieManagerImplTest, SetCookie_WithIncognitoMode_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, SetCookie(_, _, _, _)).WillOnce(Return(NWEB_OK));
  EXPECT_EQ(NWEB_OK, cookie_manager_->SetCookie("http://example.com", "cookie=value", true));
}

TEST_F(NWebCookieManagerImplTest, SetCookieWithHttpOnly_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookieWithHttpOnly("http://example.com", "cookie=value", true, true));
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookieWithHttpOnly("http://example.com", "cookie=value", false, false));
}

TEST_F(NWebCookieManagerImplTest, ReturnCookie_WithCallback_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebStringValueCallback>();
  cookie_manager_->ReturnCookie("http://example.com", callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ReturnCookie_WithCallback_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebStringValueCallback>();
  EXPECT_CALL(*mock_delegate, ReturnCookie(::testing::StrEq("http://example.com"), false,
              ::testing::Eq(std::static_pointer_cast<NWebStringValueCallback>(callback)))).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->ReturnCookie("http://example.com", callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ReturnCookie_WithIncognitoMode_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  bool is_valid = true;
  std::string result1 = cookie_manager_->ReturnCookie("http://example.com", is_valid, true);
  EXPECT_EQ("", result1);
  std::string result2 = cookie_manager_->ReturnCookie("http://example.com", is_valid, false);
  EXPECT_EQ("", result2);
}

TEST_F(NWebCookieManagerImplTest, ReturnCookie_WithIncognitoMode_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  bool is_valid = false;
  EXPECT_CALL(*mock_delegate, ReturnCookie(::testing::Matcher<const std::string&>(_), ::testing::Matcher<bool&>(_),
              ::testing::Matcher<bool>(_))).WillOnce(DoAll(SetArgReferee<1>(true), Return("cookie1=value1")));
  EXPECT_EQ("cookie1=value1", cookie_manager_->ReturnCookie("http://example.com", is_valid, true));
  EXPECT_TRUE(is_valid);
}

TEST_F(NWebCookieManagerImplTest, ReturnCookieWithHttpOnly_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  bool is_valid = true;
  std::string result1 = cookie_manager_->ReturnCookieWithHttpOnly("http://example.com", is_valid, true, true);
  EXPECT_EQ("", result1);
  std::string result2 = cookie_manager_->ReturnCookieWithHttpOnly("http://example.com", is_valid, false, false);
  EXPECT_EQ("", result2);
}

TEST_F(NWebCookieManagerImplTest, ExistCookies_WithCallback_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  cookie_manager_->ExistCookies(callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ExistCookies_WithCallback_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);

  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  EXPECT_CALL(*mock_delegate, ExistCookies(::testing::Matcher<std::shared_ptr<NWebBoolValueCallback>>(_))).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->ExistCookies(callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, ExistCookies_WithIncognitoMode_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_FALSE(cookie_manager_->ExistCookies(true));
  EXPECT_FALSE(cookie_manager_->ExistCookies(false));
}

TEST_F(NWebCookieManagerImplTest, ExistCookies_WithIncognitoMode_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, ExistCookies(::testing::Matcher<bool>(_))).WillOnce(Return(true));
  EXPECT_TRUE(cookie_manager_->ExistCookies(true));
}

TEST_F(NWebCookieManagerImplTest, Store_WithCallback_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  cookie_manager_->Store(callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, Store_WithCallback_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  EXPECT_CALL(*mock_delegate, Store(::testing::Eq(std::static_pointer_cast<NWebBoolValueCallback>(callback))))
              .Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->Store(callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, Store_WithoutParams_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_FALSE(cookie_manager_->Store());
}

TEST_F(NWebCookieManagerImplTest, Store_WithoutParams_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, Store()).WillOnce(Return(true));
  EXPECT_TRUE(cookie_manager_->Store());
}

TEST_F(NWebCookieManagerImplTest, DeleteSessionCookies_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  cookie_manager_->DeleteSessionCookies(callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, DeleteSessionCookies_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  EXPECT_CALL(*mock_delegate,
              DeleteSessionCookies(::testing::Eq(std::static_pointer_cast<NWebBoolValueCallback>(callback))))
              .Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->DeleteSessionCookies(callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, DeleteCookieEntirely_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  cookie_manager_->DeleteCookieEntirely(callback, true);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, DeleteCookieEntirely_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebBoolValueCallback>();
  EXPECT_CALL(*mock_delegate, DeleteCookieEntirely(::testing::_, true)).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->DeleteCookieEntirely(callback, true));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, GetCookieAsync_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebStringValueCallback>();
  cookie_manager_->GetCookieAsync("http://example.com", true, callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, GetCookieAsync_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebStringValueCallback>();
  EXPECT_CALL(*mock_delegate, ReturnCookie(::testing::StrEq("http://example.com"), true,
              ::testing::Eq(std::static_pointer_cast<NWebStringValueCallback>(callback)))).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->GetCookieAsync("http://example.com", true, callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, SetCookieSync_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookieSync("http://example.com", "cookie=value", true, true));
  EXPECT_EQ(NWEB_ERR, cookie_manager_->SetCookieSync("http://example.com", "cookie=value", false, false));
}

TEST_F(NWebCookieManagerImplTest, SetCookieSync_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  EXPECT_CALL(*mock_delegate, SetCookie(_, _, _, _)).WillOnce(Return(NWEB_OK));
  EXPECT_EQ(NWEB_OK, cookie_manager_->SetCookieSync("http://example.com", "cookie=value", true, true));
}

TEST_F(NWebCookieManagerImplTest, SetCookieAsync_DelegateIsNull) {
  cookie_manager_->delegate_.reset();
  auto callback = std::make_shared<TestNWebLongValueCallback>();
  cookie_manager_->SetCookieAsync("http://example.com", "cookie=value", true, true, callback);
  cookie_manager_->SetCookieAsync("http://example.com", "cookie=value", false, false, callback);
  EXPECT_EQ(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}

TEST_F(NWebCookieManagerImplTest, SetCookieAsync_DelegateIsNotNull) {
  auto mock_delegate = std::make_shared<NiceMock<MockNWebCookieManagerDelegate>>();
  NWebCookieManagerImplTestAccessor::SetDelegate(cookie_manager_, mock_delegate);
  auto callback = std::make_shared<TestNWebLongValueCallback>();
  EXPECT_CALL(*mock_delegate, ConfigCookie(::testing::_, ::testing::_, true, true, ::testing::_)).Times(1);
  EXPECT_NO_FATAL_FAILURE(cookie_manager_->SetCookieAsync("http://example.com", "cookie=value", true, true, callback));
  EXPECT_NE(NWebCookieManagerImplTestAccessor::GetDelegate(cookie_manager_), nullptr);
}