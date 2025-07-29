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
#include "third_party/crashpad/crashpad/minidump/minidump_user_extension_stream_data_source.h"
#include "third_party/crashpad/crashpad/snapshot/process_snapshot.h"
#include "third_party/crashpad/crashpad/util/linux/direct_ptrace_connection.h"
#include "third_party/crashpad/crashpad/minidump/minidump_extensions.h"

using namespace crashpad;

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

TEST(OhosUserStreamSourceTest, ProduceStreamData) {
    class TestProcessSnapshot : public ProcessSnapshot {
    public:
        explicit TestProcessSnapshot(int pid) : pid_(pid) {}
        
        crashpad::ProcessID ProcessID() const override { return pid_; }
        crashpad::ProcessID ParentProcessID() const override { return 0; }
        void SnapshotTime(timeval* snapshot_time) const override {
            gettimeofday(snapshot_time, nullptr);
        }
        void ProcessStartTime(timeval* start_time) const override {
            *start_time = {};
        }
        void ProcessCPUTimes(timeval* user_time, timeval* system_time) const override {
            *user_time = {};
            *system_time = {};
        }
        void ReportID(UUID* client_id) const override {
            *client_id = UUID();
        }       
        void ClientID(UUID* client_id) const override {
            *client_id = UUID();
        }

        const std::map<std::string, std::string>& AnnotationsSimpleMap() const override{
            static std::map<std::string, std::string> empty_map;
            return empty_map;
        }       
        const SystemSnapshot* System() const override {
            return nullptr;
        }
        std::vector<const ModuleSnapshot*> Modules() const override {
            return {};
        }
        std::vector<UnloadedModuleSnapshot> UnloadedModules() const override {
            return {};
        }
        std::vector<const ThreadSnapshot*> Threads() const override {
            return {};
        }
        const ExceptionSnapshot* Exception() const override {
            return nullptr;
        }
        std::vector<const MemoryMapRegionSnapshot*> MemoryMap() const override{
            return {};
        }
        std::vector<HandleSnapshot> Handles() const override {
            return {};
        }
        std::vector<const MemorySnapshot*> ExtraMemory() const override {
            return {};
        }
        const ProcessMemory* Memory() const override {
            return nullptr;
        }

    private:
        int pid_;
    };

    class TestConnection : public DirectPtraceConnection {
    public:
        TestConnection(int pid) : pid_(pid) {}
        pid_t GetProcessID() override { return pid_; }

        bool ReadFileContents(const base::FilePath& path, std::string* contents) override {
            *contents = "test maps content";
            return true;
        }
    private:
        int pid_;
    };

    int test_pid = 1234;
    TestConnection connection(test_pid);
    OhosUserStreamDataSource source(&connection);

    TestProcessSnapshot process_snapshot(test_pid);
    std::unique_ptr<MinidumpUserExtensionStreamDataSource> stream_data = 
        source.ProduceStreamData(&process_snapshot);

    EXPECT_NE(stream_data, nullptr);
    EXPECT_EQ(KMinidumpStreamTypeOhosDfxInfo, stream_data->stream_type());
}