#include <memory>

#include "arkweb/build/features/features.h"
#include "audio_system_manager_adapter.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/system/system_monitor.h"
#include "base/task/bind_post_task.h"
#include "gtest/gtest.h"
#include "media/audio/audio_thread.h"
#include "media/base/media_switches.h"
#include "ohos_adapter_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#define protected public
#include "ohos_audio_manager.h"
#undef protected
#undef private

using namespace testing;
namespace media {
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumOutputBufferSize = 2048;
#if BUILDFLAG(ARKWEB_WEBRTC)
constexpr int kMinimumInputBufferSize = 2048;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

class MockOhosAdapterHelper : public OHOS::NWeb::OhosAdapterHelper {
 public:
  MOCK_METHOD(std::unique_ptr<AafwkAppMgrClientAdapter>,
              CreateAafwkAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<PowerMgrClientAdapter>,
              CreatePowerMgrClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DisplayManagerAdapter>,
              CreateDisplayMgrAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<BatteryMgrClientAdapter>,
              CreateBatteryClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NetConnectAdapter>,
              CreateNetConnectAdapter,
              (),
              (override));
  MOCK_METHOD(OhosWebDataBaseAdapter&,
              GetOhosWebDataBaseAdapterInstance,
              (),
              (override));
  MOCK_METHOD(PasteBoardClientAdapter&, GetPasteBoard, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioRendererAdapter>,
              CreateAudioRendererAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<AudioCapturerAdapter>,
              CreateAudioCapturerAdapter,
              (),
              (override));
  MOCK_METHOD(AudioSystemManagerAdapter&,
              GetAudioSystemManager,
              (),
              (override));
  MOCK_METHOD(OhosWebPermissionDataBaseAdapter&,
              GetWebPermissionDataBaseInstance,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MMIAdapter>, CreateMMIAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SocPerfClientAdapter>,
              CreateSocPerfClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>,
              GetResourceAdapter,
              (const std::string&),
              (override));
  MOCK_METHOD(SystemPropertiesAdapter&,
              GetSystemPropertiesInstance,
              (),
              (override));
  MOCK_METHOD(VSyncAdapter&, GetVSyncAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosInitWebAdapter>,
              GetInitWebAdapter,
              (),
              (override));
  MOCK_METHOD(KeystoreAdapter&, GetKeystoreAdapterInstance, (), (override));
  MOCK_METHOD(EnterpriseDeviceManagementAdapter&,
              GetEnterpriseDeviceManagementInstance,
              (),
              (override));
  MOCK_METHOD(DatashareAdapter&, GetDatashareInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IMFAdapter>, CreateIMFAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>,
              GetRootCertDataAdapter,
              (),
              (override));
  MOCK_METHOD(AccessTokenAdapter&,
              GetAccessTokenAdapterInstance,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<EventHandlerAdapter>,
              GetEventHandlerAdapter,
              (),
              (override));
  MOCK_METHOD(PrintManagerAdapter&, GetPrintManagerInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IConsumerSurfaceAdapter>,
              CreateConsumerSurfaceAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<PlayerAdapter>,
              CreatePlayerAdapter,
              (),
              (override));
  MOCK_METHOD(WindowAdapter&, GetWindowAdapterInstance, (), (override));
  MOCK_METHOD(HiSysEventAdapter&, GetHiSysEventAdapterInstance, (), (override));
  MOCK_METHOD(HiTraceAdapter&, GetHiTraceAdapterInstance, (), (override));
  MOCK_METHOD(NetProxyAdapter&, GetNetProxyInstance, (), (override));
  MOCK_METHOD(CameraManagerAdapter&, GetCameraManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenCaptureAdapter>,
              CreateScreenCaptureAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DateTimeFormatAdapter>,
              CreateDateTimeFormatAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecDecoderAdapter>,
              CreateMediaCodecDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NativeImageAdapter>,
              CreateNativeImageAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecAdapter>,
              CreateMediaCodecEncoderAdapter,
              (),
              (override));
  MOCK_METHOD(MediaCodecListAdapter&, GetMediaCodecListAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<FlowbufferAdapter>,
              CreateFlowbufferAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MediaAVSessionAdapter>,
              CreateMediaAVSessionAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>,
              CreateOhosImageDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<SensorAdapter>,
              CreateSensorAdapter,
              (),
              (override));
  MOCK_METHOD(void,
              SetArkWebCoreHapPathOverride,
              (const std::string&),
              (override));
  MOCK_METHOD(OhosNativeBufferAdapter&,
              GetOhosNativeBufferAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<AudioCodecDecoderAdapter>,
              CreateAudioCodecDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DrmAdapter>,
              CreateDrmAdapter,
              (),
              (override));
  MOCK_METHOD(OhosDrawingTextFontAdapter&,
               GetOhosDrawingTextFontAdapter,
               (),
               (override));
  MOCK_METHOD(OhosDrawingTextTypographyAdapter&,
              GetOhosDrawingTextTypographyAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MigrationManagerAdapter>,
              CreateMigrationMgrAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<ScreenlockManagerAdapter>,
              CreateScreenlockManagerAdapter,
              (),
              (override));
  static MockOhosAdapterHelper& GetInstance() {
    static MockOhosAdapterHelper instance;
    return instance;
  }
};

class AudioManagerDeviceChangeCallbackAdapter {
 public:
  virtual void OnDeviceChange() = 0;
};

class MockAudioManagerDeviceChangeCallback
    : public AudioManagerDeviceChangeCallbackAdapter {
 public:
  MOCK_METHOD(void, OnDeviceChange, (), (override));
};

void SimulateDeviceChange(
    NiceMock<MockAudioManagerDeviceChangeCallback>& callback) {
  callback.OnDeviceChange();
}

class MockAudioThread : public AudioThread {
 public:
  void Stop() { std::cout << "Stop function called" << std::endl; }

  bool IsHung() const { return false; }

  base::SingleThreadTaskRunner* GetTaskRunner() { return nullptr; }

  base::SingleThreadTaskRunner* GetWorkerTaskRunner() {
    return nullptr;
  }
};

class TestableOHOSAudioManager : public media::OHOSAudioManager {
 public:
  TestableOHOSAudioManager(std::unique_ptr<MockAudioThread> mock_audio_thread,
                           AudioLogFactory* audio_log_factory)
      : media::OHOSAudioManager(std::move(mock_audio_thread),
                                audio_log_factory) {}

  AudioParameters GetPreferredOutputStreamParametersForTest(
      const std::string& output_device_id,
      const AudioParameters& input_params) {
    AudioParameters outputParams;
    outputParams =
        AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                        ChannelLayoutConfig::Guess(kDefaultChannelCount),
                        kDefaultSampleRate, kMinimumOutputBufferSize);
    return outputParams;
  }

  AudioParameters GetPreferredInputStreamParametersForTest(
      const std::string& input_device_id,
      const AudioParameters& input_params) {
    AudioParameters inputParams;
    inputParams =
        AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY,
                        ChannelLayoutConfig::Guess(kDefaultChannelCount),
                        kDefaultSampleRate, kMinimumInputBufferSize);
    return inputParams;
  }

  void SelectAudioDevicePublic(const std::string& device_id, bool is_output) {
    SelectAudioDevice(device_id, is_output);
  }
};

class ConcreteAudioInputStream : public AudioInputStream {
 public:
  OpenOutcome Open() override { return OpenOutcome::kSuccess; }

  void Start(AudioInputCallback* callback) override {
    std::cout << "Starting audio input stream." << std::endl;
    if (callback != nullptr) {
      std::cout << "Callback is not null." << std::endl;
    } else {
      std::cout << "Callback is null." << std::endl;
    }
  }

  void Stop() override {}

  void Close() override {}

  double GetMaxVolume() override { return 0.0; }

  void SetVolume(double volume) override {}

  double GetVolume() override { return 0.0; }

  bool SetAutomaticGainControl(bool enabled) override { return false; }

  bool GetAutomaticGainControl() override { return false; }

  bool IsMuted() override { return false; }

  void SetOutputDeviceForAec(const std::string& output_device_id) override {}
};

class OHOSAudioManagerTest : public ::testing::Test {
 public:
  TestableOHOSAudioManager ohos_audio_manager;
  OHOSAudioManagerTest()
      : ohos_audio_manager(std::make_unique<MockAudioThread>(), nullptr) {}
  NiceMock<MockAudioManagerDeviceChangeCallback> callback_;

  void SetUp() override {
    mock_adapter_helper_ = new testing::NiceMock<media::MockOhosAdapterHelper>;
    std::unique_ptr<MockAudioThread> mock_audio_thread =
        std::make_unique<MockAudioThread>();
    ohos_audio_manager_ = std::make_unique<media::OHOSAudioManager>(
        std::move(mock_audio_thread),
        /*audio_log_factory=*/nullptr);
  }

  void TearDown() override { 
    ohos_audio_manager_.reset();
    delete mock_adapter_helper_;
  }

  std::unique_ptr<media::OHOSAudioManager> ohos_audio_manager_;
  media::MockOhosAdapterHelper* mock_adapter_helper_;
};

TEST_F(OHOSAudioManagerTest, OnDeviceChangeWithSystemMonitor) {
  EXPECT_CALL(callback_, OnDeviceChange());
  SimulateDeviceChange(callback_);
}

TEST_F(OHOSAudioManagerTest, TestGetName) {
  auto renderer_client_ = std::make_shared<OHOSAudioManager>(
      /*mock_audio_thread=*/nullptr,
      /*audio_log_factory=*/nullptr);

  EXPECT_STREQ("OHOS", renderer_client_->GetName());
}

TEST_F(OHOSAudioManagerTest, HasAudioOutputDevices) {
  EXPECT_TRUE(ohos_audio_manager_->HasAudioOutputDevices());
}

TEST_F(OHOSAudioManagerTest, HasAudioInputDevices) {
  EXPECT_TRUE(ohos_audio_manager_->HasAudioInputDevices());
}

TEST_F(OHOSAudioManagerTest, TestGetAudioOutputDeviceNames) {
  media::AudioDeviceNames deviceNames;
  ohos_audio_manager_->GetAudioOutputDeviceNames(&deviceNames);
  EXPECT_GT(deviceNames.size(), 0);
  bool foundExpectedDevice = false;
  for (const auto& deviceName : deviceNames) {
    if (!deviceName.device_name.empty()) {
      foundExpectedDevice = true;
      break;
    }
  }
  EXPECT_TRUE(foundExpectedDevice);
}

#if BUILDFLAG(ARKWEB_WEBRTC)

TEST_F(OHOSAudioManagerTest, TestGetAudioInputDeviceNames) {
  media::AudioDeviceNames deviceNames;
  ohos_audio_manager_->GetAudioInputDeviceNames(&deviceNames);

  EXPECT_GT(deviceNames.size(), 0);
  bool foundExpectedDevice = false;
  for (const auto& deviceName : deviceNames) {
    if (!deviceName.device_name.empty()) {
      foundExpectedDevice = true;
      break;
    }
  }
  EXPECT_TRUE(foundExpectedDevice);
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

TEST_F(OHOSAudioManagerTest, TestMakeLowLatencyInputStream) {
  AudioParameters params;
  std::string device_id = "some_device_id";
  AudioManagerBase::LogCallback log_callback;
  AudioInputStream* inputStream =
      ohos_audio_manager_->MakeLowLatencyInputStream(params, device_id,
                                                     log_callback);

  EXPECT_NE(inputStream, nullptr);
}

TEST_F(OHOSAudioManagerTest, TestGetPreferredOutputStreamParameters) {
  std::string output_device_id = "some_device_id";
  AudioParameters expectedOutputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumOutputBufferSize);
  AudioParameters output_params;
  AudioParameters actualOutputParams =
      ohos_audio_manager.GetPreferredOutputStreamParametersForTest(
          output_device_id, output_params);

  EXPECT_EQ(actualOutputParams.sample_rate(),
            expectedOutputParams.sample_rate());
  EXPECT_EQ(actualOutputParams.channel_layout(),
            expectedOutputParams.channel_layout());
  EXPECT_EQ(actualOutputParams.frames_per_buffer(),
            expectedOutputParams.frames_per_buffer());
}
#if BUILDFLAG(ARKWEB_WEBRTC)
TEST_F(OHOSAudioManagerTest, TestGetPreferredInputStreamParameters) {
  std::string input_device_id = "some_device_id";
  AudioParameters expectedInputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);

  expectedInputParams.set_effects(0);
  AudioParameters input_params;
  AudioParameters actualInputParams =
      ohos_audio_manager.GetPreferredInputStreamParametersForTest(
          input_device_id, input_params);

  EXPECT_EQ(actualInputParams.sample_rate(), expectedInputParams.sample_rate());
  EXPECT_EQ(actualInputParams.channel_layout(),
            expectedInputParams.channel_layout());
  EXPECT_EQ(actualInputParams.frames_per_buffer(),
            expectedInputParams.frames_per_buffer());
  EXPECT_EQ(actualInputParams.effects(), expectedInputParams.effects());
}

TEST_F(OHOSAudioManagerTest, TestSelectAudioDevice) {
  std::string device_id = "some_device_id";
  ohos_audio_manager.SelectAudioDevicePublic(device_id, false);
}

TEST_F(OHOSAudioManagerTest, TestGetPreferredInputStreamParameters001) {
  std::string input_device_id = "";
  AudioParameters expectedInputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);

  expectedInputParams.set_effects(0);
  AudioParameters input_params;
  AudioParameters actualInputParams =
      ohos_audio_manager.GetPreferredInputStreamParametersForTest(
          input_device_id, input_params);

  EXPECT_EQ(actualInputParams.sample_rate(), expectedInputParams.sample_rate());
}

TEST_F(OHOSAudioManagerTest, TestGetPreferredInputStreamParameters002) {
  std::string input_device_id = "screen:systemAudio:-2:0";
  AudioParameters expectedInputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);

  expectedInputParams.set_effects(0);
  AudioParameters input_params;
  AudioParameters actualInputParams =
      ohos_audio_manager.GetPreferredInputStreamParametersForTest(
          input_device_id, input_params);

  EXPECT_EQ(actualInputParams.sample_rate(), expectedInputParams.sample_rate());
}

TEST_F(OHOSAudioManagerTest, TestGetPreferredOutputStreamParameters001) {
  std::string output_device_id = "";
  AudioParameters expectedOutputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumOutputBufferSize);
  AudioParameters output_params;
  AudioParameters actualOutputParams =
      ohos_audio_manager.GetPreferredOutputStreamParametersForTest(
          output_device_id, output_params);

  EXPECT_EQ(actualOutputParams.sample_rate(),
            expectedOutputParams.sample_rate());
}

TEST_F(OHOSAudioManagerTest, TestGetPreferredOutputStreamParameters002) {
  std::string output_device_id = "screen:systemAudio:-2:0";
  AudioParameters expectedOutputParams(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumOutputBufferSize);
  AudioParameters output_params;
  AudioParameters actualOutputParams =
      ohos_audio_manager.GetPreferredOutputStreamParametersForTest(
          output_device_id, output_params);

  EXPECT_EQ(actualOutputParams.sample_rate(),
            expectedOutputParams.sample_rate());
}

TEST_F(OHOSAudioManagerTest, TestSelectAudioDevice001) {
  std::string device_id = "";
  ohos_audio_manager_->SelectAudioDevice(device_id, false);
  EXPECT_EQ(ohos_audio_manager_->device_id_, device_id);
}

TEST_F(OHOSAudioManagerTest, TestSelectAudioDevice002) {
  std::string device_id = "screen:systemAudio:-2:0";
  ohos_audio_manager_->SelectAudioDevice(device_id, false);
  EXPECT_EQ(ohos_audio_manager_->device_id_, device_id);
}

TEST_F(OHOSAudioManagerTest, TestSelectAudioDevice003) {
  std::string device_id = "some_device_id";
  EXPECT_CALL(*mock_adapter_helper_, GetAudioSystemManager()).Times(0);
  ohos_audio_manager_->SelectAudioDevice(device_id, false);
  EXPECT_EQ(ohos_audio_manager_->device_id_, device_id);
}

TEST_F(OHOSAudioManagerTest, TestGetSelectAudioDeviceId001) {
  std::string device_id = "";
  ohos_audio_manager_->SelectAudioDevice(device_id, false);
  ohos_audio_manager_->GetSelectAudioDeviceId();
  EXPECT_EQ(ohos_audio_manager_->device_id_, device_id);
}

TEST_F(OHOSAudioManagerTest, TestGetSelectAudioDeviceId002) {
  std::string device_id = "some_device_id";
  ohos_audio_manager_->SelectAudioDevice(device_id, false);
  ohos_audio_manager_->GetSelectAudioDeviceId();
  EXPECT_EQ(ohos_audio_manager_->device_id_, device_id);
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}  // namespace media
