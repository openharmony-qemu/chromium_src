#ifndef BINARY_WRITER_UT_H
#define BINARY_WRITER_UT_H

#if defined(OH_ENABLE_HEAP_DUMP)

#include <string>
#include <vector>

#include "../binary_writer_base.h"
#include "src/base/logging.h"

namespace dfx {

class BinaryWriterTest : public BinaryWriterBase {
 public:
  BinaryWriterTest() = default;
  ~BinaryWriterTest() override {}
  void OpenFile(std::string path) override {}
  std::string& GetFilePath() override { return path_; }

  void WriteBinBlock(const uint8_t* block, uint32_t block_size) override {
    data_.insert(data_.end(), block, block + block_size);
  }

  void CloseFile() override {}

  uint8_t* Data() { return data_.data(); }
  uint32_t Size() { return static_cast<uint32_t>(data_.size()); }

 private:
  void MaybeWriteChunk() override {}
  void WriteChunk() override {}

  std::vector<uint8_t> data_;
  std::string path_{""};
};

}  // namespace dfx

#endif  // OH_ENABLE_HEAP_DUMP
#endif  // BINARY_WRITER_UT_H
