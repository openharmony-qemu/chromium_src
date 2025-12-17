/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "arkweb/chromium_ext/third_party/blink/public/common/messaging/string_message_codec_ext.h"

#include <memory>
#include <string>
#include <vector>

#include "base/check_op.h"
#include "base/containers/buffer_iterator.h"
#include "base/containers/span.h"
#include "base/functional/overloaded.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/numerics/checked_math.h"
#include "mojo/public/cpp/base/big_buffer.h"
#include "third_party/blink/public/mojom/array_buffer/array_buffer_contents.mojom.h"
#if BUILDFLAG(ARKWEB_MSGPORT)
#include <climits>
#endif
namespace blink {
namespace {
enum class ErrorTag : uint8_t {
  // The error is a EvalError. No accompanying data.
  kEvalErrorPrototype = 'E',
  // The error is a RangeError. No accompanying data.
  kRangeErrorPrototype = 'R',
  // The error is a ReferenceError. No accompanying data.
  kReferenceErrorPrototype = 'F',
  // The error is a SyntaxError. No accompanying data.
  kSyntaxErrorPrototype = 'S',
  // The error is a TypeError. No accompanying data.
  kTypeErrorPrototype = 'T',
  // The error is a URIError. No accompanying data.
  kUriErrorPrototype = 'U',
  // Followed by message: string.
  kMessage = 'm',
  // Followed by a JS object: cause.
  kCause = 'c',
  // Followed by stack: string.
  kStack = 's',
  // The end of this error information.
  kEnd = '.',
  kOther = 'O',
};
const uint32_t kVarIntShift = 7;
const uint32_t kVarIntMask = (1 << kVarIntShift) - 1;
const uint8_t kVersionTag = 0xFF;
const uint8_t kPaddingTag = '\0';
const uint32_t kErrorVersion = 20;
const uint32_t kLatestVersion = 14;
const uint8_t kOneByteStringTag = '"';
const uint8_t kTwoByteStringTag = 'c';
const uint8_t kArrayBuffer = 'B';
const uint8_t kArrayBufferTransferTag = 't';
const uint8_t kTrue = 'T';
const uint8_t kFalse = 'F';
const uint8_t kDouble = 'N';
const uint8_t kInt32 = 'I';
const uint8_t kUint32 = 'U';
const uint8_t kError = 'r';
const uint8_t kBeginDenseJSArray = 'A';
const uint8_t kEndDenseJSArray = '$';
const uint8_t kOther = 'O';
const double valueZero = 0.0;

void WriteUint8(uint8_t value, std::vector<uint8_t>* buffer) {
  buffer->push_back(value);
}

void WriteUint32(uint32_t value, std::vector<uint8_t>* buffer) {
  for (;;) {
    uint8_t b = (value & kVarIntMask);
    value >>= kVarIntShift;
    if (!value) {
      WriteUint8(b, buffer);
      break;
    }
    WriteUint8(b | (1 << kVarIntShift), buffer);
  }
}

void WriteBytes(base::span<const uint8_t> bytes, std::vector<uint8_t>* buffer) {
  buffer->insert(buffer->end(), bytes.begin(), bytes.end());
}
}

extern void WriteString(const std::string& str, std::vector<uint8_t>* buffer);
extern void WriteHeader(std::vector<uint8_t>* buffer);
extern void WriteU16string(const std::u16string& str, std::vector<uint8_t>* buffer);
extern void WriteBool(const bool& value, std::vector<uint8_t>* buffer);
extern void WriteDouble(const double& value, std::vector<uint8_t>* buffer);
extern void WriteInt64(const int64_t& value, std::vector<uint8_t>* buffer);
extern void WriteError(const std::u16string& name,
                const std::u16string& message,
                std::vector<uint8_t>* buffer);
extern void WriteArrayBuffer(std::vector<uint8_t>& arrbuf,
                      TransferableMessage& message,
                      std::vector<uint8_t>* buffer);
extern void WriteStringArray(std::vector<std::u16string>& arr,
                      std::vector<uint8_t>* buffer);
extern void WriteBoolArray(std::vector<bool>& arr, std::vector<uint8_t>* buffer);
extern void WriteDoubleArray(std::vector<double>& arr, std::vector<uint8_t>* buffer);
extern void WriteInt64Array(std::vector<int64_t>& arr, std::vector<uint8_t>* buffer);
extern TransferableMessage EncodeWebMessagePayload(
    struct WebMessagePort::Message& original);
extern bool ReadOneByteString(base::BufferIterator<const uint8_t>& iter,
                       std::u16string& str);
extern bool ReadTwoByteString(base::BufferIterator<const uint8_t>& iter,
                       std::u16string& str);
extern bool ReadDouble(base::BufferIterator<const uint8_t>& iter, double& result);
extern bool ReadU16string(base::BufferIterator<const uint8_t>& iter,
                   std::u16string& str);
extern bool ReadInt32(base::BufferIterator<const uint8_t>& iter, int32_t& result);
extern bool ReadInt32InArray(base::BufferIterator<const uint8_t>& iter,
                      int32_t& result);
extern bool IsBoolElement(uint8_t element_tag);
extern bool IsStringElement(uint8_t element_tag);
extern bool IsNumberElement(uint8_t element_tag);
extern bool ReadArray(base::BufferIterator<const uint8_t>& iter,
               struct WebMessagePort::Message& decoded_msg);
extern std::u16string GetProtoTypeName(ErrorTag tag);
extern bool ReadError(base::BufferIterator<const uint8_t>& iter,
               struct WebMessagePort::Message& decoded_msg);
extern bool ReadHeaderTag(base::BufferIterator<const uint8_t>& iter, uint8_t& tag);
extern bool DecodeToWebMessagePayload(const TransferableMessage& message,
                               struct WebMessagePort::Message& decoded_msg);

TEST(StringMessageCodecExtTest, WriteString) {
    const std::string str = "test";
    std::vector<uint8_t> buffer;
    WriteString(str, &buffer);
    EXPECT_EQ(buffer[0], kOneByteStringTag);
}

TEST(StringMessageCodecExtTest, WriteHeader) {
    std::vector<uint8_t> buffer;
    WriteHeader(&buffer);
    EXPECT_EQ(buffer[0], kVersionTag);
}

TEST(StringMessageCodecExtTest, WriteU16string) {
    const std::u16string& strTest1 = u"\uFF00";
    const std::u16string& strTest2 = u"\uFFFFFFFF00";
    const std::u16string& strTest3 = u"\u0000";

    std::vector<uint8_t> buffer;
    WriteU16string(strTest1, &buffer);
    buffer.clear();
    WriteU16string(strTest2, &buffer);
    buffer.clear();
    WriteU16string(strTest3, &buffer);
    EXPECT_EQ(buffer[0], kOneByteStringTag);
}

TEST(StringMessageCodecExtTest, WriteBool) {
    const bool valueTrue = true;
    const bool valueFalse = false;
    std::vector<uint8_t> buffer;
    WriteBool(valueTrue, &buffer);
    EXPECT_EQ(buffer[0], kTrue);
    buffer.clear();
    WriteBool(valueFalse, &buffer);
    EXPECT_EQ(buffer[0], kFalse);
}

TEST(StringMessageCodecExtTest, WriteDouble) {
    const double value = 0.0;

    std::vector<uint8_t> buffer;
    WriteDouble(value, &buffer);
    EXPECT_EQ(buffer[0], kDouble);
}

TEST(StringMessageCodecExtTest, WriteInt64) {
    const int64_t valueMin = INT_MIN;
    const int64_t valueMax = INT_MAX;
    const int64_t valueUMax = UINT_MAX;
    const int64_t valueUMaxOver = 0x1FFFFFFFF;

    std::vector<uint8_t> buffer;
    WriteInt64(valueMin, &buffer);
    EXPECT_EQ(buffer[0], kInt32);
    buffer.clear();
    WriteInt64(valueMax, &buffer);
    EXPECT_EQ(buffer[0], kInt32);
    buffer.clear();
    WriteInt64(valueUMax, &buffer);
    EXPECT_EQ(buffer[0], kUint32);
    buffer.clear();
    WriteInt64(valueUMaxOver, &buffer);
    EXPECT_EQ(buffer[0], kDouble);
}

TEST(StringMessageCodecExtTest, WriteError) {
    const std::u16string message = u"message";
    const std::u16string nameEvalError = u"EvalError";
    const std::u16string nameRangeError = u"RangeError";
    const std::u16string nameReferenceError = u"ReferenceError";
    const std::u16string nameSyntaxError = u"SyntaxError";
    const std::u16string nameTypeError = u"TypeError";
    const std::u16string nameURIError = u"URIError";
    const std::u16string nameOtherError = u"OtherError";

    std::vector<uint8_t> buffer;
    WriteError(nameEvalError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
    buffer.clear();
    WriteError(nameRangeError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
    buffer.clear();
    WriteError(nameReferenceError, message, &buffer);
    buffer.clear();
    WriteError(nameSyntaxError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
    buffer.clear();
    WriteError(nameTypeError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
    buffer.clear();
    WriteError(nameURIError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
    buffer.clear();
    WriteError(nameOtherError, message, &buffer);
    EXPECT_EQ(buffer[0], kError);
}

TEST(StringMessageCodecExtTest, WriteArrayBuffer) {
    std::vector<uint8_t> arrBuffer{1};
    TransferableMessage message;

    std::vector<uint8_t> buffer;
    WriteArrayBuffer(arrBuffer, message, &buffer);
    EXPECT_EQ(buffer[0], kArrayBufferTransferTag);
}

TEST(StringMessageCodecExtTest, WriteStringArray) {
    std::vector<std::u16string> arrBuffer{u"test"};

    std::vector<uint8_t> buffer;
    WriteStringArray(arrBuffer, &buffer);
    EXPECT_EQ(buffer[0], kBeginDenseJSArray);
}

TEST(StringMessageCodecExtTest, WriteBoolArray) {
    std::vector<bool> arrBuffer{true};

    std::vector<uint8_t> buffer;
    WriteBoolArray(arrBuffer, &buffer);
    EXPECT_EQ(buffer[0], kBeginDenseJSArray);
}

TEST(StringMessageCodecExtTest, WriteDoubleArray) {
    std::vector<double> arrBuffer{0.0};

    std::vector<uint8_t> buffer;
    WriteDoubleArray(arrBuffer, &buffer);
    EXPECT_EQ(buffer[0], kBeginDenseJSArray);
}

TEST(StringMessageCodecExtTest, WriteInt64Array) {
    std::vector<int64_t> arrBuffer{1};

    std::vector<uint8_t> buffer;
    WriteInt64Array(arrBuffer, &buffer);
    EXPECT_EQ(buffer[0], kBeginDenseJSArray);
}

TEST(StringMessageCodecExtTest, EncodeWebMessagePayload) {
    struct WebMessagePort::Message original;
    original.data = u"test";
    original.bool_value_ = true;
    original.double_value_ = 0.0;
    original.int64_value_ = 1;
    original.string_arr_.push_back(u"t");
    original.bool_arr_.push_back(true);
    original.double_arr_.push_back(0.0);
    original.int64_arr_.push_back(1);
    original.err_name_ = u"err";
    original.err_msg_ = u"msg";
    TransferableMessage messageTemp;

    original.type_ = WebMessagePort::Message::MessageType::STRING;
    auto message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::INTEGER;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::BOOLEAN;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::DOUBLE;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::BINARY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::ERROR;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::STRINGARRAY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::BOOLEANARRAY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::DOUBLEARRAY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::INT64ARRAY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::NONE;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::DICTIONARY;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
    original.type_ = WebMessagePort::Message::MessageType::LIST;
    message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
}

TEST(StringMessageCodecExtTest, ReadOneByteString) {
    std::vector<uint8_t> buffer{1};
    base::BufferIterator<const uint8_t> iter(buffer);
    base::BufferIterator<const uint8_t> iterNull;

    std::u16string str;
    auto result = ReadOneByteString(iter, str);
    EXPECT_EQ(result , true);
    result = ReadOneByteString(iterNull, str);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, ReadTwoByteString) {
    std::vector<uint8_t> buffer{1};
    base::BufferIterator<const uint8_t> iter(buffer);
    base::BufferIterator<const uint8_t> iterNull;

    std::u16string str;
    auto result = ReadTwoByteString(iter, str);
    EXPECT_EQ(result , true);
    result = ReadTwoByteString(iterNull, str);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, ReadDouble) {
    base::BufferIterator<const uint8_t> iterNull;
    double data;
    std::vector<uint8_t> buffer(64);

    base::BufferIterator<const uint8_t> iter(buffer);
    auto result = ReadDouble(iter, data);
    EXPECT_EQ(result , true);
    result = ReadDouble(iterNull, data);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, ReadU16string) {
    base::BufferIterator<const uint8_t> iterNull;
    std::u16string str;
    std::vector<uint8_t> buffer;

    const std::u16string& strTest3 = u"\u0000";
    WriteU16string(strTest3, &buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    auto result = ReadU16string(iter1, str);
    EXPECT_EQ(result , true);
    buffer.clear();
    const std::u16string& strTest1 = u"\uFF00";
    WriteU16string(strTest1, &buffer);
    base::BufferIterator<const uint8_t> iter2(buffer);
    result = ReadU16string(iter2, str);
    EXPECT_EQ(result , true);
    buffer.clear();
    WriteHeader(&buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadU16string(iter3, str);
    EXPECT_EQ(result , false);
    result = ReadU16string(iterNull, str);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, ReadInt32) {
    base::BufferIterator<const uint8_t> iterNull;
    int32_t data;
    std::vector<uint8_t> buffer(4);

    base::BufferIterator<const uint8_t> iter(buffer);
    auto result = ReadInt32(iter, data);
    EXPECT_EQ(result , true);
    buffer.clear();
    result = ReadInt32(iterNull, data);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, ReadInt32InArray) {
    base::BufferIterator<const uint8_t> iterNull;
    int32_t data;
    int64_t valueMin = 1;
    std::vector<uint8_t> buffer;

    WriteInt64(valueMin, &buffer);
    base::BufferIterator<const uint8_t> iter(buffer);
    auto result = ReadInt32InArray(iter, data);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteHeader(&buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    result = ReadInt32InArray(iter1, data);
    EXPECT_EQ(result , false);
    buffer.clear();
    WriteUint8(kTrue, &buffer);
    base::BufferIterator<const uint8_t> iter2(buffer);
    result = ReadInt32InArray(iter2, data);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kEndDenseJSArray , &buffer);
    WriteUint8(kEndDenseJSArray , &buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadInt32InArray(iter3, data);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(kInt32, &buffer);
    WriteUint8(kInt32, &buffer);
    base::BufferIterator<const uint8_t> iter4(buffer);
    result = ReadInt32InArray(iter4, data);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(kTrue, &buffer);
    WriteUint8(kFalse, &buffer);
    base::BufferIterator<const uint8_t> iter6(buffer);
    result = ReadInt32InArray(iter6, data);
    EXPECT_EQ(result, false);
    buffer.clear();
    result = ReadInt32InArray(iterNull, data);
    EXPECT_EQ(result, false);
}

TEST(StringMessageCodecExtTest, IsBoolElement) {
    uint8_t element_tag = kTrue;
    auto result = IsBoolElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kFalse;
    result = IsBoolElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kArrayBuffer;
    result = IsBoolElement(element_tag);
    EXPECT_EQ(result , false);
}

TEST(StringMessageCodecExtTest, IsStringElement) {
    uint8_t element_tag = kOneByteStringTag;
    auto result = IsStringElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kTwoByteStringTag;
    result = IsStringElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kArrayBuffer;
    result = IsStringElement(element_tag);
    EXPECT_EQ(result , false);
}

TEST(StringMessageCodecExtTest, IsNumberElement) {
    uint8_t element_tag = kDouble;
    auto result = IsNumberElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kInt32;
    result = IsNumberElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kUint32;
    result = IsNumberElement(element_tag);
    EXPECT_EQ(result , true);
    element_tag = kArrayBuffer;
    result = IsNumberElement(element_tag);
    EXPECT_EQ(result , false);
}

TEST(StringMessageCodecExtTest, ReadError_001) {
    struct WebMessagePort::Message decoded_msg;
    std::vector<uint8_t> buffer;
    WriteUint8(kError, &buffer);
    base::BufferIterator<const uint8_t> iterError(buffer);
    auto result = ReadError(iterError, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kStack), &buffer);
    base::BufferIterator<const uint8_t> iterStack(buffer);
    result = ReadError(iterStack, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kEvalErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterEval(buffer);
    result = ReadError(iterEval, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kRangeErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterRange(buffer);
    result = ReadError(iterRange, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kReferenceErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterReference(buffer);
    result = ReadError(iterReference, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kSyntaxErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterSyntax(buffer);
    result = ReadError(iterSyntax, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, ReadError_002) {
    struct WebMessagePort::Message decoded_msg;
    std::vector<uint8_t> buffer;

    WriteUint8(static_cast<uint8_t>(ErrorTag::kTypeErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterType(buffer);
    auto result = ReadError(iterType, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kUriErrorPrototype), &buffer);
    base::BufferIterator<const uint8_t> iterUri(buffer);
    result = ReadError(iterUri, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kMessage), &buffer);
    base::BufferIterator<const uint8_t> iterMessage(buffer);
    result = ReadError(iterMessage, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kCause), &buffer);
    base::BufferIterator<const uint8_t> iterkCause(buffer);
    result = ReadError(iterkCause, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kEnd), &buffer);
    base::BufferIterator<const uint8_t> iterkEnd(buffer);
    result = ReadError(iterkEnd, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(static_cast<uint8_t>(ErrorTag::kOther), &buffer);
    base::BufferIterator<const uint8_t> iterkOther(buffer);
    result = ReadError(iterkOther, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, ReadArray_001) {
    std::vector<uint8_t> buffer;
    struct WebMessagePort::Message decoded_msg;
    base::BufferIterator<const uint8_t> iterNull;
    auto result = ReadArray(iterNull, decoded_msg);
    EXPECT_EQ(result, false);

    std::vector<std::u16string> arrBuffer{ u'"' };
    WriteStringArray(arrBuffer, &buffer);
    for (size_t i = 1; i < buffer.size(); i++) {
        buffer[i] = kTrue;
    }
    base::BufferIterator<const uint8_t> iter(buffer);
    result = ReadArray(iter, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteStringArray(arrBuffer, &buffer);
    for (size_t i = 1; i < buffer.size(); i++) {
        buffer[i] = kFalse;
    }
    base::BufferIterator<const uint8_t> iter6(buffer);
    result = ReadArray(iter6, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();

    WriteUint8(kBeginDenseJSArray, &buffer);
    WriteUint8(kOneByteStringTag, &buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    result = ReadArray(iter1, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();

    std::vector<std::u16string> arrBuffer2{ u'"' };
    WriteStringArray(arrBuffer2, &buffer);
    buffer[1] = kTwoByteStringTag;
    base::BufferIterator<const uint8_t> iter2(buffer);
    result = ReadArray(iter2, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();

    WriteUint8(kBeginDenseJSArray, &buffer);
    WriteUint8(kTwoByteStringTag, &buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadArray(iter3, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();

    std::vector<std::u16string> arrBuffer4{ u'"' };
    WriteStringArray(arrBuffer4, &buffer);
    buffer[1] = kTrue;
    base::BufferIterator<const uint8_t> iter4(buffer);
    result = ReadArray(iter4, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();

    std::vector<std::u16string> arrBuffer5{ u'"' };
    WriteStringArray(arrBuffer5, &buffer);
    buffer[1] = kFalse;
    base::BufferIterator<const uint8_t> iter5(buffer);
    result = ReadArray(iter5, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, ReadHeaderTag) {
    std::vector<uint8_t> buffer;
    uint8_t tag;
    base::BufferIterator<const uint8_t> iterNull;
    auto result = ReadHeaderTag(iterNull, tag);
    EXPECT_EQ(result, false);
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(21, &buffer);
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(21, &buffer);
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter(buffer);
    result = ReadHeaderTag(iter, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    result = ReadHeaderTag(iter1, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadHeaderTag(iter3, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    base::BufferIterator<const uint8_t> iter4(buffer);
    result = ReadHeaderTag(iter4, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kTrue, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter5(buffer);
    result = ReadHeaderTag(iter5, tag);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kPaddingTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter6(buffer);
    result = ReadHeaderTag(iter6, tag);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kPaddingTag, &buffer);
    base::BufferIterator<const uint8_t> iter7(buffer);
    result = ReadHeaderTag(iter7, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteUint8(kVersionTag, &buffer);
    WriteUint32(20, &buffer);
    WriteUint8(kVersionTag, &buffer);
    base::BufferIterator<const uint8_t> iter8(buffer);
    result = ReadHeaderTag(iter8, tag);
    EXPECT_EQ(result, false);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, DecodeToWebMessagePayload_001) {
    struct WebMessagePort::Message original;
    original.data = u"test";
    original.bool_value_ = true;
    original.double_value_ = valueZero;
    original.int64_value_ = 1;
    original.string_arr_.push_back(u"t");
    original.bool_arr_.push_back(true);
    original.double_arr_.push_back(valueZero);
    original.int64_arr_.push_back(1);
    original.err_name_ = u"err";
    original.err_msg_ = u"msg";
    uint8_t typeList[] = {
        kOneByteStringTag, kTwoByteStringTag, kArrayBuffer, kTrue, kFalse,
        kDouble, kInt32, kUint32, kError, kBeginDenseJSArray, kOther};

    for(int i = 0; i < static_cast<int>(sizeof(typeList)); i++) {
        struct WebMessagePort::Message decoded_msg;
        original.type_ = static_cast<WebMessagePort::Message::MessageType>(i);
        auto message = EncodeWebMessagePayload(original);
        message.owned_encoded_message[0] = typeList[i];
        TransferableMessage msg;
        msg.owned_encoded_message = message.owned_encoded_message;
        msg.encoded_message = msg.owned_encoded_message;
        auto result = DecodeToWebMessagePayload(msg, decoded_msg);
        if (typeList[i] == kDouble) {
            EXPECT_EQ(result , false);
        } else {
            EXPECT_EQ(result , true);
        }
    }
}

TEST(StringMessageCodecExtTest, DecodeToWebMessagePayload_002) {
    TransferableMessage msg;
    struct WebMessagePort::Message decoded_msg;
    msg.owned_encoded_message.push_back(kOneByteStringTag);
    msg.encoded_message = msg.owned_encoded_message;
    auto result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kTwoByteStringTag);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kArrayBuffer);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    const double value = valueZero;
    std::vector<uint8_t> buffer;
    buffer.push_back(kDouble);
    WriteDouble(value, &buffer);
    msg.owned_encoded_message = buffer;
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , true);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kInt32);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kUint32);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kError);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
    msg.owned_encoded_message.push_back(kBeginDenseJSArray);
    msg.encoded_message = msg.owned_encoded_message;
    result = DecodeToWebMessagePayload(msg, decoded_msg);
    EXPECT_EQ(result , false);
    msg.owned_encoded_message.clear();
}

TEST(StringMessageCodecExtTest, ReadArray_002) {
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> temp;
    struct WebMessagePort::Message decoded_msg;

    WriteUint32(sizeof(int32_t), &buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    auto result = ReadArray(iter1, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    std::string data_latin1("test");
    auto data_latin1_as_bytes = base::as_byte_span(data_latin1);
    WriteUint32(
        2 * sizeof(int32_t) + sizeof(uint8_t) + data_latin1.length(),
        &buffer);
    WriteUint8(kOneByteStringTag, &buffer);
    WriteUint32(data_latin1.size(), &buffer);
    WriteBytes(data_latin1_as_bytes, &buffer);
    base::BufferIterator<const uint8_t> iter2(buffer);
    result = ReadArray(iter2, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    const double value = 0.0;
    WriteDouble(value, &temp);
    WriteUint32(sizeof(int32_t) + temp.size(), &buffer);
    WriteDouble(value, &buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadArray(iter3, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    temp.clear();
    WriteUint32(sizeof(int32_t) + 1, &buffer);
    WriteUint8(kDouble, &buffer);
    base::BufferIterator<const uint8_t> iter6(buffer);
    result = ReadArray(iter6, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    temp.clear();
    WriteUint32(sizeof(int32_t) + 3 * sizeof(uint8_t), &buffer);
    WriteUint8(kInt32, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    base::BufferIterator<const uint8_t> iter4(buffer);
    result = ReadArray(iter4, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    temp.clear();
    WriteUint32(sizeof(int32_t) + 1, &buffer);
    WriteUint8(kInt32, &buffer);
    base::BufferIterator<const uint8_t> iter7(buffer);
    result = ReadArray(iter7, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    WriteInt64(UINT_MAX, &temp);
    WriteUint32(sizeof(int32_t) + temp.size(), &buffer);
    WriteInt64(UINT_MAX, &buffer);
    base::BufferIterator<const uint8_t> iter5(buffer);
    result = ReadArray(iter5, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
    temp.clear();
    WriteUint32(sizeof(int32_t) + 1, &buffer);
    WriteUint8(kUint32, &buffer);
    base::BufferIterator<const uint8_t> iter8(buffer);
    result = ReadArray(iter8, decoded_msg);
    EXPECT_EQ(result, false);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, ReadArray_003) {
    std::vector<uint8_t> buffer;
    struct WebMessagePort::Message decoded_msg;
    WriteUint32(1, &buffer);
    WriteUint8(kInt32, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    base::BufferIterator<const uint8_t> iter1(buffer);
    auto result = ReadArray(iter1, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint32(1, &buffer);
    WriteUint8(kUint32, &buffer);
    WriteUint8(kUint32, &buffer);
    WriteUint8(kUint32, &buffer);
    base::BufferIterator<const uint8_t> iter2(buffer);
    result = ReadArray(iter2, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint32(1, &buffer);
    WriteUint8(kOneByteStringTag, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    base::BufferIterator<const uint8_t> iter3(buffer);
    result = ReadArray(iter3, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    WriteUint32(1, &buffer);
    WriteUint8(kTwoByteStringTag, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    WriteUint8(kEndDenseJSArray, &buffer);
    base::BufferIterator<const uint8_t> iter4(buffer);
    result = ReadArray(iter4, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
    const double value = 0.0;
    WriteUint32(1, &buffer);
    WriteDouble(value, &buffer);
    base::BufferIterator<const uint8_t> iter5(buffer);
    result = ReadArray(iter5, decoded_msg);
    EXPECT_EQ(result, true);
    buffer.clear();
}

TEST(StringMessageCodecExtTest, EncodeWebMessagePayload_001) {
    const int typeOther = 255;
    struct WebMessagePort::Message original;
    original.data = u"test";
    original.bool_value_ = true;
    original.double_value_ = 0.0;
    original.int64_value_ = 1;
    original.string_arr_.push_back(u"t");
    original.bool_arr_.push_back(true);
    original.double_arr_.push_back(0.0);
    original.int64_arr_.push_back(1);
    original.err_name_ = u"err";
    original.err_msg_ = u"msg";
    original.type_ =
        static_cast<WebMessagePort::Message::MessageType>(typeOther);
    auto message = EncodeWebMessagePayload(original);
    EXPECT_EQ(message.owned_encoded_message[0], kVersionTag);
}

TEST(StringMessageCodecExtTest, WriteInt64_001) {
    const int64_t valueMin = INT_MIN;
    const int64_t valueMax = INT_MAX;
    const int64_t valueUMax = UINT_MAX;
    const int64_t valueUMaxOver = 0x1FFFFFFFF;
    std::vector<uint8_t> buffer;
    WriteInt64(valueMin - 1, &buffer);
    EXPECT_EQ(buffer[0], kDouble);
    buffer.clear();
    WriteInt64(valueMax + 1, &buffer);
    EXPECT_EQ(buffer[0], kUint32);
    buffer.clear();
}
}
