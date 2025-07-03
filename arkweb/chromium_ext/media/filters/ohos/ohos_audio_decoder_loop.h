// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_AUDIO_DECODER_LOOP_H
#define OHOS_AUDIO_DECODER_LOOP_H

#include <memory>
#include <utility>
#include <vector>
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "base/logging.h"
#include "base/timer/timer.h"
#include "media/base/decoder_status.h"
#include "media/base/encryption_scheme.h"
#include "media/base/media_export.h"
#include "media/base/subsample_entry.h"
#include "media/base/waiting.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "third_party/bounds_checking_function/include/securec.h"

#include "third_party/ohos_ndk/includes/ohos_adapter/audio_codec_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_cenc_info_adapter.h"

namespace media {
using namespace OHOS::NWeb;

class OutputBufferData {
public:
    uint32_t index_;

    std::vector<uint8_t> data_;

    uint32_t size_;

    int64_t pts_;

    BufferFlag flag_;

    OutputBufferData() {}

    OutputBufferData(uint32_t index, uint8_t* data, uint32_t size, int64_t pts, BufferFlag flag)
        : index_(index), data_(size), size_(size), pts_(pts), flag_(flag) {
      if (memcpy_s(data_.data(), size, data, size) != EOK) {
        LOG(ERROR) << "OHOSAudioDecoderLoop OutputBufferData copy data error index: " << index;
      }
    }

    ~OutputBufferData() {}

    uint32_t GetIndex() const {
      return index_;
    }

    std::vector<uint8_t> GetData() const {
      return data_;
    }

    uint32_t GetSize() const {
      return size_;
    }

    int64_t GetPts() const {
      return pts_;
    }

    BufferFlag GetBufferFlag() const {
      return flag_;
    }
};

class OHOSAudioDecoderLoop {
 public:
  // Data that the client wants to put into an input buffer.
  struct InputData {
    InputData();

    InputData(const InputData& other);

    InputData& operator=(const InputData& other) = delete;

    ~InputData();

    uint8_t* memory = nullptr;

    size_t length = 0;

    base::TimeDelta presentation_time;

    bool is_eos = false;

    std::shared_ptr<AudioCencInfoAdapter> cenc_info = nullptr;

    bool is_encrypted = false;

    bool is_valid = true;
  };

  enum BufferIndex : uint32_t { kInvalidBufferIndex = 0xFFFFFFFF };

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

    virtual AudioDecoderAdapterCode FlushDecoder() = 0;

    virtual AudioDecoderAdapterCode QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t* bufferData,
        int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, bool isEncrypted, BufferFlag flag) = 0;

    virtual AudioDecoderAdapterCode ReleaseOutputBufferDec(uint32_t index) = 0;

   protected:
    virtual ~Client() {}
  };

  OHOSAudioDecoderLoop(Client* client,
                 scoped_refptr<base::SingleThreadTaskRunner> timer_task_runner,
                 bool disable_timer = false);

  OHOSAudioDecoderLoop(const OHOSAudioDecoderLoop&) = delete;

  OHOSAudioDecoderLoop& operator=(const OHOSAudioDecoderLoop&) = delete;

  ~OHOSAudioDecoderLoop();

  // start the loop
  void ExpectWork();

  bool TryFlush();

  void OnKeyAdded();

 protected:
  enum State {
    READY,

    WAITING_FOR_KEY,

    DRAINING,

    DRAINED,

    ERROR,
  };

  struct InputBuffer {
    InputBuffer() {}

    explicit InputBuffer(int64_t buffer_index) : index(buffer_index) {}

    int64_t index = kInvalidBufferIndex;
  };

  void DoPendingWork();

  bool ProcessOneInputBuffer();

  bool EnqueueInputBuffer(const InputBuffer& input_buffer);

  InputBuffer DequeueInputBuffer();

  bool ProcessOneOutputBuffer();

  void ManageTimer(bool did_work);

  void SetState(State new_state);

  State state_;

  raw_ptr<Client> client_;

  std::shared_ptr<AudioCodecDecoderAdapter> audio_decoder_;

  base::RepeatingTimer io_timer_;

  base::TimeTicks idle_time_begin_;

  const bool disable_timer_;
};

} // namespace media

#endif //OHOS_AUDIO_DECODER_LOOP_H