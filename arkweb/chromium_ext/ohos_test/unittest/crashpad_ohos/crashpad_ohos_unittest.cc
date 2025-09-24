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

#include "arkweb/chromium_ext/components/crash/core/app/crashpad_ohos.cc"

#include <gtest/gtest.h>

const std::string PRODUCT_NAME = "ArkwebTestCrashReporterClient";
const std::string VERSION = "1.0";
const std::string TEST_CHANNEL = "test_channel";

class CrashPadOhosTest : public testing ::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class TestCrashReporterClient : public crash_reporter::CrashReporterClient {
public:
    TestCrashReporterClient() = default;
    ~TestCrashReporterClient() = default;
    void GetProductNameAndVersion(std::string* product_name,
                                  std::string* version,
                                  std::string* channel) override {
        *product_name = PRODUCT_NAME;
        *version = VERSION;
        *channel = channel_;
    }
    void SetChannel(const std::string& channel) {
        channel_ = channel;
    }
    void Clear() {
        channel_.clear();
    }
private:
    std::string channel_;
};

namespace crash_reporter {

namespace {

TEST_F(CrashPadOhosTest, ShouldHandleCrashAndUpdateArgumentsTest) {
    bool write_minidump_to_database = true;
    bool write_minidump_to_log = true;
    std::vector<std::string> args;
    bool ret = ShouldHandleCrashAndUpdateArguments(write_minidump_to_database,
                                                   write_minidump_to_log, &args);
    EXPECT_EQ(ret, write_minidump_to_database || write_minidump_to_log);
    EXPECT_EQ(1, args.size());
    EXPECT_EQ("--write-minidump-to-log", args[0]);

    write_minidump_to_database = true;
    write_minidump_to_log = false;
    args.clear();
    ret = ShouldHandleCrashAndUpdateArguments(write_minidump_to_database,
                                                   write_minidump_to_log, &args);
    EXPECT_EQ(ret, write_minidump_to_database || write_minidump_to_log);
    EXPECT_EQ(0, args.size());

    write_minidump_to_database = false;
    write_minidump_to_log = true;
    args.clear();
    ret = ShouldHandleCrashAndUpdateArguments(write_minidump_to_database,
                                                   write_minidump_to_log, &args);
    EXPECT_EQ(ret, write_minidump_to_database || write_minidump_to_log);
    EXPECT_EQ(2, args.size());
    EXPECT_EQ("--no-write-minidump-to-database", args[0]);
    EXPECT_EQ("--write-minidump-to-log", args[1]);

    write_minidump_to_database = false;
    write_minidump_to_log = false;
    args.clear();
    ret = ShouldHandleCrashAndUpdateArguments(write_minidump_to_database,
                                                   write_minidump_to_log, &args);
    EXPECT_EQ(ret, write_minidump_to_database || write_minidump_to_log);
    EXPECT_EQ(1, args.size());
    EXPECT_EQ("--no-write-minidump-to-database", args[0]);
}

TEST_F(CrashPadOhosTest, BuildHandlerArgsTest) {
    TestCrashReporterClient client;
    client.SetChannel("test_channel");

    base::FilePath database_path;
    base::FilePath metrics_path;
    std::string url;
    std::map<std::string, std::string> process_annotations;
    std::vector<std::string> arguments;

    BuildHandlerArgs(&client, &database_path, &metrics_path, &url, &process_annotations, &arguments);
    EXPECT_EQ(process_annotations["prod"], PRODUCT_NAME);
    EXPECT_EQ(process_annotations["ver"], VERSION);
    EXPECT_EQ(process_annotations["channel"], TEST_CHANNEL);

    client.Clear();
    process_annotations.clear();
    BuildHandlerArgs(&client, &database_path, &metrics_path, &url, &process_annotations, &arguments);
    EXPECT_EQ(process_annotations["channel"], "");
}

TEST_F(CrashPadOhosTest, PlatformCrashpadInitializationTest) {
    bool initial_client = true;
    bool browser_process = true; // similar to initial_client
    bool embedded_handler = false; // must be false
    std::string user_data_dir;
    base::FilePath exe_path; // must be empty
    std::vector<std::string> initial_arguments; // must be empty
    base::FilePath database_path;
    bool ret = internal::PlatformCrashpadInitialization(initial_client, browser_process, embedded_handler,
                                              user_data_dir, exe_path, initial_arguments,
                                              &database_path);
    EXPECT_EQ(true, ret);

    initial_client = false;
    browser_process = false; // similar to initial_client
    embedded_handler = false; // must be false
    user_data_dir.clear();
    exe_path.clear(); // must be empty
    initial_arguments.clear(); // must be empty
    database_path.clear();
    ret = internal::PlatformCrashpadInitialization(initial_client, browser_process, embedded_handler,
                                              user_data_dir, exe_path, initial_arguments,
                                              &database_path);
    EXPECT_EQ(true, ret);
}

TEST_F(CrashPadOhosTest, SetLdLibraryPathTest) {
    base::FilePath exe_dir;
    bool ret = SetLdLibraryPath(exe_dir);
    EXPECT_EQ(true, ret);
}
} // namespace
} // namespace crash_reporter