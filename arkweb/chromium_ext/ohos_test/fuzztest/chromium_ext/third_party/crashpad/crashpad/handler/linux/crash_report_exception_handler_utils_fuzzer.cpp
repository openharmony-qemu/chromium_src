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
#include <memory>
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_utils.h"

#include "third_party/crashpad/crashpad/utils/linux/thread_info.h"
#include "base/files/file_path.h"
#include "base/time/time.h"
#include "base/files/file.h"
#include "base/files/file_util.h"

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
            if (data_size <= data_.size()) {
                std::copy(static_cast<const char*>(data), static_cast<const char*>(data) + data_size, data_.begin());
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

namespace base {

class StringPiece {
public:
    StringPiece(const char* data, size_t size) : data_(data), size_(size) {}
    const char* data() const { return data_; }
    size_t size() const { return size_; }
private:
    const char* data_;
    size_t size_;
};

struct MemoryMappedFile {
    MemoryMappedFile() = default;
};
typedef int PlatformFile;

} // namespace base

namespace crashpad {
namespace FileHelper{
bool ReadDirFiles(const base::FilePath& file_path, std::string* contents) {
    *contents = "mock file content";
    return true;
}
} // namespace FileHelper

// Mock PtraceConnection class for testing
class MockPtraceConnection : public crashpad::PtraceConnection {
public:
    explicit MockPtraceConnection(pid_t pid) : pid_(pid) {}
    
    pid_t GetProcessID() override {
        return pid_;
    }
    
    bool ReadFileContents(const base::FilePath& path, std::string* contents) override {
        *contents = "test maps content";
        return true;
    }

    crashpad::ProcessMemoryLinux* Memory() override {
        return nullptr;
    }

    bool Threads(std::vector<pid_t>* Threads) override {
        return true;
    }
    
    ssize_t ReadUpTo() {
        return 0;
    }

    // 只实现必要的方法，其他方法使用默认实现
    bool Attach(pid_t) override { return true; }
    bool Detach() override { return true; }
    bool Is64Bit() override { return true; }
    bool GetThreadInfo(pid_t, ThreadInfo*) override { return true; }
    bool ReadFileContentsAtOffset(const base::FilePath&, size_t, size_t, std::string*) override { return true; }
    bool ReadMemory(uintptr_t, size_t, void*) override { return true; }
    bool ReadRegisters(crashpad::ThreadInfo*) override { return true; }
    bool ReadThreadStack(pid_t, uintptr_t, size_t, std::string*) override { return true; }
    bool WriteMemory(uintptr_t, size_t, const void*) override { return true; }
    bool GetThreadName(pid_t, std::string*) override { return true; }
    bool IsThreadAlive(pid_t) override { return true; }
    bool GetProcessInfo(crashpad::ProcessInfo*) override { return true; }
    bool ListThreads(std::vector<pid_t>*) override { return true; }
    bool GetModuleInfoList(
        crashpad::ModuleSnapshot::ModuleType,
        std::vector<crashpad::ModuleInfo>*)
        override { return true; }
    bool GetProcessEnvironment(std::vector<std::string>*) override { return true; }
    bool GetProcessArguments(std::vector<std::string>*) override { return true; }
    bool GetProcessExecutable(base::FilePath*) override { return true; }
    bool GetProcessWorkingDirectory(base::FilePath*) override { return true; }
    bool GetThreadContexts(std::map<pid_t, crashpad::NativeCPUContext>*) override { return true; }
    bool GetVmaList(std::vector<crashpad::VMAddressRange>*) override { return true; }
    bool GetFdList(std::vector<crashpad::FileDescriptorInformation>*) override { return true; }
    bool GetFileOffset(uintptr_t, off_t*) override { return true; }
    bool GetFileSize(const base::FilePath&, uint64_t*) override { return true; }
    bool GetFileModificationTime(const base::FilePath&, base::Time*) override { return true; }
    bool GetFilePermissions(const base::FilePath&, base::File::Info*) override { return true; }
    bool GetFileExists(const base::FilePath&, bool*) override { return true; }
    bool GetFileIsDirectory(const base::FilePath&, bool*) override { return true; }
    bool GetFileIsRegularFile(const base::FilePath&, bool*) override { return true; }
    bool GetFileIsSymbolicLink(const base::FilePath&, bool*) override { return true; }
    bool GetFileReadable(const base::FilePath&, bool*) override { return true; }
    bool GetFileWritable(const base::FilePath&, bool*) override { return true; }
    bool GetFileExecutable(const base::FilePath&, bool*) override { return true; }
    bool GetFileHidden(const base::FilePath&, bool*) override { return true; }
    bool GetFileCreationTime(const base::FilePath&, base::Time*) override { return true; }
    bool GetFileLastAccessTime(const base::FilePath&, base::Time*) override { return true; }
    bool GetFileLastModifiedTime(const base::FilePath&, base::Time*) override { return true; }
    bool GetFileSizeByHandle(int, uint64_t*) { return true; }
    bool GetFileSizeByPath(const base::FilePath&, uint64_t*) { return true; }
    bool GetFileSizeByFd(int, uint64_t*) { return true; }
    bool GetFileSizeByFile(int, uint64_t*) { return true; }
    bool GetFileSizeByMappedFile(const base::MemoryMappedFile&, uint64_t*) { return true; }
    bool GetFileSizeByFileStream(FILE*, uint64_t*) { return true; }
    bool GetFileSizeByFileDescriptor(int, uint64_t*) { return true; }
    bool GetFileSizeByFilePath(const base::FilePath&, uint64_t*) { return true; }
    bool GetFileSizeByFileHandle(base::PlatformFile&, uint64_t*) { return true; }

private:
    pid_t pid_;
};

void CrashReportExceptionHandlerUtilsFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test InitExtendedUserStream with valid parameters
    pid_t pid = dataProvider.ConsumeIntegral<pid_t>();
    std::unique_ptr<MockPtraceConnection> connection = std::make_unique<MockPtraceConnection>(pid);
    crashpad::UserStreamDataSources extendedUserStream;
    
    crashpad::CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection.get(), &extendedUserStream);
    
}

void CrashReportExceptionHandlerUtilsFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test InitExtendedUserStream with null extendedUserStream
    pid_t pid = dataProvider.ConsumeIntegral<pid_t>();
    std::unique_ptr<MockPtraceConnection> connection = std::make_unique<MockPtraceConnection>(pid);
    
    crashpad::CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection.get(), nullptr);
    // Should not crash
}

void CrashReportExceptionHandlerUtilsFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    // Test multiple calls
    for (int i = 0; i < 5; i++) {
        pid_t pid = i;
        std::unique_ptr<MockPtraceConnection> connection = std::make_unique<MockPtraceConnection>(pid);
        crashpad::UserStreamDataSources extendedUserStream;
        
        crashpad::CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection.get(), &extendedUserStream);
    }
}

void CrashReportExceptionHandlerUtilsFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);

    // Test with existing streams
    pid_t pid = dataProvider.ConsumeIntegral<pid_t>();
    std::unique_ptr<MockPtraceConnection> connection = std::make_unique<MockPtraceConnection>(pid);
    crashpad::UserStreamDataSources extendedUserStream;
    
    // Add some existing streams
    extendedUserStream.push_back(nullptr);
    
    crashpad::CrashReportExceptionHandlerUtils::InitExtendedUserStream(connection.get(), &extendedUserStream);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Test 1: Basic functionality with valid parameters
    CrashReportExceptionHandlerUtilsFuzz001Test(data, size);
    
    // Test 2: Handling null extendedUserStream parameter
    CrashReportExceptionHandlerUtilsFuzz002Test(data, size);
    
    // Test 3: Multiple consecutive calls
    CrashReportExceptionHandlerUtilsFuzz003Test(data, size);
    
    // Test 4: With existing streams in extendedUserStream
    CrashReportExceptionHandlerUtilsFuzz004Test(data, size);
    
    return 0;
}
}  // namespace crashpad