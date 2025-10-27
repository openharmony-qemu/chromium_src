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
#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>
#include <utility>

#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/format_macros.h"
#include "base/functional/bind.h"
#include "base/test/task_environment.h"
#include "build/build_config.h"
#define protected public
#define private public
#include "components/download/internal/common/parallel_download_utils.h"
#include "components/download/public/common/arkweb_download_file_impl_ext.h"
#undef private
#undef protected
#include "components/download/public/common/download_create_info.h"
#include "components/download/public/common/download_destination_observer.h"
#include "components/download/public/common/download_features.h"
#include "components/download/public/common/download_file_impl.h"
#include "components/download/public/common/download_interrupt_reasons_utils.h"
#include "components/download/public/common/download_stats.h"
#include "components/download/public/common/download_destination_observer.h"

namespace download {
namespace {
static const std::string kTestContent = "ArkWebDownloadTestData1234567890";
static constexpr uint32_t kDownloadId = 1;
}
class MockInputStream : public InputStream {
 public:
  MockInputStream() = default;
  ~MockInputStream() override = default;

  // InputStream functions
  MOCK_METHOD0(IsEmpty, bool());
  MOCK_METHOD1(RegisterDataReadyCallback,
               void(const mojo::SimpleWatcher::ReadyCallback&));
  MOCK_METHOD0(ClearDataReadyCallback, void());
  MOCK_METHOD2(Read,
               InputStream::StreamState(scoped_refptr<net::IOBuffer>*,
                                        size_t*));
  MOCK_METHOD0(GetCompletionStatus, DownloadInterruptReason());
};

class MockDownloadDestinationObserver : public DownloadDestinationObserver {
 public:
  MockDownloadDestinationObserver() = default;
  ~MockDownloadDestinationObserver() override = default;

  MOCK_METHOD3(
      DestinationUpdate,
      void(int64_t bytes_so_far,
           int64_t bytes_per_sec,
           const std::vector<DownloadItem::ReceivedSlice>& received_slices));
  MOCK_METHOD3(DestinationError,
               void(DownloadInterruptReason reason,
                    int64_t bytes_so_far,
                    std::unique_ptr<crypto::SecureHash> hash_state));
  MOCK_METHOD2(DestinationCompleted,
               void(int64_t total_bytes,
                    std::unique_ptr<crypto::SecureHash> hash_state));
};

class MockSourceStream : public DownloadFileImpl::SourceStream {
 public:
  MockSourceStream(int64_t offset, int64_t starting_file_write_offset)
      : DownloadFileImpl::SourceStream(offset,
                                       starting_file_write_offset,
                                       std::make_unique<MockInputStream>()) {}
  ~MockSourceStream() = default;

  MOCK_METHOD0(Initialize, void());
  MOCK_METHOD1(RegisterDataReadyCallback,
               void(const mojo::SimpleWatcher::ReadyCallback&));
  MOCK_METHOD0(ClearDataReadyCallback, void());
  MOCK_METHOD2(Read,
               InputStream::StreamState(scoped_refptr<net::IOBuffer>*,
                                        size_t*));
  MOCK_METHOD0(GetCompletionStatus, DownloadInterruptReason());
  MOCK_METHOD0(starting_file_offset, int64_t());
  MOCK_METHOD0(bytes_read, int64_t());
  MOCK_METHOD0(TotalBytesConsumed, int64_t());
  MOCK_METHOD0(is_partial_request, bool());
  MOCK_METHOD0(length, int64_t());
};

class ArkWebDownloadFileImplExtTest : public ::testing::Test {
 public:
  void ReadDataFromDownloadFileDone(const std::vector<uint8_t>& data) {
    callback_called_ = true;
    received_data_ = data;
  }

 protected:
  void SetUp() override {
    test_file_path_ = CreateTestFile(
        "test_download_file.bin", kTestContent.c_str(), kTestContent.size());
    save_info_ = std::make_unique<DownloadSaveInfo>();
    EXPECT_NE(save_info_, nullptr);
    save_info_->file_path = test_file_path_;

    stream_ = std::make_unique<MockInputStream>();
    EXPECT_NE(stream_, nullptr);
    observer_ = std::make_unique<MockDownloadDestinationObserver>();
    EXPECT_NE(observer_, nullptr);
    observer_weak_factory_ =
        std::make_unique<base::WeakPtrFactory<DownloadDestinationObserver>>(
            observer_.get());
  }

  void TearDown() override {
    observer_weak_factory_.reset();
    observer_.reset();
    stream_.reset();
    save_info_.reset();
    base::DeletePathRecursively(scoped_temp_dir_.GetPath());
  }

  base::FilePath CreateTestFile(const char* test_file,
                                const char* text,
                                size_t size) {
    base::FilePath test_file_path = scoped_temp_dir_.GetPath();
    test_file_path = test_file_path.Append("download/testdata");
    EXPECT_TRUE(base::CreateDirectory(test_file_path));
    test_file_path = test_file_path.Append(test_file);
    EXPECT_TRUE(base::WriteFile(test_file_path, text));
    return test_file_path;
  }

  std::unique_ptr<ArkWebDownloadFileImplExt> CreateDownloadFile() {
    auto download = std::make_unique<ArkWebDownloadFileImplExt>(
        std::move(save_info_), test_file_path_, std::move(stream_), kDownloadId,
        observer_weak_factory_->GetWeakPtr());
    download->file_.full_path_ = test_file_path_;
    return download;
  }

  base::test::TaskEnvironment task_environment_;
  base::ScopedTempDir scoped_temp_dir_;
  base::FilePath test_file_path_;
  std::unique_ptr<DownloadSaveInfo> save_info_;
  std::unique_ptr<MockInputStream> stream_;
  std::unique_ptr<MockDownloadDestinationObserver> observer_;
  std::unique_ptr<base::WeakPtrFactory<DownloadDestinationObserver>>
      observer_weak_factory_;

  bool callback_called_ = false;
  std::vector<uint8_t> received_data_{};
};

TEST_F(ArkWebDownloadFileImplExtTest, Constructor_ValidParameters) {
  auto download_file = CreateDownloadFile();
  ASSERT_NE(download_file, nullptr);
  EXPECT_NE(download_file->AsArkWebDownloadFileImplExt(), nullptr);
}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
TEST_F(ArkWebDownloadFileImplExtTest, ReadDownloadDataFromFile_ValidRead) {
  std::vector<char> buffer(kTestContent.size());
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  bool result =
      download_file->ReadDownloadDataFromFile(0, buffer.data(), buffer.size());
  EXPECT_TRUE(result);
  EXPECT_EQ(std::string(buffer.data(), buffer.size()), kTestContent.c_str());
}

TEST_F(ArkWebDownloadFileImplExtTest, ReadDownloadDataFromFile_ZeroSize) {
  std::vector<char> buffer(kTestContent.size());
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  bool result = download_file->ReadDownloadDataFromFile(0, buffer.data(), 0);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadFileImplExtTest, ReadDownloadDataFromFile_InvalidOffset) {
  std::vector<char> buffer(kTestContent.size());
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);

  bool result = download_file->ReadDownloadDataFromFile(
      kTestContent.size() + 100, buffer.data(),
      buffer.size());  // 100 offset beyond file size
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadFileImplExtTest, RegisterReadDownloadCallback) {
  const size_t content_size = 5;
  auto callback = base::BindOnce(
      &ArkWebDownloadFileImplExtTest::ReadDataFromDownloadFileDone,
      base::Unretained(this));

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->RegisterReadDownloadCallback(std::move(callback),
                                              content_size);
  EXPECT_TRUE(download_file->read_download_callback_);
  EXPECT_EQ(download_file->read_download_size_, content_size);
}

TEST_F(ArkWebDownloadFileImplExtTest,
       RegisterReadDownloadCallback_ExistingCallback) {
  const size_t content_size = 5;
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->RegisterReadDownloadCallback(
      base::BindOnce([](const std::vector<uint8_t>&) {}), content_size);
  download_file->RegisterReadDownloadCallback(
      base::BindOnce([](const std::vector<uint8_t>&) {}), content_size + 1);
  EXPECT_EQ(download_file->read_download_size_, content_size);
}

TEST_F(ArkWebDownloadFileImplExtTest,
       ReadDownloadDataAndRunCallback_ValidData) {
  callback_called_ = false;
  received_data_.clear();
  auto callback = base::BindOnce(
      &ArkWebDownloadFileImplExtTest::ReadDataFromDownloadFileDone,
      base::Unretained(this));

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->RegisterReadDownloadCallback(
      std::move(callback), static_cast<int32_t>(kTestContent.size()));
  download_file->ReadDownloadDataAndRunCallback(
      static_cast<int32_t>(kTestContent.size()));

  task_environment_.RunUntilIdle();
  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(received_data_.size(), kTestContent.size());
  std::string received_string(received_data_.begin(), received_data_.end());
  EXPECT_EQ(received_string, kTestContent);
}

TEST_F(ArkWebDownloadFileImplExtTest,
       ReadDownloadDataAndRunCallback_NoCallback) {
  callback_called_ = false;
  received_data_.clear();

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->ReadDownloadDataAndRunCallback(
      static_cast<int32_t>(kTestContent.size()));
  task_environment_.RunUntilIdle();
  EXPECT_FALSE(callback_called_);
}

TEST_F(ArkWebDownloadFileImplExtTest,
       ReadDownloadDataAndRunCallback_ReadError) {
  auto invalid_save_info = std::make_unique<DownloadSaveInfo>();
  EXPECT_NE(invalid_save_info, nullptr);
  invalid_save_info->file_path = test_file_path_.Append("nonexistent.file");

  auto download_file = std::make_unique<ArkWebDownloadFileImplExt>(
      std::move(invalid_save_info), test_file_path_,
      std::make_unique<MockInputStream>(), kDownloadId,
      observer_weak_factory_->GetWeakPtr());
  EXPECT_NE(download_file, nullptr);

  callback_called_ = false;
  received_data_.clear();
  auto callback = base::BindOnce(
      &ArkWebDownloadFileImplExtTest::ReadDataFromDownloadFileDone,
      base::Unretained(this));

  download_file->RegisterReadDownloadCallback(
      std::move(callback), static_cast<int32_t>(kTestContent.size()));
  download_file->ReadDownloadDataAndRunCallback(
      static_cast<int32_t>(kTestContent.size()));

  task_environment_.RunUntilIdle();
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(received_data_.empty());
}

TEST_F(ArkWebDownloadFileImplExtTest,
       MaybeRunReadDownloadCallback_WithCallback) {
  callback_called_ = false;
  received_data_.clear();
  auto callback = base::BindOnce(
      &ArkWebDownloadFileImplExtTest::ReadDataFromDownloadFileDone,
      base::Unretained(this));

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->RegisterReadDownloadCallback(
      std::move(callback), static_cast<int32_t>(kTestContent.size()));
  download_file->MaybeRunReadDownloadCallback();
  task_environment_.RunUntilIdle();
  EXPECT_TRUE(callback_called_);
}

TEST_F(ArkWebDownloadFileImplExtTest, MaybeRunReadDownloadCallback_NoCallback) {
  callback_called_ = false;
  received_data_.clear();

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->MaybeRunReadDownloadCallback();
  task_environment_.RunUntilIdle();
  EXPECT_FALSE(callback_called_);
}

TEST_F(ArkWebDownloadFileImplExtTest, GetNoHoleDownloadDataSize_NonSparseFile) {
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  download_file->read_download_size_ =
      static_cast<uint32_t>(kTestContent.size());
  uint32_t size = download_file->GetNoHoleDownloadDataSize();
  EXPECT_LE(size, kTestContent.size());
}

TEST_F(ArkWebDownloadFileImplExtTest,
       ReadAndRunCallbackIfDataReady_InsufficientData) {
  callback_called_ = false;
  received_data_.clear();
  auto callback = base::BindOnce(
      &ArkWebDownloadFileImplExtTest::ReadDataFromDownloadFileDone,
      base::Unretained(this));

  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  // no callback registered
  download_file->ReadAndRunCallbackIfDataReady();
  task_environment_.RunUntilIdle();
  EXPECT_FALSE(callback_called_);

  download_file->RegisterReadDownloadCallback(
      std::move(callback), static_cast<int32_t>(kTestContent.size()));
  // bytes_so_far_ = 0
  download_file->ReadAndRunCallbackIfDataReady();
  EXPECT_FALSE(callback_called_);
  // TotalBytesReceived
  download_file->file_.bytes_so_far_ =
      kTestContent.size() - 2;  // 2 test bytes
  download_file->ReadAndRunCallbackIfDataReady();
  EXPECT_FALSE(callback_called_);
  download_file->file_.bytes_so_far_ = kTestContent.size();
  download_file->ReadAndRunCallbackIfDataReady();
  task_environment_.RunUntilIdle();
  EXPECT_TRUE(callback_called_);
}

TEST_F(ArkWebDownloadFileImplExtTest, StopDownloadJobTimer_RunningTimer) {
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  auto source_stream = std::make_unique<MockSourceStream>(0, 0);
  EXPECT_NE(source_stream, nullptr);

  // no timer stop
  ASSERT_NO_FATAL_FAILURE(download_file->StopDownloadJobTimer());
  ASSERT_NO_FATAL_FAILURE(
      download_file->CreateDownloadJobTimer(source_stream.get()));
  EXPECT_NE(download_file->download_job_timer_, nullptr);
  EXPECT_TRUE(download_file->download_job_timer_->IsRunning());
  ASSERT_NO_FATAL_FAILURE(download_file->StopDownloadJobTimer());
  EXPECT_FALSE(download_file->download_job_timer_->IsRunning());
  // repeate top
  ASSERT_NO_FATAL_FAILURE(download_file->StopDownloadJobTimer());
}

TEST_F(ArkWebDownloadFileImplExtTest, ResetDownloadJobTimer_RunningTimer) {
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  auto source_stream = std::make_unique<MockSourceStream>(0, 0);
  EXPECT_NE(source_stream, nullptr);

  // no timer reset
  ASSERT_NO_FATAL_FAILURE(download_file->ResetDownloadJobTimer());
  ASSERT_NO_FATAL_FAILURE(
      download_file->CreateDownloadJobTimer(source_stream.get()));
  EXPECT_NE(download_file->download_job_timer_, nullptr);
  EXPECT_TRUE(download_file->download_job_timer_->IsRunning());

  ASSERT_NO_FATAL_FAILURE(download_file->ResetDownloadJobTimer());
  EXPECT_EQ(download_file->download_job_timer_, nullptr);
  // repeate reset
  ASSERT_NO_FATAL_FAILURE(download_file->ResetDownloadJobTimer());
}

TEST_F(ArkWebDownloadFileImplExtTest, OnTimeout_ValidParameters) {
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);
  auto source_stream = std::make_unique<MockSourceStream>(0, 0);
  EXPECT_NE(source_stream, nullptr);

  // NotifyObserver
  EXPECT_CALL(*observer_,
              DestinationError(DOWNLOAD_INTERRUPT_REASON_NETWORK_DISCONNECTED,
                               ::testing::_, ::testing::_))
      .Times(2);
  download_file->CreateDownloadJobTimer(source_stream.get());
  download_file->OnTimeout(source_stream.get(),
                           DOWNLOAD_INTERRUPT_REASON_NETWORK_DISCONNECTED,
                           InputStream::COMPLETE, true);
  task_environment_.RunUntilIdle();
  EXPECT_EQ(download_file->download_job_timer_, nullptr);
  download_file->OnTimeout(source_stream.get(),
                           DOWNLOAD_INTERRUPT_REASON_NETWORK_DISCONNECTED,
                           InputStream::COMPLETE, true);
  task_environment_.RunUntilIdle();
}

TEST_F(ArkWebDownloadFileImplExtTest, GetNoHoleDownloadDataSize_SparseFile) {
  auto download_file = CreateDownloadFile();
  EXPECT_NE(download_file, nullptr);

  // simulate sparse file with 2 slices
  DownloadItem::ReceivedSlice slice1(0, 5);
  DownloadItem::ReceivedSlice slice2(5, 20, true);
  download_file->received_slices_.push_back(slice1);
  download_file->received_slices_.push_back(slice2);
  download_file->read_download_size_ = 30;  // larger than received slice size
  uint32_t size = download_file->GetNoHoleDownloadDataSize();
  EXPECT_EQ(size, 5u);
}
#endif
}  // namespace download