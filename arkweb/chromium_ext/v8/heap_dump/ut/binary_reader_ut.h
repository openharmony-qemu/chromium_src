#ifndef BINARY_READER_UT_H
#define BINARY_READER_UT_H
#if defined(OH_ENABLE_HEAP_DUMP)

#include "../binary_reader_base.h"
#include "src/base/logging.h"
namespace dfx {
class BinaryReaderTest : public BinaryReaderBase {
 public:
  BinaryReaderTest(uint8_t* data, uint32_t size) : data_(data), size_(size) {}

  bool ReadData(uint32_t read_size, uint8_t* out) override {
    CHECK(read_size <= size_ - cursor_);
    memcpy(out, data_ + cursor_, read_size);
    cursor_ += read_size;
    return true;
  }

  void ReadDataAt(uint32_t offset, uint32_t read_size, uint8_t* out) override {
    CHECK(offset <= size_ && read_size <= size_ - offset);
    memcpy(out, data_ + offset, read_size);
  }

  uint32_t BinarySize() const override { return size_; }
  uint32_t CurrentPosition() override { return cursor_; }
  void SetPosition(uint32_t pos) override { cursor_ = pos; }

 private:
  const uint8_t* data_;
  const uint32_t size_;
  uint32_t cursor_{0};
};
}  // namespace dfx

#endif
#endif
