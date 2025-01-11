// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/timestamp_constants.h"

#include "ohos_audio_decoder_loop.h"

namespace media {
  namespace {
    constexpr base::TimeDelta kDecodePollDelay = base::Milliseconds(10);
    constexpr base::TimeDelta kIdleTimerTimeout = base::Seconds(1);
  }  // namespace

OHOSAudioDecoderLoop::InputData::InputData() {}

OHOSAudioDecoderLoop::InputData::InputData(const InputData& other) : memory(other.memory),
    length(other.length),
    presentation_time(other.presentation_time),
    is_eos(other.is_eos),
    cenc_info(other.cenc_info),
    is_encrypted(other.is_encrypted) {}

OHOSAudioDecoderLoop::InputData::~InputData() {}

OHOSAudioDecoderLoop::OHOSAudioDecoderLoop(
  Client* client,
  scoped_refptr<base::SingleThreadTaskRunner> timer_task_runner,
  bool disable_timer) : state_(READY),
    client_(client),
    disable_timer_(disable_timer) {
  LOG(INFO) << "OHOSAudioDecoderLoop::OHOSAudioDecoderLoop initialize";
  if (timer_task_runner) {
    io_timer_.SetTaskRunner(timer_task_runner);
  }

  if (client_ == nullptr) {
    SetState(ERROR);
  }
}

OHOSAudioDecoderLoop::~OHOSAudioDecoderLoop() {
  LOG(INFO) << "OHOSAudioDecoderLoop::~OHOSAudioDecoderLoop";
  io_timer_.Stop();
}

void OHOSAudioDecoderLoop::OnKeyAdded() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::OnKeyAdded";
  ExpectWork();
}

bool OHOSAudioDecoderLoop::TryFlush() {
  LOG(INFO) << "OHOSAudioDecoderLoop::TryFlush";
  // Here the input queue is obviously empty and is handled by the caller.
  // Flush is not allowed in ERROR state, but is allowed in other states.
  if (state_ == ERROR) {
    LOG(INFO) << "OHOSAudioDecoderLoop::TryFlush state: " << static_cast<int32_t>(state_);
    return false;
  }

  io_timer_.Stop();
  if (client_->FlushDecoder() != AudioDecoderAdapterCode::DECODER_OK) {
    SetState(ERROR);
    return false;
  }
  SetState(READY);
  return true;
}

void OHOSAudioDecoderLoop::ExpectWork() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::ExpectWork";
  // Start the timer
  ManageTimer(true);
  DoPendingWork();
}

void OHOSAudioDecoderLoop::DoPendingWork() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::DoPendingWork";
  if (state_ == ERROR) {
    return;
  }

  bool did_work = false;
  bool did_input = false;
  bool did_output = false;
  do {
    // Get a buffer data from the kernel and send it to oh audio decoder
    did_input = ProcessOneInputBuffer();
    LOG(DEBUG) << "OHOSAudioDecoderLoop::DoPendingWork did input: " << did_input;

    // Get a decoded buffer data from oh audio decoder
    did_output = ProcessOneOutputBuffer();
    LOG(DEBUG) << "OHOSAudioDecoderLoop::DoPendingWork did output: " << did_output;
    if (did_input || did_output) {
      did_work = true;
    }
  } while (did_input || did_output);

  ManageTimer(did_work);
}

void OHOSAudioDecoderLoop::ManageTimer(bool did_work) {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::ManageTimer";
  if (disable_timer_) {
    return;
  }
  bool should_be_running = true;

  base::TimeTicks now = base::TimeTicks::Now();
  // idle_time_begin_ represents the last time we triggered the timer
  if (did_work || idle_time_begin_ == base::TimeTicks()) {
    idle_time_begin_ = now;
  } else {
    // Checks if enough time (more than 1s) has passed since the last call to DoPending.
    // If so, sets should_be_running to false.
    if (now - idle_time_begin_ > kIdleTimerTimeout) {
      should_be_running = false;
    }
  }

  if (should_be_running && !io_timer_.IsRunning()) {
    // Start the timer and call the MediaCodecLoop::DoPendingWork method after a delay of 10ms.
    io_timer_.Start(FROM_HERE, kDecodePollDelay, this,
            &OHOSAudioDecoderLoop::DoPendingWork);
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ManageTimer start timer";
  } else if (!should_be_running && io_timer_.IsRunning()) {
    // If should_be_running is false and the timer is still running, close the timer
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ManageTimer stop timer";
    io_timer_.Stop();
  }
}

void OHOSAudioDecoderLoop::SetState(State new_state) {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::SetState state_: " << static_cast<int32_t>(state_)
    << " new_state: " << static_cast<int32_t>(new_state);
  const State old_state = state_;
  state_ = new_state;
  if (old_state != new_state && new_state == ERROR)
    client_->OnCodecLoopError();
}

bool OHOSAudioDecoderLoop::ProcessOneInputBuffer() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneInputBuffer";
  if (state_ != READY) {
    return false;
  }

  if (!client_->IsAnyInputPending()) {
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneInputBuffer no input_queue DecoderBuffer";
    return false;
  }

  // Call the DequeueInputBuffer method to dequeue an input buffer from the codec.
  // This method may set the state to ERROR.
  InputBuffer input_buffer = DequeueInputBuffer();
  if (input_buffer.index == kInvalidBufferIndex) {
    return false;
  }

  LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneInputBuffer input_buffer.index: " << input_buffer.index;
  TRACE_EVENT1("media", "OHOSAudioDecoderLoop::ProcessOneInputBuffer", "buffer index", input_buffer.index);

  bool ret = EnqueueInputBuffer(input_buffer);
  if (!ret) {
    LOG(WARNING) << "OHOSAudioDecoderLoop::ProcessOneInputBuffer write fail, add buffer index to queue";
    client_->EnqueueInputBuffer(input_buffer.index);
  }

  return state_ != ERROR;
}

OHOSAudioDecoderLoop::InputBuffer OHOSAudioDecoderLoop::DequeueInputBuffer() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::DequeueInputBuffer";
  int64_t input_buf_index = kInvalidBufferIndex;
  int32_t ret = client_->DequeueInputBuffer(input_buf_index);
  LOG(DEBUG) << "OHOSAudioDecoderLoop::DequeueInputBuffer input_buf_index: "
    << input_buf_index << "return index: " << ret;
  if (ret < 0) {
    LOG(DEBUG) << "OHOSAudioDecoderLoop::DequeueInputBuffer cannot get empty input buffer index";
  }
  return InputBuffer(input_buf_index);
}

bool OHOSAudioDecoderLoop::EnqueueInputBuffer(const InputBuffer& input_buffer) {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::EnqueueInputBuffer";

  InputData input_data = client_->ProvideInputData();
  if (!input_data.is_valid) {
    // decoder buffer is null, skip write buffer to oh audio decoder
    LOG(DEBUG) << "OHOSAudioDecoderLoop::EnqueueInputBuffer input data is invalid";
    return false;
  }

  // EOS frame data processing
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  if (input_data.is_eos) {
    flag = BufferFlag::CODEC_BUFFER_FLAG_EOS;
    SetState(DRAINING);
  }

  LOG(DEBUG) << "OHOSAudioDecoderLoop::EnqueueInputBuffer start QueueInputBufferDec input buffer index: "
    << input_buffer.index;
  AudioDecoderAdapterCode code = client_->QueueInputBufferDec(
        static_cast<uint32_t>(input_buffer.index), input_data.presentation_time.InMilliseconds(),
        input_data.memory, input_data.length, input_data.cenc_info, input_data.is_encrypted, flag);

  switch (code) {
    case AudioDecoderAdapterCode::DECODER_RETRY:
      LOG(DEBUG) << "OHOSAudioDecoderLoop::EnqueueInputBuffer AudioDecoderAdapterCode::DECODER_RETRY";
      // Do not process RETRY, return directly and wait for the next cycle to write
      break;

    case AudioDecoderAdapterCode::DECODER_OK:
      LOG(DEBUG) << "OHOSAudioDecoderLoop::EnqueueInputBuffer AudioDecoderAdapterCode::DECODER_OK";
      client_->OnInputDataQueued(true);
      break;

    case AudioDecoderAdapterCode::DECODER_ERROR:
    default:
      LOG(ERROR) << "OHOSAudioDecoderLoop::EnqueueInputBuffer DECODER_ERROR from QueueInputBuffer";
      client_->OnInputDataQueued(false);
      SetState(ERROR);
      break;
  }
  return true;
}

bool OHOSAudioDecoderLoop::ProcessOneOutputBuffer() {
  LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer";
  TRACE_EVENT0("media", "OHOSAudioDecoderLoop::ProcessOneOutputBuffer");
  if (state_ == ERROR) {
    return false;
  }

  OutputBufferData out;
  int32_t code = client_->DequeueOutputBuffer(out);
  if (code == -1) {
    return false;
  }
  LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer start process output buffer";
  if (out.GetBufferFlag() == BufferFlag::CODEC_BUFFER_FLAG_EOS) {
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer get eos output buffer";
    // Receiving an EOS frame indicates that the media stream has been completely decoded
    // and there is no more data to be processed. At this time, the decoder state should be set to DRAINED.
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer set state DRAINED";
    SetState(DRAINED);

    LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer ReleaseOutputBufferDec index " << out.GetIndex();
    client_->ReleaseOutputBufferDec(out.GetIndex());

    if (!client_->OnDecodedEos(out)) {
      SetState(ERROR);
    }
  } else {
    LOG(DEBUG) << "OHOSAudioDecoderLoop::ProcessOneOutputBuffer get normal output buffer";
    // Notify the audio decoder to process a decrypted buffer of data
    if (!client_->OnDecodedFrame(out)) {
      SetState(ERROR);
    }
  }
  return true;
}

} // namespace media
