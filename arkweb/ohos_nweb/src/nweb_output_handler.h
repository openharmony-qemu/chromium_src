/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_OUTPUT_HANDLER_H
#define NWEB_OUTPUT_HANDLER_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

#include "base/memory/raw_ptr.h"

namespace OHOS::NWeb {
class NWebOutputHandler
    : public std::enable_shared_from_this<NWebOutputHandler> {
 public:
  static std::shared_ptr<NWebOutputHandler> Create(
      uint32_t width,
      uint32_t height,
      std::function<bool(const char*, uint32_t, uint32_t)> output_frame_cb);

  explicit NWebOutputHandler(
      std::function<bool(const char*, uint32_t, uint32_t)> output_frame_cb);
  ~NWebOutputHandler();

  void Init(uint32_t width, uint32_t height);
  void GetWindowInfo(uint32_t& width, uint32_t& height);
  void Resize(uint32_t width, uint32_t height);
  void SetFrameInfoDump(bool flag);
  void StartFrameStat();
  void SetDumpPath(const std::string& path);
  void StartRenderOutput();
  void SetNWebId(uint32_t id);
  bool IsSizeValid();

  void* GetNativeWindowFromSurface(void* surface);

 private:
  class BmpDumpHelper {
   public:
    void DumpToBmp(const std::string& filename,
                   char* buf,
                   int width,
                   int height);

   private:
    void RgbaToRgb(char* buf, int width, int height);
    void WriteToBmp(const std::string& filename,
                    char* buf,
                    int width,
                    int height);
  };

  void StartDumpToFile();

 private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  uint32_t frame_size_ = 0;

  raw_ptr<void> window_ = nullptr;

  std::function<bool(const char*, uint32_t, uint32_t)> output_frame_cb_;

  std::string dump_path_;
  std::mutex dump_mtx_;
  std::condition_variable dump_cv_;
  std::unique_ptr<char[]> dump_buf_;

  /* stat info */
  bool frame_info_dump_ = false;
  std::mutex frame_stat_mtx_;
  int64_t render_count_ = 0L;
  int64_t local_render_count_ = 0L;
  int64_t local_time_base_ = 0L;
  int64_t local_time_curr_ = 0L;
  int64_t frame_miss_count_ = 0L;

  uint32_t nweb_id_ = 0;
  bool is_initialized_resize_ = false;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_OUTPUT_HANDLER_H
