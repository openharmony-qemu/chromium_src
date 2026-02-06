#ifndef BINARY_WRITER_H
#define BINARY_WRITER_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include <string>
#include <vector>

#include "binary_writer_base.h"
#include "src/base/logging.h"

namespace dfx {

class BinaryWriter : public BinaryWriterBase {
 public:
  BinaryWriter();
  BinaryWriter(const uint32_t chunk_size);
  ~BinaryWriter() override;
  void OpenFile(std::string path) override;
  std::string& GetFilePath() override;

  void WriteBinBlock(const uint8_t* block, uint32_t block_size) override;

  void CloseFile() override;

 private:
  void flush();
  void MaybeWriteChunk() override {
    CHECK(cur_size_ <= chunk_size_);
    if (V8_UNLIKELY(cur_size_ == chunk_size_)) {
      WriteChunk();
    }
  }
  void WriteChunk() override;

  const uint32_t chunk_size_;
  uint32_t cur_size_{0};
  std::vector<uint8_t> chunk_;
  int fd_{-1};
  std::string path_;
  // std::ofstream os_;
};

}  // namespace dfx

#endif  // OH_ENABLE_HEAP_DUMP
#endif  // BINARY_WRITER_H
