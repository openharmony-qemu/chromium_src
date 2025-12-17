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

#include <gmock/gmock.h>

#include <utility>

#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/files/scoped_file.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/run_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/test/task_environment.h"
#include "base/trace_event/trace_event.h"
#include "gpu/config/gpu_finch_features.h"
#include "gtest/gtest.h"
#include "media/base/video_frame.h"
#include "media/gpu/macros.h"
#include "media/gpu/ohos/ohos_heif_image_decoder.h"
#include "media/parsers/webp_parser.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_fence.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"
#include "ui/gfx/native_pixmap_handle.h"
#define private public
#include "media/gpu/ohos/ohos_image_decode_accelerator_worker.h"
#undef private

namespace media {
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

constexpr gfx::BufferFormat kFormatForDecodes = gfx::BufferFormat::YVU_420;

class MockOhosImageDecoder : public OhosImageDecoder {
 public:
  MockOhosImageDecoder() = default;
  ~MockOhosImageDecoder() override = default;

  MOCK_METHOD(bool, Initialize, (), (override));
  MOCK_METHOD(OhosImageDecodeStatus,
              Decode,
              (base::span<const uint8_t> encoded_image),
              (override));
  MOCK_METHOD(std::unique_ptr<NativePixmapAndSizeInfo>,
              ExportAsNativePixmapDmaBuf,
              (OhosImageDecodeStatus * status),
              (override));
  MOCK_METHOD(gpu::ImageDecodeAcceleratorSupportedProfile,
              GetSupportedProfile,
              (),
              (const override));
  MOCK_METHOD(gpu::ImageDecodeAcceleratorType, GetType, (), (const override));
  MOCK_METHOD(SkYUVColorSpace, GetYUVColorSpace, (), (const override));
  MOCK_METHOD(void, ReleaseDecodedPixelMap, (), (override));
  MOCK_METHOD(OHOS::NWeb::AllocatorType,
              GetDecodeAllocatorType,
              (),
              (const override));
  MOCK_METHOD(bool, IsYuvFormat, (), (const override));
};

class MockNativePixmapDmaBuf : public gfx::NativePixmapDmaBuf {
 public:
  explicit MockNativePixmapDmaBuf(const gfx::Size& size)
      : gfx::NativePixmapDmaBuf(size,
                                kFormatForDecodes,
                                gfx::NativePixmapHandle()) {}
  ~MockNativePixmapDmaBuf() override = default;

  MOCK_METHOD(bool, AreDmaBufFdsValid, (), (const, override));
  MOCK_METHOD(int, GetDmaBufFd, (size_t plane), (const, override));
  MOCK_METHOD(uint32_t, GetDmaBufPitch, (size_t plane), (const, override));
  MOCK_METHOD(size_t, GetDmaBufOffset, (size_t plane), (const, override));
  MOCK_METHOD(size_t, GetDmaBufPlaneSize, (size_t plane), (const, override));
  MOCK_METHOD(uint64_t, GetBufferFormatModifier, (), (const, override));
  MOCK_METHOD(gfx::BufferFormat, GetBufferFormat, (), (const, override));
  MOCK_METHOD(size_t, GetNumberOfPlanes, (), (const, override));
  MOCK_METHOD(bool, SupportsZeroCopyWebGPUImport, (), (const, override));
  MOCK_METHOD(gfx::Size, GetBufferSize, (), (const, override));
  MOCK_METHOD(uint32_t, GetUniqueId, (), (const, override));

  MOCK_METHOD(bool,
              ScheduleOverlayPlane,
              (gfx::AcceleratedWidget widget,
               const gfx::OverlayPlaneData& overlay_plane_data,
               std::vector<gfx::GpuFence> acquire_fences,
               std::vector<gfx::GpuFence> release_fences),
              (override));

  gfx::NativePixmapHandle ExportHandle() const override {
    gfx::NativePixmapHandle handle{};
    DCHECK_EQ(gfx::BufferFormat::YVU_420, GetBufferFormat());
    if (is_empty_) {
      handle.planes = std::vector<gfx::NativePixmapPlane>();
    } else {
      handle.planes = std::vector<gfx::NativePixmapPlane>(3u);
    }
    return handle;
  }
  MOCK_METHOD(void*, GetWindowBuffer, (), (override));

  bool is_empty_ = true;
};

std::unique_ptr<OhosImageDecoder> CreateTestDecoder(bool initialize) {
  auto decoder = std::make_unique<NiceMock<MockOhosImageDecoder>>();
  EXPECT_CALL(*decoder, Initialize()).WillOnce(testing::Return(initialize));
  return decoder;
}

class OhosImageDecodeAcceleratorWorkerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    auto mock_decoder = std::make_unique<NiceMock<MockOhosImageDecoder>>();
    mock_decoder_ = mock_decoder.get();
    ON_CALL(*mock_decoder_, GetType())
        .WillByDefault(Return(gpu::ImageDecodeAcceleratorType::kHeif));
    gpu::ImageDecodeAcceleratorSupportedProfile expected_profile;
    expected_profile.image_type = gpu::ImageDecodeAcceleratorType::kHeif;

    EXPECT_CALL(*mock_decoder_, GetSupportedProfile())
        .WillOnce(Return(expected_profile));

    OhosImageDecoderVector decoders;
    decoders.push_back(std::move(mock_decoder));

    worker_ = base::WrapUnique(
        new OhosImageDecodeAcceleratorWorker(std::move(decoders)));
    EXPECT_NE(worker_, nullptr);
  }

  void TearDown() override {
    worker_.reset();
    mock_decoder_ = nullptr;
  }

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<OhosImageDecodeAcceleratorWorker> worker_;
  raw_ptr<MockOhosImageDecoder> mock_decoder_;
};

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Create) {
  std::function<std::unique_ptr<OhosImageDecoder>()> decoder = []() {
    return CreateTestDecoder(true);
  };
  SetTestDecoder(decoder);
  auto worker = OhosImageDecodeAcceleratorWorker::Create();
  EXPECT_NE(worker, nullptr);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, CreateFailure) {
  std::function<std::unique_ptr<OhosImageDecoder>()> decoder = []() {
    return CreateTestDecoder(false);
  };
  SetTestDecoder(decoder);
  auto worker = OhosImageDecodeAcceleratorWorker::Create();
  EXPECT_EQ(worker, nullptr);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, GetSupportedProfiles) {
  auto profiles = worker_->GetSupportedProfiles();
  EXPECT_EQ(1u, profiles.size());
  EXPECT_EQ(gpu::ImageDecodeAcceleratorType::kHeif, profiles[0].image_type);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, GetDecoderForImage) {
  auto decoder = worker_->GetDecoderForImage();
  EXPECT_EQ(decoder, mock_decoder_);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, ReleaseDecodedPixelMap) {
  auto mock_decoder = std::make_unique<NiceMock<MockOhosImageDecoder>>();
  auto decoder_ = mock_decoder.get();
  ON_CALL(*decoder_, GetType())
      .WillByDefault(Return(gpu::ImageDecodeAcceleratorType::kJpeg));
  gpu::ImageDecodeAcceleratorSupportedProfile expected_profile;
  expected_profile.image_type = gpu::ImageDecodeAcceleratorType::kJpeg;

  EXPECT_CALL(*decoder_, GetSupportedProfile())
      .WillOnce(Return(expected_profile));

  OhosImageDecoderVector decoders;
  decoders.push_back(std::move(mock_decoder));

  auto worker = base::WrapUnique(
      new OhosImageDecodeAcceleratorWorker(std::move(decoders)));
  EXPECT_NE(worker, nullptr);
  EXPECT_CALL(*decoder_, ReleaseDecodedPixelMap()).Times(0);
  worker->ReleaseDecodedPixelMap();
  EXPECT_CALL(*mock_decoder_, ReleaseDecodedPixelMap()).Times(1);
  worker_->ReleaseDecodedPixelMap();
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Decode_NoDecoderAvailable) {
  OhosImageDecoderVector empty_decoders;
  auto empty_worker = base::WrapUnique(
      new OhosImageDecodeAcceleratorWorker(std::move(empty_decoders)));
  std::vector<uint8_t> encoded_data = {0x00, 0x01};
  gfx::Size output_size(100, 100);

  bool callback_called = false;
  gpu::ImageDecodeAcceleratorWorker::CompletedDecodeCB decode_cb =
      base::BindOnce(
          [](bool* callback_called,
             std::unique_ptr<gpu::ImageDecodeAcceleratorWorker::DecodeResult>
                 result) {
            *callback_called = true;
            EXPECT_EQ(result, nullptr);
          },
          &callback_called);
  testing::internal::CaptureStderr();
  empty_worker->Decode(std::move(encoded_data), output_size,
                       std::move(decode_cb));
  task_environment_.RunUntilIdle();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[HeifSupport] DecodeTask: No decoder is available "
                            "for supplied image"),
            std::string::npos);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Decode_DecodeFailure) {
  std::vector<uint8_t> encoded_data = {0x00, 0x01};
  gfx::Size output_size(100, 100);

  bool callback_called = false;
  auto decode_cb = base::BindOnce(
      [](bool* callback_called,
         std::unique_ptr<gpu::ImageDecodeAcceleratorWorker::DecodeResult>
             result) {
        *callback_called = true;
        EXPECT_EQ(result, nullptr);
      },
      &callback_called);

  EXPECT_CALL(*mock_decoder_, Decode(_))
      .WillOnce(Return(OhosImageDecodeStatus::kExecuteDecodeFailed));
  testing::internal::CaptureStderr();
  worker_->Decode(std::move(encoded_data), output_size, std::move(decode_cb));
  task_environment_.RunUntilIdle();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[HeifSupport]  Failed to decode - status ="),
            std::string::npos);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Decode_ExportFailure) {
  std::vector<uint8_t> encoded_data = {0x00, 0x01};
  gfx::Size output_size(100, 100);

  bool callback_called = false;
  auto decode_cb = base::BindOnce(
      [](bool* callback_called,
         std::unique_ptr<gpu::ImageDecodeAcceleratorWorker::DecodeResult>
             result) {
        *callback_called = true;
        EXPECT_EQ(result, nullptr);
      },
      &callback_called);
  EXPECT_CALL(*mock_decoder_, Decode(_))
      .WillOnce(Return(OhosImageDecodeStatus::kSuccess));
  EXPECT_CALL(*mock_decoder_, ExportAsNativePixmapDmaBuf(_))
      .WillOnce([](OhosImageDecodeStatus* status) {
        *status = OhosImageDecodeStatus::kExecuteDecodeFailed;
        return nullptr;
      });
  testing::internal::CaptureStderr();
  worker_->Decode(std::move(encoded_data), output_size, std::move(decode_cb));
  task_environment_.RunUntilIdle();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[HeifSupport] Failed to export surface - status"),
            std::string::npos);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Decode_SizeMismatch) {
  std::vector<uint8_t> encoded_data = {0x00, 0x01};
  gfx::Size output_size(100, 100);
  gfx::Size actual_size(200, 200);

  bool callback_called = false;
  auto decode_cb = base::BindOnce(
      [](bool* callback_called,
         std::unique_ptr<gpu::ImageDecodeAcceleratorWorker::DecodeResult>
             result) {
        *callback_called = true;
        EXPECT_EQ(result, nullptr);
      },
      &callback_called);

  EXPECT_CALL(*mock_decoder_, Decode(_))
      .WillOnce(Return(OhosImageDecodeStatus::kSuccess));

  EXPECT_CALL(*mock_decoder_, ExportAsNativePixmapDmaBuf(_))
      .WillOnce([actual_size](OhosImageDecodeStatus* status) {
        *status = OhosImageDecodeStatus::kSuccess;
        auto exported_pixmap = std::make_unique<NativePixmapAndSizeInfo>();
        auto mock_pixmap =
            base::MakeRefCounted<MockNativePixmapDmaBuf>(actual_size);
        EXPECT_CALL(*mock_pixmap, GetBufferSize())
            .WillRepeatedly(Return(actual_size));
        exported_pixmap->pixmap = mock_pixmap;
        exported_pixmap->byte_size = 1000;
        return exported_pixmap;
      });

  testing::internal::CaptureStderr();
  worker_->Decode(std::move(encoded_data), output_size, std::move(decode_cb));
  task_environment_.RunUntilIdle();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[HeifSupport] Scaling is not supported"),
            std::string::npos);
}

TEST_F(OhosImageDecodeAcceleratorWorkerTest, Decode_ExportHandleFailure) {
  std::vector<uint8_t> encoded_data = {0x00, 0x01};
  gfx::Size output_size(100, 100);
  gfx::Size actual_size(100, 100);

  bool callback_called = false;
  auto decode_cb = base::BindOnce(
      [](bool* callback_called,
         std::unique_ptr<gpu::ImageDecodeAcceleratorWorker::DecodeResult>
             result) {
        *callback_called = true;
        EXPECT_EQ(result, nullptr);
      },
      &callback_called);

  EXPECT_CALL(*mock_decoder_, Decode(_))
      .WillOnce(Return(OhosImageDecodeStatus::kSuccess));

  EXPECT_CALL(*mock_decoder_, ExportAsNativePixmapDmaBuf(_))
      .WillOnce([actual_size](OhosImageDecodeStatus* status) {
        *status = OhosImageDecodeStatus::kSuccess;
        auto exported_pixmap = std::make_unique<NativePixmapAndSizeInfo>();
        auto mock_pixmap =
            base::MakeRefCounted<MockNativePixmapDmaBuf>(actual_size);
        EXPECT_CALL(*mock_pixmap, GetBufferSize())
            .WillRepeatedly(Return(actual_size));
        exported_pixmap->pixmap = mock_pixmap;
        exported_pixmap->byte_size = 1000;
        return exported_pixmap;
      });

  testing::internal::CaptureStderr();
  worker_->Decode(std::move(encoded_data), output_size, std::move(decode_cb));
  task_environment_.RunUntilIdle();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("[HeifSupport] Could not export the NativePixmapHandle"),
      std::string::npos);
}
}  // namespace media