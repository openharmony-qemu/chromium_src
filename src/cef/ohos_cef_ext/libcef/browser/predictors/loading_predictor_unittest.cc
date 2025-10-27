// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "content/public/test/test_browser_context.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_cef_ext/libcef/browser/predictors/preconnect_manager.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"
#include "url/gurl.h"
#include "url/origin.h"
#define private public
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#undef private

namespace network {
class MockSimpleURLLoader : public SimpleURLLoader {
 public:
  MOCK_METHOD(void,
              DownloadToString,
              (mojom::URLLoaderFactory * url_loader_factory,
               BodyAsStringCallbackDeprecated body_as_string_callback,
               size_t max_body_size),
              (override));
  MOCK_METHOD(void,
              DownloadToString,
              (mojom::URLLoaderFactory * url_loader_factory,
               BodyAsStringCallback body_as_string_callback,
               size_t max_body_size),
              (override));
  MOCK_METHOD(void,
              DownloadToStringOfUnboundedSizeUntilCrashAndDie,
              (mojom::URLLoaderFactory * url_loader_factory,
               BodyAsStringCallbackDeprecated body_as_string_callback),
              (override));
  MOCK_METHOD(void,
              DownloadToStringOfUnboundedSizeUntilCrashAndDie,
              (mojom::URLLoaderFactory * url_loader_factory,
               BodyAsStringCallback body_as_string_callback),
              (override));
  MOCK_METHOD(void,
              DownloadHeadersOnly,
              (mojom::URLLoaderFactory * url_loader_factory,
               HeadersOnlyCallback headers_only_callback),
              (override));
  MOCK_METHOD(
      void,
      DownloadToFile,
      (mojom::URLLoaderFactory * url_loader_factory,
       DownloadToFileCompleteCallback download_to_file_complete_callback,
       const base::FilePath& file_path,
       int64_t max_body_size),
      (override));
  MOCK_METHOD(
      void,
      DownloadToTempFile,
      (mojom::URLLoaderFactory * url_loader_factory,
       DownloadToFileCompleteCallback download_to_file_complete_callback,
       int64_t max_body_size),
      (override));
  MOCK_METHOD(void,
              DownloadAsStream,
              (mojom::URLLoaderFactory * url_loader_factory,
               SimpleURLLoaderStreamConsumer* stream_consumer),
              (override));
  MOCK_METHOD(void,
              SetOnRedirectCallback,
              (const SimpleURLLoader::OnRedirectCallback& on_redirect_callback),
              (override));
  MOCK_METHOD(
      void,
      SetOnResponseStartedCallback,
      (SimpleURLLoader::OnResponseStartedCallback on_response_started_callback),
      (override));
  MOCK_METHOD(
      void,
      SetOnUploadProgressCallback,
      (SimpleURLLoader::UploadProgressCallback on_upload_progress_callback),
      (override));
  MOCK_METHOD(
      void,
      SetOnDownloadProgressCallback,
      (SimpleURLLoader::DownloadProgressCallback on_download_progress_callback),
      (override));
  MOCK_METHOD(void,
              SetAllowPartialResults,
              (bool allow_partial_results),
              (override));
  MOCK_METHOD(void,
              SetAllowHttpErrorResults,
              (bool allow_http_error_results),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (std::string_view upload_data,
               std::string_view upload_content_type),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (std::string_view upload_data),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (const char* upload_data, std::string_view upload_content_type),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (const char* upload_data),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (std::string && upload_data,
               std::string_view upload_content_type),
              (override));
  MOCK_METHOD(void,
              AttachStringForUpload,
              (std::string && upload_data),
              (override));
  MOCK_METHOD(void,
              AttachFileForUpload,
              (const base::FilePath& upload_file_path,
               const std::string& upload_content_type,
               uint64_t offset,
               uint64_t length),
              (override));
  MOCK_METHOD(void,
              AttachFileForUpload,
              (const base::FilePath& upload_file_path,
               uint64_t offset,
               uint64_t length),
              (override));
  MOCK_METHOD(void,
              SetRetryOptions,
              (int max_retries, int retry_mode),
              (override));
  MOCK_METHOD(void, SetURLLoaderFactoryOptions, (uint32_t options), (override));
  MOCK_METHOD(void, SetRequestID, (int32_t request_id), (override));
  MOCK_METHOD(void,
              SetTimeoutDuration,
              (base::TimeDelta timeout_duration),
              (override));
  MOCK_METHOD(int, NetError, (), (const, override));
  MOCK_METHOD(mojom::URLResponseHead*, ResponseInfo, (), (const, override));
  MOCK_METHOD(mojom::URLResponseHeadPtr, TakeResponseInfo, (), (override));
  MOCK_METHOD(const std::optional<URLLoaderCompletionStatus>&,
              CompletionStatus,
              (),
              (const, override));
  MOCK_METHOD(const GURL&, GetFinalURL, (), (const, override));
  MOCK_METHOD(bool, LoadedFromCache, (), (const, override));
  MOCK_METHOD(int64_t, GetContentSize, (), (const, override));
  MOCK_METHOD(int, GetNumRetries, (), (const, override));
  MOCK_METHOD(base::WeakPtr<const SimpleURLLoader>,
              GetWeakPtr,
              (),
              (const, override));
};
}  // namespace network

namespace ohos_predictors {
class MockPreconnectManager : public PreconnectManager {
 public:
  MockPreconnectManager(base::WeakPtr<Delegate> delegate,
                        content::BrowserContext* context)
      : PreconnectManager(delegate, context) {}
  MOCK_METHOD(void,
              Start,
              (const GURL& url, std::vector<PreconnectRequest> requests),
              (override));
  MOCK_METHOD(void,
              StartPreresolveHost,
              (const GURL& url,
               const net::NetworkAnonymizationKey& network_anonymization_key),
              (override));
  MOCK_METHOD(void,
              StartPreresolveHosts,
              (const std::vector<std::string>& hostnames,
               const net::NetworkAnonymizationKey& network_anonymization_key),
              (override));
  MOCK_METHOD(void,
              StartPreconnectUrl,
              (const GURL& url,
               bool allow_credentials,
               net::NetworkAnonymizationKey network_anonymization_key,
               int num_sockets),
              (override));
  MOCK_METHOD(void, Stop, (const GURL& url), (override));
};

class MockDelegate : public PreconnectManager::Delegate {
 public:
  MockDelegate() : weak_factory_(this) {}
  base::WeakPtr<Delegate> GetWeakPtr() { return weak_factory_.GetWeakPtr(); }
  MOCK_METHOD(void,
              PreconnectFinished,
              (std::unique_ptr<PreconnectStats> stats),
              (override));

 private:
  base::WeakPtrFactory<Delegate> weak_factory_;
};

class HwLoadingPredictorTest : public testing::Test {
 protected:
  void SetUp() {
    browser_context_ = new content::TestBrowserContext(base::FilePath());
    predictor_ = std::make_unique<LoadingPredictor>(LoadingPredictorConfig(),
                                                    browser_context_);
    mock_delegate_ = std::make_unique<MockDelegate>();
    mock_preconnect_manager_ =
        std::make_unique<testing::NiceMock<MockPreconnectManager>>(
            mock_delegate_->GetWeakPtr(), browser_context_);
    mock_preconnect_manager_1_ = mock_preconnect_manager_.get();
    net::NetworkTrafficAnnotationTag network_traffic_annotation_tag =
        net::DefineNetworkTrafficAnnotation("loading_predictor", R"(
            semantics {
              sender: "Loading Predictor"
              description: "Prefetches resources for faster page loading."
              trigger: "Heuristic prediction of user navigation."
              data: "None"
              destination: WEBSITE })");
    predictor_->loader_map_.emplace(
        123, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        234, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        345, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        456, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        567, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        678, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        789, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
    predictor_->loader_map_.emplace(
        999, network::SimpleURLLoader::Create(
                 std::make_unique<network::ResourceRequest>(),
                 network_traffic_annotation_tag));
  }
  void TearDown() {
    mock_preconnect_manager_1_ = nullptr;
    mock_preconnect_manager_.reset();
    mock_delegate_.reset();
    predictor_->Shutdown();
    predictor_.reset();
    browser_context_ = nullptr;
  }

  content::BrowserContext* browser_context_ = nullptr;
  std::unique_ptr<LoadingPredictor> predictor_ = nullptr;
  std::unique_ptr<MockDelegate> mock_delegate_ = nullptr;
  std::unique_ptr<testing::NiceMock<MockPreconnectManager>>
      mock_preconnect_manager_ = nullptr;
  testing::NiceMock<MockPreconnectManager>* mock_preconnect_manager_1_ =
      nullptr;
  std::vector<std::string> keys_ = {
      "test_cache_key",   "test_cache_key_1", "test_cache_key_2",
      "test_cache_key_3", "test_cache_key_4", "test_cache_key_5",
      "test_cache_key_6", "test_cache_key_7", "test_cache_key_8"};
};

TEST_F(HwLoadingPredictorTest,
       OnSimpleURLLoaderComplete_ClearPrefetchedResource_001) {
  network::MockSimpleURLLoader mock_loader;
  network::mojom::URLResponseHead mock_response;
  mock_response.response_time = base::Time::Now();
  mock_response.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 404 OK");
  EXPECT_CALL(mock_loader, ResponseInfo())
      .WillRepeatedly(testing::Return(&mock_response));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_1", 123, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_TRUE(predictor_->loader_map_.count(123));
  mock_response.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_1", 123, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(123));
  mock_response.headers = nullptr;
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_2", 234, 0,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(234));
  predictor_->OnSimpleURLLoaderComplete(&mock_loader, "test_cache_key_3", 345,
                                        3600, nullptr);
  EXPECT_FALSE(predictor_->loader_map_.count(345));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_3", 345, 3000,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(345));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_4", 456, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(456));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_5", 567, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(567));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_6", 678, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(678));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_7", 789, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(789));
  const std::vector<std::string>& cache_key_list = keys_;
  predictor_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(HwLoadingPredictorTest,
       OnSimpleURLLoaderComplete_ClearPrefetchedResource_002) {
  network::MockSimpleURLLoader mock_loader;
  EXPECT_CALL(mock_loader, ResponseInfo())
      .WillRepeatedly(testing::Return(nullptr));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key", 999, 3600,
      std::make_unique<std::string>("response content"));
  EXPECT_FALSE(predictor_->loader_map_.count(999));
  const std::vector<std::string>& cache_key_list = keys_;
  predictor_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(HwLoadingPredictorTest, GetResourceHandler_001) {
  network::MockSimpleURLLoader mock_loader;
  network::mojom::URLResponseHead mock_response;
  mock_response.response_time = base::Time::Now();
  mock_response.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
  EXPECT_CALL(mock_loader, ResponseInfo())
      .WillRepeatedly(testing::Return(&mock_response));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_1", 123, 3600,
      std::make_unique<std::string>("response content"));
  const GURL test_url("http://example.com");
  auto result = predictor_->GetResourceHandler(test_url, "test_cache_key_2");
  EXPECT_EQ(result, nullptr);
  auto predictor_resource_handler =
      predictor_->GetResourceHandler(test_url, "test_cache_key_1");
  EXPECT_NE(predictor_resource_handler, nullptr);
  const std::vector<std::string>& cache_key_list = keys_;
  predictor_->ClearPrefetchedResource(cache_key_list);

  char data_out[100];
  int bytes_read = 0;
  auto res =
      predictor_resource_handler->Read(data_out, 10, bytes_read, nullptr);
  EXPECT_TRUE(res);
  EXPECT_EQ(10, bytes_read);
  res = predictor_resource_handler->Read(nullptr, 10, bytes_read, nullptr);
  EXPECT_FALSE(result);
  EXPECT_EQ(-2, bytes_read);
}

TEST_F(HwLoadingPredictorTest, GetResourceHandler_002) {
  network::MockSimpleURLLoader mock_loader;
  network::mojom::URLResponseHead mock_response;
  mock_response.response_time = base::Time::Now() + base::Seconds(1000);
  mock_response.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
  EXPECT_CALL(mock_loader, ResponseInfo())
      .WillRepeatedly(testing::Return(&mock_response));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_1", 123, 3600,
      std::make_unique<std::string>("response content"));
  const GURL test_url("http://example.com");
  auto result = predictor_->GetResourceHandler(test_url, "test_cache_key_1");
  EXPECT_EQ(result, nullptr);
  const std::vector<std::string>& cache_key_list = keys_;
  predictor_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(HwLoadingPredictorTest, GetResourceHandler_003) {
  network::MockSimpleURLLoader mock_loader;
  network::mojom::URLResponseHead mock_response;
  mock_response.response_time = base::Time::Now() - base::Seconds(4000);
  mock_response.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
  EXPECT_CALL(mock_loader, ResponseInfo())
      .WillRepeatedly(testing::Return(&mock_response));
  predictor_->OnSimpleURLLoaderComplete(
      &mock_loader, "test_cache_key_1", 123, 3600,
      std::make_unique<std::string>("response content"));
  const GURL test_url("http://example.com");
  auto result = predictor_->GetResourceHandler(test_url, "test_cache_key_1");
  EXPECT_EQ(result, nullptr);
  const std::vector<std::string>& cache_key_list = keys_;
  predictor_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(HwLoadingPredictorTest, GetResourceHandler_004) {
  const GURL test_url("http://example.com");
  auto result = predictor_->GetResourceHandler(test_url, "test_cache_key_1");
  EXPECT_EQ(result, nullptr);
  const std::vector<std::string>& cache_key_list = {};
  predictor_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(HwLoadingPredictorTest, PreconnectFinished) {
  GURL stats("https://example.com/resource");
  predictor_->active_hints_.emplace(GURL("https://example.com/resource"),
                                    base::TimeTicks::Now());
  predictor_->active_hints_.emplace(GURL("https://baidu.com/resource"),
                                    base::TimeTicks::Now());

  predictor_->Shutdown();
  predictor_->PreconnectFinished(std::make_unique<PreconnectStats>(stats));
  EXPECT_TRUE(
      predictor_->active_hints_.count(GURL("https://example.com/resource")));

  predictor_->StartInitialization();
  predictor_->PreconnectFinished(std::make_unique<PreconnectStats>(stats));
  EXPECT_FALSE(
      predictor_->active_hints_.count(GURL("https://example.com/resource")));
}

TEST_F(HwLoadingPredictorTest, CancelActiveHint) {
  predictor_->active_hints_.emplace(GURL("https://example.com/resource"),
                                    base::TimeTicks::Now());
  auto result = predictor_->CancelActiveHint(predictor_->active_hints_.end());
  EXPECT_EQ(result, predictor_->active_hints_.end());
  EXPECT_TRUE(
      predictor_->active_hints_.count(GURL("https://example.com/resource")));

  predictor_->CancelActiveHint(predictor_->active_hints_.begin());
  EXPECT_EQ(predictor_->active_hints_.size(), 0);
  EXPECT_FALSE(
      predictor_->active_hints_.count(GURL("https://example.com/resource")));
}

TEST_F(HwLoadingPredictorTest, OnNavigationStarted_OnNavigationFinished) {
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  EXPECT_CALL(
      *mock_preconnect_manager_1_,
      StartPreconnectUrl(testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_,
              StartPreresolveHost(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_, Start(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());

  NavigationID navigation_id;
  navigation_id.main_frame_url = GURL("http://example.com");
  navigation_id.tab_id = SessionID::FromSerializedValue(123);
  NavigationID old_navigation_id;
  old_navigation_id.main_frame_url = GURL("https://example.com/resource");
  old_navigation_id.tab_id = SessionID::FromSerializedValue(456);
  old_navigation_id.creation_time = base::TimeTicks::Now();
  NavigationID new_navigation_id;
  new_navigation_id.main_frame_url = GURL("https://baidu.com/resource");
  new_navigation_id.tab_id = SessionID::FromSerializedValue(789);
  new_navigation_id.creation_time = base::TimeTicks::Now();
  predictor_->active_navigations_.emplace(old_navigation_id);
  predictor_->active_navigations_.emplace(new_navigation_id);
  predictor_->config_.max_navigation_lifetime_seconds = 3600;

  predictor_->Shutdown();
  predictor_->OnNavigationStarted(navigation_id);
  EXPECT_EQ(predictor_->active_navigations_.size(), 2);
  EXPECT_FALSE(predictor_->active_navigations_.count(navigation_id));
  predictor_->OnNavigationFinished(old_navigation_id, new_navigation_id, false);
  predictor_->CancelPageLoadHint(GURL("https://example.com"));
  EXPECT_EQ(predictor_->active_navigations_.size(), 2);
  EXPECT_TRUE(predictor_->active_navigations_.count(old_navigation_id));

  predictor_->StartInitialization();
  predictor_->OnNavigationStarted(navigation_id);
  EXPECT_EQ(predictor_->active_navigations_.size(), 3);
  EXPECT_TRUE(predictor_->active_navigations_.count(navigation_id));
  predictor_->OnNavigationFinished(old_navigation_id, new_navigation_id, false);
  EXPECT_EQ(predictor_->active_navigations_.size(), 2);
  EXPECT_FALSE(predictor_->active_navigations_.count(old_navigation_id));
}

TEST_F(HwLoadingPredictorTest, CleanupAbandonedHintsAndNavigations) {
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  EXPECT_CALL(
      *mock_preconnect_manager_1_,
      StartPreconnectUrl(testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_,
              StartPreresolveHost(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_, Start(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());

  NavigationID navigation_id;
  navigation_id.tab_id = SessionID::FromSerializedValue(123);
  predictor_->config_.max_navigation_lifetime_seconds = 3600;

  predictor_->CleanupAbandonedHintsAndNavigations(navigation_id);

  predictor_->active_hints_.emplace(GURL("https://example.com"),
                                    base::TimeTicks::Now());
  predictor_->active_hints_.emplace(
      GURL("https://baidu.com"),
      (base::TimeTicks::Now() - base::Seconds(4000)));
  NavigationID one_navigation_id;
  one_navigation_id.tab_id = SessionID::FromSerializedValue(123);
  NavigationID two_navigation_id;
  two_navigation_id.tab_id = SessionID::FromSerializedValue(234);
  two_navigation_id.creation_time = base::TimeTicks::Now();
  NavigationID three_navigation_id;
  three_navigation_id.tab_id = SessionID::FromSerializedValue(456);
  three_navigation_id.creation_time =
      base::TimeTicks::Now() - base::Seconds(4000);
  predictor_->active_navigations_.emplace(one_navigation_id);
  predictor_->active_navigations_.emplace(two_navigation_id);
  predictor_->active_navigations_.emplace(three_navigation_id);

  predictor_->CleanupAbandonedHintsAndNavigations(navigation_id);
  EXPECT_EQ(predictor_->active_hints_.size(), 1);
  EXPECT_FALSE(predictor_->active_hints_.count(GURL("https://baidu.com")));
  EXPECT_EQ(predictor_->active_navigations_.size(), 1);
  EXPECT_FALSE(predictor_->active_navigations_.count(one_navigation_id));
  EXPECT_FALSE(predictor_->active_navigations_.count(three_navigation_id));
}

TEST_F(HwLoadingPredictorTest, HandleOmniboxHint) {
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  EXPECT_CALL(
      *mock_preconnect_manager_1_,
      StartPreconnectUrl(testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_,
              StartPreresolveHost(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_, Start(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());

  predictor_->HandleOmniboxHint(GURL("is_valid++-*/"), false, 123);
  predictor_->HandleOmniboxHint(GURL("https://"), false, 123);
  predictor_->StartInitialization();
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), true,
                                123);
  EXPECT_EQ(predictor_->last_omnibox_origin_,
            url::Origin::Create(GURL("https://example.com/resource")));
  predictor_->Shutdown();
  predictor_->last_omnibox_origin_ =
      url::Origin::Create(GURL("https://example.com/resource"));
  predictor_->last_omnibox_preconnect_time_ =
      base::TimeTicks::Now() - base::Seconds(360);
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), true,
                                123);
  predictor_->last_omnibox_preconnect_time_ = base::TimeTicks::Now();
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), true,
                                123);
  predictor_->StartInitialization();
  predictor_->last_omnibox_origin_ =
      url::Origin::Create(GURL("https://baidu.com/resource"));
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), false,
                                123);
  EXPECT_EQ(predictor_->last_omnibox_origin_,
            url::Origin::Create(GURL("https://example.com/resource")));
  predictor_->Shutdown();
  predictor_->last_omnibox_origin_ =
      url::Origin::Create(GURL("https://example.com/resource"));
  predictor_->last_omnibox_preresolve_time_ =
      base::TimeTicks::Now() - base::Seconds(360);
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), false,
                                123);
  predictor_->last_omnibox_preresolve_time_ = base::TimeTicks::Now();
  predictor_->HandleOmniboxHint(GURL("https://example.com/resource"), false,
                                123);
}

TEST_F(HwLoadingPredictorTest, PrepareForPageLoad) {
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  EXPECT_CALL(
      *mock_preconnect_manager_1_,
      StartPreconnectUrl(testing::_, testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_,
              StartPreresolveHost(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  EXPECT_CALL(*mock_preconnect_manager_1_, Start(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());

  predictor_->Shutdown();
  predictor_->PrepareForPageLoad(GURL("https://example.com/resource"),
                                 HintOrigin::OMNIBOX, false, 123);

  predictor_->StartInitialization();
  predictor_->last_omnibox_origin_ =
      url::Origin::Create(GURL("https://baidu.com/resource"));
  predictor_->PrepareForPageLoad(GURL("https://example.com/resource"),
                                 HintOrigin::OMNIBOX, false, 123);
  EXPECT_EQ(predictor_->last_omnibox_origin_,
            url::Origin::Create(GURL("https://example.com/resource")));

  predictor_->active_hints_.emplace(GURL("https://example.com/resource"),
                                    base::TimeTicks::Now());
  predictor_->PrepareForPageLoad(GURL("https://example.com/resource"),
                                 HintOrigin::NAVIGATION, false, 123);
  predictor_->PrepareForPageLoad(GURL("ftp://baidu.com/resource"),
                                 HintOrigin::NAVIGATION, false, 123);
  predictor_->PrepareForPageLoad(GURL("https://baidu.com/resource"),
                                 HintOrigin::NAVIGATION, false, 123);
  EXPECT_EQ(predictor_->total_hints_activated_, 1);
  EXPECT_TRUE(
      predictor_->active_hints_.count(GURL("https://baidu.com/resource")));
  predictor_->PrepareForPageLoad(GURL("http://baidu.com/resource"),
                                 HintOrigin::NAVIGATION, false, 123);
  EXPECT_EQ(predictor_->total_hints_activated_, 2);
  EXPECT_TRUE(
      predictor_->active_hints_.count(GURL("http://baidu.com/resource")));
}

TEST_F(HwLoadingPredictorTest, MaybeRemovePreconnect) {
  GURL url("https://example.com/resource");
  predictor_->preconnect_manager_ = nullptr;
  predictor_->MaybeRemovePreconnect(url);

  predictor_->StartInitialization();
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  predictor_->MaybeRemovePreconnect(url);
}

TEST_F(HwLoadingPredictorTest, MaybeAddPreconnect) {
  predictor_->Shutdown();
  auto result = predictor_->preconnect_manager();
  EXPECT_EQ(result, nullptr);
  predictor_->MaybeAddPreconnect(GURL("https://example.com/resource"), {},
                                 HintOrigin::OMNIBOX);

  predictor_->StartInitialization();
  predictor_->preconnect_manager_ = nullptr;
  result = predictor_->preconnect_manager();
  EXPECT_NE(result, nullptr);
  predictor_->preconnect_manager_ = std::move(mock_preconnect_manager_);
  result = predictor_->preconnect_manager();
  EXPECT_NE(result, nullptr);
  EXPECT_CALL(*mock_preconnect_manager_1_, Start(testing::_, testing::_))
      .Times(testing::AnyNumber())
      .WillRepeatedly(testing::Return());
  predictor_->MaybeAddPreconnect(GURL("https://example.com/resource"), {},
                                 HintOrigin::OMNIBOX);
}
}  // namespace ohos_predictors
