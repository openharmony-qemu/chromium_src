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

#define private public
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_cenc_info_adapter_impl.h"
#undef private

#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {

const char *OH_AVCODEC_MIMETYPE_AUDIO_MPEG = "audio/mpeg";
const char *OH_AVCODEC_NAME_AUDIO_MPEG = "OH.Media.Codec.Decoder.Audio.Mpeg";
uint8_t keyId[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
uint8_t iv[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

class AudioDecoderCallbackAdapterMock : public AudioDecoderCallbackAdapter {
 public:
  AudioDecoderCallbackAdapterMock() = default;

  ~AudioDecoderCallbackAdapterMock() override = default;

  void OnError(int32_t errorCode) override {}

  void OnOutputFormatChanged() override {}

  void OnInputBufferAvailable(uint32_t index) override {}

  void OnOutputBufferAvailable(
      uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags) override {}
};

void AudioCodecDecoderImpl__DecoderFormat(FuzzedDataProvider* fdp)
{
  std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
  int32_t intTmp = fdp->ConsumeIntegralInRange<int32_t>(0, 64);
  int32_t configSize = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
  std::vector<uint8_t> config(configSize);
  fdp->ConsumeData(config.data(), configSize);
  int64_t biteRate = fdp->ConsumeIntegral<int64_t>();
  bool isAdts = fdp->ConsumeBool();
  format->SetSampleRate(intTmp);
  format->SetChannelCount(intTmp);
  format->SetBitRate(biteRate);
  format->SetMaxInputSize(intTmp);
  format->SetAACIsAdts(isAdts);
  format->SetAudioSampleFormat(intTmp);
  format->SetIdentificationHeader(intTmp);
  format->SetSetupHeader(intTmp);
  format->SetCodecConfig(config.data());
  format->SetCodecConfigSize(configSize);
  format->PrintFormatData(format);
  format->GetSampleRate();
  format->GetChannelCount();
  format->GetBitRate();
  format->GetMaxInputSize();
  format->GetAACIsAdts();
  format->GetAudioSampleFormat();
  format->GetIdentificationHeader();
  format->GetSetupHeader();
  format->GetCodecConfig();
  format->GetCodecConfigSize();
}

void AudioCodecDecoderImpl__DecoderCencInfo(FuzzedDataProvider* fdp)
{
  std::shared_ptr<AudioCencInfoAdapterImpl> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
  uint32_t uintTmp = fdp->ConsumeIntegral<int32_t>();
  uint32_t keyIdLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint8_t> vecKeyId(keyIdLen);
  fdp->ConsumeData(vecKeyId.data(), keyIdLen);
  uint32_t ivLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint8_t> vecIv(ivLen);
  fdp->ConsumeData(vecIv.data(), ivLen);
  uint32_t tmpLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint32_t> clearHeaderLens(tmpLen);
  std::vector<uint32_t> payLoadLens(tmpLen);
  fdp->ConsumeData(clearHeaderLens.data(), tmpLen);
  fdp->ConsumeData(payLoadLens.data(), tmpLen);
  cencInfo->SetKeyId(vecKeyId.data());
  cencInfo->SetKeyIdLen(keyIdLen);
  cencInfo->SetIv(vecIv.data());
  cencInfo->SetIvLen(ivLen);
  cencInfo->SetAlgo(uintTmp);
  cencInfo->SetEncryptedBlockCount(uintTmp);
  cencInfo->SetSkippedBlockCount(uintTmp);
  cencInfo->SetFirstEncryptedOffset(uintTmp);
  cencInfo->SetClearHeaderLens(clearHeaderLens);
  cencInfo->SetPayLoadLens(payLoadLens);
  cencInfo->SetMode(uintTmp);
  cencInfo->GetKeyId();
  cencInfo->GetKeyIdLen();
  cencInfo->GetIv();
  cencInfo->GetIvLen();
  cencInfo->GetAlgo();
  cencInfo->GetEncryptedBlockCount();
  cencInfo->GetSkippedBlockCount();
  cencInfo->GetFirstEncryptedOffset();
  cencInfo->GetClearHeaderLens();
  cencInfo->GetPayLoadLens();
  cencInfo->GetMode();
}

void AudioCodecDecoderImpl__DecoderCallback(FuzzedDataProvider* fdp)
{
  std::shared_ptr<AudioDecoderCallbackAdapterMock> cbMock = std::make_shared<AudioDecoderCallbackAdapterMock>();
  AudioDecoderCallbackAdapterImpl cb(cbMock);

  int32_t error = fdp->ConsumeIntegralInRange<int32_t>(0, 64);
  uint32_t index = fdp->ConsumeIntegralInRange<uint32_t>(0, 64);
  int32_t size = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
  std::vector<uint8_t> bufferData(size);
  fdp->ConsumeData(bufferData.data(), size);
  int64_t pts = fdp->ConsumeIntegral<int64_t>();
  int32_t offset = fdp->ConsumeIntegral<int32_t>();
  uint32_t flags = fdp->ConsumeIntegral<uint32_t>();
  cb.OnError(error);
  cb.OnOutputFormatChanged();
  cb.OnInputBufferAvailable(index);
  cb.OnOutputBufferAvailable(index, bufferData.data(), size, pts, offset, flags);

  AudioDecoderCallbackAdapterImpl cb1(nullptr);
  cb1.OnError(error);
  cb1.OnOutputFormatChanged();
  cb1.OnInputBufferAvailable(index);
  cb1.OnOutputBufferAvailable(index, bufferData.data(), size, pts, offset, flags);
}

void AudioCodecDecoderImpl__DecoderCallback2(FuzzedDataProvider* fdp)
{
  std::shared_ptr<AudioCodecDecoderAdapterImpl> decoder = std::make_shared<AudioCodecDecoderAdapterImpl>();
  if (decoder->CreateAudioDecoderByName(std::string(OH_AVCODEC_NAME_AUDIO_MPEG))
      != AudioDecoderAdapterCode::DECODER_OK) {
    return;
  }

  AudioDecoderCallbackManager::OnError(nullptr, 0, nullptr);
  AudioDecoderCallbackManager::OnError(decoder->GetAVCodec(), 0, nullptr);
  AudioDecoderCallbackManager::OnOutputFormatChanged(nullptr, 0, nullptr);
  AudioDecoderCallbackManager::OnOutputFormatChanged(decoder->GetAVCodec(), 0, nullptr);
  constexpr int32_t MEMSIZE = 1024 * 1024;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
  AudioDecoderCallbackManager::OnInputBufferAvailable(nullptr, 0, nullptr, nullptr);
  AudioDecoderCallbackManager::OnInputBufferAvailable(decoder->GetAVCodec(), 0, nullptr, nullptr);
  AudioDecoderCallbackManager::OnInputBufferAvailable(decoder->GetAVCodec(), 0, buffer, nullptr);
  AudioDecoderCallbackManager::OnInputBufferAvailable(
      decoder->GetAVCodec(), 0, buffer, nullptr);
  AudioDecoderCallbackManager::OnOutputBufferAvailable(nullptr, 0, nullptr, nullptr);
  AudioDecoderCallbackManager::OnOutputBufferAvailable(decoder->GetAVCodec(), 0, nullptr, nullptr);
  AudioDecoderCallbackManager::OnOutputBufferAvailable(decoder->GetAVCodec(), 0, buffer, nullptr);
  AudioDecoderCallbackManager::OnOutputBufferAvailable(
      decoder->GetAVCodec(), 0, buffer, nullptr);
}

void AudioCodecDecoderImpl__CreateAudioDecoder(FuzzedDataProvider* fdp)
{
  AudioCodecDecoderAdapterImpl decoder;
  std::string mimetype = fdp->ConsumeRandomLengthString(64);
  std::string name = fdp->ConsumeRandomLengthString(64);
  decoder.CreateAudioDecoderByMime(mimetype);
  decoder.CreateAudioDecoderByName(name);

  decoder.CreateAudioDecoderByMime(std::string(OH_AVCODEC_MIMETYPE_AUDIO_MPEG));
  decoder.CreateAudioDecoderByName(std::string(OH_AVCODEC_NAME_AUDIO_MPEG));

  decoder.CreateAudioDecoderByMime(std::string(OH_AVCODEC_MIMETYPE_AUDIO_MPEG));
  decoder.CreateAudioDecoderByName(std::string(OH_AVCODEC_NAME_AUDIO_MPEG));
}

void SetCencInfoAboutClearHeaderAndPayLoadLens(std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    const std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
    const std::vector<uint32_t> payLoadLens = {4, 5, 6};
    const uint32_t ERR_BLOCK_COUNT = 10000;
    cencInfo->SetClearHeaderLens(clearHeaderLens);
    cencInfo->SetPayLoadLens(payLoadLens);
    cencInfo->SetEncryptedBlockCount(ERR_BLOCK_COUNT);
    cencInfo->SetSkippedBlockCount(0);
    cencInfo->SetFirstEncryptedOffset(0);
}

void SetCencInfoAboutKeyIdIvAlgo(std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    cencInfo->SetAlgo(0);
    cencInfo->SetKeyId(keyId);
    cencInfo->SetKeyIdLen(DRM_KEY_ID_SIZE);
    cencInfo->SetIv(iv);
    cencInfo->SetIvLen(DRM_KEY_ID_SIZE);
}

void AudioCodecDecoderImpl__QueueInputBufferDec(FuzzedDataProvider* fdp)
{
  AudioCodecDecoderAdapterImpl decoder;
  std::string mimetype = std::string(OH_AVCODEC_MIMETYPE_AUDIO_MPEG);
  decoder.CreateAudioDecoderByMime(mimetype);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, nullptr, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);

  std::shared_ptr<AudioCencInfoAdapterImpl> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  const uint32_t ERR_ALGO = 10000;
  cencInfo->SetAlgo(ERR_ALGO);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  SetCencInfoAboutKeyIdIvAlgo(cencInfo);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  SetCencInfoAboutClearHeaderAndPayLoadLens(cencInfo);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  cencInfo->SetEncryptedBlockCount(0);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  const uint32_t ERR_MODE = 10000;
  cencInfo->SetMode(ERR_MODE);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  cencInfo->SetMode(0);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);

  constexpr int32_t MEMSIZE = 1024 * 1024;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
  decoder.SetInputBuffer(0, buffer);
  decoder.SetOutputBuffer(0, buffer);
  decoder.SetInputBuffer(0, nullptr);
  decoder.SetOutputBuffer(0, nullptr);
  decoder.QueueInputBufferDec(1, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_NONE);
  decoder.QueueInputBufferDec(0, 0, nullptr, 0, cencInfo, true,
      BufferFlag::CODEC_BUFFER_FLAG_EOS);
  decoder.ReleaseOutputBufferDec(0);
  decoder.ReleaseOutputBufferDec(100);
  OH_AVBuffer_Destroy(buffer);
  buffer = nullptr;
}

void TestQueueInputBufferDec(FuzzedDataProvider* fdp,
    std::shared_ptr<AudioCodecDecoderAdapterImpl> decoder, uint32_t index)
{
  int32_t size = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
  std::vector<uint8_t> bufferData(size);
  fdp->ConsumeData(bufferData.data(), size);
  int64_t pts = fdp->ConsumeIntegral<int64_t>();
  BufferFlag flag = static_cast<BufferFlag>(fdp->ConsumeIntegralInRange<uint32_t>(0, 8));
  bool isEncrypted = fdp->ConsumeBool();
  std::shared_ptr<AudioCencInfoAdapterImpl> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
  uint32_t uintTmp = fdp->ConsumeIntegral<int32_t>();
  uint32_t keyIdLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint8_t> vecKeyId(keyIdLen);
  fdp->ConsumeData(vecKeyId.data(), keyIdLen);
  uint32_t ivLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint8_t> vecIv(ivLen);
  fdp->ConsumeData(vecIv.data(), ivLen);
  uint32_t tmpLen = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
  std::vector<uint32_t> clearHeaderLens(tmpLen);
  std::vector<uint32_t> payLoadLens(tmpLen);
  fdp->ConsumeData(clearHeaderLens.data(), tmpLen);
  fdp->ConsumeData(payLoadLens.data(), tmpLen);
  cencInfo->SetKeyId(vecKeyId.data());
  cencInfo->SetKeyIdLen(keyIdLen);
  cencInfo->SetIv(vecIv.data());
  cencInfo->SetIvLen(ivLen);
  cencInfo->SetAlgo(uintTmp);
  cencInfo->SetEncryptedBlockCount(uintTmp);
  cencInfo->SetSkippedBlockCount(uintTmp);
  cencInfo->SetFirstEncryptedOffset(uintTmp);
  cencInfo->SetClearHeaderLens(clearHeaderLens);
  cencInfo->SetPayLoadLens(payLoadLens);
  cencInfo->SetMode(uintTmp);

  decoder->QueueInputBufferDec(index, pts, bufferData.data(), size, cencInfo, isEncrypted, flag);
}

void AudioCodecDecoderImpl__DecoderFunctions(FuzzedDataProvider* fdp)
{
  std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
  int32_t intTmp = fdp->ConsumeIntegralInRange<int32_t>(0, 64);
  int32_t configSize = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
  std::vector<uint8_t> config(configSize);
  fdp->ConsumeData(config.data(), configSize);
  int64_t biteRate = fdp->ConsumeIntegral<int64_t>();
  bool isAdts = fdp->ConsumeBool();
  format->SetSampleRate(intTmp);
  format->SetChannelCount(intTmp);
  format->SetBitRate(biteRate);
  format->SetMaxInputSize(intTmp);
  format->SetAACIsAdts(isAdts);
  format->SetAudioSampleFormat(intTmp);
  format->SetIdentificationHeader(intTmp);
  format->SetSetupHeader(intTmp);
  format->SetCodecConfig(config.data());
  format->SetCodecConfigSize(configSize);
  std::shared_ptr<AudioCodecDecoderAdapterImpl> decoder = std::make_shared<AudioCodecDecoderAdapterImpl>();
  std::shared_ptr<AudioDecoderCallbackAdapterMock> cbMock = std::make_shared<AudioDecoderCallbackAdapterMock>();
  uint32_t index = fdp->ConsumeIntegralInRange<uint32_t>(0, 64);
  decoder->CreateAudioDecoderByName(std::string(OH_AVCODEC_NAME_AUDIO_MPEG));
  decoder->GetAVCodec();
  decoder->GetAudioDecoderCallBack();
  decoder->GetDecoderMutex();
  decoder->ConfigureDecoder(format);
  decoder->SetParameterDecoder(format);
  decoder->FlushDecoder();
  decoder->StopDecoder();
  decoder->ResetDecoder();
  decoder->SetCallbackDec(cbMock);
  decoder->PrepareDecoder();
  decoder->StartDecoder();
  TestQueueInputBufferDec(fdp, decoder, index);
  decoder->ReleaseOutputBufferDec(index);
  decoder->GetInputBuffer(index);
  decoder->GetOutputBuffer(index);
  decoder->StopDecoder();
  decoder->ReleaseDecoder();
}

void AudioCodecDecoderImpl__DecoderFunctions02(FuzzedDataProvider* fdp)
{
  AudioCodecDecoderAdapterImpl decoder;
  // create decoder by normal mimetype.
  std::string mimetype = std::string(OH_AVCODEC_MIMETYPE_AUDIO_MPEG);
  AudioDecoderAdapterCode ret = decoder.CreateAudioDecoderByMime(mimetype);

  // config decoder.
  std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
  constexpr uint32_t DEFAULT_SAMPLERATE = 44100;
  constexpr uint32_t DEFAULT_BITRATE = 32000;
  constexpr uint32_t DEFAULT_CHANNEL_COUNT = 2;
  constexpr uint32_t DEFAULT_MAX_INPUT_SIZE = 1152;
  format->SetSampleRate(DEFAULT_SAMPLERATE);
  format->SetChannelCount(DEFAULT_CHANNEL_COUNT);
  format->SetBitRate(DEFAULT_BITRATE);
  format->SetMaxInputSize(DEFAULT_MAX_INPUT_SIZE);
  decoder.ConfigureDecoder(format);
  decoder.GetOutputFormatDec(format);

  // set callback for decoding.
  std::shared_ptr<AudioDecoderCallbackAdapter> callback = std::make_shared<AudioDecoderCallbackAdapterMock>();
  decoder.SetCallbackDec(callback);

  // prepare decoder.
  decoder.PrepareDecoder();
  decoder.StartDecoder();
  decoder.SetParameterDecoder(nullptr);
  decoder.SetParameterDecoder(format);
  decoder.FlushDecoder();
  decoder.StopDecoder();
  decoder.ResetDecoder();
  decoder.SetCallbackDec(callback);
  decoder.GetBufferFlag(OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS);
  decoder.ResetDecoder();
  decoder.FlushDecoder();
  decoder.StopDecoder();
  decoder.StartDecoder();
  decoder.GetDecoderMutex();
}

void AudioCodecDecoderImpl__DecoderFunctions03(FuzzedDataProvider* fdp)
{
  AudioCodecDecoderAdapterImpl decoder;
  std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
  std::shared_ptr<AudioDecoderCallbackAdapter> callback = std::make_shared<AudioDecoderCallbackAdapterMock>();
  decoder.ConfigureDecoder(format);
  decoder.SetParameterDecoder(format);
  decoder.PrepareDecoder();
  decoder.StartDecoder();
  decoder.SetParameterDecoder(nullptr);
  decoder.FlushDecoder();
  decoder.ResetDecoder();
  decoder.SetCallbackDec(nullptr);
  decoder.SetCallbackDec(callback);
  decoder.GetBufferFlag(OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS);
  decoder.GetBufferFlag(OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_DISPOSABLE);
  decoder.StopDecoder();
  decoder.GetDecoderMutex();
  decoder.ReleaseDecoder();
  decoder.GetOutputFormatDec(format);
  decoder.SetDecryptionConfig(nullptr, false);
  int32_t tmp = 100;
  void *session = reinterpret_cast<void*>(&tmp);
  decoder.SetDecryptionConfig(session, false);
    decoder.ReleaseOutputBufferDec(0);

  decoder.CreateAudioDecoderByName(std::string(OH_AVCODEC_NAME_AUDIO_MPEG));
  constexpr uint32_t DEFAULT_SAMPLERATE = 44100;
  constexpr uint32_t DEFAULT_BITRATE = 32000;
  constexpr uint32_t DEFAULT_CHANNEL_COUNT = 2;
  constexpr uint32_t DEFAULT_MAX_INPUT_SIZE = 1152;
  format->SetSampleRate(DEFAULT_SAMPLERATE);
  format->SetChannelCount(DEFAULT_CHANNEL_COUNT);
  format->SetBitRate(DEFAULT_BITRATE);
  format->SetMaxInputSize(DEFAULT_MAX_INPUT_SIZE);
  decoder.ConfigureDecoder(format);
  decoder.SetParameterDecoder(nullptr);
  decoder.SetParameterDecoder(format);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);
  OHOS::AudioCodecDecoderImpl__DecoderFormat(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderCencInfo(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderCallback(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderCallback2(&fdp);
  OHOS::AudioCodecDecoderImpl__CreateAudioDecoder(&fdp);
  OHOS::AudioCodecDecoderImpl__QueueInputBufferDec(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderFunctions(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderFunctions02(&fdp);
  OHOS::AudioCodecDecoderImpl__DecoderFunctions03(&fdp);

  return 0;
}
