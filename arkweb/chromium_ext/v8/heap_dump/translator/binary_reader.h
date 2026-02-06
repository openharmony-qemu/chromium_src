#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))
#ifndef BINARY_READER_H
#define BINARY_READER_H
#include <fstream>
#include <iostream>
#include <string>

#include "../binary_reader_base.h"
#include "../dump_format.h"

namespace dfx {
class BinaryReader : public dfx::BinaryReaderBase {
 public:
  BinaryReader(std::string path);
  ~BinaryReader();
  void ReadHeader(dfx::RawHeapHeader& header);
  bool ReadData(uint32_t read_size, uint8_t* out) override;
  void ReadDataAt(uint32_t offset, uint32_t read_size, uint8_t* out) override;

  uint32_t BinarySize() const override;
  uint32_t CurrentPosition() override;
  void SetPosition(uint32_t pos) override;

  bool IsValid() const;

 private:
  std::string path_;
  std::ifstream file_;
  uint32_t file_size_{0};
};
}  // namespace dfx
#endif
#endif