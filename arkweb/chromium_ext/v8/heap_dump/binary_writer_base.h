#ifndef BINARY_WRITER_BASE_H
#define BINARY_WRITER_BASE_H
#include <string>
namespace dfx {
// After OpenFile, all data write to the path
// todo(hh): if more than one isolate oom?
class BinaryWriterBase {
 public:
  virtual ~BinaryWriterBase() = default;
  virtual void OpenFile(std::string path) = 0;
  virtual std::string& GetFilePath() = 0;
  virtual void WriteBinBlock(const uint8_t* block, uint32_t block_size) = 0;
  virtual void CloseFile() = 0;

 protected:
  virtual void MaybeWriteChunk() = 0;
  virtual void WriteChunk() = 0;
};

}  // namespace dfx

#endif
