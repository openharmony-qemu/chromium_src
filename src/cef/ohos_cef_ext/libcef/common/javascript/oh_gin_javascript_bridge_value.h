// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_VALUE_H
#define OH_GIN_JAVASCRIPT_BRIDGE_VALUE_H

#include <stdint.h>

#include <memory>

#include "base/pickle.h"
#include "base/values.h"

// In Javascript Bridge, we need to pass some kinds of values that can't
// be put into base::Value. And since base::Value is not extensible,
// we transfer these special values via base::Value.

class OhGinJavascriptBridgeValue {
 public:
  enum Type {
    TYPE_FIRST_VALUE = 0,
    // JavaScript 'undefined'
    TYPE_UNDEFINED = 0,
    // JavaScript NaN and Infinity
    TYPE_NONFINITE,
    // Bridge Object ID
    TYPE_OBJECT_ID,
    // Bridge H5 Object ID
    TYPE_H5_OBJECT_ID,
    // Bridge H5 Function ID
    TYPE_H5_FUNCTION_ID,
    // Uint32 type
    TYPE_UINT32,
    TYPE_LAST_VALUE
  };

  OhGinJavascriptBridgeValue(const OhGinJavascriptBridgeValue&) = delete;
  OhGinJavascriptBridgeValue& operator=(const OhGinJavascriptBridgeValue&) =
      delete;

  // Serialization
  static std::unique_ptr<base::Value> CreateUndefinedValue();
  static std::unique_ptr<base::Value> CreateNonFiniteValue(float in_value);
  static std::unique_ptr<base::Value> CreateNonFiniteValue(double in_value);
  static std::unique_ptr<base::Value> CreateObjectIDValue(int32_t in_value);
  static std::unique_ptr<base::Value> CreateH5FunctionIDValue(int32_t in_value);
  static std::unique_ptr<base::Value> CreateH5ObjectIDValueWithMdNames(
      std::string in_value);
  static std::unique_ptr<base::Value> CreateUInt32Value(uint32_t in_value);

  // De-serialization
  static bool ContainsOhGinJavascriptBridgeValue(const base::Value* value);
  static std::unique_ptr<const OhGinJavascriptBridgeValue> FromValue(
      const base::Value* value);

  Type GetType() const;
  bool IsType(Type type) const;

  bool GetAsNonFinite(float* out_value) const;
  bool GetAsObjectID(int32_t* out_object_id) const;
  bool GetAsObjectIDWithMdNames(std::string* out_object_id) const;
  bool GetAsUInt32(uint32_t* out_value) const;

 private:
  explicit OhGinJavascriptBridgeValue(Type type);
  explicit OhGinJavascriptBridgeValue(const base::Value* value);
  std::unique_ptr<base::Value> SerializeToBinaryValue();

  base::Pickle pickle_;
};

#endif  // OH_GIN_JAVASCRIPT_BRIDGE_VALUE_H
