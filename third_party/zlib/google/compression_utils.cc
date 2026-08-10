// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/zlib/google/compression_utils.h"

#include "base/check_op.h"
#include "base/logging.h"
#include "base/process/memory.h"

#include "third_party/zlib/google/compression_utils_portable.h"

#if BUILDFLAG(IS_OHOS)
#include "securec.h"
#endif

namespace compression {

bool GzipCompress(base::span<const char> input,
                  char* output_buffer,
                  size_t output_buffer_size,
                  size_t* compressed_size,
                  void* (*malloc_fn)(size_t),
                  void (*free_fn)(void*)) {
  static_assert(sizeof(Bytef) == 1, "");

  // uLongf can be larger than size_t.
  uLongf compressed_size_long = static_cast<uLongf>(output_buffer_size);
  if (zlib_internal::GzipCompressHelper(
          reinterpret_cast<Bytef*>(output_buffer), &compressed_size_long,
          reinterpret_cast<const Bytef*>(input.data()),
          static_cast<uLongf>(input.size()), malloc_fn, free_fn) != Z_OK) {
    return false;
  }
  // No overflow, as compressed_size_long <= output.size() which is a size_t.
  *compressed_size = static_cast<size_t>(compressed_size_long);
  return true;
}

bool GzipCompress(base::span<const char> input, std::string* output) {
  return GzipCompress(base::as_bytes(input), output);
}

bool GzipCompress(base::span<const uint8_t> input, std::string* output) {
  // Not using std::vector<> because allocation failures are recoverable,
  // which is hidden by std::vector<>.
  static_assert(sizeof(Bytef) == 1, "");
  const uLongf input_size = static_cast<uLongf>(input.size());

  uLongf compressed_data_size =
      zlib_internal::GzipExpectedCompressedSize(input_size);

  Bytef* compressed_data;
  if (!base::UncheckedMalloc(compressed_data_size,
                             reinterpret_cast<void**>(&compressed_data))) {
    return false;
  }

#if BUILDFLAG(IS_OHOS)
  if (zlib_internal::GzipCompressHelper(
          compressed_data, &compressed_data_size,
          reinterpret_cast<const Bytef*>(input.data()), input_size, nullptr,
          nullptr) != Z_OK) {
    base::UncheckedFree(compressed_data);
    return false;
  }

  // Do not pass UncheckedMalloc-allocated pointers to realloc(), as it may
  // dispatch to a different allocator (e.g. jemalloc) via ELF symbol
  // interposition in dual-so builds, causing a SIGSEGV. Use UncheckedMalloc +
  // memcpy + UncheckedFree instead.
  Bytef* resized_data;
  if (!base::UncheckedMalloc(compressed_data_size,
                             reinterpret_cast<void**>(&resized_data))) {
    LOG(ERROR) << "GzipCompress: UncheckedMalloc failed for resized_data";
    base::UncheckedFree(compressed_data);
    return false;
  }
  memcpy_s(resized_data, compressed_data_size, compressed_data, compressed_data_size);
  base::UncheckedFree(compressed_data);
#else
  if (zlib_internal::GzipCompressHelper(
          compressed_data, &compressed_data_size,
          reinterpret_cast<const Bytef*>(input.data()), input_size, nullptr,
          nullptr) != Z_OK) {
    free(compressed_data);
    return false;
  }

  Bytef* resized_data =
      reinterpret_cast<Bytef*>(realloc(compressed_data, compressed_data_size));
  if (!resized_data) {
    free(compressed_data);
    return false;
  }
#endif
  output->assign(resized_data, resized_data + compressed_data_size);
  DCHECK_EQ(input_size, GetUncompressedSize(*output));

#if BUILDFLAG(IS_OHOS)
  base::UncheckedFree(resized_data);
#else
  free(resized_data);
#endif
  return true;
}

bool GzipUncompress(const std::string& input, std::string* output) {
  std::string uncompressed_output;
  uLongf uncompressed_size = static_cast<uLongf>(GetUncompressedSize(input));
  if (size_t{uncompressed_size} > uncompressed_output.max_size())
    return false;

  uncompressed_output.resize(uncompressed_size);
  if (zlib_internal::GzipUncompressHelper(
          reinterpret_cast<Bytef*>(uncompressed_output.data()),
          &uncompressed_size, reinterpret_cast<const Bytef*>(input.data()),
          static_cast<uLongf>(input.length())) == Z_OK) {
    output->swap(uncompressed_output);
    return true;
  }
  return false;
}

bool GzipUncompress(base::span<const char> input, base::span<char> output) {
  return GzipUncompress(base::as_bytes(input), base::as_writable_bytes(output));
}

bool GzipUncompress(base::span<const uint8_t> input,
                    base::span<uint8_t> output) {
  uLongf uncompressed_size = GetUncompressedSize(input);
  if (uncompressed_size > output.size())
    return false;
  return zlib_internal::GzipUncompressHelper(
             reinterpret_cast<Bytef*>(output.data()), &uncompressed_size,
             reinterpret_cast<const Bytef*>(input.data()),
             static_cast<uLongf>(input.size())) == Z_OK;
}

bool GzipUncompress(base::span<const char> input, std::string* output) {
  return GzipUncompress(base::as_bytes(input), output);
}

bool GzipUncompress(base::span<const uint8_t> input, std::string* output) {
  // Disallow in-place usage, i.e., |input| using |*output| as underlying data.
  DCHECK_NE(reinterpret_cast<const char*>(input.data()), output->data());
  uLongf uncompressed_size = GetUncompressedSize(input);
  output->resize(uncompressed_size);
  return zlib_internal::GzipUncompressHelper(
             reinterpret_cast<Bytef*>(output->data()), &uncompressed_size,
             reinterpret_cast<const Bytef*>(input.data()),
             static_cast<uLongf>(input.size())) == Z_OK;
}

uint32_t GetUncompressedSize(base::span<const char> compressed_data) {
  return GetUncompressedSize(base::as_bytes(compressed_data));
}

uint32_t GetUncompressedSize(base::span<const uint8_t> compressed_data) {
  return zlib_internal::GetGzipUncompressedSize(
      reinterpret_cast<const Bytef*>(compressed_data.data()),
      compressed_data.size());
}

}  // namespace compression
