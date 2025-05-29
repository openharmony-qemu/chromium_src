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

namespace content {

bool V8ValueConverter::Strategy::FromV8Object(v8::Local<v8::Object> value,
                                              std::unique_ptr<base::Value>* out,
                                              v8::Isolate* isolate,
                                              bool is_function,
                                              bool is_promise) {
  return false;
}

void V8ValueConverterImpl::SetPromiseAllowed(bool val) {
  promise_allowed_ = val;
}

std::unique_ptr<base::Value> V8ValueConverterImpl::FromV8Object(v8::Local<v8::Object> val,
                                                                FromV8ValueState* state,
                                                                v8::Isolate* isolate,
                                                                bool is_function,
                                                                bool is_promise) const {
  ScopedUniquenessGuard uniqueness_guard(state, val);
  if (!uniqueness_guard.is_valid()) {
    return std::make_unique<base::Value>();
  }

  std::unique_ptr<v8::Context::Scope> scope;
  // If val was created in a different context than our current one, change to
  // that context, but change back after val is converted.
  v8::Local<v8::Context> creation_context;
  if (val->GetCreationContext().ToLocal(&creation_context) &&
      creation_context != isolate->GetCurrentContext()) {
    scope = std::make_unique<v8::Context::Scope>(creation_context);
  }

  if (strategy_) {
    std::unique_ptr<base::Value> out;
    LOG(DEBUG) << "FromV8Object is_function = " << is_function
               << ", is_promise = " << is_promise;
    if (strategy_->FromV8Object(val, &out, isolate, is_function, is_promise)) {
      return out;
    }
  }

  // Don't consider DOM objects. This check matches isHostObject() in Blink's
  // bindings/v8/V8Binding.h used in structured cloning. It reads:
  //
  // If the object has any internal fields, then we won't be able to serialize
  // or deserialize them; conveniently, this is also a quick way to detect DOM
  // wrapper objects, because the mechanism for these relies on data stored in
  // these fields.
  //
  // NOTE: check this after |strategy_| so that callers have a chance to
  // do something else, such as convert to the node's name rather than NULL.
  //
  // ANOTHER NOTE: returning an empty dictionary here to minimise surprise.
  // See also http://crbug.com/330559.
  base::Value::Dict result;

  if (val->InternalFieldCount()) {
    return std::make_unique<base::Value>(std::move(result));
  }

  v8::Local<v8::Array> property_names;
  if (!val->GetOwnPropertyNames(isolate->GetCurrentContext())
           .ToLocal(&property_names)) {
    return std::make_unique<base::Value>(std::move(result));
  }

  for (uint32_t i = 0; i < property_names->Length(); ++i) {
    v8::Local<v8::Value> key =
        property_names->Get(isolate->GetCurrentContext(), i).ToLocalChecked();

    // Extend this test to cover more types as necessary and if sensible.
    if (!key->IsString() && !key->IsNumber()) {
      NOTREACHED() << "Key \"" << *v8::String::Utf8Value(isolate, key)
                   << "\" "
                      "is neither a string nor a number";
      continue;
    }

    v8::String::Utf8Value name_utf8(isolate, key);

    v8::TryCatch try_catch(isolate);
    v8::Local<v8::Value> child_v8;
    v8::MaybeLocal<v8::Value> maybe_child =
        val->Get(isolate->GetCurrentContext(), key);
    if (try_catch.HasCaught() || !maybe_child.ToLocal(&child_v8)) {
      LOG(WARNING) << "Getter for property " << *name_utf8
                   << " threw an exception.";
      child_v8 = v8::Null(isolate);
    }

    std::unique_ptr<base::Value> child =
        FromV8ValueImpl(state, child_v8, isolate);
    if (!child) {
      // JSON.stringify skips properties whose values don't serialize, for
      // example undefined and functions. Emulate that behavior.
      continue;
    }

    // Strip null if asked (and since undefined is turned into null, undefined
    // too). The use case for supporting this is JSON-schema support,
    // specifically for extensions, where "optional" JSON properties may be
    // represented as null, yet due to buggy legacy code elsewhere isn't
    // treated as such (potentially causing crashes). For example, the
    // "tabs.create" function takes an object as its first argument with an
    // optional "windowId" property.
    //
    // Given just
    //
    //   tabs.create({})
    //
    // this will work as expected on code that only checks for the existence of
    // a "windowId" property (such as that legacy code). However given
    //
    //   tabs.create({windowId: null})
    //
    // there *is* a "windowId" property, but since it should be an int, code
    // on the browser which doesn't additionally check for null will fail.
    // We can avoid all bugs related to this by stripping null.
    if (strip_null_from_objects_ && child->is_none()) {
      continue;
    }

    result.Set(std::string(*name_utf8, name_utf8.length()),
               base::Value::FromUniquePtrValue(std::move(child)));
  }

  return std::make_unique<base::Value>(std::move(result));
}

}  // namespace content
