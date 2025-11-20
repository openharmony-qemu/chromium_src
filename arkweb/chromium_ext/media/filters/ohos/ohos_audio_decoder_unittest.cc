// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <utility>
#include <vector>
#include <cstring>
#include <mutex>
#include <string>
#include <charconv>

#include "testing/gtest/include/gtest/gtest.h"
#include <gmock/gmock.h>
#include "base/test/test_simple_task_runner.h"
#include "base/run_loop.h"
#include "base/test/test_future.h"
#include "base/test/task_environment.h"
#include "base/test/bind.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

#include "arkweb/chromium_ext/media/cdm/wiseplay_cdm_common.h"
#define private public
#include "arkweb/chromium_ext/media/filters/ohos/ohos_audio_decoder.h"
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_drm_bridge.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#undef private

namespace media {

class OHOSAudioCencInfoTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(OHOSAudioCencInfoTest, GetKeyID_ShouldReturnKeyId_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint8_t keyId[10] = {0};
  cenc.SetKeyId(keyId);
  EXPECT_EQ(cenc.GetKeyId(), keyId);
}

TEST_F(OHOSAudioCencInfoTest, GetKeyIDLen_ShouldReturnKeyIdLen_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t keyIdLen = 16;
  cenc.SetKeyIdLen(keyIdLen);
  EXPECT_EQ(cenc.GetKeyIdLen(), keyIdLen);
}

TEST_F(OHOSAudioCencInfoTest, GetIv_ShouldReturnIv_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint8_t iv[10] = {0};
  cenc.SetIv(iv);
  EXPECT_EQ(cenc.GetIv(), iv);
}

TEST_F(OHOSAudioCencInfoTest, GetIvLen_ShouldReturnIvLen_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t ivLen = 16;
  cenc.SetIvLen(ivLen);
  EXPECT_EQ(cenc.GetIvLen(), ivLen);
}

TEST_F(OHOSAudioCencInfoTest, GetAlgo_ShouldReturnAlgo_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t algo = 1;
  cenc.SetAlgo(algo);
  EXPECT_EQ(cenc.GetAlgo(), algo);
}

TEST_F(OHOSAudioCencInfoTest,
       GetEncryptedBlockCount_ShouldReturnEncryptedBlockCount_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t encryptedBlockCount = 10;
  cenc.SetEncryptedBlockCount(encryptedBlockCount);
  EXPECT_EQ(cenc.GetEncryptedBlockCount(), encryptedBlockCount);
}

TEST_F(OHOSAudioCencInfoTest,
       GetSkippedBlockCount_ShouldReturnSkippedBlockCount_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t skippedBlockCount = 5;
  cenc.SetSkippedBlockCount(skippedBlockCount);
  EXPECT_EQ(cenc.GetSkippedBlockCount(), skippedBlockCount);
}

TEST_F(OHOSAudioCencInfoTest,
       GetFirstEncryptedOffset_ShouldReturnFirstEncryptedOffset_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t firstEncryptedOffset = 5;
  cenc.SetFirstEncryptedOffset(firstEncryptedOffset);
  EXPECT_EQ(cenc.GetFirstEncryptedOffset(), firstEncryptedOffset);
}

TEST_F(OHOSAudioCencInfoTest,
       GetClearHeaderLens_ShouldReturnClearHeaderLens_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
  cenc.SetClearHeaderLens(clearHeaderLens);
  EXPECT_EQ(cenc.GetClearHeaderLens(), clearHeaderLens);
}

TEST_F(OHOSAudioCencInfoTest, GetPayLoadLens_ShouldReturnPayLoadLens_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  std::vector<uint32_t> payLoadLens = {4, 5, 6};
  cenc.SetPayLoadLens(payLoadLens);
  EXPECT_EQ(cenc.GetPayLoadLens(), payLoadLens);
}

TEST_F(OHOSAudioCencInfoTest, GetMode_ShouldReturnMode_WhenCalled)
{
  OHOSAudioCencInfo cenc;
  uint32_t mode = 1;
  cenc.SetMode(mode);
  EXPECT_EQ(cenc.GetMode(), mode);
}

class OHOSAudioDecoderFormatTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(OHOSAudioCencInfoTest, FunctionsCalled)
{
  constexpr int32_t DEFAULT_SAMPLERATE = 44100;
  constexpr int64_t DEFAULT_BITRATE = 32000;
  constexpr int32_t DEFAULT_CHANNEL_COUNT = 2;
  constexpr int32_t DEFAULT_MAX_INPUT_SIZE = 1152;
  constexpr bool DEFAUL_AAC_IS_ADTS = false;
  constexpr int32_t DEFAULT_AUDIO_SAMPLE_FORMAT = 100;
  constexpr int32_t DEFAUL_ID_HEARDER = 100;
  constexpr int32_t DEFAULT_SETUP_HEADER = 100;
  uint8_t codecConfig[10] = {0};
  constexpr uint32_t DEFAULT_CODEC_Config_SIZE = 10;
  std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
  format->SetSampleRate(DEFAULT_SAMPLERATE);
  format->SetChannelCount(DEFAULT_CHANNEL_COUNT);
  format->SetBitRate(DEFAULT_BITRATE);
  format->SetMaxInputSize(DEFAULT_MAX_INPUT_SIZE);
  format->SetAACIsAdts(DEFAUL_AAC_IS_ADTS);
  format->SetAudioSampleFormat(DEFAULT_AUDIO_SAMPLE_FORMAT);
  format->SetIdentificationHeader(DEFAUL_ID_HEARDER);
  format->SetSetupHeader(DEFAULT_SETUP_HEADER);
  format->SetCodecConfig(codecConfig);
  format->SetCodecConfigSize(DEFAULT_CODEC_Config_SIZE);
  format->PrintFormatData(format);
  EXPECT_EQ(format->GetSampleRate(), DEFAULT_SAMPLERATE);
  EXPECT_EQ(format->GetChannelCount(), DEFAULT_CHANNEL_COUNT);
  EXPECT_EQ(format->GetBitRate(), DEFAULT_BITRATE);
  EXPECT_EQ(format->GetMaxInputSize(), DEFAULT_MAX_INPUT_SIZE);
  EXPECT_EQ(format->GetAACIsAdts(), DEFAUL_AAC_IS_ADTS);
  EXPECT_EQ(format->GetAudioSampleFormat(), DEFAULT_AUDIO_SAMPLE_FORMAT);
  EXPECT_EQ(format->GetIdentificationHeader(), DEFAUL_ID_HEARDER);
  EXPECT_EQ(format->GetSetupHeader(), DEFAULT_SETUP_HEADER);
  EXPECT_EQ(format->GetCodecConfig(), codecConfig);
  EXPECT_EQ(format->GetCodecConfigSize(), DEFAULT_CODEC_Config_SIZE);
}


class AudioDecoderCallbackTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

class AudioDecoderCallbackClientMock : public AudioDecoderCallback::Client {
public:
  MOCK_METHOD(void, AddInputBuffer, (uint32_t index), (override));

  MOCK_METHOD(void, AddOutputBuffer,
      (uint32_t index, uint8_t* bufferData, uint32_t size, int64_t pts, BufferFlag flag), (override));

  MOCK_METHOD(void, UpdateOutputFormat, (), (override));

  MOCK_METHOD(void, OnError, (int32_t errorCode), (override));
};

TEST_F(AudioDecoderCallbackTest, FunctionsCalled) {
  AudioDecoderCallbackClientMock* client = new AudioDecoderCallbackClientMock();

  EXPECT_CALL(*client, OnError(0)).Times(1);
  EXPECT_CALL(*client, UpdateOutputFormat()).Times(1);
  EXPECT_CALL(*client, AddInputBuffer(0)).Times(1);
  EXPECT_CALL(*client, AddOutputBuffer(0, nullptr, 0, 0, static_cast<BufferFlag>(0))).Times(1);

  AudioDecoderCallback cb(client);
  cb.OnError(0);
  cb.OnOutputFormatChanged();
  cb.OnInputBufferAvailable(0);
  cb.OnOutputBufferAvailable(0, nullptr, 0, 0, 0, 0);

  delete client;
  client = nullptr;

  AudioDecoderCallback cb1(nullptr);
  cb1.OnError(0);
  cb1.OnOutputFormatChanged();
  cb1.OnInputBufferAvailable(0);
  cb1.OnOutputBufferAvailable(0, nullptr, 0, 0, 0, 0);
}

namespace {
  CreateFetcherCB create_fetcher_cb;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;

  std::map<AudioCodec, std::string> mimeTypeMap_ = {
      {AudioCodec::kAAC, "audio/mp4a-latm"},
      {AudioCodec::kFLAC, "audio/flac"},
      {AudioCodec::kVorbis, "audio/vorbis"},
      {AudioCodec::kMP3, "audio/mpeg"},
      {AudioCodec::kAMR_NB, "audio/3gpp"},
      {AudioCodec::kAMR_WB, "audio/amr-wb"},
      {AudioCodec::kUnknown, ""},
      {AudioCodec::kOpus, ""},
      {AudioCodec::kPCM, ""},
      {AudioCodec::kPCM_MULAW, ""},
      {AudioCodec::kGSM_MS, ""},
      {AudioCodec::kPCM_S16BE, ""},
      {AudioCodec::kPCM_S24BE, ""},
      {AudioCodec::kPCM_ALAW, ""},
      {AudioCodec::kALAC, ""},
      {AudioCodec::kAC3, ""},
      {AudioCodec::kEAC3, ""},
      {AudioCodec::kDTS, ""},
      {AudioCodec::kDTSXP2, ""},
      {AudioCodec::kDTSE, ""},
      {AudioCodec::kMpegHAudio, ""},
      {AudioCodec::kAC4, ""},
      {AudioCodec::kIAMF, ""},
  };

  AudioDecoderConfig NewAudioConfig(
      AudioCodec codec,
      EncryptionScheme encryption_scheme = EncryptionScheme::kUnencrypted,
      std::vector<uint8_t> extra_data = std::vector<uint8_t>()) {
      return AudioDecoderConfig(codec, kSampleFormatS16, CHANNEL_LAYOUT_MONO, 44100,
                                extra_data, encryption_scheme);
  }
}

class OhosAudioDecoderTest : public testing::Test {
 public:
  std::unique_ptr<CdmContext> cdm_context_;
  std::unique_ptr<OHOSAudioDecoder> decoder_;
  base::test::SingleThreadTaskEnvironment task_environment_;

  void SetUp() override {
    decoder_ = std::make_unique<OHOSAudioDecoder>(task_environment_.GetMainThreadTaskRunner());
    ASSERT_TRUE(decoder_);
    cdm_context_ = CreateOHOSCdmContext();
    ASSERT_TRUE(cdm_context_);
  }

  void TearDown() override {
    decoder_.reset();
    cdm_context_.reset();
  }
  std::unique_ptr<CdmContext> CreateOHOSCdmContext();
  void InitializeDecoder();
};

std::unique_ptr<CdmContext> OhosAudioDecoderTest::CreateOHOSCdmContext()
{
  std::string origin_id = "xxx";
  OHOSMediaDrmBridge::SecurityLevel level = OHOSMediaDrmBridge::SECURITY_LEVEL_3;
  bool require_media_crypto = true;
  base::RunLoop run_loop;
  base::RepeatingTimer timer;
  const base::TimeDelta timeout = base::Seconds(2);
  base::TimeTicks start_time = base::TimeTicks::Now();

  std::unique_ptr<OHOSMediaDrmStorageBridge> storage = std::make_unique<OHOSMediaDrmStorageBridge>();
  std::vector<uint8_t> wiseplay_uuid = std::vector<uint8_t>(kWiseplayUuid, kWiseplayUuid + sizeof(kWiseplayUuid));
  std::unique_ptr<OHOSMediaDrmBridge> bridge = std::make_unique<OHOSMediaDrmBridge>(
      wiseplay_uuid, origin_id, level, require_media_crypto, std::move(storage),
      create_fetcher_cb, session_message_cb, session_closed_cb,
      session_keys_change_cb, session_expiration_update_cb);
  bridge->is_license_ready_ = true;

  timer.Start(FROM_HERE, base::Milliseconds(100),
        base::BindLambdaForTesting([&]() {
            task_environment_.GetMainThreadTaskRunner()->RunsTasksInCurrentSequence();
            if (base::TimeTicks::Now() - start_time > timeout) {
                timer.Stop();
                run_loop.Quit();
            }
            if (bridge->ohos_media_key_session_ != nullptr) {
                timer.Stop();
                run_loop.Quit();
            }
        }));
  run_loop.Run();
  EXPECT_NE(bridge->ohos_media_key_session_, nullptr);
  return std::move(bridge);
}

void OhosAudioDecoderTest::InitializeDecoder() {
  AudioDecoderConfig config = NewAudioConfig(AudioCodec::kMP3);
  config.SetIsEncrypted(true);
  DecoderStatus target_status = media::DecoderStatus::Codes::kOk;
  decoder_->Initialize(config, cdm_context_.get(),
                       base::BindOnce(
                          [](DecoderStatus target_status, DecoderStatus status) {
                              EXPECT_EQ(target_status, status);
                          },
                          target_status),
                       base::DoNothing(),
                       base::DoNothing());
  base::RunLoop().RunUntilIdle();
}

TEST_F(OhosAudioDecoderTest, Initialize)
{
  std::vector<uint8_t> invalid_first_byte = {{0x00, 0xff, 0xff, 0xff, 0xff}};
  AudioDecoderConfig config =
      NewAudioConfig(AudioCodec::kUnknown, EncryptionScheme::kUnencrypted, invalid_first_byte);
  {
    // config is kUnencrypted
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    AudioDecoder::OutputCB output_cb = base::DoNothing();
    WaitingCB waiting_cb = base::DoNothing();
    decoder_->Initialize(config, nullptr,
    std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kUnsupportedCodec);
  }

  {
    // decoder state is error
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    AudioDecoder::OutputCB output_cb = base::DoNothing();
    WaitingCB waiting_cb = base::DoNothing();
    config = NewAudioConfig(AudioCodec::kUnknown, EncryptionScheme::kCenc, invalid_first_byte);
    decoder_->state_ = OHOSAudioDecoder::ERROR;
    decoder_->Initialize(config, nullptr,
        std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kFailed);
  }

  {
    // decoder format is error
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    AudioDecoder::OutputCB output_cb = base::DoNothing();
    WaitingCB waiting_cb = base::DoNothing();
    decoder_->state_ = OHOSAudioDecoder::UNINITIALIZED;
    config = NewAudioConfig(AudioCodec::kUnknown, EncryptionScheme::kCenc, invalid_first_byte);
    decoder_->Initialize(config, nullptr,
        std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kUnsupportedCodec);
  }

  {
    // cdm_context is nullptr
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    AudioDecoder::OutputCB output_cb = base::DoNothing();
    WaitingCB waiting_cb = base::DoNothing();
    decoder_->state_ = OHOSAudioDecoder::UNINITIALIZED;
    config = NewAudioConfig(AudioCodec::kMP3, EncryptionScheme::kCenc, invalid_first_byte);
    decoder_->Initialize(config, nullptr,
        std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kUnsupportedEncryptionMode);
  }

  {
    // cdm_context is not nullptr
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    AudioDecoder::OutputCB output_cb = base::DoNothing();
    WaitingCB waiting_cb = base::DoNothing();
    decoder_->state_ = OHOSAudioDecoder::UNINITIALIZED;
    decoder_->Initialize(config, cdm_context_.get(),
        std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
    EXPECT_TRUE(decoder_->ohos_crypto_context_);
  }
}

TEST_F(OhosAudioDecoderTest, GetMimeType)
{
  for (auto it = mimeTypeMap_.begin(); it != mimeTypeMap_.end(); it++) {
    AudioDecoderConfig config = NewAudioConfig(it->first);
    EXPECT_EQ(decoder_->GetMimeType(config), it->second);
    decoder_->mime_type_ = "";
  }
}

TEST_F(OhosAudioDecoderTest, InitializeNotEncrypted)
{
  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    decoder_->mime_type_ = decoder_->GetMimeType(NewAudioConfig(AudioCodec::kUnknown));
    decoder_->InitializeNotEncrypted(std::move(init_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kFailed);
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    decoder_->sample_rate_ = 44100;
    decoder_->channel_count_ = 2;
    decoder_->mime_type_ = decoder_->GetMimeType(NewAudioConfig(AudioCodec::kMP3));
    decoder_->InitializeNotEncrypted(std::move(init_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kOk);
  }
}

TEST_F(OhosAudioDecoderTest, SetCdm)
{
  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    decoder_->SetCdm(nullptr, std::move(init_cb));
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kFailed);
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::InitCB init_cb = future.GetCallback();
    decoder_->SetCdm(cdm_context_.get(), std::move(init_cb));
    ASSERT_TRUE(decoder_->ohos_crypto_context_);
  }
}

TEST_F(OhosAudioDecoderTest, OnCdmContextEvent)
{
  decoder_->OnCdmContextEvent(CdmContext::Event::kHardwareContextReset);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::UNINITIALIZED);

  decoder_->OnCdmContextEvent(CdmContext::Event::kHasAdditionalUsableKey);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::READY);

  decoder_->CreateOhosDecoderLoop();
  decoder_->OnCdmContextEvent(CdmContext::Event::kHasAdditionalUsableKey);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::READY);
}

TEST_F(OhosAudioDecoderTest, OnMediaCryptoReady)
{
  base::test::TestFuture<DecoderStatus> future;
  AudioDecoder::InitCB init_cb = future.GetCallback();
  decoder_->OnMediaCryptoReady(std::move(init_cb), nullptr, false);
  EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kUnsupportedEncryptionMode);

  decoder_->mime_type_ = "test";
  decoder_->ohos_crypto_context_ = cdm_context_->GetOHOSMediaCryptoContext();
  AudioDecoder::InitCB init_cb1 = future.GetCallback();
  int32_t test_session;
  decoder_->OnMediaCryptoReady(std::move(init_cb1), &test_session, false);
  EXPECT_EQ(decoder_->mediaKeySession_, &test_session);
}

TEST_F(OhosAudioDecoderTest, ClearInputQueue)
{
  base::test::TestFuture<DecoderStatus> future;
  AudioDecoder::DecodeCB decode_cb = future.GetCallback();
  decoder_->input_queue_.clear();
  scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
  decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
  decoder_->ClearInputQueue(media::DecoderStatus::Codes::kFailed);
  EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kFailed);
}

TEST_F(OhosAudioDecoderTest, OnError)
{
  decoder_->state_ = OHOSAudioDecoder::State::UNINITIALIZED;
  decoder_->waiting_for_key_ = true;
  decoder_->OnError(0);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::WAITING_FOR_MEDIA_CRYPTO);

  decoder_->state_ = OHOSAudioDecoder::State::UNINITIALIZED;
  decoder_->waiting_for_key_ = false;
  decoder_->OnError(0);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::UNINITIALIZED);

  decoder_->state_ = OHOSAudioDecoder::State::WAITING_FOR_MEDIA_CRYPTO;
  decoder_->waiting_for_key_ = true;
  decoder_->OnError(0);
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::WAITING_FOR_MEDIA_CRYPTO);
}

TEST_F(OhosAudioDecoderTest, NeedsBitstreamConversion)
{
  decoder_->PrepareParameters(NewAudioConfig(AudioCodec::kAAC));
  ASSERT_TRUE(decoder_->NeedsBitstreamConversion());

  decoder_->PrepareParameters(NewAudioConfig(AudioCodec::kMP3));
  ASSERT_FALSE(decoder_->NeedsBitstreamConversion());
}

TEST_F(OhosAudioDecoderTest, OnInputDataQueued)
{
  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    decoder_->OnInputDataQueued(true);
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kOk);
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    decoder_->OnInputDataQueued(true);
    ASSERT_FALSE(future.IsReady());
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    decoder_->OnInputDataQueued(false);
    EXPECT_EQ(future.Get(), media::DecoderStatus::Codes::kFailed);
  }
}

TEST_F(OhosAudioDecoderTest, IsAnyInputPending)
{
  decoder_->state_ = OHOSAudioDecoder::State::ERROR;
  ASSERT_FALSE(decoder_->IsAnyInputPending());

  decoder_->state_ = OHOSAudioDecoder::State::READY;
  decoder_->input_queue_.clear();
  ASSERT_FALSE(decoder_->IsAnyInputPending());

  base::test::TestFuture<DecoderStatus> future;
  AudioDecoder::DecodeCB decode_cb = future.GetCallback();
  decoder_->input_queue_.clear();
  scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
  decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
  ASSERT_TRUE(decoder_->IsAnyInputPending());
}

TEST_F(OhosAudioDecoderTest, ProvideInputData)
{
  {
    decoder_->input_queue_.clear();
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_FALSE(data.is_valid);
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_TRUE(data.is_eos);
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_FALSE(data.is_eos);
    ASSERT_FALSE(data.cenc_info);
  }

  const uint32_t crypt_byte_block = 1;
  const uint32_t skip_byte_block = 1;
  std::vector<SubsampleEntry> subsamples;
  std::string iv(DecryptConfig::kDecryptionKeySize, 't');
  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    std::optional<EncryptionPattern> opt = EncryptionPattern{crypt_byte_block, skip_byte_block};
    std::unique_ptr<DecryptConfig> decrypt_config = std::make_unique<DecryptConfig>(
        EncryptionScheme::kUnencrypted, "key_id", iv, subsamples, std::move(opt));
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    buffer->set_decrypt_config(std::move(decrypt_config));
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_TRUE(data.cenc_info);
    ASSERT_FALSE(data.is_encrypted);
    EXPECT_EQ(data.cenc_info->GetEncryptedBlockCount(), crypt_byte_block);
    ASSERT_TRUE(data.cenc_info->GetClearHeaderLens().empty());
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    std::optional<EncryptionPattern> opt = EncryptionPattern{crypt_byte_block, skip_byte_block};
    subsamples = {{4096, 0}, {0, 4096}};
    std::unique_ptr<DecryptConfig> decrypt_config = std::make_unique<DecryptConfig>(
        EncryptionScheme::kUnencrypted, "key_id", iv, subsamples, std::move(opt));
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    buffer->set_decrypt_config(std::move(decrypt_config));
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_TRUE(data.cenc_info);
    ASSERT_FALSE(data.is_encrypted);
    EXPECT_EQ(data.cenc_info->GetEncryptedBlockCount(), crypt_byte_block);
    ASSERT_FALSE(data.cenc_info->GetClearHeaderLens().empty());
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    std::optional<EncryptionPattern> opt = EncryptionPattern{crypt_byte_block, skip_byte_block};
    std::unique_ptr<DecryptConfig> decrypt_config = std::make_unique<DecryptConfig>(
        EncryptionScheme::kCenc, "key_id", iv, subsamples, std::move(opt));
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    buffer->set_decrypt_config(std::move(decrypt_config));
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_TRUE(data.cenc_info);
    ASSERT_TRUE(data.is_encrypted);
    EXPECT_EQ(data.cenc_info->GetAlgo(), uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CTR));
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    std::optional<EncryptionPattern> opt = EncryptionPattern{crypt_byte_block, skip_byte_block};
    std::unique_ptr<DecryptConfig> decrypt_config = std::make_unique<DecryptConfig>(
        EncryptionScheme::kCbcs, "key_id", iv, subsamples, std::move(opt));
    decoder_->input_queue_.clear();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    buffer->set_decrypt_config(std::move(decrypt_config));
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    OHOSAudioDecoderLoop::InputData data = decoder_->ProvideInputData();
    ASSERT_TRUE(data.is_valid);
    ASSERT_TRUE(data.cenc_info);
    ASSERT_TRUE(data.is_encrypted);
    EXPECT_EQ(data.cenc_info->GetAlgo(), uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CBC));
  }
}

TEST_F(OhosAudioDecoderTest, OnDecodedEos)
{
  OutputBufferData out;
  decoder_->input_queue_.clear();
  ASSERT_FALSE(decoder_->OnDecodedEos(out));

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(1);
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    ASSERT_FALSE(decoder_->OnDecodedEos(out));
  }

  {
    base::test::TestFuture<DecoderStatus> future;
    AudioDecoder::DecodeCB decode_cb = future.GetCallback();
    decoder_->ClearInputQueue(media::DecoderStatus::Codes::kFailed);
    scoped_refptr<DecoderBuffer> buffer = DecoderBuffer::CreateEOSBuffer();
    decoder_->input_queue_.push_back(std::make_pair(std::move(buffer), std::move(decode_cb)));
    ASSERT_TRUE(decoder_->OnDecodedEos(out));
  }
}

TEST_F(OhosAudioDecoderTest, OnDecodedFrame)
{
  size_t size = 1024;
  uint32_t index = 0;
  uint8_t data[1024] = {0};
  int64_t pts = 0;
  int sample_rate_ = 44100;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  {
    OutputBufferData bufferData(index, nullptr, 0, pts, flag);
    ASSERT_FALSE(decoder_->OnDecodedFrame(bufferData));
  }

  {
    OutputBufferData bufferData(OHOSAudioDecoderLoop::kInvalidBufferIndex, data , size , pts, flag);
    ASSERT_FALSE(decoder_->OnDecodedFrame(bufferData));
  }

  {
    OutputBufferData bufferData(index, data , size , pts, flag);
    ASSERT_FALSE(decoder_->OnDecodedFrame(bufferData));
  }

  {
    decoder_->CreateOhosDecoderLoop();
    OutputBufferData bufferData(index, data , size , pts, flag);
    ASSERT_FALSE(decoder_->OnDecodedFrame(bufferData));
  }

  {
    // audio_decoder_ is nullptr
    OutputBufferData bufferData(index, data , size , pts, flag);
    ASSERT_FALSE(decoder_->OnDecodedFrame(bufferData));
  }

  {
    InitializeDecoder();
    OutputBufferData bufferData(index, data , size , pts, flag);
    ASSERT_TRUE(decoder_->OnDecodedFrame(bufferData));
    ASSERT_TRUE(decoder_->timestamp_helper_->base_timestamp());
    ASSERT_TRUE(decoder_->OnDecodedFrame(bufferData));
  }
}

TEST_F(OhosAudioDecoderTest, OnCodecLoopError)
{
  decoder_->OnCodecLoopError();
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::State::ERROR);
}

TEST_F(OhosAudioDecoderTest, DequeueInputBuffer)
{
  int64_t index = 1;
  decoder_->input_buffer_queue_.clear();
  EXPECT_EQ(decoder_->DequeueInputBuffer(index), -1);

  decoder_->AddInputBuffer(index);
  EXPECT_EQ(decoder_->DequeueInputBuffer(index), index);
}

TEST_F(OhosAudioDecoderTest, EnqueueInputBuffer)
{
  int64_t index = 1;
  decoder_->input_buffer_queue_.clear();
  decoder_->EnqueueInputBuffer(index);
  ASSERT_FALSE(decoder_->input_buffer_queue_.empty());
}

TEST_F(OhosAudioDecoderTest, DequeueOutputBuffer)
{
  OutputBufferData out;
  decoder_->output_buffer_queue_.clear();
  EXPECT_EQ(decoder_->DequeueOutputBuffer(out), -1);

  size_t size = 1024;
  uint32_t index = 0;
  uint8_t data[1024] = {0};
  int64_t pts = 0;
  int sample_rate_ = 44100;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  decoder_->AddOutputBuffer(index, data, size, pts, flag);
  decoder_->EnqueueInputBuffer(index);
  EXPECT_EQ(decoder_->DequeueOutputBuffer(out), 0);
}

TEST_F(OhosAudioDecoderTest, QueueInputBufferDec)
{
  int32_t index = 1;
  decoder_->SetState(OHOSAudioDecoder::WAITING_FOR_MEDIA_CRYPTO);
  auto code = decoder_->QueueInputBufferDec(index, 0, nullptr, 0, nullptr, true, BufferFlag::CODEC_BUFFER_FLAG_EOS);
  EXPECT_EQ(code, AudioDecoderAdapterCode::DECODER_RETRY);

  decoder_->SetState(OHOSAudioDecoder::READY);
  code = decoder_->QueueInputBufferDec(index, 0, nullptr, 0, nullptr, true, BufferFlag::CODEC_BUFFER_FLAG_EOS);
  EXPECT_EQ(code, AudioDecoderAdapterCode::DECODER_ERROR);

  decoder_->audio_decoder_ = OhosAdapterHelper::GetInstance().CreateAudioCodecDecoderAdapter();
  code = decoder_->QueueInputBufferDec(index, 0, nullptr, 0, nullptr, true, BufferFlag::CODEC_BUFFER_FLAG_EOS);
  EXPECT_EQ(code, AudioDecoderAdapterCode::DECODER_RETRY);

  decoder_->SetState(OHOSAudioDecoder::READY);
  decoder_->audio_decoder_ = OhosAdapterHelper::GetInstance().CreateAudioCodecDecoderAdapter();
  code = decoder_->QueueInputBufferDec(index, 0, nullptr, 0, nullptr, true, BufferFlag::CODEC_BUFFER_FLAG_EOS);
  EXPECT_EQ(code, AudioDecoderAdapterCode::DECODER_RETRY);

  decoder_->SetState(OHOSAudioDecoder::READY);
  decoder_->waiting_for_key_ = false;
  code = decoder_->QueueInputBufferDec(index, 0, nullptr, 0, nullptr, true, BufferFlag::CODEC_BUFFER_FLAG_EOS);
  EXPECT_EQ(code, AudioDecoderAdapterCode::DECODER_ERROR);
}

TEST_F(OhosAudioDecoderTest, WaitingForLicence)
{
  decoder_->waiting_for_key_ = false;
  decoder_->WaitingForLicence();
  EXPECT_EQ(decoder_->time_out_count_, 0);

  decoder_->waiting_for_key_ = true;
  decoder_->time_out_count_ = 5;
  decoder_->WaitingForLicence();
  EXPECT_EQ(decoder_->time_out_count_, 0);

  decoder_->time_out_count_ = 0;
  decoder_->WaitingForLicence();
  EXPECT_EQ(decoder_->time_out_count_, 1);

  decoder_->WaitingForLicence();
  EXPECT_EQ(decoder_->time_out_count_, 2);
}

TEST_F(OhosAudioDecoderTest, InitAudioDecoder)
{
  bool ret = decoder_->InitAudioDecoder("test");
  EXPECT_EQ(ret, false);
}

TEST_F(OhosAudioDecoderTest, Decode)
{
  const uint8_t kData[] = "hello";
  scoped_refptr<DecoderBuffer> buffer1(DecoderBuffer::CopyFrom(kData));
  const char kKeyId[] = "key id";
  const char kIv[] = "0123456789abcdef";
  std::vector<SubsampleEntry> subsamples;
  subsamples.emplace_back(10, 5);
  subsamples.emplace_back(15, 7);
  buffer1->set_decrypt_config(
      DecryptConfig::CreateCencConfig(kKeyId, kIv, subsamples));
  auto cb1 = base::BindOnce([](DecoderStatus status) {
    EXPECT_EQ(status, DecoderStatus::Codes::kFailed);
  });
  decoder_->Decode(std::move(buffer1), std::move(cb1));

  auto buffer2 = DecoderBuffer::CreateEOSBuffer();
  auto cb2 = base::BindOnce([](DecoderStatus status) {
    EXPECT_EQ(status, DecoderStatus::Codes::kFailed);
  });
  decoder_->Decode(std::move(buffer2), std::move(cb2));

  scoped_refptr<DecoderBuffer> buffer3(new DecoderBuffer(0));
  buffer3->set_timestamp(kNoTimestamp);
  auto cb3 = base::BindOnce([](DecoderStatus status) {
    EXPECT_EQ(status, DecoderStatus::Codes::kFailed);
  });
  decoder_->Decode(std::move(buffer3), std::move(cb3));

  auto buffer4 = DecoderBuffer::CreateEOSBuffer();
  auto cb4 = base::BindOnce([](DecoderStatus status) {});
  decoder_->SetState(OHOSAudioDecoder::WAITING_FOR_MEDIA_CRYPTO);
  decoder_->CreateOhosDecoderLoop();
  auto input_queue_size = decoder_->input_queue_.size();
  decoder_->Decode(std::move(buffer4), std::move(cb4));
  EXPECT_EQ(decoder_->input_queue_.size(), input_queue_size + 1);

  auto buffer5 = DecoderBuffer::CreateEOSBuffer();
  auto cb5 = base::BindOnce([](DecoderStatus status) {});
  decoder_->SetState(OHOSAudioDecoder::READY);
  input_queue_size = decoder_->input_queue_.size();
  decoder_->Decode(std::move(buffer5), std::move(cb5));
  EXPECT_EQ(decoder_->input_queue_.size(), input_queue_size + 1);
}

TEST_F(OhosAudioDecoderTest, Reset)
{
  decoder_->SetState(OHOSAudioDecoder::WAITING_FOR_MEDIA_CRYPTO);
  decoder_->Reset(base::BindLambdaForTesting([]() {}));
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::WAITING_FOR_MEDIA_CRYPTO);

  decoder_->audio_decoder_ = OhosAdapterHelper::GetInstance().CreateAudioCodecDecoderAdapter();
  decoder_->CreateOhosDecoderLoop();
  decoder_->PrepareParameters(NewAudioConfig(AudioCodec::kFLAC));
  decoder_->Reset(base::BindOnce([]() {}));
  EXPECT_EQ(decoder_->state_, OHOSAudioDecoder::ERROR);
}
}