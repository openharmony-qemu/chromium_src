#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include "binary_writer.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../v8_ohlog.h"

namespace dfx {
BinaryWriter::BinaryWriter() : BinaryWriter(4 * 1024 * 1024) {}
BinaryWriter::BinaryWriter(const uint32_t chunk_size)
    : chunk_size_(chunk_size), chunk_(chunk_size) {}

BinaryWriter::~BinaryWriter() {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return;
  }
  flush();
  ::close(fd_);
  // os_.close();
}

void BinaryWriter::WriteBinBlock(const uint8_t* block, uint32_t block_size) {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return;
  }
  uint32_t cursor = 0;
  while (block_size) {
    MaybeWriteChunk();
    uint32_t dst_size = chunk_size_ - cur_size_;
    uint32_t write_size = std::min(dst_size, block_size);
    std::stringstream ss;
    ss << "cur_size_:" << cur_size_ << " write_size:" << write_size
       << " chunk_size_:" << chunk_size_ << std::endl;
    std::string message = ss.str();
    LogInfo(message);
    memcpy(chunk_.data() + cur_size_, block + cursor, write_size);
    cursor += write_size;
    block_size -= write_size;
    cur_size_ += write_size;
  }
}

void BinaryWriter::WriteChunk() {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return;
  }
  CHECK(cur_size_ == chunk_size_);
  uint8_t* data = chunk_.data();
  // os_.write(reinterpret_cast<char*>(data), cur_size_);
  while (cur_size_) {
    ssize_t r = ::write(fd_, data, cur_size_);
    if (r < 0) {
      std::stringstream ss;
      ss << "HeapDumper Write file failed " << std::endl;
      std::string message = ss.str();
      LogInfo(message);
    }
    data += static_cast<size_t>(r);
    cur_size_ -= static_cast<size_t>(r);
  }
}

void BinaryWriter::OpenFile(std::string path) {
  if (fd_ >= 0) {
    LogInfo("HeapDump already opend");
    return;
  }
  path_ = path;
  fd_ = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd_ < 0) {
    std::stringstream ss;
    ss << "HeapDumper open file failed " << std::endl;
    ss << std::string("open failed for '") + path + "': " + strerror(errno)
       << std::endl;
    std::string message = ss.str();
    LogInfo(message);
  }
}

std::string& BinaryWriter::GetFilePath() {
  return path_;
}

void BinaryWriter::CloseFile() {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return;
  }
  flush();
  ::close(fd_);
}

void BinaryWriter::flush() {
  // write rest
  if (cur_size_) {
    uint8_t* data = chunk_.data();
    while (cur_size_) {
      ssize_t r = ::write(fd_, data, cur_size_);
      // os_.write(reinterpret_cast<char*>(data), cur_size_);
      if (r < 0) {
        std::stringstream ss;
        ss << "HeapDumper Write file failed " << std::endl;
        std::string message = ss.str();
        LogInfo(message);
      }
      data += static_cast<size_t>(r);
      cur_size_ -= static_cast<size_t>(r);
    }
  }
}

}  // namespace dfx

#endif
