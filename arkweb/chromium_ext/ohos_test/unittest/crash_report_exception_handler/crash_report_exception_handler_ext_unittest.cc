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
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_ext.h"
#include <gtest/gtest.h>
#include "third_party/crashpad/crashpad/util/linux/direct_ptrace_connection.h"
#include "third_party/crashpad/crashpad/snapshot/linux/process_snapshot_linux.h"
#include "third_party/crashpad/crashpad/util/linux/ptrace_client.h"

namespace crashpad {

class OhosDfxDataSourceTest : public testing ::Test {
public: 
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class TestDelegate : public crashpad::MinidumpUserExtensionStreamDataSource::Delegate {
public:
    bool ExtensionStreamDataSourceRead(const void* data, size_t size) override {
        return true;
    }
};

class TestConnection : public DirectPtraceConnection {
public:
    explicit TestConnection(int pid) : pid_(pid) {}
    pid_t GetProcessID() override { return pid_; }
    bool ReadFileContents(const base::FilePath& path, std::string* contents) override {
        *contents = "test maps content";
        return true;
    }
private:
    int pid_;
};

TEST_F(OhosDfxDataSourceTest, OhosDfxDataSourceTest001) {
    const char* test_data = "test_data";
    size_t data_size = strlen(test_data);
    OhosDfxDataSource source(KMinidumpStreamTypeOhosDfxInfo, test_data, data_size);

    TestDelegate delegate{};
    source.ReadStreamData(&delegate);
    source.ReadStreamData(nullptr);
    EXPECT_EQ(data_size, source.StreamDataSize());
}

TEST_F(OhosDfxDataSourceTest, OhosDfxDataSourceTest002) {
    const char* test_data = "";
    size_t data_size = strlen(test_data);
    OhosDfxDataSource source(KMinidumpStreamTypeOhosDfxInfo, test_data, data_size);
    TestDelegate delegate{};
    source.ReadStreamData(&delegate);
    source.ReadStreamData(nullptr);
    EXPECT_EQ(data_size, source.StreamDataSize());
}

TEST_F(OhosDfxDataSourceTest, OhosDfxDataSourceTest003) {
    int test_pid = getpid();
    TestConnection connection(test_pid);
    OhosUserStreamDataSource source(&connection);
    ProcessSnapshotLinux process_snapshot{};
    std::unique_ptr<MinidumpUserExtensionStreamDataSource> stream_data = 
        source.ProduceStreamData(&process_snapshot);
    EXPECT_NE(stream_data, nullptr);
    EXPECT_EQ(KMinidumpStreamTypeOhosDfxInfo, stream_data->stream_type());
}
}//namespace crashpad