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
#include <ranges>
#define private public
#include "components/download/public/common/download_item_impl.h"
#undef private
#include "build/build_config.h"
#include "build/buildflag.h"

#include "arkweb/chromium_ext/components/download/internal/common/arkweb_download_item_impl_ext.h"

#include "components/download/public/common/download_file_impl.h"
#include "arkweb/chromium_ext/components/download/public/common/arkweb_download_file_impl_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/functional/callback_forward.h"
#include "base/test/task_environment.h"
#include "components/download/public/common/download_item_impl_delegate.h"
#include "components/download/public/common/download_item_rename_handler.h"
#include "components/download/public/common/download_task_runner.h"
#include "components/download/public/common/resume_mode.h"
#include "url/gurl.h"

namespace download {
class MockDelegate;
class MockDownloadFile;
class ArkWebDownloadItemImplExtTest : public ::testing::Test {
 protected:
  void SetUp() {
    delegate_ = std::make_unique<MockDelegate>();
    DownloadJob::CancelRequestCallback callback = base::BindOnce(
        [](bool cancel_value, std::optional<std::string> optional_str) {});
    download_item_ = std::make_unique<ArkWebDownloadItemImplExt>(
        delegate_.get(), 0, base::FilePath(), GURL(), "", std::move(callback));
    base::CommandLine::Init(0, nullptr);
    original_command_line_ =
        base::CommandLine::ForCurrentProcess()->GetCommandLineString();
    mock_download_file_ = std::make_unique<MockDownloadFile>();
  }
  void TearDown() {
    task_environment_.RunUntilIdle();
    download_item_.reset();
    delegate_.reset();
  }
  void SetCommandLineSwitch(bool enabled) {
    if (enabled) {
      base::CommandLine::ForCurrentProcess()->AppendSwitch(
          switches::kEnableNwebExDownload);
    } else {
      base::CommandLine::ForCurrentProcess()->RemoveSwitch(
          switches::kEnableNwebExDownload);
    }
  }
  void SetLastReason(DownloadInterruptReason last_reason) {
    if (download_item_) {
      download_item_->last_reason_ = last_reason;
    }
  }
  bool IsAllowedAutoResume() {
    if (download_item_) {
      return download_item_->IsAllowedAutoResume();
    }
    return false;
  }
  void IsNeedAutoResume(DownloadInterruptReason reason,
                        ResumeMode& resume_mode,
                        bool& need_auto_resume) {
    if (download_item_) {
      download_item_->IsNeedAutoResume(reason, resume_mode, need_auto_resume);
    }
  }
  bool CheckIsNeedAutoResume(DownloadInterruptReason reason) {
    if (download_item_) {
      return download_item_->CheckIsNeedAutoResume(reason);
    }
    return false;
  }
  bool IsBeforeInProgress() {
    if (download_item_) {
      return download_item_->IsBeforeInProgress();
    }
    return false;
  }
  bool IsDownloadInProgress() {
    if (download_item_) {
      return download_item_->IsDownloadInProgress();
    }
    return false;
  }
  void SetState(DownloadItemImpl::DownloadInternalState state) {
    if (download_item_) {
      download_item_->state_ =
          static_cast<DownloadItemImpl::DownloadInternalState>(state);
    }
  }
  void SetAutoResumeCount(int32_t count) {
    if (download_item_) {
      download_item_->auto_resume_count_ = count;
    }
  }
  void SetPaused(bool paused) {
    if (download_item_) {
      download_item_->paused_ = paused;
    }
  }
  void SetAllowedAutoResume(bool allowed) {
    if (download_item_) {
      download_item_->last_reason_ =
          allowed ? DOWNLOAD_INTERRUPT_REASON_NONE
                  : DOWNLOAD_INTERRUPT_REASON_SERVER_NO_RANGE;
    }
  }
  void SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)> callback) {
    if (download_item_) {
      download_item_->read_download_callback_from_ui_ = std::move(callback);
    }
  }
  void SetDownloadFile(std::unique_ptr<DownloadFile> download_file) {
    if (download_item_) {
      download_item_->download_file_ = std::move(download_file);
    }
  }
  void RunCallbackIfStateMatch() {
    if (download_item_) {
      download_item_->RunCallbackIfStateMatch();
    }
  }
  void ReadDataFromDownloadFileDone(const std::vector<uint8_t>& data) {
    if (download_item_) {
      download_item_->ReadDataFromDownloadFileDone(data);
    }
  }
  bool IsReadDownload() {
    if (download_item_->read_download_callback_from_ui_) {
      return true;
    }
    return false;
  }
  void RunCallbackIfExistsCallback() {
    if (download_item_) {
      download_item_->RunCallbackIfExistsCallback();
    }
  }
  bool IsDownloadFile() {
    if (download_item_->download_file_) {
      return true;
    }
    return false;
  }
  void SetPercentCompleteTrue() {
    download_item_->delegate_delayed_complete_ = false;
    download_item_->total_bytes_ = 1;
    download_item_->destination_info_.received_bytes = 1;
  }
 protected:
  std::string original_command_line_;
  std::unique_ptr<MockDelegate> delegate_;
  std::unique_ptr<ArkWebDownloadItemImplExt> download_item_;
  std::unique_ptr<MockDownloadFile> mock_download_file_;
  base::test::TaskEnvironment task_environment_;
};

class MockDelegate : public DownloadItemImplDelegate {
 public:
  MockDelegate() = default;
  ~MockDelegate() override = default;
  MOCK_METHOD2(DetermineDownloadTarget,
               void(DownloadItemImpl*, DownloadTargetCallback));
  MOCK_METHOD2(ShouldCompleteDownload,
               bool(DownloadItemImpl*, base::OnceClosure));
  MOCK_METHOD2(ShouldOpenDownload,
               bool(DownloadItemImpl*, ShouldOpenDownloadCallback));
  MOCK_METHOD2(ShouldAutomaticallyOpenFile,
               bool(const GURL&, const base::FilePath&));
  MOCK_METHOD2(ShouldAutomaticallyOpenFileByPolicy,
               bool(const GURL&, const base::FilePath&));
  MOCK_METHOD1(CheckForFileRemoval, void(DownloadItemImpl*));
  MOCK_CONST_METHOD0(GetApplicationClientIdForFileScanning, std::string());
  MOCK_METHOD2(ResumeInterruptedDownload,
               void(std::unique_ptr<DownloadUrlParameters>,
                    const std::string&));
  MOCK_METHOD1(UpdatePersistence, void(DownloadItemImpl*));
  MOCK_METHOD1(OpenDownload, void(DownloadItemImpl*));
  MOCK_METHOD1(ShowDownloadInShell, void(DownloadItemImpl*));
  MOCK_METHOD1(DownloadRemoved, void(DownloadItemImpl*));
  MOCK_METHOD1(DownloadInterrupted, void(DownloadItemImpl*));
  MOCK_CONST_METHOD0(IsOffTheRecord, bool());
  MOCK_CONST_METHOD0(IsActiveNetworkMetered, bool());
  MOCK_METHOD1(ReportBytesWasted, void(DownloadItemImpl*));
  MOCK_METHOD1(BindWakeLockProvider,
               void(mojo::PendingReceiver<device::mojom::WakeLockProvider>));
  MOCK_METHOD0(GetQuarantineConnectionCallback, QuarantineConnectionCallback());
  MOCK_METHOD1(GetRenameHandlerForDownload,
               std::unique_ptr<DownloadItemRenameHandler>(DownloadItemImpl*));
};
class MockArkWebDownloadFileImplExt : public ArkWebDownloadFileImplExt {
 public:
  MockArkWebDownloadFileImplExt()
      : ArkWebDownloadFileImplExt(std::make_unique<DownloadSaveInfo>(),
                                  base::FilePath(),
                                  std::unique_ptr<InputStream>(nullptr),
                                  0,
                                  nullptr) {}
  MOCK_METHOD0(AsArkWebDownloadFileImplExt, ArkWebDownloadFileImplExt*());
  MOCK_METHOD0(ReadAndRunCallbackIfDataReady, void());
  MOCK_METHOD2(RegisterReadDownloadCallback,
               void(ReadDownloadDataCallback callback, int32_t size));
  MOCK_METHOD3(ReadDownloadDataFromFile,
               bool(int64_t offset, char* data, size_t size));
  MOCK_METHOD0(GetNoHoleDownloadDataSize, uint32_t());
  MOCK_METHOD1(ReadDownloadDataAndRunCallback,
               void(int32_t read_download_size));
  MOCK_METHOD1(CreateDownloadJobTimer, void(SourceStream* source_stream));
  MOCK_METHOD0(StopDownloadJobTimer, void());
  MOCK_METHOD0(ResetDownloadJobTimer, void());
};

class MockDownloadFile : public DownloadFile {
 public:
  MOCK_METHOD0(AsArkWebDownloadFileImplExt, ArkWebDownloadFileImplExt*());
  MOCK_METHOD3(Initialize,
               void(InitializeCallback initialize_callback,
                    CancelRequestCallback cancel_request_callback,
                    const DownloadItem::ReceivedSlices& received_slices));
  MOCK_METHOD2(AddInputStream,
               void(std::unique_ptr<InputStream> stream, int64_t offset));
  MOCK_METHOD2(RenameAndUniquify,
               void(const base::FilePath& full_path,
                    RenameCompletionCallback callback));
  MOCK_METHOD7(
      RenameAndAnnotate,
      void(const base::FilePath& full_path,
           const std::string& client_guid,
           const GURL& source_url,
           const GURL& referrer_url,
           const std::optional<url::Origin>& request_initiator,
           mojo::PendingRemote<quarantine::mojom::Quarantine> remote_quarantine,
           RenameCompletionCallback callback));
  MOCK_METHOD0(Detach, void());
  MOCK_METHOD0(Cancel, void());
  MOCK_METHOD1(SetPotentialFileLength, void(int64_t length));
  MOCK_CONST_METHOD0(FullPath, const base::FilePath&());
  MOCK_CONST_METHOD0(InProgress, bool());
  MOCK_METHOD0(Pause, void());
  MOCK_METHOD0(Resume, void());
  MOCK_METHOD0(IsMemoryFile, bool());
};

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_001) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_SERVER_NO_RANGE);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_002) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_HASH_MISMATCH);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_003) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_TOO_SHORT);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_004) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_NETWORK_SERVER_DOWN);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_005) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_SERVER_UNREACHABLE);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_006) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_CRASH);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_007) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_SERVER_FAILED);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_008) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_NO_SPACE);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_009) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_ACCESS_DENIED);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_010) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_NAME_TOO_LONG);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_011) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_FILE_TOO_LARGE);
  EXPECT_FALSE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, IsAllowedAutoResume_012) {
  SetLastReason(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_TRUE(IsAllowedAutoResume());
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_001) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  SetAutoResumeCount(1);
  SetPaused(false);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_002) {
  SetCommandLineSwitch(false);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_FALSE(command_line->HasSwitch(switches::kEnableNwebExDownload))
      << "Command line switch not set";

  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  SetAutoResumeCount(0);
  SetPaused(false);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_003) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::CANCELLED_INTERNAL);
  SetAutoResumeCount(0);
  SetPaused(false);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_004) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::IN_PROGRESS_INTERNAL);
  SetAutoResumeCount(0);
  SetPaused(true);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_005) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";
  SetState(DownloadItemImpl::IN_PROGRESS_INTERNAL);
  SetAutoResumeCount(0);
  SetPaused(false);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_USER_SHUTDOWN);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_006) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  SetAutoResumeCount(0);
  SetPaused(false);
  SetAllowedAutoResume(false);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, CheckIsNeedAutoResume_007) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::IN_PROGRESS_INTERNAL);
  SetAutoResumeCount(DownloadItemImpl::kMaxAutoResumeAttempts + 1);
  SetPaused(false);
  SetAllowedAutoResume(true);
  bool result = CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE);
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsNeedAutoResume_001) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";
  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  SetAutoResumeCount(1);
  SetPaused(false);
  SetAllowedAutoResume(true);

  ResumeMode resume_mode = download::ResumeMode::IMMEDIATE_CONTINUE;
  bool need_auto_resume = false;
  IsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE, resume_mode,
                   need_auto_resume);
  EXPECT_TRUE(CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_NONE));
}

TEST_F(ArkWebDownloadItemImplExtTest, IsNeedAutoResume_002) {
  SetCommandLineSwitch(true);
  auto* command_line = base::CommandLine::ForCurrentProcess();
  ASSERT_NE(command_line, nullptr) << "CommandLine is null";
  ASSERT_TRUE(command_line->HasSwitch("enable-nweb-ex-download"))
      << "Command line switch not set";

  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  SetAutoResumeCount(1);
  SetPaused(false);
  SetAllowedAutoResume(true);
  ResumeMode resume_mode = download::ResumeMode::IMMEDIATE_CONTINUE;
  bool need_auto_resume = true;
  IsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_USER_SHUTDOWN, resume_mode,
                   need_auto_resume);
  EXPECT_FALSE(CheckIsNeedAutoResume(DOWNLOAD_INTERRUPT_REASON_USER_SHUTDOWN));
}

TEST_F(ArkWebDownloadItemImplExtTest, IsBeforeInProgress_001) {
  SetState(DownloadItemImpl::INITIAL_INTERNAL);
  bool result = IsBeforeInProgress();
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsBeforeInProgress_002) {
  SetState(DownloadItemImpl::TARGET_PENDING_INTERNAL);
  bool result = IsBeforeInProgress();
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsBeforeInProgress_003) {
  SetState(DownloadItemImpl::TARGET_RESOLVED_INTERNAL);
  bool result = IsBeforeInProgress();
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsBeforeInProgress_004) {
  SetState(DownloadItemImpl::INTERRUPTED_TARGET_PENDING_INTERNAL);
  bool result = IsBeforeInProgress();
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsBeforeInProgress_005) {
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  bool result = IsBeforeInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_001) {
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  bool result = IsDownloadInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_002) {
  SetState(DownloadItemImpl::COMPLETING_INTERNAL);
  bool result = IsDownloadInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_003) {
  SetState(DownloadItemImpl::INTERRUPTED_INTERNAL);
  bool result = IsDownloadInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_004) {
  SetState(DownloadItemImpl::INTERRUPTED_TARGET_PENDING_INTERNAL);
  bool result = IsDownloadInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_005) {
  SetState(DownloadItemImpl::CANCELLED_INTERNAL);
  bool result = IsDownloadInProgress();
  EXPECT_FALSE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, IsDownloadInProgress_006) {
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  bool result = IsDownloadInProgress();
  EXPECT_TRUE(result);
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_001) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  SetDownloadFile(std::move(mock_download_file_));
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_002) {
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));

  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  SetDownloadFile(std::move(mock_download_file_));
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_003) {
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_004) {
  int read_data_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;

  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .WillOnce(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, GetNoHoleDownloadDataSize())
      .WillOnce(testing::Return(1024));
  EXPECT_CALL(mock_ext, ReadDownloadDataAndRunCallback(1024))
      .WillOnce([this, &read_data_callback_called_count]() {
        read_data_callback_called_count++;
        LOG(INFO) << "ReadDownloadDataAndRunCallback called, count: "
                  << read_data_callback_called_count;
        ReadDataFromDownloadFileDone(std::vector<uint8_t>{1, 2, 3});
      });
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  RunCallbackIfStateMatch();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_data_callback_called_count);
  EXPECT_TRUE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_005) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_006) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  SetDownloadFile(std::move(mock_download_file_));
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_007) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfStateMatch_008) {
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));

  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  RunCallbackIfStateMatch();
  EXPECT_EQ(0u, task_environment_.GetPendingMainThreadTaskCount());
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfExistsCallback_001) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  RunCallbackIfExistsCallback();
  EXPECT_FALSE(IsReadDownload());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfExistsCallback_002) {
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));
  RunCallbackIfExistsCallback();
  EXPECT_FALSE(IsDownloadFile());
}

TEST_F(ArkWebDownloadItemImplExtTest, RunCallbackIfExistsCallback_003) {
  int read_data_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;
  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .WillOnce(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, GetNoHoleDownloadDataSize())
      .WillOnce(testing::Return(1024));
  EXPECT_CALL(mock_ext, ReadDownloadDataAndRunCallback(1024))
      .WillOnce([this, &read_data_callback_called_count]() {
        read_data_callback_called_count++;
        LOG(INFO) << "ReadDownloadDataAndRunCallback called, count: "
                  << read_data_callback_called_count;
        ReadDataFromDownloadFileDone(std::vector<uint8_t>{1, 2, 3});
      });
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));
  RunCallbackIfExistsCallback();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_data_callback_called_count);
  EXPECT_TRUE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDataFromDownloadFileDone_001) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  std::vector<uint8_t> test_data = {1, 2, 3, 4, 5};
  ReadDataFromDownloadFileDone(test_data);
  SUCCEED();
  EXPECT_FALSE(IsReadDownload());
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDataFromDownloadFileDone_002) {
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, std::vector<uint8_t>* data,
         const std::vector<uint8_t>& result) {
        *called = true;
        *data = result;
      },
      &callback_called, &received_data));
  std::vector<uint8_t> test_data = {1, 2, 3, 4, 5};
  ReadDataFromDownloadFileDone(test_data);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(received_data, test_data);
  callback_called = false;
  received_data.clear();
  ReadDataFromDownloadFileDone(std::vector<uint8_t>{6, 7, 8});
  EXPECT_FALSE(callback_called);
  EXPECT_TRUE(received_data.empty());
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDataFromDownloadFileDone_003) {
  bool callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &callback_called));
  std::vector<uint8_t> test_data = {1, 2, 3, 4, 5};
  ReadDataFromDownloadFileDone(test_data);
  EXPECT_FALSE(IsReadDownload());
  EXPECT_TRUE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_001) {
  bool existing_callback_called = false;
  SetReadDownloadCallback(base::BindOnce(
      [](bool* called, const std::vector<uint8_t>& data) { *called = true; },
      &existing_callback_called));
  bool new_callback_called = false;
  base::OnceCallback<void(const std::vector<uint8_t>&)> new_callback =
      base::BindOnce([](bool* called,
                        const std::vector<uint8_t>& data) { *called = true; },
                     &new_callback_called);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(new_callback));
  EXPECT_FALSE(new_callback_called);
  EXPECT_FALSE(existing_callback_called);
  EXPECT_TRUE(IsReadDownload());
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_002) {
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetDownloadFile(nullptr);
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  base::OnceCallback<void(const std::vector<uint8_t>&)> callback =
      base::BindOnce(
          [](bool* called, std::vector<uint8_t>* data,
             const std::vector<uint8_t>& result) {
            *called = true;
            *data = result;
          },
          &callback_called, &received_data);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(callback));
  EXPECT_TRUE(callback_called);
  EXPECT_TRUE(received_data.empty());
  EXPECT_FALSE(IsReadDownload());
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_003) {
  int read_data_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;
  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .Times(2)
      .WillRepeatedly(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, RegisterReadDownloadCallback(testing::_, 1024))
      .Times(1);
  EXPECT_CALL(mock_ext, GetNoHoleDownloadDataSize())
      .WillOnce(testing::Return(1024));
  EXPECT_CALL(mock_ext, ReadDownloadDataAndRunCallback(1024))
      .WillOnce([this, &read_data_callback_called_count]() {
        read_data_callback_called_count++;
        LOG(INFO) << "ReadDownloadDataAndRunCallback called, count: "
                  << read_data_callback_called_count;
        ReadDataFromDownloadFileDone(std::vector<uint8_t>{1, 2, 3});
      });
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  base::OnceCallback<void(const std::vector<uint8_t>&)> callback =
      base::BindOnce(
          [](bool* called, std::vector<uint8_t>* data,
             const std::vector<uint8_t>& result) {
            *called = true;
            *data = result;
          },
          &callback_called, &received_data);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(callback));
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_data_callback_called_count);
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_004) {
  int read_and_run_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;
  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .WillOnce(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, RegisterReadDownloadCallback(testing::_, 1024))
      .Times(1);
  EXPECT_CALL(mock_ext, ReadAndRunCallbackIfDataReady())
      .WillOnce([&read_and_run_callback_called_count]() {
        read_and_run_callback_called_count++;
        LOG(INFO) << "ReadAndRunCallbackIfDataReady called, count: "
                  << read_and_run_callback_called_count;
      });
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  base::OnceCallback<void(const std::vector<uint8_t>&)> callback =
      base::BindOnce(
          [](bool* called, std::vector<uint8_t>* data,
             const std::vector<uint8_t>& result) {
            *called = true;
            *data = result;
          },
          &callback_called, &received_data);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(callback));
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_and_run_callback_called_count);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_005) {
  SetReadDownloadCallback(
    base::OnceCallback<void(const std::vector<uint8_t>&)>());
bool new_callback_called = false;
  base::OnceCallback<void(const std::vector<uint8_t>&)> new_callback =
      base::BindOnce([](bool* called,
                        const std::vector<uint8_t>& data) { *called = true; },
                     &new_callback_called);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(new_callback));
  EXPECT_FALSE(IsReadDownload());
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_006) {
  int read_data_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;
  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .Times(2)
      .WillRepeatedly(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, RegisterReadDownloadCallback(testing::_, 1024))
      .Times(1);
  EXPECT_CALL(mock_ext, GetNoHoleDownloadDataSize())
      .WillOnce(testing::Return(1024));
  EXPECT_CALL(mock_ext, ReadDownloadDataAndRunCallback(1024))
      .WillOnce([this, &read_data_callback_called_count]() {
        read_data_callback_called_count++;
        LOG(INFO) << "ReadDownloadDataAndRunCallback called, count: "
                  << read_data_callback_called_count;
        ReadDataFromDownloadFileDone(std::vector<uint8_t>{1, 2, 3});
      });
  SetState(DownloadItemImpl::COMPLETE_INTERNAL);
  SetPercentCompleteTrue();
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  base::OnceCallback<void(const std::vector<uint8_t>&)> callback =
      base::BindOnce(
          [](bool* called, std::vector<uint8_t>* data,
             const std::vector<uint8_t>& result) {
            *called = true;
            *data = result;
          },
          &callback_called, &received_data);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(callback));
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_data_callback_called_count);
}

TEST_F(ArkWebDownloadItemImplExtTest, ReadDownloadData_007) {
  int read_data_callback_called_count = 0;
  MockArkWebDownloadFileImplExt mock_ext;
  EXPECT_CALL(*mock_download_file_, AsArkWebDownloadFileImplExt())
      .Times(2)
      .WillRepeatedly(testing::Return(&mock_ext));
  EXPECT_CALL(mock_ext, RegisterReadDownloadCallback(testing::_, 1024))
      .Times(1);
  EXPECT_CALL(mock_ext, GetNoHoleDownloadDataSize())
      .WillOnce(testing::Return(1024));
  EXPECT_CALL(mock_ext, ReadDownloadDataAndRunCallback(1024))
      .WillOnce([this, &read_data_callback_called_count]() {
        read_data_callback_called_count++;
        LOG(INFO) << "ReadDownloadDataAndRunCallback called, count: "
                  << read_data_callback_called_count;
        ReadDataFromDownloadFileDone(std::vector<uint8_t>{1, 2, 3});
      });
  SetState(DownloadItemImpl::MAX_DOWNLOAD_INTERNAL_STATE);
  SetPercentCompleteTrue();
  SetReadDownloadCallback(
      base::OnceCallback<void(const std::vector<uint8_t>&)>());
  SetDownloadFile(std::move(mock_download_file_));
  bool callback_called = false;
  std::vector<uint8_t> received_data;
  base::OnceCallback<void(const std::vector<uint8_t>&)> callback =
      base::BindOnce(
          [](bool* called, std::vector<uint8_t>* data,
             const std::vector<uint8_t>& result) {
            *called = true;
            *data = result;
          },
          &callback_called, &received_data);
  download_item_->ReadDownloadData("test_guid", 1024, std::move(callback));
  task_environment_.RunUntilIdle();
  EXPECT_EQ(1, read_data_callback_called_count);
}
}  // namespace download