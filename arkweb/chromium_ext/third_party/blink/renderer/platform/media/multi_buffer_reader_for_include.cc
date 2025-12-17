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

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
MultiBufferReader::MultiBufferReader(
    MultiBuffer* multibuffer,
    int64_t start,
    int64_t end,
    bool is_client_audio_element,
    uint64_t preload_size,
    uint64_t request_size,
    uint16_t byte_rate,
    std::string id,
    base::RepeatingCallback<void(int64_t, int64_t)> progress_callback,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : multibuffer_(multibuffer),
      // If end is -1, we use a very large (but still supported) value instead.
      end_(end == -1LL ? (1LL << (multibuffer->block_size_shift() + 30)) : end),
      preload_high_(0),
      preload_low_(0),
      max_buffer_forward_(0),
      max_buffer_backward_(0),
      current_buffer_size_(0),
      pinned_range_(0, 0),
      pos_(start),
      is_client_audio_element_(is_client_audio_element),
      preload_pos_(-1),
      preload_size_(block_ceil(preload_size)),
      request_size_(block_ceil(request_size)),
      byte_rate_(byte_rate),
      id_(id),
      loading_(true),
      current_wait_size_(0),
      progress_callback_(std::move(progress_callback)),
      task_runner_(std::move(task_runner)) {
  DCHECK_GE(start, 0);
  DCHECK_GE(end_, 0);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
}
