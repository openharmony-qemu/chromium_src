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

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_ARKWEB_WEB_MESSAGE_PORT_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_ARKWEB_WEB_MESSAGE_PORT_H_

#if BUILDFLAG(ARKWEB_MSGPORT)
  enum class MessageType : unsigned char {
    NONE = 0,
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    BINARY,
    DICTIONARY,
    LIST,
    ERROR,
    STRINGARRAY,
    BOOLEANARRAY,
    DOUBLEARRAY,
    INT64ARRAY
  };
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
  // Creates a message with the given |array_buffer|.
  explicit Message(std::vector<uint8_t> array_buffer);
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
  // std::vector<uint8_t>: the ArrayBuffer.
  std::vector<uint8_t> array_buffer;
  MessageType type_ = MessageType::NONE;
  bool bool_value_;
  double double_value_;
  int64_t int64_value_;
  std::vector<std::u16string> string_arr_;
  std::vector<bool> bool_arr_;
  std::vector<double> double_arr_;
  std::vector<int64_t> int64_arr_;
  std::u16string err_name_;
  std::u16string err_msg_;
#endif

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_ARKWEB_WEB_MESSAGE_PORT_H_
