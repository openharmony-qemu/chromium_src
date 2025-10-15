// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_AUDIO_DECODER_LOOP_H
#define OHOS_AUDIO_DECODER_LOOP_H

#include <memory>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/base/decoder_status.h"
#include "media/base/encryption_scheme.h"
#include "media/base/media_export.h"
#include "media/base/subsample_entry.h"
#include "media/base/waiting.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace media {
using namespace media;

enum class BufferFlag : uint32_t {
  kCodecBufferFlagNone = 0,
  kCodecBufferFlagEos = 1 << 0,
  kCodecBufferFlagSyncFrame = 1 << 1,
  kCodecBufferFlagPartialFrame = 1 << 2,
  kCodecBufferFlagCodecData = 1 << 3,
};

enum class OhosAudioDecoderCode : int32_t {
  kDecoderOk = 0,
  kDecoderError = 1,
  kDecoderRetry = 2,
  kDecoderDecryptFailed = 3,
  kDecoderNoKey = 4
};

class OhosAudioCencInfo {
 public:
  OhosAudioCencInfo() = default;

  ~OhosAudioCencInfo();

  uint8_t* GetKeyId() { return key_id_; }

  uint32_t GetKeyIdLen() { return key_id_len_; }

  uint8_t* GetIv() { return iv_; }

  uint32_t GetIvLen() { return iv_len_; }

  uint32_t GetEncryptedBlockCount() { return encrypted_block_count_; }

  uint32_t GetAlgo() { return algo_; }

  uint32_t GetSkippedBlockCount() { return skipped_block_count_; }

  uint32_t GetFirstEncryptedOffset() { return first_encrypted_offset_; }

  std::vector<uint32_t> GetClearHeaderLens() { return clear_header_lens_; }

  std::vector<uint32_t> GetPayLoadLens() { return pay_load_lens_; }

  uint32_t GetMode() { return mode_; }

  void SetKeyId(uint8_t* key_id) { key_id_ = key_id; }

  void SetKeyIdLen(uint32_t key_id_len) { key_id_len_ = key_id_len; }

  void SetIv(uint8_t* iv) { iv_ = iv; }

  void SetIvLen(uint32_t iv_len) { iv_len_ = iv_len; }

  void SetAlgo(uint32_t algo) { algo_ = algo; }

  void SetEncryptedBlockCount(uint32_t encrypted_block_count) {
    encrypted_block_count_ = encrypted_block_count;
  }

  void SetSkippedBlockCount(uint32_t skipped_block_count) {
    skipped_block_count_ = skipped_block_count;
  }

  void SetFirstEncryptedOffset(uint32_t first_encrypted_offset) {
    first_encrypted_offset_ = first_encrypted_offset;
  }

  void SetClearHeaderLens(const std::vector<uint32_t>& clear_header_lens) {
    clear_header_lens_ = clear_header_lens;
  }

  void SetPayLoadLens(const std::vector<uint32_t>& pay_load_lens) {
    pay_load_lens_ = pay_load_lens;
  }

  void SetMode(uint32_t mode) { mode_ = mode; }

 private:
  uint8_t* key_id_;

  uint32_t key_id_len_;

  uint8_t* iv_;

  uint32_t iv_len_;

  // DRM encryption algorithm
  uint32_t algo_;

  // Number of encrypted blocks
  uint32_t encrypted_block_count_;

  // Number of unencrypted blocks
  uint32_t skipped_block_count_;

  uint32_t first_encrypted_offset_;

  // The length of the head clear flow data
  std::vector<uint32_t> clear_header_lens_;

  // The length of encrypted data
  std::vector<uint32_t> pay_load_lens_;

  // Include set or non-set [key_id/iv/subsample] mode
  uint32_t mode_;
};

class OhosAudioDecoderFormat {
 public:
  OhosAudioDecoderFormat() = default;

  int32_t GetSampleRate();

  int32_t GetChannelCount();

  int64_t GetBitRate();

  int32_t GetMaxInputSize();

  bool GetAACIsAdts();

  int32_t GetAudioSampleFormat();

  int32_t GetIdentificationHeader();

  int32_t GetSetupHeader();

  uint8_t* GetCodecConfig();

  uint32_t GetCodecConfigSize();

  void SetSampleRate(int32_t sample_rate);

  void SetChannelCount(int32_t channel_count);

  void SetBitRate(int64_t bit_rate);

  void SetMaxInputSize(int32_t max_input_size);

  void SetAACIsAdts(bool is_adts);

  void SetAudioSampleFormat(int32_t audio_sample_format);

  void SetIdentificationHeader(int32_t id_header);

  void SetSetupHeader(int32_t setup_header);

  void SetCodecConfig(uint8_t* codec_config);

  void SetCodecConfigSize(uint32_t size);

 private:
  int32_t sample_rate_;

  int32_t channel_count_;

  int64_t bit_rate_;

  int32_t max_input_size_;

  bool is_adts_;

  int32_t audio_sample_format_;

  int32_t id_header_;

  int32_t setup_header_;

  uint8_t* codec_config_;

  uint32_t codec_config_size_;
};

class OutputBufferData {
 public:
  uint32_t index_;

  std::vector<uint8_t> data_;

  uint32_t size_;

  int64_t pts_;

  BufferFlag flag_;

  OutputBufferData() {}

  OutputBufferData(uint32_t index,
                   uint8_t* data,
                   uint32_t size,
                   int64_t pts,
                   BufferFlag flag)
      : index_(index),
        data_(data, data + size),
        size_(size),
        pts_(pts),
        flag_(flag) {}

  ~OutputBufferData() {}

  uint32_t GetIndex() const { return index_; }

  std::vector<uint8_t> GetData() const { return data_; }

  uint32_t GetSize() const { return size_; }

  int64_t GetPts() const { return pts_; }

  BufferFlag GetBufferFlag() const { return flag_; }
};

class OhosAudioDecoderLoop {
 public:
  // Data that the client wants to put into an input buffer.
  struct InputData {
    InputData();

    InputData(const InputData&);

    InputData& operator=(const InputData& other);

    ~InputData();

    uint8_t* memory = nullptr;

    size_t length = 0;

    base::TimeDelta presentation_time;

    bool is_eos = false;

    std::shared_ptr<OhosAudioCencInfo> cenc_info = nullptr;

    bool is_encrypted = false;

    bool is_valid = true;
  };

  enum { K_INVALID_BUFFER_INDEX = -1 };

  class Client {
   public:
    virtual bool IsAnyInputPending() const = 0;

    virtual InputData ProvideInputData() = 0;

    virtual void OnInputDataQueued(bool success) = 0;

    virtual bool OnDecodedEos(const OutputBufferData& out) = 0;

    virtual bool OnDecodedFrame(const OutputBufferData& out) = 0;

    virtual void OnCodecLoopError() = 0;

    virtual int32_t DequeueInputBuffer(int64_t& buffer_index) = 0;

    virtual void EnqueueInputBuffer(int64_t buffer_index) = 0;

    virtual int32_t DequeueOutputBuffer(OutputBufferData& out) = 0;

    virtual OhosAudioDecoderCode FlushDecoder() = 0;

    virtual OhosAudioDecoderCode QueueInputBufferDec(
        uint32_t index,
        int64_t presentation_time_us,
        uint8_t* buffer_data,
        int32_t buffer_size,
        std::shared_ptr<OhosAudioCencInfo> cenc_info,
        bool is_encrypted,
        BufferFlag flag) = 0;

    virtual OhosAudioDecoderCode ReleaseOutputBufferDec(uint32_t index) = 0;

   protected:
    virtual ~Client() {}
  };

  OhosAudioDecoderLoop(
      Client* client,
      scoped_refptr<base::SingleThreadTaskRunner> timer_task_runner,
      bool disable_timer = false);

  OhosAudioDecoderLoop(const OhosAudioDecoderLoop&) = delete;

  OhosAudioDecoderLoop& operator=(const OhosAudioDecoderLoop&) = delete;

  ~OhosAudioDecoderLoop();

  // start the loop
  void ExpectWork();

  bool TryFlush();

  void OnKeyAdded();

 protected:
  enum State {
    kReady,

    kWaitingForKey,

    kDraining,

    kDrained,

    kError,
  };

  struct InputBuffer {
    InputBuffer() {}

    explicit InputBuffer(int64_t buffer_index) : index(buffer_index) {}

    int64_t index = K_INVALID_BUFFER_INDEX;
  };

  void DoPendingWork();

  bool ProcessOneInputBuffer();

  bool EnqueueInputBuffer(const InputBuffer& input_buffer);

  InputBuffer DequeueInputBuffer();

  bool ProcessOneOutputBuffer();

  void ManageTimer(bool start);

  void SetState(State new_state);

  State state_;

  raw_ptr<Client> client_;

  base::RepeatingTimer io_timer_;

  base::TimeTicks idle_time_begin_;

  const bool disable_timer_;
};

}  // namespace media

#endif  // OHOS_AUDIO_DECODER_LOOP_H
