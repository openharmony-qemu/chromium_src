#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))
#include "binary_reader.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstdint>
#include <vector>

#include "../../v8_ohlog.h"
#include "../dump_format.h"
#include "src/base/logging.h"
namespace dfx {
BinaryReader::BinaryReader(std::string path)
    : path_(path), file_(path, std::ios::binary | std::ios::in) {
  if (file_.is_open()) {
    file_.seekg(0, std::ios::end);
    file_size_ = file_.tellg();
    file_.seekg(0, std::ios::beg);
  } else {
    std::string errorMsg = "Failed to open file: " + path;
    LogInfo(errorMsg);
  }
}

bool BinaryReader::IsValid() const {
  return file_.is_open();
}

uint32_t BinaryReader::BinarySize() const {
  return file_size_;
}

void BinaryReader::ReadHeader(dfx::RawHeapHeader& header) {
  bool ret =
      ReadData(sizeof(dfx::RawHeapHeader), reinterpret_cast<uint8_t*>(&header));
  if (!ret) {
    LogInfo("Fail to read header info!");
  }
}

bool BinaryReader::ReadData(uint32_t read_size, uint8_t* out) {
  if (!IsValid() || !file_.good()) {
    return false;
  }
  std::streampos offset = file_.tellg();
  if (offset < 0 || read_size > file_size_ - offset) {
    return false;
  }

  std::streamsize remaining = file_size_ - offset;
  if (remaining < static_cast<std::streamsize>(read_size)) {
    return false;
  }

  file_.read(reinterpret_cast<char*>(out), read_size);
  if (file_.gcount() != static_cast<std::streamsize>(read_size)) {
    return false;
  }
  return true;
}

void BinaryReader::ReadDataAt(uint32_t offset,
                              uint32_t read_size,
                              uint8_t* out) {
  CHECK(IsValid() && file_.good());

  // record
  std::streampos cur_pos = file_.tellg();

  CHECK(
      !(cur_pos < 0 || offset > file_size_ || read_size > file_size_ - offset));
  file_.seekg(offset, std::ios::beg);

  file_.read(reinterpret_cast<char*>(out), read_size);
  CHECK(file_.gcount() == static_cast<std::streamsize>(read_size));
  // restore
  file_.seekg(cur_pos, std::ios::beg);
  return true;
}

uint32_t BinaryReader::CurrentPosition() {
  return file_.tellg();
}

void BinaryReader::SetPosition(uint32_t pos) {
  file_.seekg(pos, std::ios::beg);
}

BinaryReader::~BinaryReader() {
  if (IsValid()) {
    file_.close();
  }
}
}  // namespace dfx
#endif