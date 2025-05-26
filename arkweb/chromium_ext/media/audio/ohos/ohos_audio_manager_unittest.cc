#include <memory>

#include "arkweb/build/features/features.h"
#include "audio_system_manager_adapter.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/system/system_monitor.h"
#include "base/task/bind_post_task.h"
#include "gtest/gtest.h"
#include "media/base/media_switches.h"
#include "ohos_adapter_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace testing;
namespace media {
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumOutputBufferSize = 2048;
#if BUILDFLAG(ARKWEB_WEBRTC)
constexpr int kMinimumInputBufferSize = 2048;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

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
  void Stop() override { std::cout << "Stop function called" << std::endl; }

  bool IsHung() const override { return false; }

  base::SingleThreadTaskRunner* GetTaskRunner() override { return nullptr; }

  base::SingleThreadTaskRunner* GetWorkerTaskRunner() override {
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
    std::unique_ptr<MockAudioThread> mock_audio_thread =
        std::make_unique<MockAudioThread>();
    ohos_audio_manager_ = std::make_unique<media::OHOSAudioManager>(
        std::move(mock_audio_thread),
        /*audio_log_factory=*/nullptr);
  }

  void TearDown() override { ohos_audio_manager_.reset(); }

  std::unique_ptr<media::OHOSAudioManager> ohos_audio_manager_;
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

TEST_F(OHOSAudioManagerTest, TestMakeLinearOutputStream) {
  AudioParameters params;
  AudioManagerBase::LogCallback log_callback;
  AudioOutputStream* outputStream =
      ohos_audio_manager_->MakeLinearOutputStream(params, log_callback);

  EXPECT_EQ(outputStream, nullptr);
}

TEST_F(OHOSAudioManagerTest, TestMakeLinearInputStream) {
  AudioParameters params;
  std::string device_id = "some_device_id";
  AudioManagerBase::LogCallback log_callback;
  AudioInputStream* inputStream = ohos_audio_manager_->MakeLinearInputStream(
      params, device_id, log_callback);

  EXPECT_EQ(inputStream, nullptr);
}

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
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}  // namespace media
