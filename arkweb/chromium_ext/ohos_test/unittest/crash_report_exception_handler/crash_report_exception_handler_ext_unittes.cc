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

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <sys/time.h>
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_ext.h"
#include "third_party/crashpad/crashpad/snapshot/process_snapshot.h"
#include "third_party/crashpad/crashpad/util/linux/direct_ptrace_connection.h"
#include "third_party/crashpad/crashpac/minidump/minidump_extensions.h"

using namespace cashpad;

TEST(OhosDfxDataSourceTest, ConstructorAndStreamDataSize) {
    const char* test_data = "test_data";
    size_t data_size = strlen(test_data);
    OhosDfxDataSource source(KMinidumpStreamTypeOhosDfxInfo, test_data, data_size);

    EXPECT_EQ(data_size, source.StreamDataSize());
}




TEST(OhosDfxDataSourceTest, ReadStreamData) {
    const char* test_data = "test_data";
    size_t data_size = strlen(test_data);
    OhosDfxDataSource source(KMinidumpStreamTypeOhosDfxInfo, test_data, data_size);

    class TestDelegate : public crashpad::MinidumpUserExtensionStreamDataSource::Delegate {
    public:
        bool ExtensionStreamDataSourceRead(const void* data, size_t size) override {
            return true;
        }
    }delegate;

    EXPECT_TRUE(source.ReadStreamData(&delegate));
}



TEST(OhosUdrDataSourceTest, ProduceStreamData) {
    class TestProcessSnapshot : public ProcessSnapshot {
    public:
        explicit TestProcessSnapshot(int pid) : pid_(pid) {}
        
        crashpad::ProcessID ProcessID() const override { return pid_; }
        crashpad::ProcessID ParentProcessID() const override { return 0; }

        void processStartTime(timeval* snapshot_time) const override {
            gettimeofday(snapshot_time, nullptr);
        }

        

    }
}