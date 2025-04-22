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

#include "nweb_output_handler.h"

#include <sys/time.h>
#include <unistd.h>
#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <thread>

#include "base/trace_event/trace_event.h"
#include "nweb_hilog.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
namespace {
#pragma pack(1)
struct BmpFileHeader {
  unsigned int bf_size;             /* Size of file */
  unsigned short bf_reserved_1 = 0; /* Reserved */
  unsigned short bf_reserved_2 = 0; /* ... */
  unsigned int bf_offbits = 0x36;   /* Offset to bitmap data */
};

/**** BMP file info structure ****/
struct BmpInfoHeader {
  unsigned int bi_size;                 /* Size of info header */
  int bi_width;                         /* Width of image */
  int bi_height;                        /* Height of image */
  unsigned short bi_planes = 1;         /* Number of color planes */
  unsigned short bi_bit_count = 24;     /* Number of bits per pixel */
  unsigned int bi_compression = 0;      /* Type of compression to use */
  unsigned int bi_size_image = 0;       /* Size of image data */
  int bi_x_pixels_per_meter = 5000;     /* X pixels per meter */
  int bi_y_pixels_per_meter = 5000;     /* Y pixels per meter */
  unsigned int bi_colors_used = 0;      /* Number of colors used */
  unsigned int bi_colors_important = 0; /* Number of important colors */
};

constexpr uint8_t kBitsPerPixel = 4;
}  // namespace

// static
std::shared_ptr<NWebOutputHandler> NWebOutputHandler::Create(
    uint32_t width,
    uint32_t height,
    std::function<bool(const char*, uint32_t, uint32_t)> output_frame_cb) {
  if (output_frame_cb == nullptr) {
    WVLOG_E("NWeb user must provide valid outputFrame callback!");
    return nullptr;
  }

  auto output_handler = std::make_shared<NWebOutputHandler>(output_frame_cb);
  if (output_handler == nullptr) {
    WVLOG_E("fail to create NWebOutputHandler instance");
    return nullptr;
  }

  output_handler->Init(width, height);

  return output_handler;
}

NWebOutputHandler::NWebOutputHandler(
    std::function<bool(const char*, uint32_t, uint32_t)> output_frame_cb)
    : output_frame_cb_(output_frame_cb) {
  dump_buf_.reset(nullptr);
}

NWebOutputHandler::~NWebOutputHandler() {}

void NWebOutputHandler::Init(uint32_t width, uint32_t height) {
  Resize(width, height);
}

void NWebOutputHandler::GetWindowInfo(uint32_t& width, uint32_t& height) {
  width = width_;
  height = height_;
}

void NWebOutputHandler::Resize(uint32_t width, uint32_t height) {
  if (width_ != width || height_ != height) {
    WVLOG_I(
        "NWeb size change from %{public}u*%{public}u to "
        "%{public}u*%{public}u, nweb id = %{public}u",
        width_, height_, width, height, nweb_id_);
    std::string trace_output_str = "origin size = " + std::to_string(width_) + "*" + std::to_string(height_) +
      ", new size = " + std::to_string(width) + "*" + std::to_string(height) +
      ", nweb id = " + std::to_string(nweb_id_);
    TRACE_EVENT1("base", "NWebOutputHandler::Resize", "change info", trace_output_str.c_str());
    width_ = width;
    height_ = height;
    frame_size_ = width_ * height_ * kBitsPerPixel;
    if (!dump_path_.empty() || dump_buf_ == nullptr) {
      dump_buf_.reset(new char[frame_size_]);
    }

    if (!is_initialized_resize_) {
      int32_t ret =
          OHOS::NWeb::OhosAdapterHelper::GetInstance()
              .GetWindowAdapterInstance()
              .NativeWindowSetBufferGeometry(reinterpret_cast<void*>(window_.get()),
                                    width, height);
      if (ret == OHOS::NWeb::GSErrorCode::GSERROR_OK) {
        is_initialized_resize_ = true;
        WVLOG_I("need to resize for emulator early firstly, result = %{public}d", ret);
      } else {
        WVLOG_W("resize for emulator early failed, result = %{public}d", ret);
      }
    }
  }
}

void NWebOutputHandler::StartDumpToFile() {
  std::weak_ptr<NWebOutputHandler> output_handler_weak(shared_from_this());
  std::thread dump_thread([output_handler_weak]() {
    while (!output_handler_weak.expired() &&
           !output_handler_weak.lock()->dump_path_.empty()) {
      auto outputHandler = output_handler_weak.lock();
      std::unique_lock<std::mutex> lk(outputHandler->dump_mtx_);
      outputHandler->dump_cv_.wait(lk);
      static BmpDumpHelper helper;
      const std::string kFileName = "1.bmp";
      std::string full_name = outputHandler->dump_path_ + kFileName;
      if (outputHandler->dump_buf_ == nullptr) {
        continue;
      }
      helper.DumpToBmp(full_name, outputHandler->dump_buf_.get(),
                       outputHandler->width_, outputHandler->height_);
    }
  });
  dump_thread.detach();
}

void NWebOutputHandler::StartFrameStat() {
  std::weak_ptr<NWebOutputHandler> output_handler_weak(shared_from_this());
  std::thread frame_stat_thread([output_handler_weak]() {
    while (!output_handler_weak.expired() &&
           output_handler_weak.lock()->frame_info_dump_) {
      {
        auto output_handler = output_handler_weak.lock();
        std::unique_lock<std::mutex> lk(output_handler->frame_stat_mtx_);
        if (output_handler->local_time_base_ !=
            output_handler->local_time_curr_) {
          auto fps_cur = 1000.0 * 1000 *
                         (output_handler->local_render_count_ - 1) /
                         (output_handler->local_time_curr_ -
                          output_handler->local_time_base_);
          WVLOG_W(
              "NWeb %{public}u fps_cur=%{public}0.1f "
              "frame_total_count=%{public}" PRId64
              ""
              " frame_miss_count=%{public}" PRId64 "",
              output_handler->nweb_id_, fps_cur, output_handler->render_count_,
              output_handler->frame_miss_count_);
          std::cout << "NWeb " << output_handler->nweb_id_
                    << " fps_cur=" << std::fixed << std::setprecision(1)
                    << fps_cur
                    << " frame_total_count=" << output_handler->render_count_
                    << " frame_miss_count=" << output_handler->frame_miss_count_
                    << std::endl;
        }
        output_handler->local_render_count_ = 0;
        output_handler->local_time_base_ = 0;
        output_handler->local_time_curr_ = 0;
      }
      // during sleep(), output_handler_weak should not be
      // promote to shared_ptr, to avoid add refcount
      constexpr int kOutoutFrameStatDur = 1;
      sleep(kOutoutFrameStatDur);
    }
  });
  frame_stat_thread.detach();
}

void NWebOutputHandler::StartRenderOutput() {
  if (!dump_path_.empty()) {
    StartDumpToFile();
  }
  if (frame_info_dump_) {
    StartFrameStat();
  }
}

void NWebOutputHandler::SetFrameInfoDump(bool flag) {
  frame_info_dump_ = flag;
}

void NWebOutputHandler::SetDumpPath(const std::string& path) {
  dump_path_ = path;
}

void NWebOutputHandler::BmpDumpHelper::DumpToBmp(const std::string& filename,
                                                 char* buf,
                                                 int width,
                                                 int height) {
  RgbaToRgb(buf, width, height);
  WriteToBmp(filename, buf, width, height);
  WVLOG_I("save render frame to %{public}s", filename.c_str());
}

void NWebOutputHandler::BmpDumpHelper::RgbaToRgb(char* buf,
                                                 int width,
                                                 int height) {
  char* p_rgba = buf;
  char* p_rgb = buf;
  uint64_t len = width * height * kBitsPerPixel;
  for (uint64_t i = 0; i < len; i++, p_rgba++) {
    if (i % 4 == 3) {
      // check alpha value, if 0, set related color to white
      if (buf[i] == 0) {
        *(unsigned char*)(p_rgb - 3) = 255;
        *(unsigned char*)(p_rgb - 2) = 255;
        *(unsigned char*)(p_rgb - 1) = 255;
      }
      continue;
    }
    *p_rgb++ = *p_rgba;
    if (i % 4 == 2) {
      char tmp = *(p_rgb - 1);
      *(p_rgb - 1) = *(p_rgb - 3);
      *(p_rgb - 3) = tmp;
    }
  }
}

void NWebOutputHandler::BmpDumpHelper::WriteToBmp(const std::string& filename,
                                                  char* buf,
                                                  int width,
                                                  int height) {
  BmpFileHeader bmp_file_header;
  BmpInfoHeader bmp_info_header;
  // Magic number for file. It does not fit in the header structure
  // due to alignment requirements, so put it outside
  unsigned short bmp_file_type = 0x4d42;
  constexpr int kRgbBitsPerPixel = 3;
  int rgb_buf_size = width * height * kRgbBitsPerPixel;
  bmp_file_header.bf_size =
      sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + rgb_buf_size;
  bmp_info_header.bi_size = sizeof(BmpInfoHeader);
  bmp_info_header.bi_width = width;
  bmp_info_header.bi_height = -height;

  FILE* file = fopen(filename.c_str(), "wb");
  if (!file) {
    WVLOG_E("FAIL to open %{public}s", filename.c_str());
    return;
  }

  // Write headers
  fwrite(&bmp_file_type, sizeof(bmp_file_type), 1, file);
  fwrite(&bmp_file_header, sizeof(bmp_file_header), 1, file);
  fwrite(&bmp_info_header, sizeof(bmp_info_header), 1, file);
  fwrite(buf, rgb_buf_size, 1, file);
  fclose(file);
}

void NWebOutputHandler::SetNWebId(uint32_t id) {
  nweb_id_ = id;
}

bool NWebOutputHandler::IsSizeValid() {
  return (width_ > 0) && (height_ > 0);
}

void* NWebOutputHandler::GetNativeWindowFromSurface(void* surface) {
  window_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                .GetWindowAdapterInstance()
                .CreateNativeWindowFromSurface(surface);
  return window_;
}
}  // namespace OHOS::NWeb
