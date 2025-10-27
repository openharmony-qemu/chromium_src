// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/javascript/oh_gin_javascript_bridge_value_converter.h"

#include <stddef.h>
#include <stdint.h>

#include <cmath>

#include "base/memory/ptr_util.h"
#include "base/values.h"
#include "gin/array_buffer.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_value.h"
#include "libcef/renderer/javascript/oh_gin_javascript_bridge_object.h"
#include "v8/include/v8-typed-array.h"

namespace NWEB {
OhGinJavascriptBridgeValueConverter::OhGinJavascriptBridgeValueConverter(
    const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher)
    : converter_(content::V8ValueConverter::Create()), dispatcher_(dispatcher) {
  converter_->SetDateAllowed(false);
  converter_->SetRegExpAllowed(false);
  converter_->SetFunctionAllowed(true);
  converter_->SetPromiseAllowed(true);
  converter_->SetStrategy(this);
}

OhGinJavascriptBridgeValueConverter::OhGinJavascriptBridgeValueConverter()
    : converter_(content::V8ValueConverter::Create()) {
  converter_->SetDateAllowed(false);
  converter_->SetRegExpAllowed(false);
  converter_->SetFunctionAllowed(true);
  converter_->SetPromiseAllowed(true);
  converter_->SetStrategy(this);
}

OhGinJavascriptBridgeValueConverter::~OhGinJavascriptBridgeValueConverter() {}

v8::Local<v8::Value> OhGinJavascriptBridgeValueConverter::ToV8Value(
    const base::Value* value,
    v8::Local<v8::Context> context) const {
  return converter_->ToV8Value(*value, context);
}

std::unique_ptr<base::Value> OhGinJavascriptBridgeValueConverter::FromV8Value(
    v8::Local<v8::Value> value,
    v8::Local<v8::Context> context) const {
  return converter_->FromV8Value(value, context);
}

bool OhGinJavascriptBridgeValueConverter::FromV8Object(
    v8::Local<v8::Object> value,
    std::unique_ptr<base::Value>* out,
    v8::Isolate* isolate) {
  OhGinJavascriptBridgeObject* unwrapped;
  v8::HandleScope handle_scope(isolate);
  if (gin::ConvertFromV8(isolate, value, &unwrapped)) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object native object";
    *out =
        OhGinJavascriptBridgeValue::CreateObjectIDValue(unwrapped->object_id());
  } else if (dispatcher_ && dispatcher_->HasH5ObjectMethod(value)) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object "
                  "HasH5ObjectMethod true";
    int h5_object_id = dispatcher_->AddH5Object(value);
    std::vector<std::string> names =
        dispatcher_->GetH5ObjectMethodNames(value, h5_object_id, false);
    std::string str = std::to_string(h5_object_id) + std::string(";");
    for (auto name : names) {
      str += name + std::string(";");
    }
    *out = OhGinJavascriptBridgeValue::CreateH5ObjectIDValueWithMdNames(str);
  } else {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object return false";
    return false;
  }
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object return true";
  return true;
}

bool OhGinJavascriptBridgeValueConverter::FromV8Object(
    v8::Local<v8::Object> value,
    std::unique_ptr<base::Value>* out,
    v8::Isolate* isolate,
    bool is_function,
    bool is_promise) {
  OhGinJavascriptBridgeObject* unwrapped;
  v8::HandleScope handle_scope(isolate);
  if (gin::ConvertFromV8(isolate, value, &unwrapped)) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object native object";
    *out =
        OhGinJavascriptBridgeValue::CreateObjectIDValue(unwrapped->object_id());
  } else {
    if (!dispatcher_) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::FromV8Object dispatcher_ null";
      return false;
    }

    if (is_function) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::FromV8Object function true";
      int h5_function_id = dispatcher_->AddH5Object(value);
      *out =
          OhGinJavascriptBridgeValue::CreateH5FunctionIDValue(h5_function_id);
    } else if (is_promise) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::FromV8Object promise true";
      int h5_object_id = dispatcher_->AddH5Object(value);
      std::vector<std::string> names =
          dispatcher_->GetH5ObjectMethodNames(value, h5_object_id, true);
      std::string str = std::to_string(h5_object_id) + std::string(";");
      for (auto name : names) {
        str += name + std::string(";");
      }
      *out = OhGinJavascriptBridgeValue::CreateH5ObjectIDValueWithMdNames(str);
    } else if (dispatcher_ && dispatcher_->HasH5ObjectMethod(value)) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object "
                    "HasH5ObjectMethod true";
      int h5_object_id = dispatcher_->AddH5Object(value);
      std::vector<std::string> names =
          dispatcher_->GetH5ObjectMethodNames(value, h5_object_id, false);
      std::string str = std::to_string(h5_object_id) + std::string(";");
      for (auto name : names) {
        str += name + std::string(";");
      }
      *out = OhGinJavascriptBridgeValue::CreateH5ObjectIDValueWithMdNames(str);
    } else {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::FromV8Object return false";
      return false;
    }
  }
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::FromV8Object return true";
  return true;
}

namespace {

class TypedArraySerializer {
 public:
  virtual ~TypedArraySerializer() {}
  static std::unique_ptr<TypedArraySerializer> Create(
      v8::Local<v8::TypedArray> typed_array);
  virtual void serializeTo(char* data,
                           size_t data_length,
                           base::Value::List* out) = 0;

 protected:
  TypedArraySerializer() {}
};

template <typename ElementType, typename ListType>
class TypedArraySerializerImpl : public TypedArraySerializer {
 public:
  static std::unique_ptr<TypedArraySerializer> Create(
      v8::Local<v8::TypedArray> typed_array) {
    return base::WrapUnique(
        new TypedArraySerializerImpl<ElementType, ListType>(typed_array));
  }

  TypedArraySerializerImpl(const TypedArraySerializerImpl&) = delete;
  TypedArraySerializerImpl& operator=(const TypedArraySerializerImpl&) = delete;

  void serializeTo(char* data,
                   size_t data_length,
                   base::Value::List* out) override {
    DCHECK_EQ(data_length, typed_array_->Length() * sizeof(ElementType));
    for (ElementType *element = reinterpret_cast<ElementType*>(data),
                     *end = element + typed_array_->Length();
         element != end; ++element) {
      // Serialize the uint32 value as the binary type since base::Value
      // supports only int for the integer type, and the uint8 and the uint16
      // with Base::Value since they fit into int.
      if (std::is_same<ElementType, uint32_t>::value) {
        out->Append(base::Value::FromUniquePtrValue(
            OhGinJavascriptBridgeValue::CreateUInt32Value(*element)));
      } else {
        out->Append(base::Value(ListType(*element)));
      }
    }
  }

 private:
  explicit TypedArraySerializerImpl(v8::Local<v8::TypedArray> typed_array)
      : typed_array_(typed_array) {}

  v8::Local<v8::TypedArray> typed_array_;
};

// static
std::unique_ptr<TypedArraySerializer> TypedArraySerializer::Create(
    v8::Local<v8::TypedArray> typed_array) {
  if (typed_array->IsInt8Array()) {
    return TypedArraySerializerImpl<int8_t, int>::Create(typed_array);
  } else if (typed_array->IsUint8Array() ||
             typed_array->IsUint8ClampedArray()) {
    return TypedArraySerializerImpl<uint8_t, int>::Create(typed_array);
  } else if (typed_array->IsInt16Array()) {
    return TypedArraySerializerImpl<int16_t, int>::Create(typed_array);
  } else if (typed_array->IsUint16Array()) {
    return TypedArraySerializerImpl<uint16_t, int>::Create(typed_array);
  } else if (typed_array->IsInt32Array()) {
    return TypedArraySerializerImpl<int32_t, int>::Create(typed_array);
  } else if (typed_array->IsUint32Array()) {
    return TypedArraySerializerImpl<uint32_t, int>::Create(typed_array);
  } else if (typed_array->IsFloat32Array()) {
    return TypedArraySerializerImpl<float, double>::Create(typed_array);
  } else if (typed_array->IsFloat64Array()) {
    return TypedArraySerializerImpl<double, double>::Create(typed_array);
  }
  NOTREACHED();
}

}  // namespace

bool OhGinJavascriptBridgeValueConverter::FromV8ArrayBuffer(
    v8::Local<v8::Object> value,
    std::unique_ptr<base::Value>* out,
    v8::Isolate* isolate) {
  if (!value->IsTypedArray()) {
    *out = OhGinJavascriptBridgeValue::CreateUndefinedValue();
    return true;
  }

  char* data = NULL;
  size_t data_length = 0;
  gin::ArrayBufferView view;
  if (ConvertFromV8(isolate, value.As<v8::ArrayBufferView>(), &view)) {
    data = reinterpret_cast<char*>(view.bytes());
    data_length = view.num_bytes();
  }
  if (!data) {
    *out = OhGinJavascriptBridgeValue::CreateUndefinedValue();
    return true;
  }

  base::Value::List result;
  std::unique_ptr<TypedArraySerializer> serializer(
      TypedArraySerializer::Create(value.As<v8::TypedArray>()));
  serializer->serializeTo(data, data_length, &result);
  *out = std::make_unique<base::Value>(std::move(result));
  return true;
}

bool OhGinJavascriptBridgeValueConverter::FromV8Number(
    v8::Local<v8::Number> value,
    std::unique_ptr<base::Value>* out) {
  double double_value = value->Value();
  if (std::isfinite(double_value)) {
    return false;
  }
  *out = OhGinJavascriptBridgeValue::CreateNonFiniteValue(double_value);
  return true;
}

bool OhGinJavascriptBridgeValueConverter::FromV8Undefined(
    std::unique_ptr<base::Value>* out) {
  *out = OhGinJavascriptBridgeValue::CreateUndefinedValue();
  return true;
}
}  // namespace NWEB
