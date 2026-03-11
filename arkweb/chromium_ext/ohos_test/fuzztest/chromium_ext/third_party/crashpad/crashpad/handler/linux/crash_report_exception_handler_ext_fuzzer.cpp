/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <fuzzer/FuzzedDataProvider.h>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_ext.h"

namespace crashpad {
class ProcessSnapshot {
public:
    virtual ~ProcessSnapshot() = default;
};

class Delegate {
public:
    virtual ~Delegate() = default;
    virtual bool ExtensionStreamDataSourceRead(const void* data, size_t size) = 0;
};

class MockOhosDfxDataSource {
public:
    MockOhosDfxDataSource(uint32_t stream_type, const void* data, size_t data_size) {
        data_.resize(data_size);
        if (data_size > 0 && data != nullptr) {
            size_t copy_size = std::min(data_size, data_.size());
            if (copy_size > 0) {
                std::copy(static_cast<const char*>(data), static_cast<const char*>(data) + copy_size, data_.begin());
            }
        }
    }

    size_t StreamDataSize(){
        return data_.size();
    }

    bool ReadStreamData(Delegate* delegate){
        if (delegate == nullptr) {
            return false;
        }
        return delegate->ExtensionStreamDataSourceRead(
            data_.size() ? data_.data() : nullptr, data_.size());
    }

private:
    std::vector<char> data_;

};

class MockOhosUserStreamDataSource {
public:
    MockOhosUserStreamDataSource() = default;

    std::unique_ptr<MockOhosDfxDataSource> ProduceDataSource(ProcessSnapshot* process_snapshot) {
        std::string contents = "mock process maps content";
        return std::make_unique<MockOhosDfxDataSource>(
            0x2413, contents.c_str(), contents.size());
    }
};
} // crashpad

namespace crashpad {
namespace FileHelper {
    bool ReadFileContents(const base::FilePath& path, std::string* contents) {
        *contents = "mock file content";
        return true;
    }
} // namespace FileHelper

// Mock PtraceConnection class for testing
class MockPtraceConnection {
public:
    explicit MockPtraceConnection(pid_t pid) : pid_(pid) {}
    
    pid_t GetProcessID() {
        return pid_;
    }
    
    bool ReadFileContents(const base::FilePath& path, std::string* contents) {
        *contents = "test maps content";
        return true;
    }
    
private:
    pid_t pid_;
};

class MockDelegate : public Delegate {
public:
    bool ExtensionStreamDataSourceRead(const void* data, size_t size) override {
        return true;
    }
};

class MockProcessSnapshot : public ProcessSnapshot {
public:
    MockProcessSnapshot() = default;
};

} // namespace crashpad

void CrashReportExceptionHandlerExtFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test OhosDfxDataSource
    uint32_t stream_type = dataProvider.ConsumeIntegral<uint32_t>();
    size_t data_size = dataProvider.ConsumeIntegralInRange<size_t>(0, 1024);
    std::vector<uint8_t> test_data(data_size);
    if (data_size > 0 && data != nullptr) {
        size_t copy_size = std::min(data_size, size);
        if (copy_size > 0) {
            std::copy(data, data + copy_size, test_data.begin());
        }
    }
    
    crashpad::MockOhosDfxDataSource data_source(stream_type, test_data.data(), data_size);
    
    // Test StreamDataSize
    size_t size_result = data_source.StreamDataSize();
    
    // Test ReadStreamData (with null delegate)
    data_source.ReadStreamData(nullptr);
    crashpad::MockDelegate delegate;
    data_source.ReadStreamData(&delegate);
}

void CrashReportExceptionHandlerExtFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test OhosUserStreamDataSource
    pid_t pid = dataProvider.ConsumeIntegral<pid_t>();
    std::unique_ptr<crashpad::MockPtraceConnection> connection = std::make_unique<crashpad::MockPtraceConnection>(pid);
    
    crashpad::MockOhosUserStreamDataSource user_stream_source;
    
    // Test ProduceDataSource (with null process_snapshot)
    auto stream_data = user_stream_source.ProduceDataSource(nullptr);
}

void CrashReportExceptionHandlerExtFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    
    FuzzedDataProvider dataProvider(data, size);

    uint32_t stream_type = dataProvider.ConsumeIntegral<uint32_t>();
    size_t data_size = dataProvider.ConsumeIntegralInRange<size_t>(0, 1024);
    std::vector<uint8_t> test_data(data_size);
    pid_t pid = dataProvider.ConsumeIntegral<pid_t>();
    // Test multiple instances
    for (int i = 0; i < 5; i++) {
        // Create OhosDfxDataSource
        crashpad::MockOhosDfxDataSource data_source(stream_type, test_data.data(), data_size);
        // Create OhosUserStreamDataSource
        std::unique_ptr<crashpad::MockPtraceConnection> connection =
          std::make_unique<crashpad::MockPtraceConnection>(pid);
    }
}

void CrashReportExceptionHandlerExtFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // 测试空数据源
    crashpad::MockOhosDfxDataSource empty_data_source(0,nullptr,0);
    size_t empty_size = empty_data_source.StreamDataSize();
    empty_data_source.ReadStreamData(nullptr);

    // 测试小尺寸
    size_t small_data_size = dataProvider.ConsumeIntegralInRange<size_t>(1, 64);
    std::vector<char> small_data(small_data_size);
    if (small_data_size > 0 && data != nullptr && small_data_size <= size) {
        // 添加边界检查
        size_t safe_small_copy_size = std::min(small_data_size, size);
        if (safe_small_copy_size > 0) {
            std::copy(data, data + safe_small_copy_size, small_data.begin());
        }
    }
    crashpad::MockOhosDfxDataSource small_data_source(0x1234, small_data.data(), small_data_size);
    size_t small_size = small_data_source.StreamDataSize();
    small_data_source.ReadStreamData(nullptr);

    // 测试大数据
    size_t large_data_size = dataProvider.ConsumeIntegralInRange<size_t>(1024, 4096);
    std::vector<char> large_data(large_data_size);
    if (large_data_size > 0 && data != nullptr && large_data_size <= size) {
        // 添加边界检查
        size_t safe_large_copy_size = std::min(large_data_size, size);
        if (safe_large_copy_size > 0) {
            std::copy(data, data + safe_large_copy_size, large_data.begin());    
        }
    }
    crashpad::MockOhosDfxDataSource large_data_source(0x5678, large_data.data(), large_data_size);
    size_t large_size = large_data_source.StreamDataSize();
    large_data_source.ReadStreamData(nullptr);

    // 测试超大尺寸数据
    size_t huge_data_size = dataProvider.ConsumeIntegralInRange<size_t>(8192, 16384);
    std::vector<char> huge_data(huge_data_size);
    if (huge_data_size > 0 && data != nullptr && huge_data_size <= size) {
        size_t safe_huge_copy_size = std::min(huge_data_size, size);
        if (safe_huge_copy_size > 0) {
            std::copy(data, data + safe_huge_copy_size, huge_data.begin());
        }
    }
    crashpad::MockOhosDfxDataSource huge_data_source(0x9012, huge_data.data(), huge_data_size);
    size_t huge_size = huge_data_source.StreamDataSize();
    huge_data_source.ReadStreamData(nullptr);
}

void CrashReportExceptionHandlerExtFuzz005Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    size_t test_data_size = dataProvider.ConsumeIntegralInRange<size_t>(0, 2048);
    std::vector<char> test_data(test_data_size);
    if (test_data_size > 0 && data != nullptr && test_data_size <= size) {
        size_t safe_test_copy_size = std::min(test_data_size, size);
        if (safe_test_copy_size > 0) {
            std::copy(data, data + safe_test_copy_size, test_data.begin());
        }
    }

    crashpad::MockOhosDfxDataSource data_source(0x9ABC, test_data.data(), test_data_size);

    crashpad::MockDelegate mock_delegate;
    data_source.ReadStreamData(&mock_delegate);

    data_source.ReadStreamData(nullptr);

    size_t data_size = data_source.StreamDataSize();

    uint32_t stream_type = dataProvider.ConsumeIntegral<uint32_t>();
    crashpad::MockOhosDfxDataSource data_source2(stream_type, test_data.data(), test_data_size);
    size_t data_size2 = data_source2.StreamDataSize();
    data_source2.ReadStreamData(nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Test 1: OhosDfxDataSource functionality
    CrashReportExceptionHandlerExtFuzz001Test(data, size);
    
    // Test 2: OhosUserStreamDataSource functionality
    CrashReportExceptionHandlerExtFuzz002Test(data, size);
    
    // Test 3: Multiple instances creation
    CrashReportExceptionHandlerExtFuzz003Test(data, size);
    
    // Test 4: CrashReportExceptionHandlerExt functionality
    CrashReportExceptionHandlerExtFuzz004Test(data, size);

    // Test 5: CrashReportExceptionHandlerExt functionality
    CrashReportExceptionHandlerExtFuzz005Test(data, size);
    
    return 0;
}