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
#ifndef OHOS_NWEB_SRC_NDK_SCHEME_HANDLER_MOCK_OBJECTS_H
#define OHOS_NWEB_SRC_NDK_SCHEME_HANDLER_MOCK_OBJECTS_H

#include "cef/include/cef_request.h"
#include "cef/include/cef_response.h"
#include "libcef/common/arkweb_request_impl_ext.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"
#include "base/logging.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockPostDataStream : public ArkWebCefPostDataStream {
 public:
  MockPostDataStream() : init_called_(false) {}
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return true; }
  bool HasAtLeastOneRef() const override { return true; }
  void SetReadCallback(CefRefPtr<ArkWebCefPostDataStreamReadCallback> callback) override {}
  void Init(CefRefPtr<ArkWebCefPostDataStreamInitCallback> callback) override {
    init_called_ = true;
    stored_callback_ = callback;
  }
  void Read(void* buffer, int buf_len, CefRefPtr<ArkWebCefPostDataStreamReadCallback> callback) override {}
  void AsyncRead(void* buffer, int buf_len, CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> callback) override {}
  void Reset() override {}
  uint64_t GetSize() override { return size_; }
  uint64_t GetPosition() override { return position_; }
  bool IsChunked() override { return is_chunked_; }
  bool IsEOF() override { return is_eof_; }
  bool IsInMemory() override { return is_in_memory_; }
  bool HasNullSource() override { return false; }
  void GetChunkedDataPipeGetter(network::ResourceRequestBody* body) override {}
  void SetSize(uint64_t size) { size_ = size; }
  void SetPosition(uint64_t position) { position_ = position; }
  void SetChunked(bool chunked) { is_chunked_ = chunked; }
  void SetEOF(bool eof) { is_eof_ = eof; }
  void SetInMemory(bool in_memory) { is_in_memory_ = in_memory; }
  bool WasInitCalled() const { return init_called_; }
  CefRefPtr<ArkWebCefPostDataStreamInitCallback> GetStoredCallback() const { return stored_callback_; }
 private:
  uint64_t size_ = 0;
  uint64_t position_ = 0;
  bool is_chunked_ = false;
  bool is_eof_ = false;
  bool is_in_memory_ = true;
  bool init_called_;
  CefRefPtr<ArkWebCefPostDataStreamInitCallback> stored_callback_;
};

class MockArkWebRequestImplExt : public ArkWebRequestImplExt {
 public:
  explicit MockArkWebRequestImplExt(ArkWebCefPostDataStream* stream) 
      : stream_(stream), 
        is_main_frame_(true), 
        has_user_gesture_(false),
        frame_url_("") {}

  CefRefPtr<ArkWebRequestImplExt> AsArkWebRequestExt() override {
    return this;
  }
  CefRefPtr<ArkWebCefPostDataStream> GetUploadStream() override {
    return stream_;
  }
  void SetMainFrame(bool is_main_frame) {
    is_main_frame_ = is_main_frame;
  }
  void SetUserGesture(bool has_user_gesture) {
    has_user_gesture_ = has_user_gesture;
  }
  CefString GetFrameUrl() override {
    return frame_url_;
  }
  void SetFrameUrlString(const std::string& frame_url) {
    frame_url_ = frame_url;
  }
  TransitionType GetTransitionType() override { return TT_LINK; }
  CefString GetURL() override { return CefString(); }
  void SetURL(const CefString& url) override {}
  CefString GetMethod() override { return CefString(); }
  void SetMethod(const CefString& method) override {}
  void SetReferrer(const CefString& referrer_url, CefRequest::ReferrerPolicy policy) override {}
  CefString GetReferrerURL() override { return CefString(); }
  CefRequest::ReferrerPolicy GetReferrerPolicy() override { return REFERRER_POLICY_DEFAULT; }
  CefRefPtr<CefPostData> GetPostData() override { return nullptr; }
  void SetPostData(CefRefPtr<CefPostData> postData) override {}
  void GetHeaderMap(CefRequest::HeaderMap& headerMap) override { headerMap = CefRequest::HeaderMap(); }
  void SetHeaderMap(const CefRequest::HeaderMap& headerMap) override {}
  CefString GetHeaderByName(const CefString& name) override { return CefString(); }
  void SetHeaderByName(const CefString& name, const CefString& value, bool overwrite) override {}
  CefString GetFirstPartyForCookies() override { return CefString(); }
  void SetFirstPartyForCookies(const CefString& url) override {}
  ResourceType GetResourceType() override { return RT_MAIN_FRAME; }
  bool IsReadOnly() override { return false; }
  void Set(const net::HttpRequestHeaders& headers) override {}
  void Set(const net::RedirectInfo& redirect_info) override {}
  void Set(const network::ResourceRequest* request,
           uint64_t navigation_start_microseconds) override {}
  void Set(const CefString& url,
           const CefString& method,
           CefRefPtr<CefPostData> postData,
           const CefRequest::HeaderMap& headerMap) override {}
  int GetFlags() override { return 0; }
  void SetFlags(int flags) override {}
  bool IsMainFrame() override { return is_main_frame_; }
  bool HasUserGesture() override { return has_user_gesture_; }
  
 private:
  ArkWebCefPostDataStream* stream_;
  bool is_main_frame_;
  bool has_user_gesture_;
  std::string frame_url_;
};

class MockCefRequest : public CefRequest {
 public:
  explicit MockCefRequest(ArkWebCefPostDataStream* stream = nullptr) 
      : mock_request_ext_(new MockArkWebRequestImplExt(stream)), 
        method_("GET"), 
        referrer_(""),
        frame_url_(""),
        resource_type_(RT_MAIN_FRAME) {} 
  
  ~MockCefRequest() {
    if (mock_request_ext_) {
      mock_request_ext_->Release();
    }
  }
  void SetMethodString(const std::string& method) {
    method_ = method;
  }
  void SetReferrerString(const std::string& referrer) {
    referrer_ = referrer;
  }
  void SetFrameUrlString(const std::string& frame_url) {
    frame_url_ = frame_url;
  }
  void SetResourceType(ResourceType type) {
    resource_type_ = type;
  }
  bool IsReadOnly() override { return false; }
  CefString GetURL() override { return CefString(); }
  void SetURL(const CefString& url) override {}
  CefString GetMethod() override { return method_; }
  void SetMethod(const CefString& method) override { method_ = method.ToString(); }
  void SetReferrer(const CefString& referrer_url, ReferrerPolicy policy) override { 
    referrer_ = referrer_url.ToString(); 
  }
  CefString GetReferrerURL() override { return referrer_; }
  ReferrerPolicy GetReferrerPolicy() override { return REFERRER_POLICY_DEFAULT; }
  CefRefPtr<CefPostData> GetPostData() override { return nullptr; }
  void SetPostData(CefRefPtr<CefPostData> postData) override {}
  void GetHeaderMap(HeaderMap& headerMap) override { headerMap = headers_; }
  void SetHeaderMap(const HeaderMap& headerMap) override { headers_ = headerMap; }
  CefString GetHeaderByName(const CefString& name) override { 
    auto it = headers_.find(name.ToString());
    return (it != headers_.end()) ? it->second : CefString();
  }
  void SetHeaderByName(const CefString& name, const CefString& value, bool overwrite) override {
    if (overwrite || headers_.find(name.ToString()) == headers_.end()) {
      headers_.insert(std::make_pair(name.ToString(), value.ToString()));
    }
  }
  void Set(const CefString& url, const CefString& method, CefRefPtr<CefPostData> postData, const HeaderMap& headerMap) override {
    headers_ = headerMap;
  }
  int GetFlags() override { return 0; }
  void SetFlags(int flags) override {}
  CefString GetFirstPartyForCookies() override { return CefString(); }
  void SetFirstPartyForCookies(const CefString& url) override {}
  ResourceType GetResourceType() override { return resource_type_; }
  TransitionType GetTransitionType() override { return TT_LINK; }
  uint64_t GetIdentifier() override { return 0; }
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return true; }
  bool HasAtLeastOneRef() const override { return true; }
  CefRefPtr<ArkWebRequestImplExt> AsArkWebRequestExt() override {
    return mock_request_ext_;
  }
 private:
  CefRefPtr<MockArkWebRequestImplExt> mock_request_ext_;
  std::string method_;
  std::string referrer_;
  std::string frame_url_;
  ResourceType resource_type_;
  CefRequest::HeaderMap headers_;
};

class StrcpyMock {
  public:
    static StrcpyMock& GetInstance() {
      static StrcpyMock instance;
      return instance;
    }
    MOCK_METHOD(int, strcpy_s, (char* dest, size_t destMax, const char* src), ());

    static bool strcpyMock;
};

#ifdef __cplusplus
extern "C" {
#endif
  int __wrap_strcpy_s(char* dest, size_t destMax, const char* src);

  int __real_strcpy_s(char* dest, size_t destMax, const char* src);
#ifdef __cplusplus
}
#endif
#endif // OHOS_NWEB_SRC_NDK_SCHEME_HANDLER_MOCK_OBJECTS_H