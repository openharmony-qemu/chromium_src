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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "arkweb/ohos_adapter_ndk/access_token_adapter/access_token_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/audio_capturer_adapter/audio_capturer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/battery_mgr_adapter/battery_mgr_client_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/cert_mgr_adapter/cert_mgr_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/display_manager_adapter/native_display_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/drawing_text_adapter/ohos_drawing_text_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"
#define private public
#include "arkweb/ohos_adapter_ndk/interfaces/ark_ohos_adapter_helper_wrapper.h"
#undef private
#include "arkweb/ohos_adapter_ndk/keystore_adapter/keystore_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/drm_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/include/media_codec_list_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/include/video_encoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/media_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_config_adapter/net_config_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_event_adapter/net_event_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_proxy_adapter_impl/net_proxy_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_image_adapter/ohos_image_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_native_buffer_adapter/ohos_native_buffer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"
#include "datashare_adapter/datashare_adapter_impl.h"
#include "distributeddatamgr_adapter/ohos_web_data_base_adapter_impl.h"
#include "hiviewdfx_adapter/hitrace_adapter_impl.h"
#include "ohos_adapter/bridge/ark_aafwk_app_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_access_token_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_capturer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_renderer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_system_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_background_task_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_info_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_cert_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_datashare_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_date_time_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_drm_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_event_handler_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_flowbuffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hisysevent_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hitrace_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imfadapter_wrapper.h"
#include "ohos_adapter/bridge/ark_keystore_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_avsession_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_encoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_list_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_migration_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_mmi_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_connect_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_proxy_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_drawing_text_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_file_mapper_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_image_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_init_web_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_resource_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_web_data_base_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_player_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_print_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_running_lock_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screen_capture_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screenlock_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_sensor_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_soc_perf_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_system_properties_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_vsync_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_web_timezone_info_wrapper.h"
#include "ohos_adapter/bridge/ark_window_adapter_wrapper.h"
#include "color_picker_adapter/color_picker_adapter_impl.h"
#include "pasteboard_adapter/include/pasteboard_client_adapter_impl.h"
#include "hiviewdfx_adapter/hiappevent_adapter_impl.h"

using namespace testing;
using namespace OHOS::ArkWeb;
namespace OHOS::Nweb {
class TestArkOhosAdapterHelper : public virtual ArkOhosAdapterHelper {
public:
    TestArkOhosAdapterHelper() = default;
    ~TestArkOhosAdapterHelper() = default;
    void IncreRef() const override {}
    void DecreRef() const override {}
    virtual ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> CreateAafwkAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkPowerMgrClientAdapter> CreatePowerMgrClientAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkDisplayManagerAdapter> CreateDisplayMgrAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkBatteryMgrClientAdapter> CreateBatteryClientAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkNetConnectAdapter> CreateNetConnectAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosWebDataBaseAdapter> GetOhosWebDataBaseAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkPasteBoardClientAdapter> GetPasteBoard() {return nullptr;}
    virtual ArkWebRefPtr<ArkAudioRendererAdapter> CreateAudioRendererAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkAudioCapturerAdapter> CreateAudioCapturerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkAudioSystemManagerAdapter> GetAudioSystemManager() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> GetWebPermissionDataBaseInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkMMIAdapter> CreateMMIAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkSocPerfClientAdapter> CreateSocPerfClientAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosResourceAdapter> GetResourceAdapter(const ArkWebString& hapPath) {return nullptr;}
    virtual ArkWebRefPtr<ArkSystemPropertiesAdapter> GetSystemPropertiesInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkVSyncAdapter> GetVSyncAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosInitWebAdapter> GetInitWebAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkKeystoreAdapter> GetKeystoreAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter> GetEnterpriseDeviceManagementInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkDatashareAdapter> GetDatashareInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkIMFAdapter> CreateIMFAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkCertManagerAdapter> GetRootCertDataAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkAccessTokenAdapter> GetAccessTokenAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkEventHandlerAdapter> GetEventHandlerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkPrintManagerAdapter> GetPrintManagerInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkIConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkPlayerAdapter> CreatePlayerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkWindowAdapter> GetWindowAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkHiSysEventAdapter> GetHiSysEventAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkHiTraceAdapter> GetHiTraceAdapterInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkNetProxyAdapter> GetNetProxyInstance() {return nullptr;}
    virtual ArkWebRefPtr<ArkCameraManagerAdapter> GetCameraManagerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkScreenCaptureAdapter> CreateScreenCaptureAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkDateTimeFormatAdapter> CreateDateTimeFormatAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkMediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkNativeImageAdapter> CreateNativeImageAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkMediaCodecAdapter> CreateMediaCodecEncoderAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkMediaCodecListAdapter> GetMediaCodecListAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkFlowbufferAdapter> CreateFlowbufferAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkMediaAVSessionAdapter> CreateMediaAVSessionAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkSensorAdapter> CreateSensorAdapter() {return nullptr;}
    virtual void SetArkWebCoreHapPathOverride(const ArkWebString& hapPath) {}    
    virtual ArkWebRefPtr<ArkOhosNativeBufferAdapter> GetOhosNativeBufferAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkMigrationManagerAdapter> CreateMigrationMgrAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkAudioCodecDecoderAdapter> CreateAudioCodecDecoderAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkDrmAdapter> CreateDrmAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosDrawingTextFontAdapter> GetOhosDrawingTextFontAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter> GetOhosDrawingTextTypographyAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkScreenlockManagerAdapter> CreateScreenlockManagerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkCertManagerAdapter> GetCertManagerAdapter() {return nullptr;}
    virtual ArkWebRefPtr<ArkBackgroundTaskAdapter> CreateBackgroundTaskAdapter() {return nullptr;}
};

class ArkOhosAdapterHelperWrapperTest : public testing::Test {
protected:
    void SetUp();
    void TearDown();
    ArkOhosAdapterHelperWrapper* wrapper;
    std::unique_ptr<TestArkOhosAdapterHelper> testHelper;
    std::unique_ptr<ArkOhosAdapterHelperWrapper> testWrapper;
};

void ArkOhosAdapterHelperWrapperTest::SetUp() {
    OhosAdapterHelper& helper = OhosAdapterHelper::GetInstance();
    wrapper = reinterpret_cast<ArkOhosAdapterHelperWrapper*>(&helper);
    testHelper = std::make_unique<TestArkOhosAdapterHelper>();
    testWrapper =
        std::make_unique<ArkOhosAdapterHelperWrapper>(ArkWebRefPtr<ArkOhosAdapterHelper>(testHelper.get()));
}

void ArkOhosAdapterHelperWrapperTest::TearDown() {}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateAafwkAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateAafwkAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateAafwkAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateDisplayMgrAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateDisplayMgrAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateDisplayMgrAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetOhosWebDataBaseAdapterInstance) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(&wrapper->GetOhosWebDataBaseAdapterInstance(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateMMIAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateMMIAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateMMIAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateSocPerfClientAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateSocPerfClientAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateSocPerfClientAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetResourceAdapter) {
    ASSERT_NE(wrapper, nullptr);
    std::string testPath = "/data";
    EXPECT_NE(wrapper->GetResourceAdapter(testPath), nullptr);
    EXPECT_EQ(testWrapper->GetResourceAdapter(testPath), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetInitWebAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->GetInitWebAdapter(), nullptr);
    EXPECT_EQ(testWrapper->GetInitWebAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateIMFAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateIMFAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateIMFAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetCertManagerAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->GetCertManagerAdapter(), nullptr);
    EXPECT_EQ(testWrapper->GetCertManagerAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetEventHandlerAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->GetEventHandlerAdapter(), nullptr);
    EXPECT_EQ(testWrapper->GetEventHandlerAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetPrintManagerInstance) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(&wrapper->GetPrintManagerInstance(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, GetWindowAdapterInstance) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(&wrapper->GetWindowAdapterInstance(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateFlowbufferAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateFlowbufferAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateFlowbufferAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateMigrationMgrAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateMigrationMgrAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateMigrationMgrAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateScreenlockManagerAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateScreenlockManagerAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateScreenlockManagerAdapter(), nullptr);
}

TEST_F(ArkOhosAdapterHelperWrapperTest, CreateBackgroundTaskAdapter) {
    ASSERT_NE(wrapper, nullptr);
    EXPECT_NE(wrapper->CreateBackgroundTaskAdapter(), nullptr);
    EXPECT_EQ(testWrapper->CreateBackgroundTaskAdapter(), nullptr);
}
}   // namespace OHOS::Nweb