// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_value.h"

#include "base/containers/span.h"

namespace {

// The magic value is only used to prevent accidental attempts of reading
// OhGinJavascriptBridgeValue  from a random BinaryValue.
// OhGinJavascriptBridgeValue  is not intended for scenarios where with
// BinaryValues are being used for anything else than holding
// OhGinJavascriptBridgeValue s.  If a need for such scenario ever emerges, the
// best solution would be to extend OhGinJavascriptBridgeValue  to be able to
// wrap raw BinaryValues.
const uint32_t kHeaderMagic = 0xBEEFCAFE;

#pragma pack(push, 4)
struct Header : public base::Pickle::Header {
  uint32_t magic;
  int32_t type;
};
#pragma pack(pop)

}  // namespace

// static
std::unique_ptr<base::Value>
OhGinJavascriptBridgeValue ::CreateUndefinedValue() {
  OhGinJavascriptBridgeValue gin_value(TYPE_UNDEFINED);
  return gin_value.SerializeToBinaryValue();
}

// static
std::unique_ptr<base::Value> OhGinJavascriptBridgeValue ::CreateNonFiniteValue(
    float in_value) {
  OhGinJavascriptBridgeValue gin_value(TYPE_NONFINITE);
  gin_value.pickle_.WriteFloat(in_value);
  return gin_value.SerializeToBinaryValue();
}

// static
std::unique_ptr<base::Value> OhGinJavascriptBridgeValue ::CreateNonFiniteValue(
    double in_value) {
  return CreateNonFiniteValue(static_cast<float>(in_value));
}

// static
std::unique_ptr<base::Value> OhGinJavascriptBridgeValue ::CreateObjectIDValue(
    int32_t in_value) {
  OhGinJavascriptBridgeValue gin_value(TYPE_OBJECT_ID);
  gin_value.pickle_.WriteInt(in_value);
  return gin_value.SerializeToBinaryValue();
}

// static
std::unique_ptr<base::Value>
OhGinJavascriptBridgeValue ::CreateH5FunctionIDValue(int32_t in_value) {
  OhGinJavascriptBridgeValue gin_value(TYPE_H5_FUNCTION_ID);
  gin_value.pickle_.WriteInt(in_value);
  return gin_value.SerializeToBinaryValue();
}

// static
std::unique_ptr<base::Value>
OhGinJavascriptBridgeValue ::CreateH5ObjectIDValueWithMdNames(
    std::string in_value) {
  OhGinJavascriptBridgeValue gin_value(TYPE_H5_OBJECT_ID);
  gin_value.pickle_.WriteData(in_value.c_str(), in_value.size());
  return gin_value.SerializeToBinaryValue();
}

// static
std::unique_ptr<base::Value> OhGinJavascriptBridgeValue ::CreateUInt32Value(
    uint32_t in_value) {
  OhGinJavascriptBridgeValue gin_value(TYPE_UINT32);
  gin_value.pickle_.WriteUInt32(in_value);
  return gin_value.SerializeToBinaryValue();
}

// static
bool OhGinJavascriptBridgeValue ::ContainsOhGinJavascriptBridgeValue(
    const base::Value* value) {
  if (!value->is_blob()) {
    return false;
  }
  if (value->GetBlob().size() < sizeof(Header)) {
    return false;
  }
  base::Pickle pickle =
      base::Pickle::WithUnownedBuffer(base::as_byte_span(value->GetBlob()));
  // Broken binary value: payload or header size is wrong
  if (!pickle.data() ||
      pickle.size() - pickle.payload_size() != sizeof(Header)) {
    return false;
  }
  Header* header = pickle.headerT<Header>();
  return (header->magic == kHeaderMagic && header->type >= TYPE_FIRST_VALUE &&
          header->type < TYPE_LAST_VALUE);
}

// static
std::unique_ptr<const OhGinJavascriptBridgeValue>
OhGinJavascriptBridgeValue ::FromValue(const base::Value* value) {
  return std::unique_ptr<const OhGinJavascriptBridgeValue>(
      value->is_blob() ? new OhGinJavascriptBridgeValue(value) : NULL);
}

OhGinJavascriptBridgeValue ::Type OhGinJavascriptBridgeValue ::GetType() const {
  const Header* header = pickle_.headerT<Header>();
  DCHECK(header->type >= TYPE_FIRST_VALUE && header->type < TYPE_LAST_VALUE);
  return static_cast<Type>(header->type);
}

bool OhGinJavascriptBridgeValue ::IsType(Type type) const {
  return GetType() == type;
}

bool OhGinJavascriptBridgeValue ::GetAsNonFinite(float* out_value) const {
  if (GetType() == TYPE_NONFINITE) {
    base::PickleIterator iter(pickle_);
    return iter.ReadFloat(out_value);
  } else {
    return false;
  }
}

bool OhGinJavascriptBridgeValue ::GetAsObjectID(int32_t* out_object_id) const {
  if (GetType() == TYPE_OBJECT_ID || GetType() == TYPE_H5_FUNCTION_ID) {
    base::PickleIterator iter(pickle_);
    return iter.ReadInt(out_object_id);
  } else {
    return false;
  }
}

bool OhGinJavascriptBridgeValue::GetAsObjectIDWithMdNames(
    std::string* out_object_id) const {
  if (GetType() == TYPE_H5_OBJECT_ID) {
    base::PickleIterator iter(pickle_);
    return iter.ReadString(out_object_id);
  } else {
    return false;
  }
}

bool OhGinJavascriptBridgeValue ::GetAsUInt32(uint32_t* out_value) const {
  if (GetType() == TYPE_UINT32) {
    base::PickleIterator iter(pickle_);
    return iter.ReadUInt32(out_value);
  } else {
    return false;
  }
}

OhGinJavascriptBridgeValue ::OhGinJavascriptBridgeValue(Type type)
    : pickle_(sizeof(Header)) {
  Header* header = pickle_.headerT<Header>();
  header->magic = kHeaderMagic;
  header->type = type;
}

OhGinJavascriptBridgeValue ::OhGinJavascriptBridgeValue(
    const base::Value* value)
    : pickle_(base::Pickle::WithUnownedBuffer(
          base::as_byte_span(value->GetBlob()))) {
  DCHECK(ContainsOhGinJavascriptBridgeValue(value));
}

std::unique_ptr<base::Value>
OhGinJavascriptBridgeValue ::SerializeToBinaryValue() {
  const auto* data = static_cast<const uint8_t*>(pickle_.data());
  return base::Value::ToUniquePtrValue(
      base::Value(base::span<const uint8_t>(data, pickle_.size())));
}
