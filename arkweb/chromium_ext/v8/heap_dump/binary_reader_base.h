#ifndef BINARY_READER_BASE_H
#define BINARY_READER_BASE_H

#include <cstdint>
#include <string>

namespace dfx {
class BinaryReaderBase {
 public:
  virtual ~BinaryReaderBase() = default;
  // read data from current pos and then advance pos
  virtual bool ReadData(uint32_t read_size, uint8_t* out) = 0;
  // read data at offset, but not change current pos
  virtual void ReadDataAt(uint32_t offset, uint32_t read_size, uint8_t* out) {}
  virtual uint32_t BinarySize() const = 0;
  virtual uint32_t CurrentPosition() = 0;
  virtual void SetPosition(uint32_t pos) {};
};
}  // namespace dfx
#endif
