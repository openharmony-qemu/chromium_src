// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_audio_decoder_loop.h"

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/timestamp_constants.h"

namespace media {
namespace {
constexpr base::TimeDelta kDecodePollDelay = base::Milliseconds(10);
constexpr base::TimeDelta kIdleTimerTimeout = base::Seconds(1);
}  // namespace

OhosAudioCencInfo::~OhosAudioCencInfo() {}

int32_t OhosAudioDecoderFormat::GetSampleRate() {
  return sample_rate_;
}

int32_t OhosAudioDecoderFormat::GetChannelCount() {
  return channel_count_;
}

int64_t OhosAudioDecoderFormat::GetBitRate() {
  return bit_rate_;
}

int32_t OhosAudioDecoderFormat::GetMaxInputSize() {
  return max_input_size_;
}
bool OhosAudioDecoderFormat::GetAACIsAdts() {
  return is_adts_;
}

int32_t OhosAudioDecoderFormat::GetAudioSampleFormat() {
  return audio_sample_format_;
}

int32_t OhosAudioDecoderFormat::GetIdentificationHeader() {
  return id_header_;
}

int32_t OhosAudioDecoderFormat::GetSetupHeader() {
  return setup_header_;
}

uint8_t* OhosAudioDecoderFormat::GetCodecConfig() {
  return codec_config_;
}

uint32_t OhosAudioDecoderFormat::GetCodecConfigSize() {
  return codec_config_size_;
}

void OhosAudioDecoderFormat::SetSampleRate(int32_t sample_rate) {
  sample_rate_ = sample_rate;
}

void OhosAudioDecoderFormat::SetChannelCount(int32_t channel_count) {
  channel_count_ = channel_count;
}

void OhosAudioDecoderFormat::SetBitRate(int64_t bit_rate) {
  bit_rate_ = bit_rate;
}

void OhosAudioDecoderFormat::SetMaxInputSize(int32_t max_input_size) {
  max_input_size_ = max_input_size;
}

void OhosAudioDecoderFormat::SetAACIsAdts(bool is_adts) {
  is_adts_ = is_adts;
}

void OhosAudioDecoderFormat::SetAudioSampleFormat(int32_t audio_sample_format) {
  audio_sample_format_ = audio_sample_format;
}

void OhosAudioDecoderFormat::SetIdentificationHeader(int32_t id_header) {
  id_header_ = id_header;
}

void OhosAudioDecoderFormat::SetSetupHeader(int32_t setup_header) {
  setup_header_ = setup_header;
}

void OhosAudioDecoderFormat::SetCodecConfig(uint8_t* codec_config) {
  codec_config_ = codec_config;
}

void OhosAudioDecoderFormat::SetCodecConfigSize(uint32_t size) {
  codec_config_size_ = size;
}

OhosAudioDecoderLoop::InputData::InputData() {}

OhosAudioDecoderLoop::InputData::InputData(const InputData& other)
    : memory(other.memory),
      length(other.length),
      presentation_time(other.presentation_time),
      is_eos(other.is_eos),
      cenc_info(other.cenc_info),
      is_encrypted(other.is_encrypted) {}

OhosAudioDecoderLoop::InputData& OhosAudioDecoderLoop::InputData::operator=(
    const InputData& other) = default;

OhosAudioDecoderLoop::InputData::~InputData() {}

OhosAudioDecoderLoop::OhosAudioDecoderLoop(
    Client* client,
    scoped_refptr<base::SingleThreadTaskRunner> timer_task_runner,
    bool disable_timer)
    : state_(kReady), client_(client), disable_timer_(disable_timer) {
  if (timer_task_runner) {
    io_timer_.SetTaskRunner(timer_task_runner);
  }

  if (client_ == nullptr) {
    SetState(kError);
  }
}

OhosAudioDecoderLoop::~OhosAudioDecoderLoop() {
  io_timer_.Stop();
}

void OhosAudioDecoderLoop::OnKeyAdded() {
  if (state_ == kWaitingForKey) {
    SetState(kReady);
  }
  ExpectWork();
}

bool OhosAudioDecoderLoop::TryFlush() {
  // Here the input queue is obviously empty and is handled by the caller.
  // Flush is not allowed in kError state, but is allowed in other states.
  if (state_ == kError) {
    LOG(WARNING) << " [AudioDecoder] OhosAudioDecoderLoop::TryFlush state: "
                 << static_cast<int32_t>(state_);
    return false;
  }

  io_timer_.Stop();
  if (client_->FlushDecoder() != OhosAudioDecoderCode::kDecoderOk) {
    SetState(kError);
    return false;
  }
  SetState(kReady);
  return true;
}

void OhosAudioDecoderLoop::ExpectWork() {
  // Start the timer
  ManageTimer(true);
  DoPendingWork();
}

void OhosAudioDecoderLoop::DoPendingWork() {
  if (state_ == kError) {
    return;
  }

  bool did_work = false;
  bool did_input = false;
  bool did_output = false;
  do {
    // Get a buffer data from the kernel and send it to oh audio decoder
    did_input = ProcessOneInputBuffer();

    // Get a decoded buffer data from oh audio decoder
    did_output = ProcessOneOutputBuffer();
    if (did_input || did_output) {
      did_work = true;
    }
  } while (did_input || did_output);

  ManageTimer(did_work);
}

void OhosAudioDecoderLoop::ManageTimer(bool did_work) {
  if (disable_timer_) {
    return;
  }
  bool should_be_running = true;

  base::TimeTicks now = base::TimeTicks::Now();
  // idle_time_begin_ represents the last time we triggered the timer
  if (did_work || idle_time_begin_ == base::TimeTicks()) {
    idle_time_begin_ = now;
  } else {
    // Checks if enough time (more than 1s) has passed since the last call to
    // DoPending. If so, sets should_be_running to false.
    if (now - idle_time_begin_ > kIdleTimerTimeout) {
      should_be_running = false;
    }
  }

  if (should_be_running && !io_timer_.IsRunning()) {
    // Start the timer and call the MediaCodecLoop::DoPendingWork method after a
    // delay of 10ms.
    io_timer_.Start(FROM_HERE, kDecodePollDelay, this,
                    &OhosAudioDecoderLoop::DoPendingWork);
  } else if (!should_be_running && io_timer_.IsRunning()) {
    // If should_be_running is false and the timer is still running, close the
    // timer
    io_timer_.Stop();
  }
}

void OhosAudioDecoderLoop::SetState(State new_state) {
  const State old_state = state_;
  state_ = new_state;
  if (old_state != new_state && new_state == kError) {
    client_->OnCodecLoopError();
  }
}

bool OhosAudioDecoderLoop::ProcessOneInputBuffer() {
  if (state_ != kReady) {
    return false;
  }

  if (!client_->IsAnyInputPending()) {
    return false;
  }

  // Call the DequeueInputBuffer method to dequeue an input buffer from the
  // codec. This method may set the state to kError.
  InputBuffer input_buffer = DequeueInputBuffer();
  if (input_buffer.index == K_INVALID_BUFFER_INDEX) {
    return false;
  }

  TRACE_EVENT1("media", "OhosAudioDecoderLoop::ProcessOneInputBuffer",
               "buffer index", input_buffer.index);

  bool ret = EnqueueInputBuffer(input_buffer);
  if (!ret) {
    LOG(WARNING)
        << " [AudioDecoder] OhosAudioDecoderLoop::ProcessOneInputBuffer write "
           "fail, add buffer index to queue";
    client_->EnqueueInputBuffer(input_buffer.index);
  }

  return state_ != kError;
}

OhosAudioDecoderLoop::InputBuffer OhosAudioDecoderLoop::DequeueInputBuffer() {
  int64_t input_buf_index = K_INVALID_BUFFER_INDEX;
  int32_t ret = client_->DequeueInputBuffer(input_buf_index);
  if (ret < 0) {
    LOG(INFO) << " [AudioDecoder] OhosAudioDecoderLoop::DequeueInputBuffer "
                 "cannot get empty input buffer index";
  }
  return InputBuffer(input_buf_index);
}

bool OhosAudioDecoderLoop::EnqueueInputBuffer(const InputBuffer& input_buffer) {
  InputData input_data = client_->ProvideInputData();
  if (!input_data.is_valid) {
    // decoder buffer is null, skip write buffer to oh audio decoder
    LOG(INFO) << "[AudioDecoder] OhosAudioDecoderLoop::EnqueueInputBuffer "
                 "input data is invalid";
    return false;
  }

  // EOS frame data processing
  BufferFlag flag = BufferFlag::kCodecBufferFlagNone;
  if (input_data.is_eos) {
    flag = BufferFlag::kCodecBufferFlagEos;
    SetState(kDraining);
  }

  OhosAudioDecoderCode code = client_->QueueInputBufferDec(
      static_cast<uint32_t>(input_buffer.index),
      input_data.presentation_time.InMilliseconds(), input_data.memory,
      input_data.length, input_data.cenc_info, input_data.is_encrypted, flag);

  switch (code) {
    case OhosAudioDecoderCode::kDecoderNoKey:
      SetState(kWaitingForKey);
      break;
    case OhosAudioDecoderCode::kDecoderRetry:
      break;
    case OhosAudioDecoderCode::kDecoderOk:
      client_->OnInputDataQueued(true);
      break;
    case OhosAudioDecoderCode::kDecoderError:
    default:
      LOG(ERROR) << "[AudioDecoder] kDecoderError from QueueInputBuffer";
      client_->OnInputDataQueued(false);
      SetState(kError);
      break;
  }
  return true;
}

bool OhosAudioDecoderLoop::ProcessOneOutputBuffer() {
  TRACE_EVENT0("media", "OhosAudioDecoderLoop::ProcessOneOutputBuffer");
  if (state_ == kError) {
    return false;
  }

  OutputBufferData out;
  int32_t code = client_->DequeueOutputBuffer(out);
  if (code == -1) {
    return false;
  }
  if (out.GetBufferFlag() == BufferFlag::kCodecBufferFlagEos) {
    // Receiving an EOS frame indicates that the media stream has been
    // completely decoded and there is no more data to be processed. At this
    // time, the decoder state should be set to kDrained.
    LOG(WARNING)
        << " [AudioDecoder] OhosAudioDecoderLoop::ProcessOneOutputBuffer set "
           "state kDrained";
    SetState(kDrained);

    client_->ReleaseOutputBufferDec(out.GetIndex());

    if (!client_->OnDecodedEos(out)) {
      SetState(kError);
    }
  } else {
    // Notify the audio decoder to process a decrypted buffer of data
    if (!client_->OnDecodedFrame(out)) {
      SetState(kError);
    }
  }
  return true;
}

}  // namespace media
