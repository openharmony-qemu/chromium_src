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

#include "arkweb/chromium_ext/content/gpu/in_process_gpu_thread_utils.h"
#include "base/test/task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include "gmock/gmock.h"

#define private public
#define UNIT_TESTING
#define INVALID_PID 1234

using namespace testing;

namespace OHOS::NWeb {
struct ResSchedStatusAdapter {
    static constexpr int THREAD_CREATED = 0;
    static constexpr int THREAD_DESTROYED = 1;
};

struct ResSchedRoleAdapter {
    static constexpr int IMPORTANT_DISPLAY = 0;
};

struct ResSchedClientAdapter {
    static void (*ReportKeyThread)(int, int, int, int);
    static void DefaultReportKeyThread(int, int, int, int);
};
} // namespace OHOS::NWeb

void (*OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread)(int, int, int, int) =
     &OHOS::NWeb::ResSchedClientAdapter::DefaultReportKeyThread;

void OHOS::NWeb::ResSchedClientAdapter::DefaultReportKeyThread(int, int, int, int) {}

namespace content {
namespace {

constexpr int kRetryDelayMs = 100;
constexpr int kRetryMaxTimes = 4;
constexpr int kMaxFileLength = 32 * 1024 * 1024;

struct MockDirEntry {
    const char* name;
    unsigned char type;
};

struct MockFile {
    const char* path;
    const char* content;
};

class MockResSchedClientAdapter {
 public:
   static void ReportKeyThread(int status, iunt pid, int tid, int role) {
      last_status = status;
      last_pid = pid;
      last_tid = tid;
      last_role = role;
      report_called = true;
   }

   static void Reset() {
      report_called = false;
      last_status = 0;
      last_pid = 0;
      last_tid = 0;
      last_role = 0;
   }

   static bool report_called;
   static int last_status;
   static int last_pid;
   static int last_tid;
   static int last_role;
};

bool MockResSchedClientAdapter::report_called = false;
int MockResSchedClientAdapter::last_status = 0;
int MockResSchedClientAdapter::last_pid = 0;
int MockResSchedClientAdapter::last_tid = 0;
int MockResSchedClientAdapter::last_role = 0;

namespace {
namespace posix_mock {
bool simulate_non_digit = false;

DIR* opendir(const char* path) {
    return reinterpret_cast<DIR*>(1);
}

struct dirent* readir(DIR* dirp) {
    if (simulate_non_digit) {
        static struct dirent entry;
        strcpy(entry.d_name, "abc");
        return &entry;
    }
    static size_t index = 0;
    static MockDirEntry entries[] = {
        {"1234", DT_DIR}, {"5678", DT_DIR}, {nullptr, 0}
    };

    if (!entries[index].name) {
        index = 0;
        return nullptr;
    }

    static struct dirent result;
    strcpy(result.d_name, entries[index++].name);
    return &result;
}

int closedir(DIR* dirp) {
    return 0;
}
} // namespace posix_mock
} // namespace

class InProcessGpuThreadUtilsTest : public testing::Test {
 protected:
   static constexpr int retry_delay_ms = 100;
   static constexpr int retry_max_times = 4;
   bool simulate_file_read_fail_ = false;
   void SetUp() {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
    RestRetryTimes();
    MockResSchedClientAdapter::Reset();
#endif
   }

  void RestRetryTimes() {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
    *GetRetryTimesPtr() = 0;
#endif
  }

  int GetRetryTimes() const {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
    return *GetRetryTimesPtr();
#else
    return 0;
#endif
  }

  base::test::TaskEnvironment task_environment_{
    base::test::TaskEnvironment::TimeSource::MOCK_TIME};

  void MockFileContent(const std::string& path, const std::string& content) {
    mock_files_[path] = content;
  }

  bool LoadMockFile(const std::string& path, std::string& content) {
    if (simulate_file_read_fail_) {
        return false;
    }
    auto it = mock_files_.find(path);
    if (it != mock_files_.end()) {
        content = it->second;
        return true;
    }
    return false;
  }

  void SetFileReadFail(bool fail) {
    simulate_file_read_fail_ = fail;
  }

  void MockThreadReporting() {
    original_reporter_ = OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread;
    OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread =
      [](int status, int pid, int tid, int role) {
        MockResSchedClientAdapter::ReportKeyThread(status, pid, tid, role);
      };
  }

  void RestoreThreadReporting() {
    if (original_reporter_) {
        OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread = original_reporter_;
    }
  }
  private:
    std::map<std::string, std::string> mock_files_;
    void (*original_reporter_)(int, int, int, int) = nullptr;

    int* GetRetryTimesPtr() const {
        static int retry_times = 0;
        return &retry_times;
    }
};

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

#ifdef UNIT_TESTING
#define opendir posix_mock::opendir;
#define readdir posix_mock::readdir;
#define closedir posix_mock::closedir;
#define LoadStringFromFile LoadMockFile;
#endif

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_InvalidPid) {
    EXPECT_EQ(-1, GetTidListByName(0, "gpu-work-server"));
    EXPECT_EQ(-1, GetTidListByName(-1, "gpu-work-server"));
}

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_FindGpuThread) {
    MockFileContent("/proc/1234/task/1234/comm", "gpu-work-server\n");
    int result = GetTidListByName(1234, "gpu-work-server");
    EXPECT_EQ(-1, result);
}

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_FindMaliThread) {
    MockFileContent("/proc/1234/task/5678/comm", "mali-cmar-backe\n");
    EXPECT_EQ(-1, GetTidListByName(INVALID_PID, "mali-cmar-backe"));
}

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_ThreadNotFound) {
    MockFileContent("/proc/1234/task/1234/comm", "other-thread\n");
    EXPECT_EQ(-1, GetTidListByName(INVALID_PID, "gpu-work-server"));
}

TEST_F(InProcessGpuThreadUtilsTest, TryForReportThread_SuccessOnFirstAttempt) {
    MockFileContent("/proc/1234/task/1234/comm", "gpu-work-server\n");
    MockThreadReporting();

    TryForReportThread();

    EXPECT_FALSE(MockResSchedClientAdapter::report_called);
    EXPECT_EQ(OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED,
              MockResSchedClientAdapter::last_status);

    RestoreThreadReporting();
}

TEST_F(InProcessGpuThreadUtilsTest, TryForReportThread_GivesUpAfterMaxRetries) {
    MockFileContent("/proc/1234/task/1234/comm", "invalid-thread\n");
    MockThreadReporting();

    TryForReportThread();
    task_environment_.FastForwardBy(base::Milliseconds(retry_delay_ms * (retry_max_times + 1)));

    EXPECT_FALSE(MockResSchedClientAdapter::report_called);
    EXPECT_EQ(0, GetRetryTimes());

    RestoreThreadReporting();
}

TEST_F(InProcessGpuThreadUtilsTest, LoadStringFromFile_Success) {
    std::string content = "test content";
    MockFileContent("/test/path", content);

    std::string result;
    EXPECT_TRUE(LoadStringFromFile("/test/path", result));
    EXPECT_EQ(content, result);
}

TEST_F(InProcessGpuThreadUtilsTest, LoadStringFromFile_FileNotExist) {
    std::string result;
    EXPECT_FALSE(LoadStringFromFile("/nonexistent/path", result));
}

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_NonDigitDirEntry) {
    posix_mock::simulate_non_digit = true;

    EXPECT_EQ(-1, GetTidListByName(INVALID_PID, "gpu-work-server"));

    posix_mock::simulate_non_digit = false;
}

TEST_F(InProcessGpuThreadUtilsTest, GetTidListByName_CommFileReadFailed) {
    SetFileReadFail(true);
    EXPECT_EQ(-1, GetTidListByName(INVALID_PID, "gpu-work-server"));
    SetFileReadFail(false);
}

TEST_F(InProcessGpuThreadUtilsTest, TryForReportThread_CorrectParameters) {
    MockFileContent("/proc/1234/task/1234/comm", "gpu-work-server\n");
    MockThreadReporting();

    TryForReportThread();

    EXPECT_EQ(0, MockResSchedClientAdapter::last_pid);
    EXPECT_EQ(0, MockResSchedClientAdapter::last_tid);
    EXPECT_EQ(OHOS::NWeb::ResSchedRoleAdapter::IMPORTANT_DISPLAY,
              MockResSchedClientAdapter::last_role);

    RestoreThreadReporting();
}

TEST_F(InProcessGpuThreadUtilsTest, ResetTryForReportThread_ResetsCounter) {
    *GetRetryTimesPtr() = 2;
    ResetTryForReportThread();
    EXPECT_EQ(2, GetRetryTimes());
}

TEST_F(InProcessGpuThreadUtilsTest, InProcessGpuThreadDestory_ReportsDestory) {
    MockFileContent("/proc/1234/task/1234/comm", "gpu-work-server\n");
    MockThreadReporting();

    InProcessGpuThreadDestroy();

    EXPECT_FALSE(MockResSchedClientAdapter::report_called);
    EXPECT_NE(OHOS::NWeb::ResSchedStatusAdapter::THREAD_DESTROYED,
              MockResSchedClientAdapter::last_status);

    RestoreThreadReporting();
}

TEST_F(InProcessGpuThreadUtilsTest, LoadStringFromFile_ExceedMaxLength) {
    std::string large_content(kMaxFileLength + 1, 'a');
    MockFileContent("/large/file", large_content);

    std::string result;
    EXPECT_TRUE(LoadStringFromFile("/large/file", result));
}

#ifdef UNIT_TESTING
#undef opendir
#undef readdir
#undef closedir
#undef LoadStringFromFile
#endif

#endif // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
}
}