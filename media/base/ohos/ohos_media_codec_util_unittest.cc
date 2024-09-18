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

#include "ohos_media_codec_util.h"
#include "ohos_adapter_helper.h" 
#include <gmock/gmock.h>
#include "testing/gtest/include/gtest/gtest.h"
#include <memory>
#include "base/logging.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "base/task/single_thread_task_executor.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Ref;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::MockFunction;

namespace media{

class MockMediaCodecListAdapter : public OHOS::NWeb::MediaCodecListAdapter {
public:
  MOCK_METHOD(std::shared_ptr<CapabilityDataAdapter>, GetCodecCapability,
              (const std::string&, bool), (override));
};

class MockOhosAdapterHelper : public OHOS::NWeb::OhosAdapterHelper {
public:
  MOCK_METHOD(std::unique_ptr<AafwkAppMgrClientAdapter>, CreateAafwkAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PowerMgrClientAdapter>, CreatePowerMgrClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DisplayManagerAdapter>, CreateDisplayMgrAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<BatteryMgrClientAdapter>, CreateBatteryClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NetConnectAdapter>, CreateNetConnectAdapter, (), (override));
  MOCK_METHOD(OhosWebDataBaseAdapter&, GetOhosWebDataBaseAdapterInstance, (), (override));
  MOCK_METHOD(PasteBoardClientAdapter&, GetPasteBoard, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioRendererAdapter>, CreateAudioRendererAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioCapturerAdapter>, CreateAudioCapturerAdapter, (), (override));
  MOCK_METHOD(AudioSystemManagerAdapter&, GetAudioSystemManager, (), (override));
  MOCK_METHOD(OhosWebPermissionDataBaseAdapter&, GetWebPermissionDataBaseInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<MMIAdapter>, CreateMMIAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SocPerfClientAdapter>, CreateSocPerfClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>, GetResourceAdapter, (const std::string&), (override));
  MOCK_METHOD(SystemPropertiesAdapter&, GetSystemPropertiesInstance, (), (override));
  MOCK_METHOD(VSyncAdapter&, GetVSyncAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosInitWebAdapter>, GetInitWebAdapter, (), (override));
  MOCK_METHOD(KeystoreAdapter&, GetKeystoreAdapterInstance, (), (override));
  MOCK_METHOD(EnterpriseDeviceManagementAdapter&, GetEnterpriseDeviceManagementInstance, (), (override));
  MOCK_METHOD(DatashareAdapter&, GetDatashareInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IMFAdapter>, CreateIMFAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>, GetRootCertDataAdapter, (), (override));
  MOCK_METHOD(AccessTokenAdapter&, GetAccessTokenAdapterInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<EventHandlerAdapter>, GetEventHandlerAdapter, (), (override));
  MOCK_METHOD(PrintManagerAdapter&, GetPrintManagerInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IConsumerSurfaceAdapter>, CreateConsumerSurfaceAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PlayerAdapter>, CreatePlayerAdapter, (), (override));
  MOCK_METHOD(WindowAdapter&, GetWindowAdapterInstance, (), (override));
  MOCK_METHOD(HiSysEventAdapter&, GetHiSysEventAdapterInstance, (), (override));
  MOCK_METHOD(HiTraceAdapter&, GetHiTraceAdapterInstance, (), (override));
  MOCK_METHOD(NetProxyAdapter&, GetNetProxyInstance, (), (override));
  MOCK_METHOD(CameraManagerAdapter&, GetCameraManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenCaptureAdapter>, CreateScreenCaptureAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DateTimeFormatAdapter>, CreateDateTimeFormatAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecDecoderAdapter>, CreateMediaCodecDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NativeImageAdapter>, CreateNativeImageAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecAdapter>, CreateMediaCodecEncoderAdapter, (), (override));
  MOCK_METHOD(MediaCodecListAdapter&, GetMediaCodecListAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<FlowbufferAdapter>, CreateFlowbufferAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaAVSessionAdapter>, CreateMediaAVSessionAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>, CreateOhosImageDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SensorAdapter>, CreateSensorAdapter, (), (override));
  MOCK_METHOD(void, SetArkWebCoreHapPathOverride, (const std::string&), (override));

  static MockOhosAdapterHelper& GetInstance() {
    static MockOhosAdapterHelper instance;
    return instance;
  } 
};

class MockCapabilityDataAdapter : public CapabilityDataAdapter {
public:
  MOCK_METHOD(int32_t, GetMaxWidth, (), (override));
  MOCK_METHOD(int32_t, GetMaxHeight, (), (override));
  MOCK_METHOD(int32_t, GetMaxframeRate, (), (override));
};

class OHOSMediaCodecUtilTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_adapter_helper_ = new testing::NiceMock<media::MockOhosAdapterHelper>;
    ON_CALL(*mock_adapter_helper_, GetMediaCodecListAdapter())
      .WillByDefault(testing::ReturnRef(mock_media_codec_list_adapter_));
  }

  void TearDown() override {
    delete mock_adapter_helper_;
  }

  std::shared_ptr<media::MockCapabilityDataAdapter> mock_capability_data_adapter_;
  media::MockOhosAdapterHelper* mock_adapter_helper_;
  media::MockMediaCodecListAdapter mock_media_codec_list_adapter_;
};

TEST_F(OHOSMediaCodecUtilTest, GetCodecCapabilityReturnsCorrectValues) {
  const std::string mime_type = "video/avc";
  bool is_encoder = false;
  auto mock_adapter = std::make_shared<MockCapabilityDataAdapter>();
  ON_CALL(mock_media_codec_list_adapter_,
          GetCodecCapability(mime_type, is_encoder))
      .WillByDefault(testing::Return(mock_adapter));
  CapabilityData capability =
      OHOSMediaCodecUtil::GetCodecCapability(mime_type, is_encoder);
  ASSERT_NE(capability.maxWidth, 0);
  ASSERT_NE(capability.maxHeight, 0);
  ASSERT_NE(capability.maxframeRate, 0);
}
}  // namespace media