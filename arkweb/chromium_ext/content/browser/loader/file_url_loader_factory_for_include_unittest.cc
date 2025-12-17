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

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_adapter_ndk/hiviewdfx_adapter/hitrace_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "net/base/mime_sniffer.h"
#include "net/base/mime_util.h"
#include "net/base/url_util.h"

namespace OHOS::NWeb {
class MockOhosResourceAdapter : public OhosResourceAdapter {
 public:
  MOCK_METHOD(
      bool,
      GetRawFileData,
      (const std::string& rawFile, size_t& len, uint8_t** dest, bool isSys),
      (override));
  MOCK_METHOD(std::shared_ptr<OhosFileMapper>,
              GetRawFileMapper,
              (const std::string& rawFile, bool isSys),
              (override));
  MOCK_METHOD(bool,
              IsRawFileExist,
              (const std::string& rawFile, bool isSys),
              (override));
  MOCK_METHOD(
      bool,
      GetRawFileLastModTime,
      (const std::string& rawFile, uint16_t& date, uint16_t& time, bool isSys),
      (override));
  MOCK_METHOD(bool,
              GetRawFileLastModTime,
              (const std::string& rawFile, time_t& time, bool isSys),
              (override));
  MOCK_METHOD(std::string, GetSystemLanguage, (), (override));
};
}  // namespace OHOS::NWeb

namespace content {
namespace {
static const char kResourceURLPrefix[] = "resource:///";
static const char kTestData[] = "datashare://";
}  // namespace

class ResourceURLLoaderTest : public FileURLLoaderFactoryTest {
 public:
  ResourceURLLoaderTest() : FileURLLoaderFactoryTest() {}
  ResourceURLLoaderTest(const ResourceURLLoaderTest&) = delete;
  ResourceURLLoaderTest& operator=(const ResourceURLLoaderTest&) = delete;
  ~ResourceURLLoaderTest() override = default;

 protected:
  void SetUp() override { FileURLLoaderFactoryTest::SetUp(); }
  void TearDown() override {
    testing::Mock::VerifyAndClearExpectations(mock_resource_adapter_.get());
    testing::Mock::VerifyAndClearExpectations(mock_ohos_adapter_helper_.get());
    OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
    mock_resource_adapter_.reset();
    mock_ohos_adapter_helper_.reset();
    FileURLLoaderFactoryTest::TearDown();
  }

  GURL FilePathToResourceURL(const base::FilePath& path,
                             const std::string& scheme = kResourceURLPrefix) {
    std::string url_string(scheme);
    std::string utf8_path = path.AsUTF8Unsafe();
    url_string.reserve(url_string.size() + (3 * utf8_path.size()));

    for (auto c : utf8_path) {
      if (c == '%' || c == ';' || c == '#' || c == '?' ||
#if BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
          c == '\\' ||
#endif
          c <= ' ') {
        url_string += '%';
        base::AppendHexEncodedByte(static_cast<uint8_t>(c), url_string);
      } else {
        url_string += c;
      }
    }

    return GURL(url_string);
  }

  void OpenSwitch() {
    base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
        switches::kOhosAppApiVersion, "9");
    base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
        switches::kOhosHapPath, "/data/app/com.example.app/base.apk");
  }

  void SetOhosAdapterHelperStub(const std::string& test_data = kTestData) {
    mock_ohos_adapter_helper_ =
        std::make_unique<OHOS::NWeb::MockOhosAdapterHelper>();
    OHOS::NWeb::OhosAdapterHelper::SetInstance(mock_ohos_adapter_helper_.get());
    mock_resource_adapter_ =
        std::make_unique<OHOS::NWeb::MockOhosResourceAdapter>();
    auto adapter_ptr = mock_resource_adapter_.get();
    EXPECT_CALL(*mock_ohos_adapter_helper_, GetResourceAdapter(testing::_))
        .WillRepeatedly(testing::Return(
            testing::ByMove(std::move(mock_resource_adapter_))));
    EXPECT_CALL(*mock_ohos_adapter_helper_, GetHiTraceAdapterInstance())
        .WillRepeatedly(testing::Invoke([]() -> OHOS::NWeb::HiTraceAdapter& {
          return OHOS::NWeb::HiTraceAdapterImpl::GetInstance();
        }));
    if (test_data.empty()) {
      EXPECT_CALL(*adapter_ptr, GetRawFileData)
          .WillRepeatedly(
              testing::Invoke([](const std::string& rawFile, size_t& len,
                                 uint8_t** dest, bool isSys) -> bool {
                len = 0;
                *dest = nullptr;
                return false;
              }));
    } else {
      EXPECT_CALL(*adapter_ptr, GetRawFileData)
          .WillRepeatedly(testing::Invoke(
              [test_data](const std::string& rawFile, size_t& len,
                          uint8_t** dest, bool isSys) -> bool {
                len = test_data.size();
                *dest = reinterpret_cast<uint8_t*>(strdup(test_data.c_str()));
                return true;
              }));
    }
  }

 private:
  std::unique_ptr<OHOS::NWeb::MockOhosResourceAdapter> mock_resource_adapter_;
  std::unique_ptr<OHOS::NWeb::MockOhosAdapterHelper> mock_ohos_adapter_helper_;
};

TEST_F(ResourceURLLoaderTest, Invalid_AppApiVersion) {
  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFile(&file));
  auto command_line = base::CommandLine::ForCurrentProcess();
  // not set kOhosAppApiVersion
  command_line->RemoveSwitch(switches::kOhosAppApiVersion);

  auto request = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request);
  request->url = FilePathToResourceURL(file);
  EXPECT_EQ(net::ERR_FAILED, CreateLoaderAndRun(std::move(request)));

  command_line->AppendSwitchASCII(switches::kOhosAppApiVersion, "10");
  auto request1 = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request1);
  request1->url = FilePathToResourceURL(file);
  EXPECT_EQ(net::ERR_FAILED, CreateLoaderAndRun(std::move(request1)));

  auto request2 = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request2);
  request2->url = FilePathToResourceURL(file);
  command_line->AppendSwitchASCII(switches::kOhosAppApiVersion,
                                  "0xffffffffffffffffff");
  EXPECT_EQ(net::ERR_FAILED, CreateLoaderAndRun(std::move(request2)));
}

TEST_F(ResourceURLLoaderTest, Invalid_ResourcesScheme) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kOhosHapPath);
  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFile(&file));

  auto request = std::make_unique<network::ResourceRequest>();
  OpenSwitch();
  // invalid scheme
  request->url = FilePathToResourceURL(file, "invalid_resources://");
  ASSERT_NE(net::OK, CreateLoaderAndRun(std::move(request)));
}

TEST_F(ResourceURLLoaderTest, Response_Success) {
  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFile(&file));
  auto request = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request);
  OpenSwitch();
  SetOhosAdapterHelperStub();
  request->url = FilePathToResourceURL(file);
  ASSERT_EQ(net::OK, CreateLoaderAndRun(std::move(request)));

  ASSERT_NE(ResponseInfo(), nullptr);
  ASSERT_NE(ResponseInfo()->headers, nullptr);
  std::string mine_type;
  ASSERT_TRUE(ResponseInfo()->headers->GetMimeType(&mine_type));
  ASSERT_EQ("text/plain", mine_type);
  ASSERT_EQ(200, ResponseInfo()->headers->response_code());
  ASSERT_EQ("OK", ResponseInfo()->headers->GetStatusText());
}

TEST_F(ResourceURLLoaderTest, GetRawFileData_Fail) {
  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFile(&file));
  auto request = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request);
  OpenSwitch();
  SetOhosAdapterHelperStub("");
  request->url = FilePathToResourceURL(file);
  ASSERT_EQ(net::ERR_FILE_NOT_FOUND, CreateLoaderAndRun(std::move(request)));
}

TEST_F(ResourceURLLoaderTest, MaxBytesToSniff) {
  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFile(&file));
  auto request = std::make_unique<network::ResourceRequest>();
  ASSERT_TRUE(request);
  std::string large_data(net::kMaxBytesToSniff + 1, 'a');
  OpenSwitch();
  SetOhosAdapterHelperStub(large_data);
  request->url = FilePathToResourceURL(file);
  ASSERT_EQ(net::OK, CreateLoaderAndRun(std::move(request)));

  ASSERT_NE(ResponseInfo(), nullptr);
  ASSERT_NE(ResponseInfo()->headers, nullptr);
  std::string mine_type;
  ASSERT_TRUE(ResponseInfo()->headers->GetMimeType(&mine_type));
  ASSERT_EQ("text/plain", mine_type);
  ASSERT_EQ(200, ResponseInfo()->headers->response_code());
  ASSERT_EQ("OK", ResponseInfo()->headers->GetStatusText());
}
}  // namespace content
