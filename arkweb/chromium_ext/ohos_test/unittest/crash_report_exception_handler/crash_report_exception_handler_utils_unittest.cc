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
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_utils.h"

#include <vector>
#include <gtest/gtest.h>

#include "third_party/crashpad/crashpad/util/linux/direct_ptrace_connection.h"

#include "third_party/crashpad/crashpad/handler/user_stream_data_source.h"

namespace crashpad {

class UserStreamDataSourceTest : public UserStreamDataSource {
public:
    explicit UserStreamDataSourceTest(PtraceConnection* connection):
    connection_(connection) {}
 
    UserStreamDataSourceTest(const UserStreamDataSourceTest&) = delete;
    UserStreamDataSourceTest& operator=(const UserStreamDataSourceTest&) = delete;
 
    std::unique_ptr<MinidumpUserExtensionStreamDataSource> ProduceStreamData(
    ProcessSnapshot* process_snapshot) override;
 
private:
    raw_ptr<PtraceConnection> connection_;
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

class CrashReportExceptionHandlerUtilsTest : public testing ::Test {
public: 
    void SetUp() {}
    void TearDown() {}
};

TEST_F(CrashReportExceptionHandlerUtilsTest, TestInitExtendedUserStream) {
    TestConnection* connection = new TestConnection(1234);
    UserStreamDataSources extendedUserStream;
    EXPECT_EQ(extendedUserStream.size(), 0);
    CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection, &extendedUserStream);
    EXPECT_EQ(extendedUserStream.size(), 1);
    delete connection;
}

TEST_F(CrashReportExceptionHandlerUtilsTest, TestInitExtendedUserStreamWithNull) {
    TestConnection* connection = new TestConnection(1234);
    CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection, nullptr);
    delete connection;
}
}//namespace crashpad
