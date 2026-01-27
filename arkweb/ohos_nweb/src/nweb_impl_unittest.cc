/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "mock_nweb_delegate.h"
#include "nweb.h"
#include "capi/nweb_download_delegate_callback.h"
#include "nweb_hit_test_result_impl.h"
#define private public
#include "nweb_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_system_properties_adapter.h"
#include "cef_delegate/nweb_application.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "arkweb/chromium_ext/net/base/net_helpers.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_snapshot_data_base.h"
#endif

using namespace testing;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class MockNWebDragEvent : public NWebDragEvent {
 public:
  ~MockNWebDragEvent() = default;
  MOCK_METHOD(DragAction, GetAction, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
  MOCK_METHOD(OHOS::NWeb::NWebDragData::DragOperationsMask,
              GetAllowedDragOperation,
              (),
              (const, override));
  MOCK_METHOD(OHOS::NWeb::NWebDragData::DragOperation,
              GetDragOperation,
              (),
              (const, override));
  MOCK_METHOD(bool, IsDragOpValid, (), (const, override));
};

class MockNWebMouseEvent : public NWebMouseEvent {
 public:
  virtual ~MockNWebMouseEvent() = default;
  MOCK_METHOD(int32_t, GetX, (), (override));
  MOCK_METHOD(int32_t, GetY, (), (override));
  MOCK_METHOD(int32_t, GetButton, (), (override));
  MOCK_METHOD(int32_t, GetAction, (), (override));
  MOCK_METHOD(int32_t, GetClickNum, (), (override));
  MOCK_METHOD(std::vector<int32_t>, GetPressKeyCodes, (), (override));
  MOCK_METHOD(int32_t, GetRawX, (), (override));
  MOCK_METHOD(int32_t, GetRawY, (), (override));
};

class MockNWebInputHandler : public NWebInputHandler {
 public:
  MockNWebInputHandler(std::shared_ptr<NWebDelegateInterface> nweb_delegate)
      : NWebInputHandler(nweb_delegate) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD3(WebSendKeyEvent,
               bool(int32_t, int32_t, const std::vector<int32_t>&));
  MOCK_METHOD5(
      WebSendMouseWheelEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
  MOCK_METHOD6(
      WebSendMouseWheelEventV2,
      void(double, double, double, double, const std::vector<int32_t>&, int32_t));
  MOCK_METHOD5(
      WebSendTouchpadFlingEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
  MOCK_METHOD0(WebSendCancelFlingEvent, void());
  MOCK_METHOD1(WebSendMouseEvent,
               void (const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>&));
  MOCK_METHOD1(SendKeyboardEvent,
               bool (const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>&));
#endif
};

class MockNWebDragData : public NWebDragData {
 public:
  MOCK_METHOD(std::string, GetLinkURL, (), (override));
  MOCK_METHOD(std::string, GetFragmentText, (), (override));
  MOCK_METHOD(std::string, GetFragmentHtml, (), (override));
  MOCK_METHOD(bool,
              GetPixelMapSetting,
              (const void** data, size_t& len, int& width, int& height),
              (override));
  MOCK_METHOD(bool, SetFragmentHtml, (const std::string& html), (override));
  MOCK_METHOD(bool,
              SetPixelMapSetting,
              (const void* data, size_t len, int width, int height),
              (override));
  MOCK_METHOD(bool, SetLinkURL, (const std::string& url), (override));
  MOCK_METHOD(bool, SetFragmentText, (const std::string& Text), (override));
  MOCK_METHOD(std::string, GetLinkTitle, (), (override));
  MOCK_METHOD(bool, SetLinkTitle, (const std::string& title), (override));
  MOCK_METHOD(void, GetDragStartPosition, (int& x, int& y), (override));
  MOCK_METHOD(bool, IsSingleImageContent, (), (override));
  MOCK_METHOD(bool, SetFileUri, (const std::string& uri), (override));
  MOCK_METHOD(std::string, GetImageFileName, (), (override));
  MOCK_METHOD(void, ClearImageFileNames, (), (override));
};

class MockNWebInputMethodHandler : public NWebInputMethodHandler {
 public:
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  MOCK_METHOD0(GetSelectInfo, std::string());
#endif
  MOCK_METHOD0(GetIsEditableNode, bool());
#if BUILDFLAG(ARKWEB_AI_WRITE)
  MOCK_METHOD0(GetSelectStartIndex, int());
  MOCK_METHOD0(GetSelectEndIndex, int());
  MOCK_METHOD0(GetAllTextInfo, std::string());
#endif // ARKWEB_AI_WRITE
  MOCK_METHOD(bool, Reattach, (uint32_t, ReattachType), ());
  MOCK_METHOD(void, SetFocusStatus, (bool), ());
};

class MockNWebCreateInfo : public NWebCreateInfo {
public:
  ~MockNWebCreateInfo() = default;
  MOCK_METHOD(uint32_t, GetWidth, (), (override));
  MOCK_METHOD(uint32_t, GetHeight, (), (override));
  MOCK_METHOD(std::shared_ptr<NWebOutputFrameCallback>,
              GetOutputFrameCallback,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<NWebEngineInitArgs>,
              GetEngineInitArgs,
              (),
              (override));
  MOCK_METHOD(void*, GetProducerSurface, (), (override));
  MOCK_METHOD(void*, GetEnhanceSurfaceInfo, (), (override));
  MOCK_METHOD(bool, GetIsIncognitoMode, (), (override));
};

class MockNWebAccessRequest : public NWebAccessRequest {
public:
  ~MockNWebAccessRequest() = default;
  MOCK_METHOD(std::string, Origin, (), (override));
  MOCK_METHOD(int, ResourceAcessId, (), (override));
  MOCK_METHOD(void, Agree, (int), (override));
  MOCK_METHOD(void, Refuse, (), (override));
};

class MockNWebJsProxyMethod : public OHOS::NWeb::NWebJsProxyMethod {
  public:
   ~MockNWebJsProxyMethod() = default;
   MOCK_METHOD(int, GetSize, (), (override));
   MOCK_METHOD(void, OnHandle, (int, const std::vector<std::string>&), (override));
};

#if BUILDFLAG(IS_ARKWEB)
class MockNWebPrintDocumentAdapterAdapter : public NWebPrintDocumentAdapterAdapter {
  public:
   ~MockNWebPrintDocumentAdapterAdapter() = default;
   MOCK_METHOD(void, OnStartLayoutWrite, (const std::string&, std::shared_ptr<NWebPrintAttributesAdapter>,
       std::shared_ptr<NWebPrintAttributesAdapter>, uint32_t, std::shared_ptr<NWebPrintWriteResultCallbackAdapter>),
       (override));
   MOCK_METHOD(void, OnJobStateChanged, (const std::string&, uint32_t), (override));
};

class NWebPrintDocumentAdapterAdapterImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  MockNWebPrintDocumentAdapterAdapter* mock_adapter;
  std::shared_ptr<NWebPrintDocumentAdapterAdapterImpl> nweb_print_adapter_impl_;
};

void NWebPrintDocumentAdapterAdapterImplTest::SetUpTestCase(void) {}

void NWebPrintDocumentAdapterAdapterImplTest::TearDownTestCase(void) {}

void NWebPrintDocumentAdapterAdapterImplTest::SetUp() {
  mock_adapter = new MockNWebPrintDocumentAdapterAdapter();
  nweb_print_adapter_impl_ = std::make_shared<NWebPrintDocumentAdapterAdapterImpl>(mock_adapter);
  ASSERT_NE(nweb_print_adapter_impl_, nullptr);
}

void NWebPrintDocumentAdapterAdapterImplTest::TearDown() {
  delete mock_adapter;
  nweb_print_adapter_impl_ = nullptr;
}

TEST_F(NWebPrintDocumentAdapterAdapterImplTest, OnStartLayoutWrite001) {
  const std::string jobId = "test";
  std::shared_ptr<NWebPrintAttributesAdapter> oldAttrs = nullptr;
  std::shared_ptr<NWebPrintAttributesAdapter> newAttrs = nullptr;
  uint32_t fd = 0;
  std::shared_ptr<NWebPrintWriteResultCallbackAdapter> callback = nullptr;
  nweb_print_adapter_impl_->ref_ = nullptr;
  EXPECT_CALL(*mock_adapter, OnStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, callback)).Times(0);
  nweb_print_adapter_impl_->OnStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, callback);
  EXPECT_EQ(nweb_print_adapter_impl_->ref_, nullptr);
}

TEST_F(NWebPrintDocumentAdapterAdapterImplTest, OnJobStateChanged001) {
  const std::string jobId = "test";
  uint32_t state = 0;
  nweb_print_adapter_impl_->ref_ = nullptr;
  EXPECT_CALL(*mock_adapter, OnJobStateChanged(jobId, state)).Times(0);
  nweb_print_adapter_impl_->OnJobStateChanged(jobId, state);
  EXPECT_EQ(nweb_print_adapter_impl_->ref_, nullptr);
}
#endif
class MockNWebStylusTouchPointInfo : public NWebStylusTouchPointInfo {
public:
  ~MockNWebStylusTouchPointInfo() = default;
  MOCK_METHOD(float, GetForce, (), (override));
  MOCK_METHOD(float, GetTiltX, (), (override));
  MOCK_METHOD(float, GetTiltY, (), (override));
  MOCK_METHOD(float, GetRollAngle, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(SourceTool, GetSourceTool, (), (override));
  MOCK_METHOD(int, GetId, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};

class MockNWebSystemConfiguration : public NWebSystemConfiguration {
public:
  ~MockNWebSystemConfiguration() = default;
  MOCK_METHOD(uint8_t, GetThemeFlags, (), (override));
};

class MockNetConnectAdapter : public NetConnectAdapter {
public:
  ~MockNetConnectAdapter() = default;
  MOCK_METHOD(int32_t, RegisterNetConnCallback, (std::shared_ptr<NetConnCallback> cb), (override));
  MOCK_METHOD(int32_t, UnregisterNetConnCallback, (int32_t id), (override));
  MOCK_METHOD(int32_t, GetDefaultNetConnect, (NetConnectType& type, NetConnectSubtype& netConnectSubtype), (override));
  MOCK_METHOD(std::vector<std::string>, GetDnsServers, (), (override));
  MOCK_METHOD(std::vector<std::string>, GetDnsServersByNetId, (int32_t netId), (override));
  MOCK_METHOD(std::vector<std::string>, GetDnsServersForVpn, (), (override));
  MOCK_METHOD(void, RegisterVpnListener, (std::shared_ptr<VpnListener>), (override));
  MOCK_METHOD(void, UnRegisterVpnListener, (), (override));
};

#if BUILDFLAG(ARKWEB_COOKIE)
class MockNWebEngineInitArgs : public NWebEngineInitArgs {
 public:
  ~MockNWebEngineInitArgs() = default;
  MOCK_METHOD(std::string, GetDumpPath, (), (override));
  MOCK_METHOD(bool, GetIsFrameInfoDump, (), (override));
  MOCK_METHOD(std::list<std::string>, GetArgsToAdd, (), (override));
  MOCK_METHOD(std::list<std::string>, GetArgsToDelete, (), (override));
  MOCK_METHOD(bool, GetIsMultiRendererProcess, (), (override));
  MOCK_METHOD(bool, GetIsEnhanceSurface, (), (override));
  MOCK_METHOD(bool, GetIsPopup, (), (override));
  MOCK_METHOD(std::string, GetSharedRenderProcessToken, (), (override));
  MOCK_METHOD(bool, GetEmulateTouchFromMouseEvent, (), (override));
};
#endif

class NWebImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  int32_t id = 1;
  std::shared_ptr<NWebImpl> nweb_impl_;
  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  std::shared_ptr<MockNWebInputHandler> input_handler_;
  static bool MockFrameCallback(const char*, uint32_t, uint32_t) { return true; }
};

void NWebImplTest::SetUpTestCase(void) {}

void NWebImplTest::TearDownTestCase(void) {}

void NWebImplTest::SetUp() {
  mock_delegate_ = std::make_shared<MockNWebDelegate>();
  nweb_impl_ = std::make_shared<NWebImpl>(id++);
  ASSERT_NE(nweb_impl_, nullptr);
}

void NWebImplTest::TearDown() {
  nweb_impl_ = nullptr;
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
TEST_F(NWebImplTest, UpdateAdblockEasyListRules001) {
  long version = 1;
  nweb_impl_->UpdateAdblockEasyListRules(version);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateAdblockEasyListRules002) {
  long version = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->UpdateAdblockEasyListRules(version));
}
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
TEST_F(NWebImplTest, SetSiteIsolationMode001) {
  EXPECT_EQ(nweb_impl_->SetSiteIsolationMode(true), 3);
}

TEST_F(NWebImplTest, SetSiteIsolationMode002) {
  EXPECT_EQ(nweb_impl_->SetSiteIsolationMode(false), 1);
}
#endif

TEST_F(NWebImplTest, UploadCallback001) {
  nweb_impl_->UploadCallback("1", "2", "3", "4");
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetVirtualDeviceRatio001) {
  nweb_impl_->device_pixel_ratio_ = 10.0f;
  EXPECT_TRUE(nweb_impl_->SetVirtualDeviceRatio());
}

TEST_F(NWebImplTest, SetVirtualDeviceRatio002) {
  EXPECT_TRUE(nweb_impl_->SetVirtualDeviceRatio());
}

TEST_F(NWebImplTest, SetVirtualDeviceRatio003) {
  nweb_impl_->is_richtext_value_ = true;
  EXPECT_TRUE(nweb_impl_->SetVirtualDeviceRatio());
}

TEST_F(NWebImplTest, SetNwebDelegateForTest001) {
  EXPECT_EQ(nweb_impl_->web_engine_args_.size(), 0);
  std::shared_ptr<NWebEngineInitArgs> init_args;
  nweb_impl_->SetNwebDelegateForTest(init_args);
  EXPECT_GT(nweb_impl_->web_engine_args_.size(), 0);
}

TEST_F(NWebImplTest, SetNwebDelegateForTest002) {
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebEngineInitArgs> init_args;
  nweb_impl_->web_engine_args_.push_back("1");
  nweb_impl_->SetNwebDelegateForTest(init_args);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
TEST_F(NWebImplTest, DisableBoost001) {
  nweb_impl_->DisableBoost(0);
  EXPECT_EQ(nweb_impl_->ResizeTime_, 0);
}

TEST_F(NWebImplTest, DisableBoost002) {
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::GetNWebSharedPtr(id);
  EXPECT_NE(result, nullptr);
  nweb_impl_->DisableBoost(id);
  EXPECT_EQ(nweb_impl_->ResizeTime_, 0);
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
TEST_F(NWebImplTest, OnTouchMove002) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->OnTouchMove(touch_point_infos, from_overlay);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnTouchMove003) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetNativeEmbedMode()).WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, IsEnableCustomVideoPlayer()).WillOnce(::testing::Return(false));
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnTouchMove(touch_point_infos, from_overlay));
}

TEST_F(NWebImplTest, OnTouchMove004) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetNativeEmbedMode()).WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, IsEnableCustomVideoPlayer()).WillOnce(::testing::Return(true));
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnTouchMove(touch_point_infos, from_overlay));
}

TEST_F(NWebImplTest, OnTouchMove005) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetNativeEmbedMode()).WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, IsEnableCustomVideoPlayer()).WillOnce(::testing::Return(true));
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnTouchMove(touch_point_infos, from_overlay));
}
#endif

TEST_F(NWebImplTest, OnStylusTouchPress001) {
  std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info;
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchPress(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchPress002) {
  auto stylus_touch_point_info = std::make_shared<MockNWebStylusTouchPointInfo>();
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchPress(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchPress003) {
  auto stylus_touch_point_info = std::make_shared<MockNWebStylusTouchPointInfo>();
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->OnStylusTouchPress(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
TEST_F(NWebImplTest, OnStylusTouchPress004) {
  auto stylus_touch_point_info = std::make_shared<MockNWebStylusTouchPointInfo>();
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->OnStylusTouchPress(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->blankless_key_, base::ohos::BlanklessController::INVALID_BLANKLESS_KEY);
}
#endif

TEST_F(NWebImplTest, OnStylusTouchRelease001) {
  std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info;
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchRelease(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchRelease002) {
  auto stylus_touch_point_info = std::make_shared<MockNWebStylusTouchPointInfo>();
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchRelease(stylus_touch_point_info, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchRelease003) {
  auto stylus_touch_point_info = std::make_shared<MockNWebStylusTouchPointInfo>();
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnStylusTouchRelease(stylus_touch_point_info, from_overlay));
}

TEST_F(NWebImplTest, OnStylusTouchMove001) {
  std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> stylus_touch_point_infos;
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchMove002) {
  std::shared_ptr<NWebStylusTouchPointInfo> info;
  std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> stylus_touch_point_infos = {info};
  bool from_overlay = false;
  nweb_impl_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
TEST_F(NWebImplTest, OnStylusTouchMove003) {
  std::shared_ptr<NWebStylusTouchPointInfo> info;
  std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> stylus_touch_point_infos = {info};
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnStylusTouchMove004) {
  std::shared_ptr<NWebStylusTouchPointInfo> info;
  std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> stylus_touch_point_infos = {info};
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetNativeEmbedMode()).WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, IsEnableCustomVideoPlayer()).WillOnce(::testing::Return(true));
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay));
}

TEST_F(NWebImplTest, OnStylusTouchMove005) {
  std::shared_ptr<NWebStylusTouchPointInfo> info;
  std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> stylus_touch_point_infos = {info};
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<NWebInputHandler>(nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetNativeEmbedMode()).WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, IsEnableCustomVideoPlayer()).WillOnce(::testing::Return(true));
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnStylusTouchMove(stylus_touch_point_infos, from_overlay));
}
#endif

TEST_F(NWebImplTest, OnReportStatisticLog001) {
  nweb_impl_->OnReportStatisticLog("123");
  EXPECT_EQ(nweb_impl_->on_report_statistic_log_callback_, nullptr);
}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(NWebImplTest, UnLoadWebExtension001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->UnLoadWebExtension("1"));
}

TEST_F(NWebImplTest, DisableWebExtension001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->DisableWebExtension("1"));
}

TEST_F(NWebImplTest, GetExtensionInfoByTabId001) {
  std::vector<WebExtensionInfo> extensionsInfo;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->GetExtensionInfoByTabId(1, extensionsInfo));
}

TEST_F(NWebImplTest, GetExtensionVersion001) {
  std::string extension_id = "";
  EXPECT_EQ(nweb_impl_->GetExtensionVersion(extension_id), "");
}

TEST_F(NWebImplTest, GetExtensionVersion002) {
  std::string extension_id = "1";
  EXPECT_EQ(nweb_impl_->GetExtensionVersion(extension_id), "");
}

TEST_F(NWebImplTest, OnAlertDialogByJS001) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  bool suppress_message = false;
  nweb_impl_->OnAlertDialogByJS(extensionId, url, message, nullptr,
                                suppress_message);
  EXPECT_EQ(nweb_impl_->on_off_screen_alert_callback_, nullptr);
}

void OffscreenDocumentAlertCallback(const char* extensionId,
                                    const char* url,
                                    const char* message,
                                    const int requestId) {}

TEST_F(NWebImplTest, OnAlertDialogByJS002) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  bool suppress_message = true;
  nweb_impl_->on_off_screen_alert_callback_ = OffscreenDocumentAlertCallback;
  nweb_impl_->OnAlertDialogByJS(extensionId, url, message, nullptr,
                                suppress_message);
  EXPECT_EQ(suppress_message, false);
}

TEST_F(NWebImplTest, OnConfirmDialogByJS001) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  bool suppress_message = false;
  nweb_impl_->OnConfirmDialogByJS(extensionId, url, message, nullptr,
                                  suppress_message);
  EXPECT_EQ(nweb_impl_->on_off_screen_confirm_callback_, nullptr);
}

void OffscreenDocumentConfirmCallback(const char* extensionId,
                                      const char* url,
                                      const char* message,
                                      const int requestId) {}

TEST_F(NWebImplTest, OnConfirmDialogByJS002) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  bool suppress_message = true;
  nweb_impl_->on_off_screen_confirm_callback_ =
      OffscreenDocumentConfirmCallback;
  nweb_impl_->OnConfirmDialogByJS(extensionId, url, message, nullptr,
                                  suppress_message);
  EXPECT_EQ(suppress_message, false);
}

TEST_F(NWebImplTest, OnPromptDialogByJS001) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  std::string value = "input";
  bool suppress_message = false;
  nweb_impl_->OnPromptDialogByJS(extensionId, url, message, value, nullptr,
                                 suppress_message);
  EXPECT_EQ(nweb_impl_->on_off_screen_prompt_callback_, nullptr);
}

void OffscreenDocumentConfirmCallback(const char* extensionId,
                                      const char* url,
                                      const char* message,
                                      const char* value,
                                      const int requestId) {}

TEST_F(NWebImplTest, OnPromptDialogByJS002) {
  std::string extensionId = "123456";
  std::string url = "//xxxx:xxx/yyy/sss.html";
  std::string message = "test";
  std::string value = "input";
  bool suppress_message = true;
  nweb_impl_->on_off_screen_prompt_callback_ = OffscreenDocumentConfirmCallback;
  nweb_impl_->OnPromptDialogByJS(extensionId, url, message, value, nullptr,
                                 suppress_message);
  EXPECT_EQ(suppress_message, false);
}

TEST_F(NWebImplTest, SetOnOffscreenDocumentAlertCallback001) {
  nweb_impl_->SetOnOffscreenDocumentAlertCallback(nullptr);
  EXPECT_EQ(nweb_impl_->on_off_screen_alert_callback_, nullptr);
}

TEST_F(NWebImplTest, SetOnOffscreenDocumentConfirmCallback001) {
  nweb_impl_->SetOnOffscreenDocumentConfirmCallback(nullptr);
  EXPECT_EQ(nweb_impl_->on_off_screen_confirm_callback_, nullptr);
}

TEST_F(NWebImplTest, SetOnOffscreenDocumentPromptCallback001) {
  nweb_impl_->SetOnOffscreenDocumentPromptCallback(nullptr);
  EXPECT_EQ(nweb_impl_->on_off_screen_prompt_callback_, nullptr);
}

TEST_F(NWebImplTest, AlertHandle001) {
  int requestId = 0;
  nweb_impl_->AlertHandle(requestId);
  EXPECT_EQ(nweb_impl_->on_off_screen_alert_callback_, nullptr);
}

TEST_F(NWebImplTest, ConfirmHandle001) {
  bool type = false;
  int requestId = 0;
  nweb_impl_->ConfirmHandle(type, requestId);
  EXPECT_EQ(nweb_impl_->on_off_screen_confirm_callback_, nullptr);
}

TEST_F(NWebImplTest, PromptHandle001) {
  bool type = false;
  std::string value = "input";
  int requestId = 0;
  nweb_impl_->PromptHandle(type, value, requestId);
  EXPECT_EQ(nweb_impl_->on_off_screen_prompt_callback_, nullptr);
}

#endif

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
TEST_F(NWebImplTest, SetConnectionTimeout001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->SetConnectionTimeout(1));
}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
TEST_F(NWebImplTest, Distill001) {
  uint64_t request_id = 1;
  DistillOptions distill_options;
  DistillCallback callback = nullptr;
  nweb_impl_->Distill(request_id, distill_options, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Distill002) {
  uint64_t request_id = 1;
  DistillOptions distill_options;
  DistillCallback callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  testing::internal::CaptureStderr();
  nweb_impl_->Distill(request_id, distill_options, callback);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("NWebImpl::Distill callback is nullptr"), std::string::npos);
}

void MyDistillCallback(int32_t nweb_id, uint64_t request_id, const char* distill_info) {}

TEST_F(NWebImplTest, Distill003) {
  uint64_t request_id = 1;
  DistillOptions distill_options;
  DistillCallback callback = MyDistillCallback;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  testing::internal::CaptureStderr();
  nweb_impl_->Distill(request_id, distill_options, callback);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("NWebImpl::Distill callback is nullptr"), std::string::npos);
  EXPECT_EQ(log_output.find("NWebImpl::Distill delegate_ is nullptr"), std::string::npos);
}

TEST_F(NWebImplTest, AbortDistill001) {
  testing::internal::CaptureStderr();
  nweb_impl_->AbortDistill();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("NWebImpl::AbortDistill delegate_ is nullptr"), std::string::npos);
}

TEST_F(NWebImplTest, AbortDistill002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  testing::internal::CaptureStderr();
  nweb_impl_->AbortDistill();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("NWebImpl::AbortDistill delegate_ is nullptr"), std::string::npos);
}
#endif

#if BUILDFLAG(ARKWEB_I18N)
TEST_F(NWebImplTest, UpdateAcceptLanguageInternal001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->UpdateAcceptLanguageInternal());
}
#endif  // BUILDFLAG(ARKWEB_I18N)

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
TEST_F(NWebImplTest, GetDownloadItemStateByGuid001) {
  std::string guid = "1";
  EXPECT_EQ(nweb_impl_->GetDownloadItemStateByGuid(guid), NWebDownloadItemState::MAX_DOWNLOAD_STATE);
}
#endif // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
TEST_F(NWebImplTest, SetDefaultBrowserZoomLevel001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->SetDefaultBrowserZoomLevel(1.0));
}
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
TEST_F(NWebImplTest, PrepareForPageLoad001) {
  std::string url = "";
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->PrepareForPageLoad(url, true, 1));
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
TEST_F(NWebImplTest, RemoveAllCache001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->RemoveAllCache(true));

  ASSERT_NO_FATAL_FAILURE(nweb_impl_->RemoveAllCache(false));
}
#endif

#if BUILDFLAG(ARKWEB_SUSPEND_ALL_TIMERS)
TEST_F(NWebImplTest, PauseAllTimers001) {
  testing::internal::CaptureStderr();
  nweb_impl_->PauseAllTimers();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("NWebImpl::PauseAllTimers content_view_ is nullptr"), std::string::npos);
}

TEST_F(NWebImplTest, ResumeAllTimers001) {
  testing::internal::CaptureStderr();
  nweb_impl_->ResumeAllTimers();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("NWebImpl::ResumeAllTimers content_view_ is nullptr"), std::string::npos);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
TEST_F(NWebImplTest, SetExceptionListForJavaScriptEnabled001) {
  std::vector<std::string> urls;
  EXPECT_TRUE(nweb_impl_->SetExceptionListForJavaScriptEnabled(urls, false));

  EXPECT_TRUE(nweb_impl_->SetExceptionListForJavaScriptEnabled(urls, true));
}

TEST_F(NWebImplTest, SetExceptionListForAcceptCookie001) {
  std::vector<std::string> urls;
  EXPECT_TRUE(nweb_impl_->SetExceptionListForAcceptCookie(urls, false));

  EXPECT_TRUE(nweb_impl_->SetExceptionListForAcceptCookie(urls, true));
}
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
TEST_F(NWebImplTest, GetDefaultUserAgent001) {
  EXPECT_NE(nweb_impl_->GetDefaultUserAgent(), "");
}
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST_F(NWebImplTest, SetMigrationPasswordReady001) {
  testing::internal::CaptureStderr();
  nweb_impl_->SetMigrationPasswordReady(true);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Autofill] InitializeWebEngine is not init."), std::string::npos);
  EXPECT_EQ(log_output.find("[Autofill] Migrate Passwords Ready:"), std::string::npos);
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
TEST_F(NWebImplTest, SetHostIP001) {
  std::string hostName = "1";
  std::string address = "12";
  int32_t aliveTime = 1;
  nweb_impl_->SetHostIP(hostName, address, aliveTime);
  auto it = net_service::NetHelpers::GetHostIP();
  EXPECT_EQ(it.size(), 1);
  nweb_impl_->ClearHostIP(hostName);
}

TEST_F(NWebImplTest, SetHostIP002) {
  std::string hostName = "";
  std::string address = "";
  int32_t aliveTime = 1;
  nweb_impl_->SetHostIP(hostName, address, aliveTime);
  auto it = net_service::NetHelpers::GetHostIP();
  EXPECT_EQ(it.size(), 0);
}

TEST_F(NWebImplTest, ClearHostIP001) {
  std::string hostName = "1";
  nweb_impl_->ClearHostIP(hostName);
  auto it = net_service::NetHelpers::GetHostIP();
  EXPECT_EQ(it.size(), 0);
}

TEST_F(NWebImplTest, ClearHostIP002) {
  std::string hostName = "1";
  std::string address = "12";
  int32_t aliveTime = 1;
  nweb_impl_->SetHostIP(hostName, address, aliveTime);
  auto ip = net_service::NetHelpers::GetHostIP();
  EXPECT_EQ(ip.size(), 1);

  nweb_impl_->ClearHostIP(hostName);
  auto it = net_service::NetHelpers::GetHostIP();
  EXPECT_EQ(it.size(), 0);
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebImplTest, PutSpanstringConvertHtmlCallback001) {
  std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback = nullptr;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->PutSpanstringConvertHtmlCallback(callback));
}
#endif

#ifdef BUILDFLAG(IS_OHOS)
TEST_F(NWebImplTest, OnConfigurationUpdated001) {
  std::shared_ptr<NWebSystemConfiguration> configuration;
  nweb_impl_->OnConfigurationUpdated(configuration);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnConfigurationUpdated002) {
  auto configuration = std::make_shared<MockNWebSystemConfiguration>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->OnConfigurationUpdated(configuration));
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
TEST_F(NWebImplTest, IsNWebEx001) {
  EXPECT_FALSE(nweb_impl_->IsNWebEx());
}
#endif

TEST_F(NWebImplTest, SetProxyOverride0001) {
  std::vector<std::string> proxyUrls;
  std::vector<std::string> proxySchemeFilters;
  std::vector<std::string> bypassRules;
  bool reverseBypass = false;
  std::shared_ptr<NWebProxyChangedCallback> callback;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, callback));
}

TEST_F(NWebImplTest, SetProxyOverride002) {
  std::vector<std::string> proxyUrls = {"12"};
  std::vector<std::string> proxySchemeFilters = {"agd"};
  std::vector<std::string> bypassRules;
  bool reverseBypass = false;
  std::shared_ptr<NWebProxyChangedCallback> callback = nullptr;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, callback));
}

TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy0001) {
  std::string objName;
  std::vector<std::string> methodName;
  std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data;
  bool isAsync = false;
  std::string permission;
  testing::internal::CaptureStderr();
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("nweb_delegate_ is nullptr"), std::string::npos);
}

TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy0002) {
  std::string objName;
  std::vector<std::string> methodName;
  auto data = std::make_shared<MockNWebJsProxyMethod>();
  bool isAsync = false;
  std::string permission;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  testing::internal::CaptureStderr();
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("NWebImpl::RegisterNativeJavaScriptProxy"), std::string::npos);
}

TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy0003) {
  std::string objName;
  std::vector<std::string> methodName;
  auto data = std::make_shared<MockNWebJsProxyMethod>();
  bool isAsync = false;
  std::string permission;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*data, GetSize()).WillOnce(testing::Return(1));
  testing::internal::CaptureStderr();
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("NWebImpl::RegisterNativeJavaScriptProxy"), std::string::npos);
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(NWebImplTest, TriggerBlanklessForUrl001) {
  std::string url = "a";
  EXPECT_FALSE(nweb_impl_->TriggerBlanklessForUrl(url));
}

TEST_F(NWebImplTest, TriggerBlanklessForUrl002) {
  std::string url = "a";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_FALSE(nweb_impl_->TriggerBlanklessForUrl(url));
}

TEST_F(NWebImplTest, CheckNetAvailable001) {
  EXPECT_FALSE(nweb_impl_->CheckNetAvailable());
}

TEST_F(NWebImplTest, CheckNetAvailable002) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto net_connect_adapter = std::make_unique<MockNetConnectAdapter>();
  EXPECT_CALL(*instance, CreateNetConnectAdapter()).WillOnce(testing::Return(std::move(net_connect_adapter)));
  EXPECT_FALSE(nweb_impl_->CheckNetAvailable());
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
TEST_F(NWebImplTest, EnableHttpsUpgrades001) {
  nweb_impl_->EnableHttpsUpgrades(false);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableHttpsUpgrades002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->EnableHttpsUpgrades(false));
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
TEST_F(NWebImplTest, IsPrivateNetworkAccessEnabled001) {
  EXPECT_TRUE(nweb_impl_->IsPrivateNetworkAccessEnabled());
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
TEST_F(NWebImplTest, SetSocketIdleTimeout001) {
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->SetSocketIdleTimeout(1));
}
#endif

TEST_F(NWebImplTest, NWebImplTest_InitialScale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(1);

  nweb_impl_->InitialScale(scale);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_InitialScale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  nweb_impl_->InitialScale(scale);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Scale()).WillOnce(::testing::Return(scale));

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, scale);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, NWebImplTest_OnFocus_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).Times(0);

  nweb_impl_->OnFocus(OHOS::NWeb::FocusReason::FOCUS_DEFAULT);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnBlur_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(0);

  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::FOCUS_SWITCH);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SendDragEvent_001) {
  auto dragEvent = std::make_shared<MockNWebDragEvent>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*dragEvent, GetAction()).Times(0);
  EXPECT_CALL(*dragEvent, GetX()).Times(0);
  EXPECT_CALL(*dragEvent, GetY()).Times(0);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(0);

  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SendDragEvent_002) {
  auto dragEvent = std::make_shared<MockNWebDragEvent>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(Return(DragAction::DRAG_OVER));
  EXPECT_CALL(*dragEvent, GetX()).Times(2);
  EXPECT_CALL(*dragEvent, GetY()).Times(2);
  EXPECT_CALL(*dragEvent, GetDragOperation()).Times(0);
  EXPECT_CALL(*dragEvent, GetAllowedDragOperation()).Times(0);

  nweb_impl_->SendDragEvent(dragEvent);
}

TEST_F(NWebImplTest, NWebImplTest_SendDragEvent_003) {
  auto dragEvent = std::make_shared<MockNWebDragEvent>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(Return(DragAction::DRAG_OVER));
  EXPECT_CALL(*dragEvent, IsDragOpValid()).WillOnce(Return(true));
  EXPECT_CALL(*dragEvent, GetX()).Times(2);
  EXPECT_CALL(*dragEvent, GetY()).Times(2);
  EXPECT_CALL(*dragEvent, GetDragOperation()).Times(1);
  EXPECT_CALL(*dragEvent, GetAllowedDragOperation()).Times(1);

  nweb_impl_->SendDragEvent(dragEvent);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_ScrollTo_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollTo_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  ON_CALL(*input_handler_, WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillByDefault(::testing::Return(true));
  nweb_impl_->input_handler_ = nullptr;

  auto ret = nweb_impl_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendCancelFlingEvent_001) {
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_, WebSendCancelFlingEvent())
      .Times(0);

  nweb_impl_->WebSendCancelFlingEvent();
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(0);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(1);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetNestedScrollMode_001) {
  auto ret = OHOS::NWeb::NestedScrollMode::SELF_ONLY;
  nweb_impl_->SetNestedScrollMode(ret);
  EXPECT_NE(nweb_impl_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_001) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_002) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(0);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(1);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, drag_data_mock);
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebImplTest, NWebImplTest_GetSelectInfo_001) {
  std::string select_info = "test";
  std::string select_info_empty = "";
  MockNWebInputMethodHandler input_method_handler;
  ON_CALL(input_method_handler, GetSelectInfo())
      .WillByDefault(::testing::Return(select_info));
  nweb_impl_->inputmethod_handler_ = nullptr;

  auto ret = nweb_impl_->GetSelectInfo();
  EXPECT_EQ(ret, select_info_empty);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_001) {
  int temp = -1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_002) {
  int temp = 1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(temp));

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(0);

  nweb_impl_->OnTextSelected();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(1);

  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebPageSnapshot) {
  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  bool result = false;

  result = nweb_impl_->WebPageSnapshot(
      id, PixelUnit::PX, width, height,
      [](const char* str, bool is, float flo, void* ptr, int a, int b) {});
}

TEST_F(NWebImplTest, OnOccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(0);

  nweb_impl_->OnOccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnOccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(1);

  nweb_impl_->OnOccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(0);

  nweb_impl_->OnUnoccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(1);

  nweb_impl_->OnUnoccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_001) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(0);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_002) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(1);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearPrefetchedResourceCall) {
  std::vector<std::string> cache_key_list = {"sdfslllllfds", "dfsfdsddddddd",
                                             "eeeefdsfffffffff"};
  nweb_impl_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(0);

  nweb_impl_->OnRenderToBackground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(1);

  nweb_impl_->OnRenderToBackground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(0);

  nweb_impl_->OnRenderToForeground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(1);

  nweb_impl_->OnRenderToForeground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrefetchResourceCall) {
  std::shared_ptr<NWebEnginePrefetchArgs> pre_args;
  std::map<std::string, std::string> additional_http_headers = {
      {"aaa", "bbb"}, {"ccc", "dddd"}, {"eee", "ffff"}};
  std::string cache_key = "aaaaaaaaadddddddfff";
  uint32_t cache_valid_time = 30;
  nweb_impl_->PrefetchResource(pre_args, additional_http_headers, cache_key,
                               cache_valid_time);
}
TEST_F(NWebImplTest, TestCreateNWebWithNullCreateInfo) {
  std::shared_ptr<MockNWebCreateInfo> nullCreateInfo = nullptr;
  auto result = NWebImpl::CreateNWeb(nullCreateInfo);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, TestGetNWebNonExistingId) {
  int32_t existingId = 1;
  auto result = NWebImpl::GetNWeb(existingId);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, SetIsOfflineWebComponentWithNullInputHandler) {
  nweb_impl_->SetIsOfflineWebComponent();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnWebviewHideWithNullInputHandler) {
  nweb_impl_->OnWebviewHide();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnWebviewShowWithNullInputHandler) {
  nweb_impl_->OnWebviewShow();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTextSelected) {
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, ResizeWithNullInputHandler) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchPressWithoutHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 1.0;
  double y = 1.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchPress(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestSetDrawRect) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetDrawRect(x, y, width, height);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerSingleParams) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 2.0;
  double y = 3.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerVectorParams) {
  nweb_impl_->input_handler_ = nullptr;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(touch_point_infos, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchCancelnputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnTouchCancel();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendKeyEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t keyCode = 1;
  int32_t keyAction = 2;
  nweb_impl_->SendKeyEvent(keyCode, keyAction);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendTouchpadFlingEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double vx = 1.0;
  double vy = 1.0;
  nweb_impl_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseWheelEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double deltaX = 1.0;
  double deltaY = 1.0;
  nweb_impl_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, LoadWithNullInputHandler) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomWithNullNwebDelegate) {
  float zoomFactor = 1.0;
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomInWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomOutWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, temp);
}
TEST_F(NWebImplTest, GetRenderProcessModeWithNullInputHandler) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto temp = RenderProcessMode::SINGLE_MODE;
  auto ret = nweb_impl_->GetRenderProcessMode();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, OnDestroyWithNullInitArgs) {
  nweb_impl_->destroyCallback_ = [](const char* str) {};
  nweb_impl_->OnDestroy();
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
}

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
TEST_F(NWebImplTest, GetOrigin001) {
  NWebPermissionRequest* request = nullptr;
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "");
}

TEST_F(NWebImplTest, GetOrigin002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "");
  delete request;
}

TEST_F(NWebImplTest, GetOrigin003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, Origin())
    .WillOnce(::testing::Return("https://example.com"));
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "https://example.com");
  delete request;
}

TEST_F(NWebImplTest, GetResouceId001) {
  NWebPermissionRequest* request = nullptr;
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, -1);
}

TEST_F(NWebImplTest, GetResouceId002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, -1);
  delete request;
}

TEST_F(NWebImplTest, GetResouceId003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, ResourceAcessId())
    .WillOnce(::testing::Return(999));
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, 999);
  delete request;
}

TEST_F(NWebImplTest, Grant001) {
  NWebPermissionRequest* request = nullptr;
  int32_t resourse_id = -1;
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, Grant002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  int32_t resourse_id = -1;
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Grant003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  int32_t resourse_id = -1;
  EXPECT_CALL(*temp, Agree(resourse_id)).Times(1);
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Deny001) {
  NWebPermissionRequest* request = nullptr;
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, Deny002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Deny003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, Refuse()).Times(1);
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}
#endif

TEST_F(NWebImplTest, OnDestroyWithNativeDestroyCallback) {
  nweb_impl_->destroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
  nweb_impl_->nativeDestroyCallback_ = []() {};
  EXPECT_NE(nweb_impl_->nativeDestroyCallback_, nullptr);
  nweb_impl_->OnDestroy();
}

TEST_F(NWebImplTest, OnDestroyWithNullNativeCallback) {
  nweb_impl_->destroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
  nweb_impl_->nativeDestroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->nativeDestroyCallback_, nullptr);
  nweb_impl_->OnDestroy();
}

TEST_F(NWebImplTest, TestCreateNWebWithCreateInfo) {
  std::shared_ptr<MockNWebCreateInfo> createInfo = std::make_shared<MockNWebCreateInfo>();
  auto result = NWebImpl::CreateNWeb(createInfo);
  EXPECT_NE(createInfo, nullptr);
}

TEST_F(NWebImplTest, AddNWebToMap001) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
}

TEST_F(NWebImplTest, AddNWebToMap002) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = nullptr;
  EXPECT_EQ(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
}

TEST_F(NWebImplTest, FromID001) {
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::FromID(id);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, FromID002) {
  int32_t id = 0;
  int32_t nweb_id = 2;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::FromID(nweb_id);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, GetNWebSharedPtr001) {
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::GetNWebSharedPtr(id);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetNWebSharedPtr002) {
  int32_t id = 0;
  int32_t nweb_id = 2;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::GetNWebSharedPtr(nweb_id);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, PutDownloadCallback001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebDownloadCallback> downloadListener = nullptr;
  nweb_impl_->PutDownloadCallback(downloadListener);
}

TEST_F(NWebImplTest, PutDownloadCallback002) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebDownloadCallback> downloadListener = nullptr;
  nweb_impl_->PutDownloadCallback(downloadListener);
}

TEST_F(NWebImplTest, SetNWebHandler001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebHandler> client = std::make_shared<NWebHandler>();
  nweb_impl_->SetNWebHandler(client);
  EXPECT_EQ(nweb_impl_->nweb_handle_, client);
}

TEST_F(NWebImplTest, SetNWebHandler002) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebHandler> client = std::make_shared<NWebHandler>();
  nweb_impl_->SetNWebHandler(client);
  EXPECT_NE(nweb_impl_->nweb_handle_, client);
}

#if BUILDFLAG(ARKWEB_AI)
TEST_F(NWebImplTest, SetNWebAgentHandler001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebAgentHandler> agent_handler = nullptr;
  nweb_impl_->SetNWebAgentHandler(agent_handler);
}

TEST_F(NWebImplTest, SetNWebAgentHandler002) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebAgentHandler> agent_handler = nullptr;
  nweb_impl_->SetNWebAgentHandler(agent_handler);
}
#endif

TEST_F(NWebImplTest, Resize001) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->OnPause();
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize002) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = nullptr;
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize003) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize004) {
  uint32_t width = 100;
  uint32_t height = 8000;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
  int32_t mode = 0;
  nweb_impl_->SetDrawMode(mode);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawRect001) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawRect(x, y, width, height);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawMode001) {
  int32_t mode = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawMode(mode);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawMode002) {
  int32_t mode = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawMode(mode);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPendingSizeStatus001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetPendingSizeStatus();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, GetPendingSizeStatus002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetPendingSizeStatus();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetPendingSizeStatus());
}

TEST_F(NWebImplTest, SetFitContentMode001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int mode = 0;
  nweb_impl_->SetFitContentMode(mode);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetFitContentMode002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int mode = 0;
  nweb_impl_->SetFitContentMode(mode);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnTouchRelease001) {
  EXPECT_NE(nweb_impl_, nullptr);
  int32_t id = 0;
  double x = 0.2;
  double y = 0.1;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnTouchRelease(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchRelease002) {
  EXPECT_NE(nweb_impl_, nullptr);
  int32_t id = 0;
  double x = 0.2;
  double y = 0.1;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnTouchRelease(id, x, y, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchMove001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int32_t id = 1;
  double x = 2.0;
  double y = 3.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchCancel001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnTouchCancel();
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnNavigateBack001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnNavigateBack();
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnNavigateBack002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnNavigateBack();
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendKeyEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int32_t keyCode = 1;
  int32_t keyAction = 2;
  auto result = nweb_impl_->SendKeyEvent(keyCode, keyAction);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(result, nweb_impl_->input_handler_->SendKeyEvent(keyCode, keyAction));
}

TEST_F(NWebImplTest, SendTouchpadFlingEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  double x = 1.0;
  double y = 1.0;
  double vx = 1.0;
  double vy = 1.0;
  nweb_impl_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseWheelEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  double x = 1.0;
  double y = 1.0;
  double deltaX = 1.0;
  double deltaY = 1.0;
  nweb_impl_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEvent002) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 3;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, Load001) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load002) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load003) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load004) {
  std::string url = "https://example.com";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  uint32_t width = 100;
  uint32_t height = 100;
  nweb_impl_->output_handler_->Resize(width, height);
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->Load(url));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, IsNavigatebackwardAllowed001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->IsNavigatebackwardAllowed();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, IsNavigatebackwardAllowed002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->IsNavigatebackwardAllowed();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CanNavigateBackOrForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int numSteps = 1;
  auto result = nweb_impl_->CanNavigateBackOrForward(numSteps);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, CanNavigateBackOrForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int numSteps = 1;
  nweb_impl_->CanNavigateBackOrForward(numSteps);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBack001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->NavigateBack();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBack002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NavigateBack();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->NavigateForward();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NavigateForward();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBackOrForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int step = 1;
  nweb_impl_->NavigateBackOrForward(step);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBackOrForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int step = 1;
  nweb_impl_->NavigateBackOrForward(step);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DeleteNavigateHistory001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->DeleteNavigateHistory();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DeleteNavigateHistory002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->DeleteNavigateHistory();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_BGTASK)
TEST_F(NWebImplTest, OnBrowserForegroundWithNullInitArgs)
{
  nweb_impl_->OnBrowserForeground();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnBrowserBackgroundWithNullInitArgs)
{
  nweb_impl_->OnBrowserBackground();
  EXPECT_NE(nweb_impl_, nullptr);
}
#endif

TEST_F(NWebImplTest, Zoom001) {
  float zoomFactor = 1.0f;
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Zoom002) {
  float zoomFactor = 1.0f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->Zoom(zoomFactor));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomIn001) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomIn002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->ZoomIn());
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomOut001) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomOut002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->ZoomOut());
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearSslCache001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClearSslCache();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearSslCache002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClearSslCache();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearClientAuthenticationCache001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClearClientAuthenticationCache();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearClientAuthenticationCache002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClearClientAuthenticationCache();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Reload001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->Reload();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Reload002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Reload();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Stop001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->Stop();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Stop002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Stop();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript001) {
  std::string code = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScript(code);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript002) {
  std::string code = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScript(code);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutBackgroundColor001) {
  int color = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PutBackgroundColor(color);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutBackgroundColor002) {
  int color = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PutBackgroundColor(color);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutOptimizeParserBudgetEnabled001) {
  bool enable = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PutOptimizeParserBudgetEnabled(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutOptimizeParserBudgetEnabled002) {
  bool enable = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PutOptimizeParserBudgetEnabled(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnContinue001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnContinue();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebComponentsBlur001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->WebComponentsBlur();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebComponentsBlur002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->WebComponentsBlur();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableHalfFrameRate001) {
  bool enabled = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetEnableHalfFrameRate(enabled);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableHalfFrameRate002) {
  bool enabled = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetEnableHalfFrameRate(enabled);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPreference001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetPreference();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, GetPreference002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetPreference();
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetPreference());
}

TEST_F(NWebImplTest, Title001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->Title();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, Title002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Title();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScriptExt001) {
  const int fd = 0;
  const size_t scriptLength = 10;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScriptExt002) {
  const int fd = 0;
  const size_t scriptLength = 10;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_MSGPORT)
TEST_F(NWebImplTest, ExecuteJavaScript0001) {
  const std::string code = " test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScript(code, callback, extention);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript0002) {
  const std::string code = " test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScript(code, callback, extention);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CreateWebMessagePorts001) {
  std::vector<std::string> empty;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->CreateWebMessagePorts();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, empty);
}

TEST_F(NWebImplTest, CreateWebMessagePorts002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->CreateWebMessagePorts();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostWebMessage001) {
  const std::string message = "testmessage";
  const std::vector<std::string> ports = {"Default", "IncludeSensitive", "Everything"};
  const std::string targetUri = "testuri";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PostWebMessage(message, ports, targetUri);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostWebMessage002) {
  const std::string message = "testmessage";
  const std::vector<std::string> ports = {"Default", "IncludeSensitive", "Everything"};
  const std::string targetUri = "testuri";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PostWebMessage(message, ports, targetUri);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClosePort001) {
  const std::string portHandle = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClosePort(portHandle);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClosePort002) {
  const std::string portHandle = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClosePort(portHandle);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostPortMessage001) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PostPortMessage(portHandle, data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostPortMessage002) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PostPortMessage(portHandle, data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPortMessageCallback001) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetPortMessageCallback(portHandle, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPortMessageCallback002) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetPortMessageCallback(portHandle, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_MSGPORT)

TEST_F(NWebImplTest, SetAutofillCallback001) {
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetAutofillCallback(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAutofillCallback002) {
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetAutofillCallback(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FillAutofillData001) {
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->FillAutofillData(data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FillAutofillData002) {
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->FillAutofillData(data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnAutofillCancel001) {
  const std::string fillContent = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnAutofillCancel(fillContent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
TEST_F(NWebImplTest, OnAutofillCancel002) {
  const std::string fillContent = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ProcessAutofillCancel(fillContent)).WillOnce(::testing::Return());
  nweb_impl_->OnAutofillCancel(fillContent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, GetHitTestResult001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetHitTestResult();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetHitTestResult002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetHitTestResult();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetHitTestResult());
}

TEST_F(NWebImplTest, GetLastHitTestResult001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetLastHitTestResult();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetLastHitTestResult002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetLastHitTestResult();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetLastHitTestResult());
}

TEST_F(NWebImplTest, PageLoadProgress001) {
  int temp = 0;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->PageLoadProgress();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PageLoadProgress002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->PageLoadProgress();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->PageLoadProgress());
}

TEST_F(NWebImplTest, ContentHeight001) {
  int temp = 0;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->ContentHeight();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ContentHeight002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->ContentHeight();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->ContentHeight());
}

TEST_F(NWebImplTest, Load0001) {
  const std::string url = "https://example.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->Load(url, additionalHttpHeaders);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Load0002) {
  const std::string url = "https://example.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Load(url, additionalHttpHeaders);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_POST_URL)
TEST_F(NWebImplTest, PostUrl001) {
  const std::string url = "https://example.com";
  const std::vector<char> postData = {'m', 'a', 'r', 'k', '1'};
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->PostUrl(url, postData);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PostUrl002) {
  const std::string url = "https://example.com";
  const std::vector<char> postData = {'m', 'a', 'r', 'k', '1'};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->PostUrl(url, postData);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->PostUrl(url, postData));
}
#endif

TEST_F(NWebImplTest, LoadWithDataAndBaseUrl001) {
  const std::string baseUrl = "https://example.com";
  const std::string data = "test";
  const std::string mimeType = "pdf";
  const std::string encoding = "code";
  const std::string historyUrl = "https://test.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, LoadWithDataAndBaseUrl002) {
  const std::string baseUrl = "https://example.com";
  const std::string data = "test";
  const std::string mimeType = "pdf";
  const std::string encoding = "code";
  const std::string historyUrl = "https://test.com";
  EXPECT_NE(nweb_impl_, nullptr);
  auto temp = OHOS::NWeb::NWEB_OK;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl))
      .WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, LoadWithData001) {
  const std::string data = "test";
  const std::string mimeType = "pdf";
  const std::string encoding = "code";
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->LoadWithData(data, mimeType, encoding);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, LoadWithData002) {
  const std::string data = "test";
  const std::string mimeType = "pdf";
  const std::string encoding = "code";
  EXPECT_NE(nweb_impl_, nullptr);
  auto temp = OHOS::NWeb::NWEB_OK;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, LoadWithData(data, mimeType, encoding)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->LoadWithData(data, mimeType, encoding);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunction001) {
  const char* objName = "test_id";
  const std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>> callbacks;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeArkJSFunction(objName, callbacks);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunction002) {
  const char* objName = "test_id";
  const std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>> callbacks;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeArkJSFunction(objName, callbacks);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunction003) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFunc> callback;
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeArkJSFunction(objName, methodName, std::move(callback), isAsync, permission);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunction004) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFunc> callback;
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeArkJSFunction(objName, methodName, std::move(callback), isAsync, permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunctionWithResult001) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFuncWithResult> callback;
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeArkJSFunctionWithResult(objName, methodName, std::move(callback), isAsync, permission);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeArkJSFunctionWithResult002) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFuncWithResult> callback;
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeArkJSFunctionWithResult(objName, methodName, std::move(callback), isAsync, permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeAsyncThreadArkJSFunctionWithResult001) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFuncWithResult> callback;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeAsyncThreadArkJSFunctionWithResult(objName, methodName, std::move(callback), permission);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeAsyncThreadArkJSFunctionWithResult002) {
  const std::string objName = "test_id";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::vector<NWebImpl::NativeJSProxyCallbackFuncWithResult> callback;
  const std::string permission = "permission";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeAsyncThreadArkJSFunctionWithResult(objName, methodName, std::move(callback), permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnRegisterNativeArkJSFunction001) {
  const char* objName = "test_id";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->UnRegisterNativeArkJSFunction(objName);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnRegisterNativeArkJSFunction002) {
  const char* objName = "test_id";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->UnRegisterNativeArkJSFunction(objName);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeLoadStartCallback001) {
  std::function<void(void)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeLoadStartCallback(std::move(callback));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeLoadStartCallback002) {
  std::function<void(void)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeLoadStartCallback(std::move(callback));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeLoadEndCallback001) {
  std::function<void(void)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeLoadEndCallback(std::move(callback));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeLoadEndCallback002) {
  std::function<void(void)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeLoadEndCallback(std::move(callback));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeScrollCallback001) {
  std::function<void(double, double)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->RegisterNativeScrollCallback(std::move(callback));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeScrollCallback002) {
  std::function<void(double, double)> callback;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->RegisterNativeScrollCallback(std::move(callback));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction001) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, std::vector<std::string>(), object_id, "")).Times(0);
  nweb_impl_->RegisterArkJSfunction(object_name, method_list, object_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction002) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, std::vector<std::string>(), object_id, "")).Times(1);
  nweb_impl_->RegisterArkJSfunction(object_name, method_list, object_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnregisterArkJSfunction001) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UnregisterArkJSfunction(object_name, method_list)).Times(0);
  nweb_impl_->UnregisterArkJSfunction(object_name, method_list);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnregisterArkJSfunction002) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UnregisterArkJSfunction(object_name, method_list)).Times(1);
  nweb_impl_->UnregisterArkJSfunction(object_name, method_list);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_JSPROXY)
TEST_F(NWebImplTest, JavaScriptOnDocumentStart001) {
  const ScriptItems scriptItems;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStart(scriptItems)).Times(0);
  nweb_impl_->JavaScriptOnDocumentStart(scriptItems);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentStart002) {
  const ScriptItems scriptItems;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStart(scriptItems)).Times(1);
  nweb_impl_->JavaScriptOnDocumentStart(scriptItems);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEnd001) {
  const ScriptItems scriptItems;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEnd(scriptItems)).Times(0);
  nweb_impl_->JavaScriptOnDocumentEnd(scriptItems);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEnd002) {
  const ScriptItems scriptItems;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEnd(scriptItems)).Times(1);
  nweb_impl_->JavaScriptOnDocumentEnd(scriptItems);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentStartByOrder001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStartByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnDocumentStartByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentStartByOrder002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStartByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnDocumentStartByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEndByOrder001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEndByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnDocumentEndByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEndByOrder002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEndByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnDocumentEndByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnHeadReadyByOrder001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnHeadReadyByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnHeadReadyByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnHeadReadyByOrder002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnHeadReadyByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnHeadReadyByOrder(scriptItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentStartByOrderV2001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStartByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnDocumentStartByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentStartByOrderV2002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentStartByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnDocumentStartByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEndByOrderV2001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEndByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnDocumentEndByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnDocumentEndByOrderV2002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnDocumentEndByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnDocumentEndByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnHeadReadyByOrderV2001) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnHeadReadyByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(0);
  nweb_impl_->JavaScriptOnHeadReadyByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, JavaScriptOnHeadReadyByOrderV2002) {
  const ScriptItems scriptItems;
  const ScriptRegexItems scriptRegexItems;
  const ScriptItemsByOrder scriptItemsByOrder;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, JavaScriptOnHeadReadyByOrder(
    scriptItems, scriptRegexItems, scriptItemsByOrder)).Times(1);
  nweb_impl_->JavaScriptOnHeadReadyByOrderV2(scriptItems, scriptRegexItems, scriptItemsByOrder);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, CallH5Function001) {
  int32_t routing_id = 0;
  int32_t h5_object_id = 0;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CallH5Function(routing_id, h5_object_id, h5_method_name, args)).Times(0);
  nweb_impl_->CallH5Function(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallH5Function002) {
  int32_t routing_id = 0;
  int32_t h5_object_id = 1;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CallH5Function(routing_id, h5_object_id, h5_method_name, args)).Times(1);
  nweb_impl_->CallH5Function(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallH5Function003) {
  int32_t routing_id = 0;
  int32_t h5_object_id = -1;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CallH5Function(routing_id, h5_object_id, h5_method_name, args)).Times(0);
  nweb_impl_->CallH5Function(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetNWebJavaScriptResultCallBack001) {
  std::shared_ptr<NWebJavaScriptResultCallBack> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterNWebJavaScriptCallBack(callback)).Times(0);
  nweb_impl_->SetNWebJavaScriptResultCallBack(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetNWebJavaScriptResultCallBack002) {
  std::shared_ptr<NWebJavaScriptResultCallBack> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterNWebJavaScriptCallBack(callback)).Times(1);
  nweb_impl_->SetNWebJavaScriptResultCallBack(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutFindCallback001) {
  std::shared_ptr<NWebFindCallback> findListener = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterFindListener(findListener)).Times(0);
  nweb_impl_->PutFindCallback(findListener);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutFindCallback002) {
  std::shared_ptr<NWebFindCallback> findListener = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterFindListener(findListener)).Times(1);
  nweb_impl_->PutFindCallback(findListener);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FindAllAsync001) {
  const std::string search_string = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, FindAllAsync(search_string)).Times(0);
  nweb_impl_->FindAllAsync(search_string);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FindAllAsync002) {
  const std::string search_string = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, FindAllAsync(search_string)).Times(1);
  nweb_impl_->FindAllAsync(search_string);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearMatches001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ClearMatches()).Times(0);
  nweb_impl_->ClearMatches();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearMatches002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ClearMatches()).Times(1);
  nweb_impl_->ClearMatches();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FindNext001) {
  const bool forward = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, FindNext(forward)).Times(0);
  nweb_impl_->FindNext(forward);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FindNext002) {
  const bool forward = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, FindNext(forward)).Times(1);
  nweb_impl_->FindNext(forward);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StoreWebArchive001) {
  const std::string base_name = "name";
  bool auto_name = false;
  std::shared_ptr<NWebStringValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StoreWebArchive(base_name, auto_name, callback)).Times(0);
  nweb_impl_->StoreWebArchive(base_name, auto_name, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StoreWebArchive002) {
  const std::string base_name = "name";
  bool auto_name = false;
  std::shared_ptr<NWebStringValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StoreWebArchive(base_name, auto_name, callback)).Times(1);
  nweb_impl_->StoreWebArchive(base_name, auto_name, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StoreWebArchive003) {
  const std::string base_name = "name";
  bool auto_name = false;
  std::shared_ptr<NWebStringValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StoreWebArchive(base_name, auto_name, callback)).Times(1);
  nweb_impl_->StoreWebArchive(base_name, auto_name, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StoreWebArchive004) {
  const std::string base_name = "name";
  bool auto_name = false;
  std::shared_ptr<NWebStringValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StoreWebArchive(base_name, auto_name, callback)).Times(1);
  nweb_impl_->StoreWebArchive(base_name, auto_name, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StoreWebArchive005) {
  const std::string base_name = "name";
  bool auto_name = false;
  std::shared_ptr<NWebStringValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StoreWebArchive(base_name, auto_name, callback)).Times(1);
  nweb_impl_->StoreWebArchive(base_name, auto_name, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragEvent001) {
  std::shared_ptr<MockNWebDragEvent> dragEvent = std::make_shared<MockNWebDragEvent>();
  EXPECT_NE(dragEvent, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  DragAction action = DragAction::DRAG_LEAVE;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(::testing::Return(action));
  EXPECT_CALL(*dragEvent, GetX()).Times(1);
  EXPECT_CALL(*dragEvent, GetY()).Times(1);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(1);
  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragEvent002) {
  std::shared_ptr<MockNWebDragEvent> dragEvent = std::make_shared<MockNWebDragEvent>();
  EXPECT_NE(dragEvent, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  DragAction action = DragAction::DRAG_ENTER;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(::testing::Return(action));
  EXPECT_CALL(*dragEvent, GetX()).Times(2);
  EXPECT_CALL(*dragEvent, GetY()).Times(2);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(1);
  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragEvent003) {
  std::shared_ptr<MockNWebDragEvent> dragEvent = std::make_shared<MockNWebDragEvent>();
  EXPECT_NE(dragEvent, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  DragAction action = DragAction::DRAG_START;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(::testing::Return(action));
  EXPECT_CALL(*dragEvent, GetX()).Times(2);
  EXPECT_CALL(*dragEvent, GetY()).Times(2);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(1);
  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragEvent004) {
  std::shared_ptr<MockNWebDragEvent> dragEvent = std::make_shared<MockNWebDragEvent>();
  EXPECT_NE(dragEvent, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  DragAction action = DragAction::DRAG_OVER;
  EXPECT_CALL(*dragEvent, GetAction()).WillOnce(::testing::Return(action));
  EXPECT_CALL(*dragEvent, GetX()).Times(2);
  EXPECT_CALL(*dragEvent, GetY()).Times(2);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(1);
  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragOverEvent001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(0);
  nweb_impl_->SendDragOverEvent();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendDragOverEvent002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(1);
  nweb_impl_->SendDragOverEvent();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetUrl001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetUrl()).Times(0);
  auto result = nweb_impl_->GetUrl();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetUrl002) {
  std::string temp = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetUrl()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetUrl();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_I18N)
TEST_F(NWebImplTest, UpdateLocalet001) {
  const std::string language = "chinese";
  const std::string region = "CN";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UpdateLocale(language, region)).Times(0);
  nweb_impl_->UpdateLocale(language, region);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateLocale002) {
  const std::string language = "chinese";
  const std::string region = "CN";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UpdateLocale(language, region)).Times(1);
  nweb_impl_->UpdateLocale(language, region);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // if BUILDFLAG(ARKWEB_I18N)

TEST_F(NWebImplTest, GetOriginalUrl001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetOriginalUrl()).Times(0);
  auto result = nweb_impl_->GetOriginalUrl();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetOriginalUrl002) {
  std::string temp = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetOriginalUrl()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetOriginalUrl();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(IS_OHOS)
TEST_F(NWebImplTest, TerminateRenderProcess001) {
  bool temp = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, TerminateRenderProcess()).Times(0);
  auto result = nweb_impl_->TerminateRenderProcess();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, TerminateRenderProcess002) {
  bool temp = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, TerminateRenderProcess()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->TerminateRenderProcess();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // if BUILDFLAG(IS_OHOS)

TEST_F(NWebImplTest, GetFavicon001) {
  bool temp = false;
  const void* da = nullptr;
  size_t width = 1;
  size_t height = 1;
  ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
  ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetFavicon(&da, width, height, colorType, alphaType)).Times(0);
  auto result = nweb_impl_->GetFavicon(&da, width, height, colorType, alphaType);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetFavicon002) {
  bool temp = true;
  const void* da = nullptr;
  size_t width = 1;
  size_t height = 1;
  ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
  ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetFavicon(&da, width, height, colorType, alphaType)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetFavicon(&da, width, height, colorType, alphaType);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PutNetworkAvailable001) {
  bool available = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PutNetworkAvailable(available)).Times(0);
  nweb_impl_->PutNetworkAvailable(available);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutNetworkAvailable002) {
  bool available = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PutNetworkAvailable(available)).Times(1);
  nweb_impl_->PutNetworkAvailable(available);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, HasImages001) {
  std::shared_ptr<NWebBoolValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetImages(callback)).Times(0);
  nweb_impl_->HasImages(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, HasImages002) {
  std::shared_ptr<NWebBoolValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetImages(callback)).Times(1);
  nweb_impl_->HasImages(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveCache001) {
  bool include_disk_files = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RemoveCache(include_disk_files)).Times(0);
  nweb_impl_->RemoveCache(include_disk_files);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveCache002) {
  bool include_disk_files = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RemoveCache(include_disk_files)).Times(1);
  nweb_impl_->RemoveCache(include_disk_files);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetHistoryList001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetHistoryList()).Times(0);
  auto result = nweb_impl_->GetHistoryList();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, GetHistoryList002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetHistoryList()).Times(1);
  nweb_impl_->GetHistoryList();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_NAVIGATION)
TEST_F(NWebImplTest, SerializeWebState001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SerializeWebState()).Times(0);
  auto result = nweb_impl_->SerializeWebState();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, SerializeWebState002) {
  std::vector<uint8_t> temp = {1, 2};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SerializeWebState()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->SerializeWebState();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_NAVIGATION)

TEST_F(NWebImplTest, RestoreWebState001) {
  bool temp = false;
  const std::vector<uint8_t> state = {1, 2};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RestoreWebState(state)).Times(0);
  auto result = nweb_impl_->RestoreWebState(state);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, RestoreWebState002) {
  bool temp = true;
  const std::vector<uint8_t> state = {1, 2};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RestoreWebState(state)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->RestoreWebState(state);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PutReleaseSurfaceCallback001) {
  std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterReleaseSurfaceListener(releaseSurfaceListener)).Times(0);
  nweb_impl_->PutReleaseSurfaceCallback(releaseSurfaceListener);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutReleaseSurfaceCallback002) {
  std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterReleaseSurfaceListener(releaseSurfaceListener)).Times(1);
  nweb_impl_->PutReleaseSurfaceCallback(releaseSurfaceListener);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
TEST_F(NWebImplTest, PageUp001) {
  bool top = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PageUp(top)).Times(0);
  nweb_impl_->PageUp(top);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PageUp002) {
  bool top = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PageUp(top)).Times(1);
  nweb_impl_->PageUp(top);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PageDown001) {
  bool bottom = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PageDown(bottom)).Times(0);
  nweb_impl_->PageDown(bottom);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PageDown002) {
  bool bottom = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PageDown(bottom)).Times(1);
  nweb_impl_->PageDown(bottom);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
TEST_F(NWebImplTest, GetScrollOffset001) {
  float offset_x = 0.0f;
  float offset_y = 0.0f;
  float scroll_offset_x = 0.0f;
  float scroll_offset_y = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetScrollOffset(&scroll_offset_x, &scroll_offset_y)).Times(0);
  nweb_impl_->GetScrollOffset(&offset_x, &offset_y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetScrollOffset002) {
  float offset_x = 0.0f;
  float offset_y = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->GetScrollOffset(&offset_x, &offset_y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPageOffset001) {
  float offset_x = 0.0f;
  float offset_y = 0.0f;
  float scroll_offset_x = 0.0f;
  float scroll_offset_y = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetScrollOffset(&scroll_offset_x, &scroll_offset_y)).Times(0);
  nweb_impl_->GetPageOffset(&offset_x, &offset_y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPageOffset002) {
  float offset_x = 0.0f;
  float offset_y = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->GetPageOffset(&offset_x, &offset_y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
#endif  // if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, ScrollByRefScreen001) {
  float delta_x = 1.5f;
  float delta_y = 1.5f;
  float vx = 1.5f;
  float vy = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollByRefScreen(delta_x, delta_y, vx, vy)).Times(0);

  nweb_impl_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ScrollByRefScreen002) {
  float delta_x = 1.5f;
  float delta_y = 1.5f;
  float vx = 1.5f;
  float vy = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollByRefScreen(delta_x, delta_y, vx, vy)).Times(1);

  nweb_impl_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ScrollByWithResult001) {
  float delta_x = 1.5f;
  float delta_y = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollByWithResult(delta_x, delta_y)).Times(0);

  nweb_impl_->ScrollByWithResult(delta_x, delta_y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ScrollByWithResult002) {
  float delta_x = 1.5f;
  float delta_y = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollByWithResult(delta_x, delta_y)).Times(1);

  nweb_impl_->ScrollByWithResult(delta_x, delta_y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebSendMouseEvent001) {
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = std::make_shared<MockNWebMouseEvent>();
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_, WebSendMouseEvent(mouseEvent)).Times(0);
  nweb_impl_->WebSendMouseEvent(mouseEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendMouseEvent002) {
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  input_handler_= std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  EXPECT_CALL(*input_handler_, WebSendMouseEvent(mouseEvent)).Times(0);
  nweb_impl_->WebSendMouseEvent(mouseEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}
#endif  // if BUILDFLAG(ARKWEB_INPUT_EVENTS)

TEST_F(NWebImplTest, GetCertChainDerData001) {
  std::vector<std::string> certChainData = {"Default", "IncludeSensitive", "Everything"};
  bool isSingleCert = false;
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetCertChainDerData(certChainData, isSingleCert)).Times(0);
  auto result = nweb_impl_->GetCertChainDerData(certChainData, isSingleCert);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetCertChainDerData002) {
  std::vector<std::string> certChainData = {"Default", "IncludeSensitive", "Everything"};
  bool isSingleCert = false;
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetCertChainDerData(certChainData, isSingleCert)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetCertChainDerData(certChainData, isSingleCert);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
TEST_F(NWebImplTest, SetAudioMuted001) {
  bool muted = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAudioMuted(muted)).Times(0);
  nweb_impl_->SetAudioMuted(muted);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioMuted002) {
  bool muted = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetAudioMuted(muted)).Times(1);
  nweb_impl_->SetAudioMuted(muted);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
TEST_F(NWebImplTest, SetShouldFrameSubmissionBeforeDraw001) {
  bool should = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetShouldFrameSubmissionBeforeDraw(should)).Times(0);
  nweb_impl_->SetShouldFrameSubmissionBeforeDraw(should);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetShouldFrameSubmissionBeforeDraw002) {
  bool should = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetShouldFrameSubmissionBeforeDraw(should)).Times(1);
  nweb_impl_->SetShouldFrameSubmissionBeforeDraw(should);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetCurrentLanguage001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetCurrentLanguage()).Times(0);
  auto result = nweb_impl_->GetCurrentLanguage();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetCurrentLanguage002) {
  std::string temp = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetCurrentLanguage()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetCurrentLanguage();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
TEST_F(NWebImplTest, SetAudioResumeInterval001) {
  int32_t resumeInterval = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAudioResumeInterval(resumeInterval)).Times(0);
  nweb_impl_->SetAudioResumeInterval(resumeInterval);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioResumeInterval002) {
  int32_t resumeInterval = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetAudioResumeInterval(resumeInterval)).Times(1);
  nweb_impl_->SetAudioResumeInterval(resumeInterval);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioExclusive001) {
  bool audioExclusive = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAudioExclusive(audioExclusive)).Times(0);
  nweb_impl_->SetAudioExclusive(audioExclusive);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioExclusive002) {
  bool audioExclusive = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetAudioExclusive(audioExclusive)).Times(1);
  nweb_impl_->SetAudioExclusive(audioExclusive);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioSessionType001) {
  int32_t audioSessionType = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAudioSessionType(audioSessionType)).Times(0);
  nweb_impl_->SetAudioSessionType(audioSessionType);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAudioSessionType002) {
  int32_t audioSessionType = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetAudioSessionType(audioSessionType)).Times(1);
  nweb_impl_->SetAudioSessionType(audioSessionType);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseAllMediaPresentations001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CloseAllMediaPresentations()).Times(0);
  nweb_impl_->CloseAllMediaPresentations();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseAllMediaPresentations002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CloseAllMediaPresentations()).Times(1);
  nweb_impl_->CloseAllMediaPresentations();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StopAllMedia001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StopAllMedia()).Times(0);
  nweb_impl_->StopAllMedia();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StopAllMedia002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StopAllMedia()).Times(1);
  nweb_impl_->StopAllMedia();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResumeAllMedia001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResumeAllMedia()).Times(0);
  nweb_impl_->ResumeAllMedia();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResumeAllMedia002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResumeAllMedia()).Times(1);
  nweb_impl_->ResumeAllMedia();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PauseAllMedia001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PauseAllMedia()).Times(0);
  nweb_impl_->PauseAllMedia();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PauseAllMedia002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PauseAllMedia()).Times(1);
  nweb_impl_->PauseAllMedia();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetMediaPlaybackState001) {
  int temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetMediaPlaybackState()).Times(0);
  auto result = nweb_impl_->GetMediaPlaybackState();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetMediaPlaybackState002) {
  int temp = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetMediaPlaybackState()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetMediaPlaybackState();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
TEST_F(NWebImplTest, SetNativeInnerWeb001) {
  bool isInnerWeb = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetNativeInnerWeb(isInnerWeb)).Times(0);
  nweb_impl_->SetNativeInnerWeb(isInnerWeb);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetNativeInnerWeb002) {
  bool isInnerWeb = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetNativeInnerWeb(isInnerWeb)).Times(1);
  nweb_impl_->SetNativeInnerWeb(isInnerWeb);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // if BUILDFLAG(ARKWEB_SAME_LAYER)

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
TEST_F(NWebImplTest, RegisterScreenLockFunction001) {
  int32_t windowId = 0;
  std::shared_ptr<NWebScreenLockCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetWakeLockCallback(windowId, callback)).Times(0);
  nweb_impl_->RegisterScreenLockFunction(windowId, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterScreenLockFunction002) {
  int32_t windowId = 0;
  std::shared_ptr<NWebScreenLockCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetWakeLockCallback(windowId, callback)).Times(1);
  nweb_impl_->RegisterScreenLockFunction(windowId, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnRegisterScreenLockFunction001) {
  int32_t windowId = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetWakeLockCallback(windowId, ::testing::_)).Times(0);
  nweb_impl_->UnRegisterScreenLockFunction(windowId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UnRegisterScreenLockFunction002) {
  int32_t windowId = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetWakeLockCallback(windowId, ::testing::_)).Times(1);
  nweb_impl_->UnRegisterScreenLockFunction(windowId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_LOCK)

TEST_F(NWebImplTest, OnOnlineRenderToForeground001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnOnlineRenderToForeground()).Times(0);
  nweb_impl_->OnOnlineRenderToForeground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnOnlineRenderToForeground002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnOnlineRenderToForeground()).Times(1);
  nweb_impl_->OnOnlineRenderToForeground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetScrollbarMode) {
  NWebImpl::SetScrollbarMode(OHOS::NWeb::ScrollbarMode::FORCE_DISPLAY_SCROLLBAR);
  nweb_impl_->scrollbarModeChanged_ = false;
  std::shared_ptr<NWebEngineInitArgs> init_args;
  nweb_impl_->InitWebEngineArgs(init_args);
  init_args.reset();
  nweb_impl_->scrollbarModeChanged_ = true;
  nweb_impl_->InitWebEngineArgs(init_args);
  init_args.reset();
  NWebImpl::SetScrollbarMode(OHOS::NWeb::ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR);
  nweb_impl_->scrollbarModeChanged_ = false;
  nweb_impl_->InitWebEngineArgs(init_args);
  init_args.reset();
  nweb_impl_->scrollbarModeChanged_ = true;
  nweb_impl_->InitWebEngineArgs(init_args);
  NWebImpl::SetScrollbarMode(OHOS::NWeb::ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR);
  NWebImpl::SetScrollbarMode(OHOS::NWeb::ScrollbarMode::FORCE_DISPLAY_SCROLLBAR);
  nweb_impl_->scrollbarModeChanged_ = false;
  nweb_impl_->InitWebEngineArgs(init_args);
  init_args.reset();
  nweb_impl_->scrollbarModeChanged_ = true;
  nweb_impl_->InitWebEngineArgs(init_args);
  EXPECT_GT(nweb_impl_->web_engine_args_.size(), 0);
}

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
TEST_F(NWebImplTest, SetDelayDurationForBackgroundTabFreezing001) {
  int64_t delay = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetDelayDurationForBackgroundTabFreezing(delay)).Times(0);
  nweb_impl_->SetDelayDurationForBackgroundTabFreezing(delay);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDelayDurationForBackgroundTabFreezing002) {
  int64_t delay = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetDelayDurationForBackgroundTabFreezing(delay)).Times(1);
  nweb_impl_->SetDelayDurationForBackgroundTabFreezing(delay);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // if BUILDFLAG(ARKWEB_ACTIVE_POLICY)

#if BUILDFLAG(IS_ARKWEB)
TEST_F(NWebImplTest, SetWindowId001) {
  uint32_t window_id = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetWindowId(window_id)).Times(0);
  nweb_impl_->SetWindowId(window_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetWindowId002) {
  uint32_t window_id = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  nweb_impl_->SetWindowId(window_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetFocusWindowId001) {
  uint32_t focus_window_id = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetFocusWindowId(focus_window_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetFocusWindowId002) {
  uint32_t focus_window_id = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetFocusWindowId(focus_window_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetToken001) {
  int64_t token = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetToken(&token)).Times(0);
  nweb_impl_->SetToken(&token);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetToken002) {
  int64_t token = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetToken(&token)).Times(1);
  nweb_impl_->SetToken(&token);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CreateWebPrintDocumentAdapter001) {
  const std::string jobName = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CreateWebPrintDocumentAdapter(jobName)).Times(0);
  nweb_impl_->CreateWebPrintDocumentAdapter(jobName);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CreateWebPrintDocumentAdapter002) {
  const std::string jobName = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CreateWebPrintDocumentAdapter(jobName)).Times(1);
  nweb_impl_->CreateWebPrintDocumentAdapter(jobName);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPrintBackground001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetPrintBackground(enable)).Times(0);
  nweb_impl_->SetPrintBackground(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPrintBackground002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetPrintBackground(enable)).Times(1);
  nweb_impl_->SetPrintBackground(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPrintBackground001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetPrintBackground()).Times(0);
  auto result = nweb_impl_->GetPrintBackground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetPrintBackground002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetPrintBackground()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetPrintBackground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PrecompileJavaScript001) {
  const std::string url = "https://test.com";
  const std::string script = "test";
  std::shared_ptr<CacheOptions> cacheOptions = nullptr;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PrecompileJavaScript(url, script, cacheOptions, callback)).Times(0);
  nweb_impl_->PrecompileJavaScript(url, script, cacheOptions, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrecompileJavaScript002) {
  const std::string url = "https://test.com";
  const std::string script = "test";
  std::shared_ptr<CacheOptions> cacheOptions = nullptr;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PrecompileJavaScript(url, script, cacheOptions, callback)).Times(1);
  nweb_impl_->PrecompileJavaScript(url, script, cacheOptions, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // if BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
TEST_F(NWebImplTest, PrefetchPage001) {
  const std::string url = "https://test.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PrefetchPage(::testing::_)).Times(0);
  nweb_impl_->PrefetchPage(url, additionalHttpHeaders);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrefetchPage002) {
  const std::string url = "https://test.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PrefetchPage(::testing::_)).Times(1);
  nweb_impl_->PrefetchPage(url, additionalHttpHeaders);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_NWEB_EX)
TEST_F(NWebImplTest, CanStoreWebArchive001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CanStoreWebArchive()).Times(0);
  auto result = nweb_impl_->CanStoreWebArchive();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, CanStoreWebArchive002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CanStoreWebArchive()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->CanStoreWebArchive();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ReloadOriginalUrl001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ReloadOriginalUrl()).Times(0);
  nweb_impl_->ReloadOriginalUrl();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ReloadOriginalUrl002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ReloadOriginalUrl()).Times(1);
  nweb_impl_->ReloadOriginalUrl();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBrowserUserAgentString001) {
  const std::string user_agent = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBrowserUserAgentString(user_agent)).Times(0);
  nweb_impl_->SetBrowserUserAgentString(user_agent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBrowserUserAgentString002) {
  const std::string user_agent = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBrowserUserAgentString(user_agent)).Times(1);
  nweb_impl_->SetBrowserUserAgentString(user_agent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutArkWebAppClientExtensionCallback001) {
  std::shared_ptr<ArkWebAppClientExtensionCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterArkWebAppClientExtensionListener(callback)).Times(0);
  nweb_impl_->PutArkWebAppClientExtensionCallback(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutArkWebAppClientExtensionCallback002) {
  std::shared_ptr<ArkWebAppClientExtensionCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterArkWebAppClientExtensionListener(callback)).Times(1);
  nweb_impl_->PutArkWebAppClientExtensionCallback(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebAppClientExtensionCallback001) {
  std::shared_ptr<NWebAppClientExtensionCallback> web_app_client_extension_listener = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterWebAppClientExtensionListener(web_app_client_extension_listener)).Times(0);
  nweb_impl_->PutWebAppClientExtensionCallback(web_app_client_extension_listener);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebAppClientExtensionCallback002) {
  std::shared_ptr<NWebAppClientExtensionCallback> web_app_client_extension_listener = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterWebAppClientExtensionListener(web_app_client_extension_listener)).Times(1);
  nweb_impl_->PutWebAppClientExtensionCallback(web_app_client_extension_listener);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveArkWebAppClientExtensionCallback001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UnRegisterArkWebAppClientExtensionListener()).Times(0);
  nweb_impl_->RemoveArkWebAppClientExtensionCallback();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveArkWebAppClientExtensionCallback002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UnRegisterArkWebAppClientExtensionListener()).Times(1);
  nweb_impl_->RemoveArkWebAppClientExtensionCallback();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveWebAppClientExtensionCallback001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UnRegisterWebAppClientExtensionListener()).Times(0);
  nweb_impl_->RemoveWebAppClientExtensionCallback();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveWebAppClientExtensionCallback002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UnRegisterWebAppClientExtensionListener()).Times(1);
  nweb_impl_->RemoveWebAppClientExtensionCallback();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetImageFromContextNode001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetImageFromContextNode()).Times(0);
  nweb_impl_->GetImageFromContextNode();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetImageFromContextNode002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetImageFromContextNode()).Times(1);
  nweb_impl_->GetImageFromContextNode();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetImageFromCache001) {
  const std::string url = "https://test.com";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetImageFromCacheEx(url)).Times(0);
  nweb_impl_->GetImageFromCache(url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetImageFromCache002) {
  const std::string url = "https://test.com";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetImageFromCacheEx(url)).Times(1);
  nweb_impl_->GetImageFromCache(url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebExtensionCallback001) {
  std::shared_ptr<NWebExtensionCallback> web_extension_listener = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterWebExtensionListener(web_extension_listener)).Times(0);
  nweb_impl_->PutWebExtensionCallback(web_extension_listener);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebExtensionCallback002) {
  std::shared_ptr<NWebExtensionCallback> web_extension_listener = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterWebExtensionListener(web_extension_listener)).Times(1);
  nweb_impl_->PutWebExtensionCallback(web_extension_listener);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveWebExtensionCallback001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UnRegisterWebExtensionListener()).Times(0);
  nweb_impl_->RemoveWebExtensionCallback();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RemoveWebExtensionCallback002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UnRegisterWebExtensionListener()).Times(1);
  nweb_impl_->RemoveWebExtensionCallback();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RunJavaScriptInFrames001) {
  FrameInfos rootFrame;
  IsolatedWorld world;
  RunJavaScriptParam param;
  param.script = "test";
  param.recursive = false;
  param.rootFrame = rootFrame;
  param.world = world;
  param.callbackId = 0;
  OnReceiveValueCallback callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RunJavaScriptInFrames(::testing::_, callback))
      .Times(0);
  nweb_impl_->RunJavaScriptInFrames(param, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RunJavaScriptInFrames002) {
  FrameInfos rootFrame;
  IsolatedWorld world;
  RunJavaScriptParam param;
  param.script = "test";
  param.recursive = false;
  param.rootFrame = rootFrame;
  param.world = world;
  param.callbackId = 0;

  OnReceiveValueCallback callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RunJavaScriptInFrames(::testing::_, callback))
      .Times(0);
  nweb_impl_->RunJavaScriptInFrames(param, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OpenDevtools001) {
  std::unique_ptr<OpenDevToolsParam> param = std::make_unique<OpenDevToolsParam>();
  param->nweb_id = -1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OpenDevtoolsWith(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->OpenDevtools(std::move(param));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OpenDevtools002) {
  std::unique_ptr<OpenDevToolsParam> param = std::make_unique<OpenDevToolsParam>();
  param->nweb_id = -1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OpenDevtoolsWith(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->OpenDevtools(std::move(param));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OpenDevtools003) {
  std::unique_ptr<OpenDevToolsParam> param = std::make_unique<OpenDevToolsParam>();
  int32_t id = 0;
  param->nweb_id = id;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::FromID(id);
  EXPECT_NE(result, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OpenDevtoolsWith(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->OpenDevtools(std::move(param));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseDevtools001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CloseDevtools()).Times(0);
  nweb_impl_->CloseDevtools();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseDevtools002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CloseDevtools()).Times(1);
  nweb_impl_->CloseDevtools();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableAutoResize001) {
  int32_t min_width = 1;
  int32_t min_height = 2;
  int32_t max_width = 3;
  int32_t max_height = 4;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableViewAutoResize(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->EnableAutoResize(min_width, min_height, max_width, max_height);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableAutoResize002) {
  int32_t min_width = 1;
  int32_t min_height = 2;
  int32_t max_width = 3;
  int32_t max_height = 4;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableViewAutoResize(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->EnableAutoResize(min_width, min_height, max_width, max_height);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DisableAutoResize001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, DisableViewAutoResize()).Times(0);
  nweb_impl_->DisableAutoResize();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DisableAutoResize002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, DisableViewAutoResize()).Times(1);
  nweb_impl_->DisableAutoResize();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(NWebImplTest, EnableVideoAssistant001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableVideoAssistant(enable)).Times(0);
  nweb_impl_->EnableVideoAssistant(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableVideoAssistant002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableVideoAssistant(enable)).Times(1);
  nweb_impl_->EnableVideoAssistant(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteVideoAssistantFunction001) {
  const std::string cmd_id = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ExecuteVideoAssistantFunction(cmd_id)).Times(0);
  nweb_impl_->ExecuteVideoAssistantFunction(cmd_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteVideoAssistantFunction002) {
  const std::string cmd_id = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ExecuteVideoAssistantFunction(cmd_id)).Times(1);
  nweb_impl_->ExecuteVideoAssistantFunction(cmd_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CustomWebMediaPlayer001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CustomWebMediaPlayer(enable)).Times(0);
  nweb_impl_->CustomWebMediaPlayer(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CustomWebMediaPlayer002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CustomWebMediaPlayer(enable)).Times(1);
  nweb_impl_->CustomWebMediaPlayer(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerPlay001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerPlay()).Times(0);
  nweb_impl_->WebMediaPlayerControllerPlay();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerPlay002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerPlay()).Times(1);
  nweb_impl_->WebMediaPlayerControllerPlay();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerPause001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerPause()).Times(0);
  nweb_impl_->WebMediaPlayerControllerPause();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerPause002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerPause()).Times(1);
  nweb_impl_->WebMediaPlayerControllerPause();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSeek001) {
  double time = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSeek(time)).Times(0);
  nweb_impl_->WebMediaPlayerControllerSeek(time);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSeek002) {
  double time = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSeek(time)).Times(1);
  nweb_impl_->WebMediaPlayerControllerSeek(time);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetMuted001) {
  bool muted = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetMuted(muted)).Times(0);
  nweb_impl_->WebMediaPlayerControllerSetMuted(muted);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetMuted002) {
  bool muted = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetMuted(muted)).Times(1);
  nweb_impl_->WebMediaPlayerControllerSetMuted(muted);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetPlaybackRate001) {
  double playback_rate = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetPlaybackRate(playback_rate)).Times(0);
  nweb_impl_->WebMediaPlayerControllerSetPlaybackRate(playback_rate);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetPlaybackRate002) {
  double playback_rate = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetPlaybackRate(playback_rate)).Times(1);
  nweb_impl_->WebMediaPlayerControllerSetPlaybackRate(playback_rate);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerExitFullscreen001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerExitFullscreen()).Times(0);
  nweb_impl_->WebMediaPlayerControllerExitFullscreen();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerExitFullscreen002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerExitFullscreen()).Times(1);
  nweb_impl_->WebMediaPlayerControllerExitFullscreen();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetVideoSurface001) {
  int32_t temp = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetVideoSurface(&temp)).Times(0);
  nweb_impl_->WebMediaPlayerControllerSetVideoSurface(&temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetVideoSurface002) {
  int32_t temp = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetVideoSurface(&temp)).Times(1);
  nweb_impl_->WebMediaPlayerControllerSetVideoSurface(&temp);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerDownload001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerDownload()).Times(0);
  nweb_impl_->WebMediaPlayerControllerDownload();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerDownload002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerDownload()).Times(1);
  nweb_impl_->WebMediaPlayerControllerDownload();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetVolume001) {
  double volume = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetVolume(volume)).Times(0);
  nweb_impl_->WebMediaPlayerControllerSetVolume(volume);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerSetVolume002) {
  double volume = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerSetVolume(volume)).Times(1);
  nweb_impl_->WebMediaPlayerControllerSetVolume(volume);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerGetVolume001) {
  double temp = -1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerGetVolume()).Times(0);
  auto result = nweb_impl_->WebMediaPlayerControllerGetVolume();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, WebMediaPlayerControllerGetVolume002) {
  double temp = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebMediaPlayerControllerGetVolume()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->WebMediaPlayerControllerGetVolume();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(NWebImplTest, GetExtensionName001) {
  std::string name;
  bool temp = false;
  auto result = NWebImpl::GetExtensionName(name);
  EXPECT_EQ(result, temp);
  EXPECT_TRUE(name.empty());
}

TEST_F(NWebImplTest, GetExtensionName002) {
  const std::string extension_name = "test";
  NWebImpl::SetExtensionName(extension_name);
  std::string name;
  bool temp = true;
  auto result = NWebImpl::GetExtensionName(name);
  EXPECT_EQ(result, temp);
  EXPECT_EQ(name, extension_name);
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(NWebImplTest, GetSelectedTextFromContextParam001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetSelectedTextFromContextParam()).Times(0);
  auto result = nweb_impl_->GetSelectedTextFromContextParam();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetSelectedTextFromContextParam002) {
  std::string temp = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetSelectedTextFromContextParam()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetSelectedTextFromContextParam();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ShowFreeCopyMenu001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ShowFreeCopyMenu()).Times(0);
  nweb_impl_->ShowFreeCopyMenu();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ShowFreeCopyMenu002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ShowFreeCopyMenu()).Times(1);
  nweb_impl_->ShowFreeCopyMenu();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ShouldShowFreeCopyMenu001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ShouldShowFreeCopyMenu()).Times(0);
  auto result = nweb_impl_->ShouldShowFreeCopyMenu();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ShouldShowFreeCopyMenu002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ShouldShowFreeCopyMenu()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->ShouldShowFreeCopyMenu();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)

#if BUILDFLAG(ARKWEB_ADBLOCK)
TEST_F(NWebImplTest, AddAdsBlockDisallowList001) {
  const std::vector<std::string> domainSuffixes = {"Default", "IncludeSensitive", "Everything"};
  NWebImpl::AddAdsBlockDisallowList(domainSuffixes);
  NWebImpl::AddAdsBlockAllowList(domainSuffixes);
  NWebImpl::RemoveAdsBlockDisallowedList(domainSuffixes);
  NWebImpl::RemoveAdsBlockAllowedList(domainSuffixes);
  NWebImpl::ClearAdsBlockDisallowedList();
  NWebImpl::ClearAdsBlockAllowedList();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, IsAdsBlockEnabledForCurPage001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabledForCurPage()).Times(0);
  auto result = nweb_impl_->IsAdsBlockEnabledForCurPage();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAdsBlockEnabledForCurPage002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabledForCurPage()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsAdsBlockEnabledForCurPage();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAdsBlockEnabled001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabled()).Times(0);
  auto result = nweb_impl_->IsAdsBlockEnabled();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAdsBlockEnabled002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabled()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsAdsBlockEnabled();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, EnableAdsBlock001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableAdsBlock(enable)).Times(0);
  nweb_impl_->EnableAdsBlock(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableAdsBlock002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableAdsBlock(enable)).Times(1);
  nweb_impl_->EnableAdsBlock(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAdBlockEnabledForSite001) {
  bool is_adblock_enabled = false;
  int main_frame_tree_node_id = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id)).Times(0);
  nweb_impl_->SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAdBlockEnabledForSite002) {
  bool is_adblock_enabled = false;
  int main_frame_tree_node_id = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id)).Times(1);
  nweb_impl_->SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, IsAnyNWebAdblockEnabled001) {
  bool temp = false;
  auto result = NWebImpl::IsAnyNWebAdblockEnabled();
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAnyNWebAdblockEnabled002) {
  uint32_t id = 0;
  bool temp = false;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  nweb->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabled()).WillOnce(::testing::Return(temp));
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::IsAnyNWebAdblockEnabled();
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAnyNWebAdblockEnabled003) {
  uint32_t id = 0;
  bool temp = true;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  nweb->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsAdsBlockEnabled()).WillOnce(::testing::Return(temp));
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  NWebImpl::IsAnyNWebAdblockEnabled();
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST_F(NWebImplTest, GetSavePasswordAutomatically001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetSavePasswordAutomatically()).Times(0);
  auto result = nweb_impl_->GetSavePasswordAutomatically();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetSavePasswordAutomatically002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetSavePasswordAutomatically()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetSavePasswordAutomatically();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, SetSavePasswordAutomatically001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetSavePasswordAutomatically(enable)).Times(0);
  nweb_impl_->SetSavePasswordAutomatically(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetSavePasswordAutomatically002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetSavePasswordAutomatically(enable)).Times(1);
  nweb_impl_->SetSavePasswordAutomatically(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetSavePassword001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetSavePassword()).Times(0);
  auto result = nweb_impl_->GetSavePassword();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetSavePassword002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetSavePassword()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetSavePassword();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, SetSavePassword001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetSavePassword(enable)).Times(0);
  nweb_impl_->SetSavePassword(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetSavePassword002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetSavePassword(enable)).Times(1);
  nweb_impl_->SetSavePassword(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SaveOrUpdatePassword001) {
  bool is_update = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SaveOrUpdatePassword(is_update)).Times(0);
  nweb_impl_->SaveOrUpdatePassword(is_update);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SaveOrUpdatePassword002) {
  bool is_update = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SaveOrUpdatePassword(is_update)).Times(1);
  nweb_impl_->SaveOrUpdatePassword(is_update);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD)

#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
TEST_F(NWebImplTest, PasswordSuggestionSelected001) {
  int list_index = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PasswordSuggestionSelected(list_index)).Times(0);
  nweb_impl_->PasswordSuggestionSelected(list_index);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PasswordSuggestionSelected002) {
  int list_index = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PasswordSuggestionSelected(list_index)).Times(1);
  nweb_impl_->PasswordSuggestionSelected(list_index);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
TEST_F(NWebImplTest, SetForceEnableZoom001) {
  bool forceEnableZoom = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetForceEnableZoom(forceEnableZoom)).Times(0);
  nweb_impl_->SetForceEnableZoom(forceEnableZoom);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetForceEnableZoom002) {
  bool forceEnableZoom = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetForceEnableZoom(forceEnableZoom)).Times(1);
  nweb_impl_->SetForceEnableZoom(forceEnableZoom);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetForceEnableZoom001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetForceEnableZoom()).Times(0);
  auto result = nweb_impl_->GetForceEnableZoom();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetForceEnableZoom002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetForceEnableZoom()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetForceEnableZoom();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif  // ARKWEB_EXT_FORCE_ZOOM

TEST_F(NWebImplTest, PutWebDownloadDelegateCallback001) {
  std::shared_ptr<NWebDownloadDelegateCallback> web_download_delegate_listener = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterWebDownloadDelegateListener(web_download_delegate_listener)).Times(0);
  nweb_impl_->PutWebDownloadDelegateCallback(web_download_delegate_listener);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebDownloadDelegateCallback002) {
  std::shared_ptr<NWebDownloadDelegateCallback> web_download_delegate_listener = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterWebDownloadDelegateListener(web_download_delegate_listener)).Times(1);
  nweb_impl_->PutWebDownloadDelegateCallback(web_download_delegate_listener);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StartDownload001) {
  const char* url = "test_url";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StartDownload(url)).Times(0);
  nweb_impl_->StartDownload(url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StartDownload002) {
  const char* url = "test_url";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StartDownload(url)).Times(1);
  nweb_impl_->StartDownload(url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResumeDownload001) {
  std::shared_ptr<NWebDownloadItem> web_download = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResumeDownload(web_download)).Times(0);
  nweb_impl_->ResumeDownload(web_download);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResumeDownload002) {
  std::shared_ptr<NWebDownloadItem> web_download = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResumeDownload(web_download)).Times(1);
  nweb_impl_->ResumeDownload(web_download);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
TEST_F(NWebImplTest, GetDownloadItemState001) {
  long item_id = 1;
  auto temp = NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetDownloadItemState(item_id)).Times(0);
  auto result = nweb_impl_->GetDownloadItemState(item_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetDownloadItemState002) {
  long item_id = 1;
  auto temp = NWebDownloadItemState::PAUSED;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetDownloadItemState(item_id)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetDownloadItemState(item_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)

TEST_F(NWebImplTest, GetFocusedAccessibilityNodeInfo001) {
  int64_t accessibilityId = 0;
  bool isAccessibilityFocus = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetFocusedAccessibilityNodeInfo(accessibilityId, isAccessibilityFocus)).Times(0);
  nweb_impl_->GetFocusedAccessibilityNodeInfo(accessibilityId, isAccessibilityFocus);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetFocusedAccessibilityNodeInfo002) {
  int64_t accessibilityId = 0;
  bool isAccessibilityFocus = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  EXPECT_CALL(*mock_delegate_, GetFocusedAccessibilityNodeInfo(accessibilityId, isAccessibilityFocus)).Times(1);
#endif
  nweb_impl_->GetFocusedAccessibilityNodeInfo(accessibilityId, isAccessibilityFocus);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetAccessibilityNodeInfoById001) {
  int64_t accessibilityId = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeInfoById(accessibilityId)).Times(0);
  nweb_impl_->GetAccessibilityNodeInfoById(accessibilityId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetAccessibilityNodeInfoById002) {
  int64_t accessibilityId = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeInfoById(accessibilityId)).Times(1);
#endif
  nweb_impl_->GetAccessibilityNodeInfoById(accessibilityId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetWebAccessibilityIdByHtmlElementId001) {
  const std::string htmlElementId = "1";
  int64_t temp = -1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetWebAccessibilityIdByHtmlElementId(htmlElementId)).Times(0);
  auto result = nweb_impl_->GetWebAccessibilityIdByHtmlElementId(htmlElementId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetWebAccessibilityIdByHtmlElementId002) {
  const std::string htmlElementId = "1";
  int64_t temp = -1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  temp = 1;
  EXPECT_CALL(*mock_delegate_, GetWebAccessibilityIdByHtmlElementId(htmlElementId)).WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->GetWebAccessibilityIdByHtmlElementId(htmlElementId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetAccessibilityVisible001) {
  int64_t accessibilityId = 0;
  bool temp = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityVisible(accessibilityId)).Times(0);
  auto result = nweb_impl_->GetAccessibilityVisible(accessibilityId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetAccessibilityVisible002) {
  int64_t accessibilityId = 0;
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  temp = false;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityVisible(accessibilityId)).WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->GetAccessibilityVisible(accessibilityId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetAccessibilityNodeInfoByFocusMove001) {
  int64_t accessibilityId = 0;
  int32_t direction = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeInfoByFocusMove(accessibilityId, direction)).Times(0);
  nweb_impl_->GetAccessibilityNodeInfoByFocusMove(accessibilityId, direction);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetAccessibilityNodeInfoByFocusMove002) {
  int64_t accessibilityId = 0;
  int32_t direction = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeInfoByFocusMove(accessibilityId, direction)).Times(1);
#endif
  nweb_impl_->GetAccessibilityNodeInfoByFocusMove(accessibilityId, direction);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAccessibilityState001) {
  bool state = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetAccessibilityState(::testing::_)).Times(0);
  nweb_impl_->SetAccessibilityState(state);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAccessibilityState002) {
  bool state = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  EXPECT_CALL(*mock_delegate_, SetAccessibilityState(::testing::_)).Times(1);
#endif
  nweb_impl_->SetAccessibilityState(state);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAccessibilityState003) {
  bool state = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  EXPECT_CALL(*mock_delegate_, SetAccessibilityState(::testing::_)).Times(1);
#endif
  nweb_impl_->SetAccessibilityState(state);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetAccessibilityNodeRectById001) {
  int64_t accessibilityId = 0;
  int32_t width = 0;
  int32_t height = 0;
  int32_t offsetX = 0;
  int32_t offsetY = 0;
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeRectById(accessibilityId, &width, &height, &offsetX, &offsetY))
      .Times(0);
  auto result = nweb_impl_->GetAccessibilityNodeRectById(accessibilityId, &width, &height, &offsetX, &offsetY);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetAccessibilityNodeRectById002) {
  int64_t accessibilityId = 0;
  int32_t width = 0;
  int32_t height = 0;
  int32_t offsetX = 0;
  int32_t offsetY = 0;
  bool temp = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  temp = true;
  EXPECT_CALL(*mock_delegate_, GetAccessibilityNodeRectById(accessibilityId, &width, &height, &offsetX, &offsetY))
      .WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->GetAccessibilityNodeRectById(accessibilityId, &width, &height, &offsetX, &offsetY);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
TEST_F(NWebImplTest, SetBrowserZoomLevel001) {
  double zoom_factor = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBrowserZoomLevel(zoom_factor)).Times(0);
  nweb_impl_->SetBrowserZoomLevel(zoom_factor);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBrowserZoomLevel002) {
  double zoom_factor = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBrowserZoomLevel(zoom_factor)).Times(1);
  nweb_impl_->SetBrowserZoomLevel(zoom_factor);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetBrowserZoomLevel001) {
  double temp = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetBrowserZoomLevel()).Times(0);
  auto result = nweb_impl_->GetBrowserZoomLevel();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetBrowserZoomLevel002) {
  double temp = 2.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetBrowserZoomLevel()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetBrowserZoomLevel();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)

#if BUILDFLAG(ARKWEB_CRASHPAD)
TEST_F(NWebImplTest, SetDefaultCrashpadLogPath001) {
  const std::string crashpad_log_path = "test";
  NWebImpl::SetDefaultCrashpadLogPath(crashpad_log_path);
  const std::string feedbacklogs_crash_path = "test";
  NWebImpl::SetDefaultFeedbacklogsCrashPath(feedbacklogs_crash_path);
  EXPECT_NE(nweb_impl_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)

#if BUILDFLAG(ARKWEB_DISCARD)
TEST_F(NWebImplTest, Discard001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, Discard()).Times(0);
  auto result = nweb_impl_->Discard();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Discard002) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Discard()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->Discard();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Discard003) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Discard()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->Discard();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Restore001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, Restore()).Times(0);
  auto result = nweb_impl_->Restore();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Restore002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Restore()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->Restore();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_DISCARD)

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
TEST_F(NWebImplTest, WebExtensionContextMenuIsIframe001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionContextMenuIsIframe()).Times(0);
  auto result = nweb_impl_->WebExtensionContextMenuIsIframe();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, WebExtensionContextMenuIsIframe002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionContextMenuIsIframe()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->WebExtensionContextMenuIsIframe();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, WebExtensionContextMenuReloadFocusedFrame001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionContextMenuReloadFocusedFrame()).Times(0);
  nweb_impl_->WebExtensionContextMenuReloadFocusedFrame();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionContextMenuReloadFocusedFrame002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionContextMenuReloadFocusedFrame()).Times(1);
  nweb_impl_->WebExtensionContextMenuReloadFocusedFrame();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(NWebImplTest, WebExtensionContextMenuGetFocusedFrameInfo001) {
  int32_t id = 0;
  std::string url = "https://example.com";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebExtensionContextMenuGetFocusedFrameInfo(id, url))
      .Times(0);
  nweb_impl_->WebExtensionContextMenuGetFocusedFrameInfo(id, url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionContextMenuGetFocusedFrameInfo002) {
  int32_t id = 0;
  std::string url = "https://example.com";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              WebExtensionContextMenuGetFocusedFrameInfo(id, url))
      .Times(1);
  nweb_impl_->WebExtensionContextMenuGetFocusedFrameInfo(id, url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
TEST_F(NWebImplTest, UpdateBrowserControlsState001) {
  int constraints = 0;
  int current = 0;
  bool animate = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UpdateBrowserControlsState(constraints, current, animate)).Times(0);
  nweb_impl_->UpdateBrowserControlsState(constraints, current, animate);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateBrowserControlsState002) {
  int constraints = 0;
  int current = 0;
  bool animate = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UpdateBrowserControlsState(constraints, current, animate)).Times(1);
  nweb_impl_->UpdateBrowserControlsState(constraints, current, animate);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateBrowserControlsHeight001) {
  int height = 0;
  bool animate = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UpdateBrowserControlsHeight(height, animate)).Times(0);
  nweb_impl_->UpdateBrowserControlsHeight(height, animate);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateBrowserControlsHeight002) {
  int height = 0;
  bool animate = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UpdateBrowserControlsHeight(height, animate)).Times(1);
  nweb_impl_->UpdateBrowserControlsHeight(height, animate);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)

TEST_F(NWebImplTest, GetIsEditTextType001) {
  bool temp = false;
  std::shared_ptr<HitTestResult> hit = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastHitTestResult()).WillOnce(::testing::Return(hit));
  auto result = nweb_impl_->GetIsEditTextType();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetIsEditTextType002) {
  std::shared_ptr<HitTestResult> hit = std::make_shared<HitTestResultImpl>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastHitTestResult()).WillOnce(::testing::Return(hit));
  nweb_impl_->inputmethod_handler_ = nullptr;
  auto result = nweb_impl_->GetIsEditTextType();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_FALSE(result);
}

TEST_F(NWebImplTest, GetIsEditTextType003) {
  bool temp = true;
  std::shared_ptr<HitTestResult> hit = std::make_shared<HitTestResultImpl>();
  hit->SetType(HitTestResult::EDIT_TEXT_TYPE);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastHitTestResult()).WillOnce(::testing::Return(hit));
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  auto result = nweb_impl_->GetIsEditTextType();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetIsEditTextType004) {
  bool temp = false;
  std::shared_ptr<HitTestResult> hit = std::make_shared<HitTestResultImpl>();
  hit->SetType(HitTestResult::SRC_IMAGE_ANCHOR_TYPE);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastHitTestResult()).WillOnce(::testing::Return(hit));
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  auto result = nweb_impl_->GetIsEditTextType();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, NeedSoftKeyboard001) {
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*inputmethod_handler, GetIsEditableNode()).Times(0);
  nweb_impl_->NeedSoftKeyboard();
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, NeedSoftKeyboard002) {
  bool temp = false;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  EXPECT_CALL(*inputmethod_handler, GetIsEditableNode()).WillOnce(::testing::Return(temp));
  nweb_impl_->NeedSoftKeyboard();
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, NeedSoftKeyboard003) {
  bool temp = true;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  EXPECT_CALL(*inputmethod_handler, GetIsEditableNode()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->NeedSoftKeyboard();
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
TEST_F(NWebImplTest, GrantV2001) {
  int32_t resourse_id = 0;
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGrant(resourse_id, nweb_request_key)).Times(0);
  nweb_impl_->GrantV2(resourse_id, nweb_request_key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GrantV2002) {
  int32_t resourse_id = 0;
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGrant(resourse_id, nweb_request_key)).Times(1);
  nweb_impl_->GrantV2(resourse_id, nweb_request_key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DenyV2001) {
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PermissionRequestDeny(nweb_request_key)).Times(0);
  nweb_impl_->DenyV2(nweb_request_key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DenyV2002) {
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PermissionRequestDeny(nweb_request_key)).Times(1);
  nweb_impl_->DenyV2(nweb_request_key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PermissionRequestDelete001) {
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PermissionRequestDelete(nweb_request_key)).Times(0);
  nweb_impl_->PermissionRequestDelete(nweb_request_key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PermissionRequestDelete002) {
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PermissionRequestDelete(nweb_request_key)).Times(1);
  nweb_impl_->PermissionRequestDelete(nweb_request_key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetOriginV2001) {
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGetOrigin(nweb_request_key)).Times(0);
  auto result = nweb_impl_->GetOriginV2(nweb_request_key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetOriginV2002) {
  std::string temp = "test";
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGetOrigin(nweb_request_key)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetOriginV2(nweb_request_key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetResourceIdV2001) {
  int32_t temp = -1;
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGetResourceId(nweb_request_key)).Times(0);
  auto result = nweb_impl_->GetResourceIdV2(nweb_request_key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetResourceIdV2002) {
  int32_t temp = 1;
  int nweb_request_key = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PermissionRequestGetResourceId(nweb_request_key)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetResourceIdV2(nweb_request_key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_EXT_PERMISSION)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
TEST_F(NWebImplTest, InsertBackForwardEntry001) {
  int temp = -2;
  int offset = 0;
  const std::string url = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InsertBackForwardEntry(offset, url)).Times(0);
  auto result = nweb_impl_->InsertBackForwardEntry(offset, url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, InsertBackForwardEntry002) {
  int temp = 0;
  int offset = 0;
  const std::string url = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, InsertBackForwardEntry(offset, url)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->InsertBackForwardEntry(offset, url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, UpdateNavigationEntryUrl001) {
  int temp = -2;
  int index = 0;
  const std::string url = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UpdateNavigationEntryUrl(index, url)).Times(0);
  auto result = nweb_impl_->UpdateNavigationEntryUrl(index, url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, UpdateNavigationEntryUrl002) {
  int temp = 0;
  int index = 0;
  const std::string url = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UpdateNavigationEntryUrl(index, url)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->UpdateNavigationEntryUrl(index, url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ClearForwardList001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ClearForwardList()).Times(0);
  nweb_impl_->ClearForwardList();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearForwardList002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ClearForwardList()).Times(1);
  nweb_impl_->ClearForwardList();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

TEST_F(NWebImplTest, TestGetNWebExistingId) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  int32_t existingId = 0;
  auto result = NWebImpl::GetNWeb(existingId);
  EXPECT_NE(result, nullptr);
}

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
TEST_F(NWebImplTest, SetWebTag001) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  int32_t nweb_id = 0;
  const char* web_tag = "test_url";
  NWebImpl::SetWebTag(nweb_id, web_tag);
}

TEST_F(NWebImplTest, SetWebTag002) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  int32_t nweb_id = 1;
  const char* web_tag = "test_url";
  NWebImpl::SetWebTag(nweb_id, web_tag);
}

TEST_F(NWebImplTest, SetWebTag003) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  int32_t nweb_id = -1;
  const char* web_tag = "test_url";
  NWebImpl::SetWebTag(nweb_id, web_tag);
}
#endif // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
TEST_F(NWebImplTest, IsSafeBrowsingEnabled001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsSafeBrowsingEnabled()).Times(0);
  auto result = nweb_impl_->IsSafeBrowsingEnabled();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsSafeBrowsingEnabled002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsSafeBrowsingEnabled()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsSafeBrowsingEnabled();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, EnableSafeBrowsing001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableSafeBrowsing(enable)).Times(0);
  nweb_impl_->EnableSafeBrowsing(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableSafeBrowsing002) {
  bool enable = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableSafeBrowsing(enable)).Times(1);
  nweb_impl_->EnableSafeBrowsing(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableSafeBrowsingDetection001) {
  bool enable = false;
  bool strictMode = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableSafeBrowsingDetection(enable, strictMode)).Times(0);
  nweb_impl_->EnableSafeBrowsingDetection(enable, strictMode);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableSafeBrowsingDetection002) {
  bool enable = false;
  bool strictMode = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableSafeBrowsingDetection(enable, strictMode)).Times(1);
  nweb_impl_->EnableSafeBrowsingDetection(enable, strictMode);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnSafeBrowsingDetectionResult001) {
  int code = 0;
  int policy = 0;
  const std::string mappingType = "type";
  const std::string url = "url";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnSafeBrowsingDetectionResult(code, policy, mappingType, url)).Times(0);
  nweb_impl_->OnSafeBrowsingDetectionResult(code, policy, mappingType, url);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnSafeBrowsingDetectionResult002) {
  int code = 0;
  int policy = 0;
  const std::string mappingType = "type";
  const std::string url = "url";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnSafeBrowsingDetectionResult(code, policy, mappingType, url)).Times(1);
  nweb_impl_->OnSafeBrowsingDetectionResult(code, policy, mappingType, url);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(NWebImplTest, StartCamera001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StartCamera()).Times(0);
  nweb_impl_->StartCamera();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StartCamera002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StartCamera()).Times(1);
  nweb_impl_->StartCamera();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StopCamera001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StopCamera()).Times(0);
  nweb_impl_->StopCamera();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, StopCamera002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StopCamera()).Times(1);
  nweb_impl_->StopCamera();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseCamera001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CloseCamera()).Times(0);
  nweb_impl_->CloseCamera();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CloseCamera002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CloseCamera()).Times(1);
  nweb_impl_->CloseCamera();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_WEBRTC)

TEST_F(NWebImplTest, GetSecurityLevel001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  int temp = 0;
#if BUILDFLAG(IS_OHOS)
  EXPECT_CALL(*mock_delegate_, GetSecurityLevel()).Times(0);
#endif
  auto result = nweb_impl_->GetSecurityLevel();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetSecurityLevel002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int temp = 0;
#if BUILDFLAG(IS_OHOS)
  temp = 3;
  EXPECT_CALL(*mock_delegate_, GetSecurityLevel()).WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->GetSecurityLevel();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_ITP)
TEST_F(NWebImplTest, EnableIntelligentTrackingPrevention001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableIntelligentTrackingPrevention(enable)).Times(0);
  nweb_impl_->EnableIntelligentTrackingPrevention(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableIntelligentTrackingPrevention002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableIntelligentTrackingPrevention(enable)).Times(1);
  nweb_impl_->EnableIntelligentTrackingPrevention(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, IsIntelligentTrackingPreventionEnabled001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsIntelligentTrackingPreventionEnabled()).Times(0);
  auto result = nweb_impl_->IsIntelligentTrackingPreventionEnabled();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsIntelligentTrackingPreventionEnabled002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsIntelligentTrackingPreventionEnabled()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsIntelligentTrackingPreventionEnabled();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAnyNWebIntelligentTrackingPreventionEnabled001) {
  uint32_t id = 0;
  bool temp = false;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  nweb->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsIntelligentTrackingPreventionEnabled()).WillOnce(::testing::Return(temp));
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::IsAnyNWebIntelligentTrackingPreventionEnabled();
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsAnyNWebIntelligentTrackingPreventionEnabled002) {
  uint32_t id = 0;
  bool temp = true;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  nweb->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsIntelligentTrackingPreventionEnabled()).WillOnce(::testing::Return(temp));
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::IsAnyNWebIntelligentTrackingPreventionEnabled();
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_ITP)

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebImplTest, GetSelectInfo001) {
  std::string select_info_empty = "";
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  ON_CALL(*inputmethod_handler, GetSelectInfo()).WillByDefault(::testing::Return(select_info_empty));
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
#if BUILDFLAG(ARKWEB_AI)
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetDataDetectorSelectText()).WillOnce(::testing::Return(select_info_empty));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
#endif // BUILDFLAG(ARKWEB_AI)
  nweb_impl_->GetSelectInfo();
}

TEST_F(NWebImplTest, GetSelectInfo002) {
  std::string select_info = "test";
  std::string select_info_empty = "";
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  ON_CALL(*inputmethod_handler, GetSelectInfo()).WillByDefault(::testing::Return(select_info_empty));
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
#if BUILDFLAG(ARKWEB_AI)
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetDataDetectorSelectText()).WillOnce(::testing::Return(select_info));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
#endif // BUILDFLAG(ARKWEB_AI)
  auto result = nweb_impl_->GetSelectInfo();
  EXPECT_EQ(result, select_info);
}

TEST_F(NWebImplTest, IsClipboardSitePermissionEnabled001) {
  bool result = nweb_impl_->IsClipboardSitePermissionEnabled();
  EXPECT_EQ(result, false);
  nweb_impl_->SetClipboardSitePermissionEnabled(true);
  result = nweb_impl_->IsClipboardSitePermissionEnabled();
  EXPECT_EQ(result, true);
}
#endif // BUILDFLAG(ARKWEB_CLIPBOARD)

TEST_F(NWebImplTest, OnCreateNativeMediaPlayer001) {
  std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterOnCreateNativeMediaPlayerListener(std::move(callback))).Times(0);
  nweb_impl_->OnCreateNativeMediaPlayer(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnCreateNativeMediaPlayer002) {
  std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  EXPECT_CALL(*mock_delegate_, RegisterOnCreateNativeMediaPlayerListener(std::move(callback))).Times(1);
#endif
  nweb_impl_->OnCreateNativeMediaPlayer(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
TEST_F(NWebImplTest, GetLastJavascriptProxyCallingFrameUrl001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetLastJavascriptProxyCallingFrameUrl()).Times(0);
  auto result = nweb_impl_->GetLastJavascriptProxyCallingFrameUrl();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetLastJavascriptProxyCallingFrameUrl002) {
  std::string temp = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastJavascriptProxyCallingFrameUrl()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetLastJavascriptProxyCallingFrameUrl();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)

TEST_F(NWebImplTest, ScaleGestureChangeV2001) {
  int temp = -1;
  int type = 0;
  double scale = 1;
  double originScale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScaleGestureChangeV2(type, scale, originScale, centerX, centerY)).Times(0);
  auto ret = nweb_impl_->ScaleGestureChangeV2(type, scale, originScale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ScaleGestureChangeV2002) {
  int temp = 1;
  int type = 0;
  double scale = 1;
  double originScale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScaleGestureChangeV2(type, scale, originScale, centerX, centerY))
      .WillOnce(::testing::Return(temp));
  auto ret = nweb_impl_->ScaleGestureChangeV2(type, scale, originScale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, SuggestionSelected001) {
  int index = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SuggestionSelected(index)).Times(0);
  nweb_impl_->SuggestionSelected(index);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SuggestionSelected002) {
  int index = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SuggestionSelected(index)).Times(1);
  nweb_impl_->SuggestionSelected(index);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_AI)
TEST_F(NWebImplTest, OnDestroyImageAnalyzerOverlay001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnDestroyImageAnalyzerOverlay()).Times(0);
  nweb_impl_->OnDestroyImageAnalyzerOverlay();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnDestroyImageAnalyzerOverlay002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnDestroyImageAnalyzerOverlay()).Times(1);
  nweb_impl_->OnDestroyImageAnalyzerOverlay();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RunDataDetectorJS001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RunDataDetectorJS()).Times(0);
  nweb_impl_->RunDataDetectorJS();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RunDataDetectorJS002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RunDataDetectorJS()).Times(1);
  nweb_impl_->RunDataDetectorJS();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDataDetectorEnable001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetDataDetectorEnable(enable)).Times(0);
  nweb_impl_->SetDataDetectorEnable(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDataDetectorEnable002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetDataDetectorEnable(enable)).Times(1);
  nweb_impl_->SetDataDetectorEnable(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnDataDetectorSelectText001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnDataDetectorSelectText()).Times(0);
  nweb_impl_->OnDataDetectorSelectText();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnDataDetectorSelectText002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnDataDetectorSelectText()).Times(1);
  nweb_impl_->OnDataDetectorSelectText();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnDataDetectorCopy001) {
  const std::vector<std::string> recordMix = {"Default", "IncludeSensitive", "Everything"};
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnDataDetectorCopy(recordMix)).Times(0);
  nweb_impl_->OnDataDetectorCopy(recordMix);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnDataDetectorCopy002) {
  const std::vector<std::string> recordMix = {"Default", "IncludeSensitive", "Everything"};
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnDataDetectorCopy(recordMix)).Times(1);
  nweb_impl_->OnDataDetectorCopy(recordMix);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
TEST_F(NWebImplTest, OnSafeInsetsChange001) {
  int left = 0;
  int top = 0;
  int right = 0; 
  int bottom = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnSafeInsetsChange(left, top, right, bottom)).Times(0);
  nweb_impl_->OnSafeInsetsChange(left, top, right, bottom);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnSafeInsetsChange002) {
  int left = 0;
  int top = 0;
  int right = 0; 
  int bottom = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnSafeInsetsChange(left, top, right, bottom)).Times(1);
  nweb_impl_->OnSafeInsetsChange(left, top, right, bottom);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
TEST_F(NWebImplTest, WebPageSnapshot001) {
  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  bool temp = true;

  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebPageSnapshot(
      id, ::testing::_, width, height, ::testing::_)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->WebPageSnapshot(
      id, PixelUnit::PX, width, height,
      [](const char* str, bool is, float flo, void* ptr, int a, int b) {});
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)

TEST_F(NWebImplTest, SetUrlTrustListWithErrMsg001) {
  const std::string urlTrustList = "test";
  std::string detailErrMsg = "message";
  int temp = -1;
#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  temp = -2;
#endif
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetUrlTrustListWithErrMsg(urlTrustList, true, false, detailErrMsg)).Times(0);
  auto result = nweb_impl_->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsg);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, SetUrlTrustListWithErrMsg002) {
  const std::string urlTrustList = "test";
  std::string detailErrMsg = "message";
  int temp = -1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  temp = 1;
  EXPECT_CALL(*mock_delegate_, SetUrlTrustListWithErrMsg(urlTrustList,
      true, false, detailErrMsg)).WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsg);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
TEST_F(NWebImplTest, SetPathAllowingUniversalAccess001) {
  const std::vector<std::string> pathList = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> moduleName = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> excludedPathList = {};
  std::string errorPath = "path";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetPathAllowingUniversalAccess(pathList, excludedPathList)).Times(0);
  nweb_impl_->SetPathAllowingUniversalAccess(pathList, moduleName, errorPath);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPathAllowingUniversalAccess002) {
  const std::vector<std::string> pathList = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> moduleName = {"Default", "IncludeSensitive", "Everything"};
  std::string errorPath = "path";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetPathAllowingUniversalAccess(pathList, moduleName, errorPath);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPathAllowingUniversalAccess003) {
  const std::vector<std::string> pathList = {};
  const std::vector<std::string> moduleName = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> excludedPathList = {};
  std::string errorPath = "path";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetPathAllowingUniversalAccess(pathList, excludedPathList)).Times(1);
  nweb_impl_->SetPathAllowingUniversalAccess(pathList, moduleName, errorPath);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrerenderPage001) {
  const std::string url = "url";
  const std::string additional_headers = "test";
  int temp = 17100001;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, PrerenderPage(url, additional_headers)).Times(0);
  auto result = nweb_impl_->PrerenderPage(url, additional_headers);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PrerenderPage002) {
  const std::string url = "url";
  const std::string additional_headers = "test";
  int temp = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, PrerenderPage(url, additional_headers)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->PrerenderPage(url, additional_headers);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, CancelAllPrerendering001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CancelAllPrerendering()).Times(0);
  nweb_impl_->CancelAllPrerendering();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CancelAllPrerendering002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CancelAllPrerendering()).Times(1);
  nweb_impl_->CancelAllPrerendering();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_NETWORK_LOAD)

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
TEST_F(NWebImplTest, DisallowSandboxFileAccessFromFileUrl001) {
  bool disallow = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, DisallowSandboxFileAccessFromFileUrl(disallow)).Times(0);
  nweb_impl_->DisallowSandboxFileAccessFromFileUrl(disallow);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DisallowSandboxFileAccessFromFileUrl002) {
  bool disallow = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, DisallowSandboxFileAccessFromFileUrl(disallow)).Times(1);
  nweb_impl_->DisallowSandboxFileAccessFromFileUrl(disallow);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

TEST_F(NWebImplTest, PerformActionV2001) {
  int64_t accessibilityId = 0;
  uint32_t action = 0;
  const std::map<std::string, std::string> actionArguments = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ExecuteAction(accessibilityId, action, actionArguments)).Times(0);
  auto result = nweb_impl_->PerformActionV2(accessibilityId, action, actionArguments);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PerformActionV2002) {
  int64_t accessibilityId = 0;
  uint32_t action = 0;
  const std::map<std::string, std::string> actionArguments = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  bool temp = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  temp = true;
  EXPECT_CALL(*mock_delegate_, ExecuteAction(accessibilityId, action, actionArguments))
      .WillOnce(::testing::Return(temp));
#endif
  auto result = nweb_impl_->PerformActionV2(accessibilityId, action, actionArguments);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
TEST_F(NWebImplTest, SendAccessibilityHoverEvent001) {
  int32_t x = 0;
  int32_t y = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendAccessibilityHoverEvent(x, y, true)).Times(0);
  nweb_impl_->SendAccessibilityHoverEvent(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendAccessibilityHoverEvent002) {
  int32_t x = 0;
  int32_t y = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SendAccessibilityHoverEvent(x, y, true)).Times(1);
  nweb_impl_->SendAccessibilityHoverEvent(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendAccessibilityHoverEventV2001) {
  int32_t x = 0;
  int32_t y = 0;
  bool isHoverEnter = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendAccessibilityHoverEvent(x, y, isHoverEnter)).Times(0);
  nweb_impl_->SendAccessibilityHoverEventV2(x, y, isHoverEnter);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendAccessibilityHoverEventV2002) {
  int32_t x = 0;
  int32_t y = 0;
  bool isHoverEnter = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SendAccessibilityHoverEvent(x, y, isHoverEnter)).Times(1);
  nweb_impl_->SendAccessibilityHoverEventV2(x, y, isHoverEnter);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_ACCESSIBILITY)

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
TEST_F(NWebImplTest, EnableMixedContentAutoUpgrades001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableMixedContentAutoUpgrades(enable)).Times(0);
  nweb_impl_->EnableMixedContentAutoUpgrades(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableMixedContentAutoUpgrades002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableMixedContentAutoUpgrades(enable)).Times(1);
  nweb_impl_->EnableMixedContentAutoUpgrades(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, IsMixedContentAutoUpgradesEnabled001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsMixedContentAutoUpgradesEnabled()).Times(0);
  auto result = nweb_impl_->IsMixedContentAutoUpgradesEnabled();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsMixedContentAutoUpgradesEnabled002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsMixedContentAutoUpgradesEnabled()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsMixedContentAutoUpgradesEnabled();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_MIXED_CONTENT)

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_F(NWebImplTest, SetUsageScenario001) {
  int32_t usage_scenario = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetUsageScenario(usage_scenario)).Times(1);
  nweb_impl_->SetUsageScenario(usage_scenario);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetUsageScenario001) {
  int32_t temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetUsageScenario()).Times(0);
  auto result = nweb_impl_->GetUsageScenario();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetUsageScenario002) {
  int32_t temp = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetUsageScenario()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetUsageScenario();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(NWebImplTest, WebExtensionTabUpdated001) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo = std::make_unique<NWebExtensionTabChangeInfo>();
  std::unique_ptr<NWebExtensionTab> tab = std::make_unique<NWebExtensionTab>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabUpdated(tab_id,
      ::testing::A<std::unique_ptr<NWebExtensionTabChangeInfo>>(),
      ::testing::A<std::unique_ptr<NWebExtensionTab>>())).Times(0);
  nweb_impl_->WebExtensionTabUpdated(tab_id, std::move(changeInfo), std::move(tab));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabUpdated002) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo = std::make_unique<NWebExtensionTabChangeInfo>();
  std::unique_ptr<NWebExtensionTab> tab = std::make_unique<NWebExtensionTab>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabUpdated(tab_id,
      ::testing::A<std::unique_ptr<NWebExtensionTabChangeInfo>>(),
      ::testing::A<std::unique_ptr<NWebExtensionTab>>())).Times(1);
  nweb_impl_->WebExtensionTabUpdated(tab_id, std::move(changeInfo), std::move(tab));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabRemoved001) {
  int tab_id = 0;
  bool isWindowClosing = false;
  int windowId = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabRemoved(tab_id, isWindowClosing, windowId)).Times(0);
  nweb_impl_->WebExtensionTabRemoved(tab_id, isWindowClosing, windowId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabRemoved002) {
  int tab_id = 0;
  bool isWindowClosing = false;
  int windowId = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabRemoved(tab_id, isWindowClosing, windowId)).Times(1);
  nweb_impl_->WebExtensionTabRemoved(tab_id, isWindowClosing, windowId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabAttached001) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo = std::make_unique<NWebExtensionTabAttachInfo>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabAttached(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->WebExtensionTabAttached(tab_id, std::move(attachInfo));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabAttached002) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo = std::make_unique<NWebExtensionTabAttachInfo>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabAttached(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->WebExtensionTabAttached(tab_id, std::move(attachInfo));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabDetached001) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo = std::make_unique<NWebExtensionTabDetachInfo>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabDetached(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->WebExtensionTabDetached(tab_id, std::move(detachInfo));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabDetached002) {
  int tab_id = 0;
  std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo = std::make_unique<NWebExtensionTabDetachInfo>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabDetached(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->WebExtensionTabDetached(tab_id, std::move(detachInfo));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabMoved001) {
  int32_t tab_id = 0;
  std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo = std::make_unique<NWebExtensionTabMoveInfo>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabMoved(tab_id, ::testing::_)).Times(0);
  nweb_impl_->WebExtensionTabMoved(tab_id, std::move(moveInfo));
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabMoved002) {
  int32_t tab_id = 0;
  std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo = std::make_unique<NWebExtensionTabMoveInfo>();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabMoved(tab_id, ::testing::_)).Times(1);
  nweb_impl_->WebExtensionTabMoved(tab_id, std::move(moveInfo));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabReplaced001) {
  int32_t addedTabId = 0;
  int32_t removedTabId = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabReplaced(addedTabId, removedTabId)).Times(0);
  nweb_impl_->WebExtensionTabReplaced(addedTabId, removedTabId);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionTabReplaced002) {
  int32_t addedTabId = 0;
  int32_t removedTabId = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionTabReplaced(addedTabId, removedTabId)).Times(1);
  nweb_impl_->WebExtensionTabReplaced(addedTabId, removedTabId);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionSetViewType001) {
  int32_t type = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebExtensionSetViewType(type)).Times(0);
  nweb_impl_->WebExtensionSetViewType(type);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebExtensionSetViewType002) {
  int32_t type = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, WebExtensionSetViewType(type)).Times(1);
  nweb_impl_->WebExtensionSetViewType(type);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_BFCACHE)
TEST_F(NWebImplTest, SetBackForwardCacheOptions001) {
  int32_t size = 0;
  int32_t timeToLive = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBackForwardCacheOptions(size, timeToLive)).Times(0);
  nweb_impl_->SetBackForwardCacheOptions(size, timeToLive);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBackForwardCacheOptions002) {
  int32_t size = 0;
  int32_t timeToLive = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBackForwardCacheOptions(size, timeToLive)).Times(1);
  nweb_impl_->SetBackForwardCacheOptions(size, timeToLive);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetMediaResumeFromBFCachePage001) {
  bool resume = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetMediaResumeFromBFCachePage(resume)).Times(0);
  nweb_impl_->SetMediaResumeFromBFCachePage(resume);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetMediaResumeFromBFCachePage002) {
  bool resume = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetMediaResumeFromBFCachePage(resume)).Times(1);
  nweb_impl_->SetMediaResumeFromBFCachePage(resume);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)
TEST_F(NWebImplTest, TrimMemoryByPressureLevel001) {
  int32_t memoryLevel = 0;
  NWebImpl::TrimMemoryByPressureLevel(memoryLevel);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TrimMemoryByPressureLevel002) {
  int32_t memoryLevel = 1;
  NWebImpl::TrimMemoryByPressureLevel(memoryLevel);
  EXPECT_NE(nweb_impl_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)

TEST_F(NWebImplTest, SetSurfaceDensity001) {
  const double density = 1.0;
  nweb_impl_->inputmethod_handler_ = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetSurfaceDensity(density)).Times(0);
  nweb_impl_->SetSurfaceDensity(density);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, SetSurfaceDensity002) {
  const double density = 1.0;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetSurfaceDensity(density)).Times(0);
  nweb_impl_->SetSurfaceDensity(density);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, SetSurfaceDensity003) {
  const double density = 1.0;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetSurfaceDensity(density)).Times(1);
  nweb_impl_->SetSurfaceDensity(density);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(NWebImplTest, SetTransformHint001) {
  uint32_t rotation = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetTransformHint(rotation)).Times(0);
  nweb_impl_->SetTransformHint(rotation);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetTransformHint002) {
  uint32_t rotation = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetTransformHint(rotation)).Times(1);
  nweb_impl_->SetTransformHint(rotation);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
TEST_F(NWebImplTest, SetPopupSurface001) {
  void* popupSurface = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  nweb_impl_->SetPopupSurface(&popupSurface);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPopupSurface002) {
  void* popupSurface = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  nweb_impl_->SetPopupSurface(&popupSurface);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
TEST_F(NWebImplTest, MaximizeResize001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, MaximizeResize()).Times(0);
  nweb_impl_->MaximizeResize();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, MaximizeResize002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, MaximizeResize()).Times(1);
  nweb_impl_->MaximizeResize();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
TEST_F(NWebImplTest, DragResize001) {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t pre_height = 0;
  uint32_t pre_width = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->DragResize(width, height, pre_height, pre_width);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, DragResize002) {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t pre_height = 0;
  uint32_t pre_width = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->DragResize(width, height, pre_height, pre_width);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, DragResize003) {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t pre_height = 0;
  uint32_t pre_width = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  nweb_impl_->DragResize(width, height, pre_height, pre_width);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, DragResize004) {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t pre_height = 1;
  uint32_t pre_width = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  nweb_impl_->DragResize(width, height, pre_height, pre_width);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_DRAG_RESIZE)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
TEST_F(NWebImplTest, StopScreenCapture001) {
  int32_t nweb_id = 0;
  const char* session_id = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StopScreenCapture(nweb_id, ::testing::_)).Times(0);
  nweb_impl_->StopScreenCapture(nweb_id, session_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(session_id, nullptr);
}

TEST_F(NWebImplTest, StopScreenCapture002) {
  int32_t nweb_id = 0;
  const char* session_id = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, StopScreenCapture(nweb_id, ::testing::_)).Times(0);
  nweb_impl_->StopScreenCapture(nweb_id, session_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(session_id, nullptr);
}

TEST_F(NWebImplTest, StopScreenCapture003) {
  int32_t nweb_id = 0;
  const char* session_id = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, StopScreenCapture(nweb_id, ::testing::_)).Times(1);
  nweb_impl_->StopScreenCapture(nweb_id, session_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(session_id, nullptr);
}

TEST_F(NWebImplTest, SetScreenCapturePickerShow001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetScreenCapturePickerShow()).Times(0);
  nweb_impl_->SetScreenCapturePickerShow();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetScreenCapturePickerShow002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetScreenCapturePickerShow()).Times(1);
  nweb_impl_->SetScreenCapturePickerShow();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DisableSessionReuse001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, DisableSessionReuse()).Times(0);
  nweb_impl_->DisableSessionReuse();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DisableSessionReuse002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, DisableSessionReuse()).Times(1);
  nweb_impl_->DisableSessionReuse();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebScreenCaptureDelegateCallback001) {
  std::shared_ptr<NWebScreenCaptureDelegateCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterScreenCaptureDelegateListener(callback)).Times(0);
  nweb_impl_->PutWebScreenCaptureDelegateCallback(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutWebScreenCaptureDelegateCallback002) {
  std::shared_ptr<NWebScreenCaptureDelegateCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterScreenCaptureDelegateListener(callback)).Times(1);
  nweb_impl_->PutWebScreenCaptureDelegateCallback(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
TEST_F(NWebImplTest, NeedToFireBeforeUnloadOrUnloadEvents001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NeedToFireBeforeUnloadOrUnloadEvents()).Times(0);
  auto result = nweb_impl_->NeedToFireBeforeUnloadOrUnloadEvents();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, NeedToFireBeforeUnloadOrUnloadEvents002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, NeedToFireBeforeUnloadOrUnloadEvents()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->NeedToFireBeforeUnloadOrUnloadEvents();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, DispatchBeforeUnload001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, DispatchBeforeUnload()).Times(0);
  nweb_impl_->DispatchBeforeUnload();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DispatchBeforeUnload002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, DispatchBeforeUnload()).Times(1);
  nweb_impl_->DispatchBeforeUnload();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, SetFocusByPosition001) {
  float x = 1.0f;
  float y = 1.0f;
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetFocusByPosition(x, y)).Times(0);
  auto result = nweb_impl_->SetFocusByPosition(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, SetFocusByPosition002) {
  float x = 1.0f;
  float y = 1.0f;
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetFocusByPosition(x, y)).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->SetFocusByPosition(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_AI_WRITE)
TEST_F(NWebImplTest, GetSelectStartIndex001) {
  int temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*inputmethod_handler, GetSelectStartIndex()).Times(0);
  auto result = nweb_impl_->GetSelectStartIndex();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetSelectEndIndex001) {
  int temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*inputmethod_handler, GetSelectEndIndex()).Times(0);
  auto result = nweb_impl_->GetSelectEndIndex();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetAllTextInfo001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*inputmethod_handler, GetAllTextInfo()).Times(0);
  auto result = nweb_impl_->GetAllTextInfo();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, GetAllTextInfo002) {
  std::string temp = "";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  ON_CALL(*inputmethod_handler, GetAllTextInfo()).WillByDefault(::testing::Return(temp));
  auto result = nweb_impl_->GetAllTextInfo();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_TRUE(result.empty());
}
#endif // BUILDFLAG(ARKWEB_AI_WRITE)

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
TEST_F(NWebImplTest, SetBorderRadiusFromWeb001) {
  double borderRadiusTopLeft = 1.0;
  double borderRadiusTopRight = 1.0;
  double borderRadiusBottomLeft = 1.0;
  double borderRadiusBottomRight = 1.0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBorderRadiusFromWeb(borderRadiusTopLeft, borderRadiusTopRight,
        borderRadiusBottomLeft, borderRadiusBottomRight)).Times(0);
  nweb_impl_->SetBorderRadiusFromWeb(borderRadiusTopLeft, borderRadiusTopRight,
        borderRadiusBottomLeft, borderRadiusBottomRight);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBorderRadiusFromWeb002) {
  double borderRadiusTopLeft = 1.0;
  double borderRadiusTopRight = 1.0;
  double borderRadiusBottomLeft = 1.0;
  double borderRadiusBottomRight = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBorderRadiusFromWeb(borderRadiusTopLeft, borderRadiusTopRight,
        borderRadiusBottomLeft, borderRadiusBottomRight)).Times(1);
  nweb_impl_->SetBorderRadiusFromWeb(borderRadiusTopLeft, borderRadiusTopRight,
        borderRadiusBottomLeft, borderRadiusBottomRight);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(NWebImplTest, SetPipNativeWindow001) {
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  void* window = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetPipNativeWindow(delegate_id, child_id, frame_routing_id, window)).Times(0);
  nweb_impl_->SetPipNativeWindow(delegate_id, child_id, frame_routing_id, window);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPipNativeWindow002) {
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  void* window = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetPipNativeWindow(delegate_id, child_id, frame_routing_id, window)).Times(1);
  nweb_impl_->SetPipNativeWindow(delegate_id, child_id, frame_routing_id, window);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendPipEvent001) {
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int event = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendPipEvent(delegate_id, child_id, frame_routing_id, event)).Times(0);
  nweb_impl_->SendPipEvent(delegate_id, child_id, frame_routing_id, event);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SendPipEvent002) {
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int event = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SendPipEvent(delegate_id, child_id, frame_routing_id, event)).Times(1);
  nweb_impl_->SendPipEvent(delegate_id, child_id, frame_routing_id, event);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_PIP)

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(NWebImplTest, SetBlanklessLoadingKey001) {
  const std::string key = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBlanklessLoadingKey(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->SetBlanklessLoadingKey(key);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetBlanklessLoadingKey002) {
  const std::string key = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBlanklessLoadingKey(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->SetBlanklessLoadingKey(key);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPrivacyStatus001) {
  bool isPrivate = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetPrivacyStatus(isPrivate);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPrivacyStatus002) {
  bool isPrivate = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetPrivacyStatus(isPrivate);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearBlanklessKey001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetBlanklessLoadingKey(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->ClearBlanklessKey();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearBlanklessKey002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetBlanklessLoadingKey(::testing::_, ::testing::_)).Times(0);
  nweb_impl_->ClearBlanklessKey();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearBlanklessKey003) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  const std::string key = "test";
  nweb_impl_->SetBlanklessLoadingKey(key);
  EXPECT_CALL(*mock_delegate_, SetBlanklessLoadingKey(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->ClearBlanklessKey();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc001) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .staticPath = "test",
    .lcpTime = 0,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = nullptr;
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc002) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .staticPath = "test",
    .lcpTime = INT32_MAX,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc003) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .staticPath = "test",
    .lcpTime = 0,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc004) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .staticPath = "test",
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc005) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
  EXPECT_TRUE(dataItem.staticPath.empty());
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc006) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .wholePath = "test",
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem, true);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFunc007) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFunc(blankless_key, dataItem, true);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
  EXPECT_TRUE(dataItem.wholePath.empty());
}

TEST_F(NWebImplTest, CallBlanklessFrameFuncForWhiteList001) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .wholePath = "test",
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFuncForWhiteList(blankless_key, dataItem, true);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, CallBlanklessFrameFuncForWhiteList002) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .lcpTime = 5,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFuncForWhiteList(blankless_key, dataItem, true);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
  EXPECT_TRUE(dataItem.wholePath.empty());
}

TEST_F(NWebImplTest, CallBlanklessFrameFuncForWhiteList003) {
  uint64_t blankless_key = 0;
  SnapshotDataItem dataItem = {
    .staticPath = "test",
    .lcpTime = INT32_MAX,
    .width = 100,
    .height = 100,
  };
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->nweb_handle_ = std::make_shared<NWebHandler>();
  nweb_impl_->CallBlanklessFrameFuncForWhiteList(blankless_key, dataItem);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_handle_, nullptr);
}

TEST_F(NWebImplTest, GetPreferenceHash001) {
  int64_t temp = -1LL;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetPreferenceHash()).Times(0);
  auto result = nweb_impl_->GetPreferenceHash();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetPreferenceHash002) {
  int64_t temp = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetPreferenceHash()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetPreferenceHash();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetPreferenceHashByNwebId001) {
  int64_t temp = -1LL;
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  int32_t nweb_id = 1;
  auto result = NWebImpl::GetPreferenceHashByNwebId(nweb_id);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetPreferenceHashByNwebId002) {
  int64_t temp = 0;
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  nweb->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetPreferenceHash()).WillOnce(::testing::Return(temp));
  int32_t nweb_id = 0;
  auto result = NWebImpl::GetPreferenceHashByNwebId(nweb_id);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
TEST_F(NWebImplTest, AvoidVisibleViewportBottom001) {
  int32_t avoidHeight = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, AvoidVisibleViewportBottom(avoidHeight)).Times(0);
  nweb_impl_->AvoidVisibleViewportBottom(avoidHeight);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, AvoidVisibleViewportBottom002) {
  int32_t avoidHeight = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, AvoidVisibleViewportBottom(avoidHeight)).Times(1);
  nweb_impl_->AvoidVisibleViewportBottom(avoidHeight);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetVisibleViewportAvoidHeight001) {
  int32_t temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).Times(0);
  auto result = nweb_impl_->GetVisibleViewportAvoidHeight();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetVisibleViewportAvoidHeight002) {
  int32_t temp = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetVisibleViewportAvoidHeight();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_VIEWPORT_AVOID)

#if BUILDFLAG(ARKWEB_MENU)
TEST_F(NWebImplTest, UpdateSingleHandleVisible001) {
  bool isVisible = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, UpdateSingleHandleVisible(isVisible)).Times(0);
  nweb_impl_->UpdateSingleHandleVisible(isVisible);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, UpdateSingleHandleVisible002) {
  bool isVisible = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, UpdateSingleHandleVisible(isVisible)).Times(1);
  nweb_impl_->UpdateSingleHandleVisible(isVisible);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetTouchHandleExistState001) {
  bool touchHandleExist = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetTouchHandleExistState(touchHandleExist)).Times(0);
  nweb_impl_->SetTouchHandleExistState(touchHandleExist);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetTouchHandleExistState002) {
  bool touchHandleExist = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetTouchHandleExistState(touchHandleExist)).Times(1);
  nweb_impl_->SetTouchHandleExistState(touchHandleExist);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetViewportScaleState001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetViewportScaleState(::testing::_)).Times(0);
  nweb_impl_->SetViewportScaleState();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetViewportScaleState002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetViewportScaleState(::testing::_)).Times(1);
  nweb_impl_->SetViewportScaleState();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
TEST_F(NWebImplTest, SetErrorPageEnabled001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetErrorPageEnabled(enable)).Times(0);
  nweb_impl_->SetErrorPageEnabled(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetErrorPageEnabled002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetErrorPageEnabled(enable)).Times(1);
  nweb_impl_->SetErrorPageEnabled(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetErrorPageEnabled001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetErrorPageEnabled()).Times(0);
  auto result = nweb_impl_->GetErrorPageEnabled();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, GetErrorPageEnabled002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetErrorPageEnabled()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->GetErrorPageEnabled();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(ARKWEB_ERROR_PAGE)

#if BUILDFLAG(ARKWEB_BGTASK)
TEST_F(NWebImplTest, OnBrowserForeground001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnBrowserForeground()).Times(0);
  nweb_impl_->OnBrowserForeground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnBrowserForeground002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnBrowserForeground()).Times(1);
  nweb_impl_->OnBrowserForeground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnBrowserBackground001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnBrowserBackground()).Times(0);
  nweb_impl_->OnBrowserBackground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnBrowserBackground002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnBrowserBackground()).Times(1);
  nweb_impl_->OnBrowserBackground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_BGTASK)

TEST_F(NWebImplTest, IsNavigateForwardAllowed001) {
  bool temp = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  auto result = nweb_impl_->IsNavigateForwardAllowed();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, IsNavigateForwardAllowed002) {
  bool temp = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).WillOnce(::testing::Return(temp));
  auto result = nweb_impl_->IsNavigateForwardAllowed();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

#if BUILDFLAG(ARKWEB_PDF)
TEST_F(NWebImplTest, ExecuteCreatePDFExt001) {
  std::shared_ptr<NWebPDFConfigArgs> pdfConfig = nullptr;
  std::shared_ptr<NWebArrayBufferValueCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ExecuteCreatePDFExt(pdfConfig, callback)).Times(0);
  nweb_impl_->ExecuteCreatePDFExt(pdfConfig, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteCreatePDFExt002) {
  std::shared_ptr<NWebPDFConfigArgs> pdfConfig = nullptr;
  std::shared_ptr<NWebArrayBufferValueCallback> callback = nullptr;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ExecuteCreatePDFExt(pdfConfig, callback)).Times(1);
  nweb_impl_->ExecuteCreatePDFExt(pdfConfig, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_PDF)

TEST_F(NWebImplTest, OnDragAttach001) {
  nweb_impl_->inputmethod_handler_ = nullptr;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnDragAttach();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnDragAttach002) {
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnDragAttach();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_MSGPORT)
TEST_F(NWebImplTest, PostPortMessageV2001) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebRomValue> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PostPortMessageV2(portHandle, data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostPortMessageV2002) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebRomValue> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PostPortMessageV2(portHandle, data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

TEST_F(NWebImplTest, FillAutofillDataV2001) {
  std::shared_ptr<NWebRomValue> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->FillAutofillDataV2(data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FillAutofillDataV2002) {
  std::shared_ptr<NWebRomValue> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->FillAutofillDataV2(data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction003) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> async_method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, "")).Times(0);
  nweb_impl_->RegisterArkJSfunction(object_name, method_list, async_method_list, object_id);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction004) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> async_method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, "")).Times(1);
  nweb_impl_->RegisterArkJSfunction(object_name, method_list, async_method_list, object_id);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction005) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> async_method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  const std::string permission = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, permission)).Times(0);
  nweb_impl_->RegisterArkJSfunctionV2(object_name, method_list, async_method_list, object_id, permission);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterArkJSfunction006) {
  const std::string object_name = "test_id";
  const std::vector<std::string> method_list = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> async_method_list = {"Default", "IncludeSensitive", "Everything"};
  const int32_t object_id = 1;
  const std::string permission = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterArkJSfunction(
      object_name, method_list, async_method_list, object_id, permission)).Times(1);
  nweb_impl_->RegisterArkJSfunctionV2(object_name, method_list, async_method_list, object_id, permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallH5FunctionV2001) {
  int32_t routing_id = 0;
  int32_t h5_object_id = 0;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebRomValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args)).Times(0);
  nweb_impl_->CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallH5FunctionV2002) {
  int32_t routing_id = 0;
  int32_t h5_object_id = 1;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebRomValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args)).Times(1);
  nweb_impl_->CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CallH5FunctionV2003) {
  int32_t routing_id = 0;
  int32_t h5_object_id = -1;
  const std::string h5_method_name = "name";
  const std::vector<std::shared_ptr<NWebRomValue>> args;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args)).Times(0);
  nweb_impl_->CallH5FunctionV2(routing_id, h5_object_id, h5_method_name, args);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnFocus001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  bool temp = false;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnFocus(::testing::_)).WillOnce(::testing::Return(temp));
  nweb_impl_->OnFocus(OHOS::NWeb::FocusReason::FOCUS_DEFAULT);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).Times(0);
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(0);
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::WINDOW_BLUR);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->inputmethod_handler_ = nullptr;
  bool temp = false;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::WINDOW_BLUR);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
TEST_F(NWebImplTest, GetScrollOffset003) {
  float offset_y = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetScrollOffset(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->GetScrollOffset(nullptr, &offset_y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(offset_y, 0.0f);
}

TEST_F(NWebImplTest, GetScrollOffset004) {
  float offset_x = 0.0f;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetScrollOffset(::testing::_, ::testing::_)).Times(1);
  nweb_impl_->GetScrollOffset(&offset_x, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(offset_x, 0.0f);
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, WebSendKeyEvent001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
}

TEST_F(NWebImplTest, WebSendMouseWheelEvent001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendMouseWheelEventV2001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  int32_t source = 0;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendTouchpadFlingEvent001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendCancelFlingEvent001) {
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendCancelFlingEvent();
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendKeyboardEvent001) {
  bool temp = false;
  const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent> keyboardEvent = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = nullptr;
  EXPECT_CALL(*input_handler_, SendKeyboardEvent(keyboardEvent)).Times(0);
  auto result = nweb_impl_->SendKeyboardEvent(keyboardEvent);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, SendKeyboardEvent002) {
  const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent> keyboardEvent = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->SendKeyboardEvent(keyboardEvent);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendMouseEvent003) {
  std::shared_ptr<MockNWebMouseEvent> mockMouseEvent = std::make_shared<MockNWebMouseEvent>();
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = mockMouseEvent;
  EXPECT_CALL(*mockMouseEvent, GetAction()).WillOnce(::testing::Return(1));
  nweb_impl_->nweb_delegate_ = nullptr;
  input_handler_= std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendMouseEvent(mouseEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, WebSendMouseEvent004) {
  std::shared_ptr<MockNWebMouseEvent> mockMouseEvent = std::make_shared<MockNWebMouseEvent>();
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = mockMouseEvent;
  EXPECT_CALL(*mockMouseEvent, GetAction()).WillOnce(::testing::Return(2));
  nweb_impl_->nweb_delegate_ = nullptr;
  input_handler_= std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->input_handler_ = input_handler_;
  nweb_impl_->WebSendMouseEvent(mouseEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

TEST_F(NWebImplTest, SetScreenOffSet001) {
  double x = 1.0;
  double y = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetScreenOffset(x, y)).Times(1);
  nweb_impl_->inputmethod_handler_ = nullptr;
  nweb_impl_->SetScreenOffSet(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, SetScreenOffSet002) {
  double x = 1.0;
  double y = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  nweb_impl_->SetScreenOffSet(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)
TEST_F(NWebImplTest, NotifyMemoryLevel001) {
  int32_t level = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NotifyMemoryLevel(level);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NotifyMemoryLevel002) {
  int32_t level = 2;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NotifyMemoryLevel(level);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_PERFORMANCE_MEMORY_THRESHOLD)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, ResizeVisibleViewport001) {
  uint32_t width = 0;
  uint32_t height = 0;
  bool isKeyboard = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).Times(0);
#endif  // BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  nweb_impl_->ResizeVisibleViewport(width, height, isKeyboard);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResizeVisibleViewport002) {
  uint32_t width = 0;
  uint32_t height = 8000;
  bool isKeyboard = 0;
  nweb_impl_->draw_mode_ = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).Times(0);
#endif  // BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  nweb_impl_->ResizeVisibleViewport(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResizeVisibleViewport003) {
  uint32_t width = 8000;
  uint32_t height = 8000;
  bool isKeyboard = 0;
  nweb_impl_->draw_mode_ = 10;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).WillOnce(::testing::Return(0));
#endif  // BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, ResizeVisibleViewport(width, height, isKeyboard)).Times(1);
  nweb_impl_->ResizeVisibleViewport(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ResizeVisibleViewport004) {
  uint32_t width = 8000;
  uint32_t height = 8000;
  bool isKeyboard = 0;
  nweb_impl_->draw_mode_ = 10;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, GetVisibleViewportAvoidHeight()).WillOnce(::testing::Return(1));
#endif  // BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  EXPECT_CALL(*mock_delegate_, ResizeVisibleViewport(width, height, isKeyboard)).Times(0);
  nweb_impl_->ResizeVisibleViewport(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

TEST_F(NWebImplTest, OnContinue002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = false;
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnContinue003) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = true;
  nweb_impl_->pending_size_ = std::nullopt;
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->is_pause_, false);
}

TEST_F(NWebImplTest, OnContinue004) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = true;
  nweb_impl_->pending_size_ = NWebImpl::ReSizeType {800, 600, false};
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->is_pause_, false);
}

TEST_F(NWebImplTest, OnContinue005) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = false;
  nweb_impl_->inputmethod_handler_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).Times(0);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnContinue006) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = false;
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).Times(1);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnContinue007) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->is_pause_ = false;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  EXPECT_CALL(*mock_delegate_, OnContinue()).Times(1);
  EXPECT_CALL(*mock_delegate_, OnFocus(::testing::_)).Times(0);
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).Times(0);
  nweb_impl_->OnContinue();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, NotifyPopupWindowDisposition001) {
  ASSERT_NE(nweb_impl_, nullptr);
  nweb_impl_->is_pause_ = false;
  nweb_impl_->NotifyPopupWindowDisposition(CEF_WOD_NEW_WINDOW);
  EXPECT_FALSE(nweb_impl_->is_pause_);
  nweb_impl_->NotifyPopupWindowDisposition(CEF_WOD_NEW_BACKGROUND_TAB);
  EXPECT_TRUE(nweb_impl_->is_pause_);
}

TEST_F(NWebImplTest, OnBlur003) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->inputmethod_handler_ = nullptr;
  bool temp = true;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> handlerImpl = std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).WillOnce(::testing::Return(handlerImpl));
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::WINDOW_BLUR);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur004) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  bool temp = true;
  nweb_impl_->is_enhance_surface_ = false;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> handlerImpl = std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).WillOnce(::testing::Return(handlerImpl));
  EXPECT_CALL(*inputmethod_handler, SetFocusStatus(::testing::_)).Times(1);
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::CLEAR_FOCUS);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur005) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  bool temp = true;
  nweb_impl_->is_enhance_surface_ = true;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> handlerImpl = std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).WillOnce(::testing::Return(handlerImpl));
  EXPECT_CALL(*inputmethod_handler, SetFocusStatus(::testing::_)).Times(1);
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::FOCUS_SWITCH);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur006) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  bool temp = true;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> handlerImpl = std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).WillOnce(::testing::Return(handlerImpl));
  EXPECT_CALL(*inputmethod_handler, SetFocusStatus(::testing::_)).Times(1);
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::VIEW_SWITCH);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

TEST_F(NWebImplTest, OnBlur007) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  CefRefPtr<MockNWebInputMethodHandler> inputmethod_handler = new MockNWebInputMethodHandler();
  nweb_impl_->inputmethod_handler_ = inputmethod_handler;
  bool temp = true;
  nweb_impl_->is_enhance_surface_ = true;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).WillOnce(::testing::Return(temp));
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(1);
  std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> handlerImpl = std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  EXPECT_CALL(*mock_delegate_, GetCustomKeyboardHandler()).WillOnce(::testing::Return(handlerImpl));
#if BUILDFLAG(ARKWEB_FOCUS)
  EXPECT_CALL(*inputmethod_handler, SetFocusStatus(::testing::_)).Times(0);
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)
  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::WINDOW_BLUR);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
}

#if BUILDFLAG(ARKWEB_PRINT)
TEST_F(NWebImplTest, CreateWebPrintDocumentAdapterV2001) {
  const std::string jobName = "test";
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, CreateWebPrintDocumentAdapterV2(jobName)).Times(0);
  nweb_impl_->CreateWebPrintDocumentAdapterV2(jobName);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CreateWebPrintDocumentAdapterV2002) {
  const std::string jobName = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, CreateWebPrintDocumentAdapterV2(jobName)).Times(1);
  nweb_impl_->CreateWebPrintDocumentAdapterV2(jobName);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(IS_ARKWEB)
TEST_F(NWebImplTest, EnableAppLinking001) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, EnableAppLinking(enable)).Times(0);
  nweb_impl_->EnableAppLinking(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, EnableAppLinking002) {
  bool enable = false;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, EnableAppLinking(enable)).Times(1);
  nweb_impl_->EnableAppLinking(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_AI_WRITE)
TEST_F(NWebImplTest, GetSelectStartIndex002) {
  int temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  auto result = nweb_impl_->GetSelectStartIndex();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, GetSelectEndIndex002) {
  int temp = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->inputmethod_handler_ = new NWebInputMethodHandler();
  auto result = nweb_impl_->GetSelectEndIndex();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(nweb_impl_->inputmethod_handler_, nullptr);
  EXPECT_EQ(result, 0);
}
#endif  // BUILDFLAG(ARKWEB_AI_WRITE)

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(NWebImplTest, SetVisibility001) {
  bool isVisible = false;
  nweb_impl_->nweb_handle_ = nullptr;
  EXPECT_EQ(nweb_impl_->nweb_handle_, nullptr);
  nweb_impl_->SetVisibility(isVisible);
  EXPECT_FALSE(nweb_impl_->is_visible_);
}

TEST_F(NWebImplTest, SetVisibility002) {
  bool isVisible = true;
  nweb_impl_->nweb_handle_ = nullptr;
  EXPECT_EQ(nweb_impl_->nweb_handle_, nullptr);
  nweb_impl_->SetVisibility(isVisible);
  EXPECT_TRUE(nweb_impl_->is_visible_);
}

TEST_F(NWebImplTest, SetVisibility003) {
  bool isVisible = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebHandler> client = std::make_shared<NWebHandler>();
  nweb_impl_->SetNWebHandler(client);
  EXPECT_EQ(nweb_impl_->nweb_handle_, client);
  nweb_impl_->SetVisibility(isVisible);
  EXPECT_TRUE(nweb_impl_->is_visible_);
}

TEST_F(NWebImplTest, GetBlanklessInfoWithKey001) {
  const std::string key = "test";
  double similarity = 0;
  int32_t loadingTime = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  auto result = nweb_impl_->GetBlanklessInfoWithKey(key, &similarity, &loadingTime);
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, GetBlanklessInfoWithKey002) {
  const std::string key = "test";
  double similarity = 1.0;
  int32_t loadingTime = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  auto result = nweb_impl_->GetBlanklessInfoWithKey(key, &similarity, &loadingTime);
  EXPECT_EQ(similarity, 0);
  EXPECT_EQ(loadingTime, 0);
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, GetBlanklessInfoWithKey003) {
  const std::string key = "test";
  int32_t loadingTime = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  auto result = nweb_impl_->GetBlanklessInfoWithKey(key, nullptr, &loadingTime);
  EXPECT_EQ(loadingTime, 0);
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, GetBlanklessInfoWithKey004) {
  const std::string key = "test";
  double similarity = 1.0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  auto result = nweb_impl_->GetBlanklessInfoWithKey(key, &similarity, nullptr);
  EXPECT_EQ(similarity, 0);
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, SetBlanklessLoadingParams001) {
  const std::string key = "test";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  auto result1 = nweb_impl_->SetBlanklessLoadingParams(key, false, 0, 0, nullptr);
  EXPECT_EQ(result1, -5);
  auto result2 = nweb_impl_->SetBlanklessLoadingParams(key, true, 0, 0, nullptr);
  EXPECT_EQ(result2, -5);
}
#endif  // BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy001) {
  const std::string objName = "test";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data = nullptr;
  bool isAsync = false;
  const std::string permission = "permission";
  
  testing::internal::CaptureStderr();
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("nweb_delegate_ is nullptr"), std::string::npos);
}

TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy002) {
  const std::string objName = "test";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::shared_ptr<MockNWebJsProxyMethod> data = std::make_shared<MockNWebJsProxyMethod>();
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_CALL(*data, GetSize()).WillOnce(::testing::Return(0));
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterNativeJSProxy(::testing::_, ::testing::_, ::testing::_, ::testing::_,
      ::testing::_)).Times(1);
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, RegisterNativeJavaScriptProxy003) {
  const std::string objName = "test";
  const std::vector<std::string> methodName = {"Default", "IncludeSensitive", "Everything"};
  std::shared_ptr<MockNWebJsProxyMethod> data = std::make_shared<MockNWebJsProxyMethod>();
  bool isAsync = false;
  const std::string permission = "permission";
  EXPECT_CALL(*data, GetSize()).WillOnce(::testing::Return(1));
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, RegisterNativeJSProxy(::testing::_, ::testing::_, ::testing::_, ::testing::_,
      ::testing::_)).Times(1);
  nweb_impl_->RegisterNativeJavaScriptProxy(objName, methodName, data, isAsync, permission);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)

TEST_F(NWebImplTest, SetProxyOverride001) {
  const std::vector<std::string> proxyUrls = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> proxySchemeFilters = {"Default", "IncludeSensitive", "Everything"};
  const std::vector<std::string> bypassRules = {"Default", "IncludeSensitive", "Everything"};
  const bool reverseBypass = false;
  std::shared_ptr<NWebProxyChangedCallback> callback = nullptr;
  EXPECT_EQ(proxySchemeFilters.size(), proxyUrls.size());
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_READER_MODE)
TEST_F(NWebImplTest, AbortDistill001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, AbortDistill()).Times(0);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, AbortDistill002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, AbortDistill()).Times(1);
  nweb_impl_->AbortDistill();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_READER_MODE)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void OnOffscreenDocumentPermissionRequestCallback(const char* extension_id,
                                                  const char* origin_url,
                                                  const int resources,
                                                  const int request_key) {}

TEST_F(NWebImplTest, OffscreenDocumentPermissionRequest001) {
  const std::string extension_id = "extension-id";
  const std::string origin_url = "arkweb-extension://extension-id/";
  int resources =
      static_cast<int>(OffscreenDocumentPermissionResourceType::VIDEO_CAPTURE);
  int request_key = 1;

  testing::internal::CaptureStderr();
  NWebImpl::OnOffscreenDocumentPermissionRequest(extension_id, origin_url,
                                                 resources, request_key);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("callback is null"), std::string::npos);

  NWebImpl::GrantOffscreenDocumentPermission(resources, request_key);
  NWebImpl::DenyOffscreenDocumentPermission(resources, request_key);
  NWebImpl::SetOnOffscreenDocumentPermissionRequestCallback(
      OnOffscreenDocumentPermissionRequestCallback);
  NWebImpl::OnOffscreenDocumentPermissionRequest(extension_id, origin_url,
                                                 resources, request_key);
  EXPECT_NE(NWebImpl::on_offscreen_document_permission_request_callback_, nullptr);
}

void OffscreenDocumentWindowNewEventCallback(
    const char* extensionId,
    const char* originUrl,
    bool isAlert,
    bool isUserTrigger,
    const char* targetUrl) {}

TEST_F(NWebImplTest, OffscreenDocumentWindowNewEvent001) {
  const std::string extensionId = "extension-id";
  const std::string originUrl = "arkweb-extension://extension-id/";
  std::string targetUrl = "//xxxxxx/yyy/sss.html";
  bool isAlert = false;
  bool isUserTrigger = false;

  nweb_impl_->OnOffscreenDocumentWindowNewEvent(
      extensionId, originUrl, isAlert, isUserTrigger, targetUrl);
  EXPECT_EQ(nweb_impl_->on_off_screen_window_new_callback_, nullptr);
  nweb_impl_->on_off_screen_window_new_callback_ =
      OffscreenDocumentWindowNewEventCallback;
  nweb_impl_->OnOffscreenDocumentWindowNewEvent(
      extensionId, originUrl, isAlert, isUserTrigger, targetUrl);
  nweb_impl_->SetOnOffscreenDocumentWindowNewCallback(nullptr);
  EXPECT_EQ(nweb_impl_->on_off_screen_window_new_callback_, nullptr);
  nweb_impl_->SetOffscreenNWebId(0);
  EXPECT_EQ(nweb_impl_->off_screen_nweb_id_, 0);
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_COOKIE)
TEST_F(NWebImplTest, LibraryLoaded001) {
  std::shared_ptr<MockNWebEngineInitArgs> initargs =
      std::make_shared<MockNWebEngineInitArgs>();
  NWebImpl::LibraryLoaded(initargs, true);
  EXPECT_NE(nweb_impl_->save_initargs_, nullptr);
  EXPECT_EQ(nweb_impl_->should_lazy_init_web_engine_, true);
}

TEST_F(NWebImplTest, ShouldLazyInitWebEngine001) {
  nweb_impl_->should_lazy_init_web_engine_ = false;
  EXPECT_EQ(NWebImpl::ShouldLazyInitWebEngine(), false);
}

TEST_F(NWebImplTest, GetSaveInitargs001) {
  nweb_impl_->save_initargs_ = nullptr;
  EXPECT_EQ(NWebImpl::GetSaveInitargs(), nullptr);
}
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(NWebImplTest, ResumeMicrophone001) {
  ASSERT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->ResumeMicrophone());
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->ResumeMicrophone());
}

TEST_F(NWebImplTest, PauseMicrophone001) {
  ASSERT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->PauseMicrophone());
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->PauseMicrophone());
}

TEST_F(NWebImplTest, StopMicrophone001) {
  ASSERT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->StopMicrophone());
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  ASSERT_NO_FATAL_FAILURE(nweb_impl_->StopMicrophone());
}
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
TEST_F(NWebImplTest, SearchBillingTrack_GetRequestHeader001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestHeader).Times(0);
  nweb_impl_->GetRequestHeader(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestHeader002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestHeader).Times(1);
  nweb_impl_->GetRequestHeader(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestUrl001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestUrl).Times(0);
  nweb_impl_->GetRequestUrl(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestUrl002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestUrl).Times(1);
  nweb_impl_->GetRequestUrl(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsRequestGesture001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsRequestGesture).Times(0);
  nweb_impl_->IsRequestGesture(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsRequestGesture002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsRequestGesture).Times(1);
  nweb_impl_->IsRequestGesture(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsMainFrame001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsMainFrame).Times(0);
  nweb_impl_->IsMainFrame(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsMainFrame002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsMainFrame).Times(1);
  nweb_impl_->IsMainFrame(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsRedirect001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsRedirect).Times(0);
  nweb_impl_->IsRedirect(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_IsRedirect002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestIsRedirect).Times(1);
  nweb_impl_->IsRedirect(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestMethod001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestMethod).Times(0);
  nweb_impl_->GetRequestMethod(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestMethod002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestMethod).Times(1);
  nweb_impl_->GetRequestMethod(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetPageTransition001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetPageTransition).Times(0);
  nweb_impl_->GetPageTransition(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetPageTransition002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetPageTransition).Times(1);
  nweb_impl_->GetPageTransition(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestType001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestType).Times(0);
  nweb_impl_->GetRequestType(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetRequestType002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestGetRequestType).Times(1);
  nweb_impl_->GetRequestType(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetMimeType001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetMimeType).Times(0);
  nweb_impl_->GetMimeType(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetMimeType002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetMimeType).Times(1);
  nweb_impl_->GetMimeType(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetEncoding001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetEncoding).Times(0);
  nweb_impl_->GetEncoding(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetEncoding002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetEncoding).Times(1);
  nweb_impl_->GetEncoding(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetStatusCode001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetStatusCode).Times(0);
  nweb_impl_->GetStatusCode(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetStatusCode002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetStatusCode).Times(1);
  nweb_impl_->GetStatusCode(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetReasonPhrase001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetReasonPhrase).Times(0);
  nweb_impl_->GetReasonPhrase(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetReasonPhrase002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetReasonPhrase).Times(1);
  nweb_impl_->GetReasonPhrase(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetResponseHeader001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetResponseHeader).Times(0);
  nweb_impl_->GetResponseHeader(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetResponseHeader002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetResponseHeader).Times(1);
  nweb_impl_->GetResponseHeader(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetIsFromNetwork001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetIsFromNetwork).Times(0);
  nweb_impl_->GetIsFromNetwork(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_GetIsFromNetwork002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseGetIsFromNetwork).Times(1);
  nweb_impl_->GetIsFromNetwork(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_ResourceRequestDelete001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceRequestDelete).Times(0);
  nweb_impl_->ResourceRequestDelete(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_ResourceRequestDelete002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceRequestDelete).Times(1);
  nweb_impl_->ResourceRequestDelete(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_ResourceResponseDelete001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ResourceResponseDelete).Times(0);
  nweb_impl_->ResourceResponseDelete(1);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SearchBillingTrack_ResourceResponseDelete002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ResourceResponseDelete).Times(1);
  nweb_impl_->ResourceResponseDelete(1);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest,
       SearchBillingTrack_GetLastCommittedEntryPageTransition001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, GetLastCommittedEntryPageTransition).Times(0);
  nweb_impl_->GetLastCommittedEntryPageTransition();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest,
       SearchBillingTrack_GetLastCommittedEntryPageTransition002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, GetLastCommittedEntryPageTransition).Times(1);
  nweb_impl_->GetLastCommittedEntryPageTransition();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, ReloadIgnoreCache001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ReloadIgnoreCache()).Times(0);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ReloadIgnoreCache002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ReloadIgnoreCache()).Times(1);
  nweb_impl_->ReloadIgnoreCache();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
}  // namespace OHOS::NWeb
                          
