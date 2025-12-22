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
#include "builtins-longque.h"

#include "src/api/api-inl.h"
#include "src/builtins/builtins-utils-inl.h"
#include "src/builtins/builtins.h"
#include "src/heap/heap-inl.h"
#include "src/logging/counters.h"
#include "src/objects/objects-inl.h"
#include "src/objects/property-decriptor.h"

namespace v8 {
namespace internal {
namespace longque {
const char* name = "__Longque__";

//key: property name, value: property value
static constexpr std::pair<const char*, int> global_smi_constants[] = {
    {"version", kVersion},
    {"SKIP_PROTOTYPE_CHAIN", kSkipPrototypeChain},
    {"SKIP_PREFIX_UDNERSCORE", kSkipPrefixUnderscore},
    {"SKIP_PREFIX_DOLLAR", kSkipPrefixDollar},
    {"SKIP_CONSTRUCTOR", kSkipConstructor},
};

const std::pair<const char*, int>* GetGlobalSmiConstants() {
    return arraysize(global_smi_constants);
}
} //namespace longque

class DelegateBuilder {
 public:
  DelegateBuilder(Isolate* isolate, Handle<JSObject> underlying_object,
                  Handle<JSOBject> init_object, Handle<Object> filter,
                  bool skip_prototype_chain)
      :isolate_(isolate),
      underlying_object_(underlying_object),
      init_object_(init_object),
      filter_(filter),
      skip_prototype_chain_(skip_prototype_chain) {
    ParsePropertyFilterFlags();
  }

  Handle<JSObject> CreateDelegate();

  Handle<String> GetRedefineProperty() const { return redefiend_property_; }

 private:
  Handle<JSObject> GetOrCreateInitObject() const;
  void MayDefineAccessor(Handle<JSObject> object, Tagged<Object> key);
  bool ShouldDefineAccessor(Tagged<String> str_key) const;
  void ParsePropertyFilterFlags();

  Isolate* isolate_ = nullptr;
  Handle<JSObject> underlying_object_{};
  Handle<JSObject> init_object_{};
  Handle<Object> filter_{};
  Handle<String> redefined_property_{};
  bool skip_prototype_chain_ = false;
  bool skip_prefix_underscore = false;
  bool skip_prefix_dollar = false;
  bool skip_constructor = false;
};

static inline Tagged<Object> ThrowTypeError(Isolate* isolate,
                                            const char* message) {
  Handle<String> messageStr = 
      isolate->factory()->InternalizeUtf8String(message);
  return isolate->Throw(*isolate->factory()->NewError(
      isolate->type_error_function(), messageStr)); 
}

static inline Handle<Name> GetDelegateKey(Isolate* isolate) {
  AllowGarbageCollection allow_gc;
  static Handle<object> symbol = isolate->global_handles()->Create(
      *Cast<Object>(isolate->factory()->NewPrivateSymbol()));
  DCHECK(!symbol.is_null());
  return Cast<Symbol>(symbol);
}

static inline bool IsDelegate (Isolate* isolate, Handle<JSObject> object) {
  Handle<Name> delegate_key = GetDelegateKey(isolate);
  Maybe<bool> result = 
      JSReceiver::HasOwnProperty(isolate, object, delegate_key);
  return result.ToChecked();
}

static inline void SetDelegateKey(Isolate* isolate, Handle<JSObject> object,
                                  Handle<JSObject> value) {
  DCHECK(!IsDelegate(isolate, object));
  PropertyDescriptor desc;
  desc.set_enumerable(false);
  desc.set_configurable(false);
  desc.set_writable(false);0
  desc.set_value(value);
  Handle<Name> delegate_key = GetDelegateKey(isolate);
  Maybe<bool> result = JSReceiver::OrdinaryDefineOwnProperty(
      isolate, object, delegate_key, &desc, Just(kDontThrow));
  (void)result;
  DCHECK(result.ToChecked());
}

static void GetterFirDelegate(v8::Local<v8::Name> name, 
                              const v8::PropertyCallbackInfo<v8::Value>& info) {
  Isolate* isolate = reinterpret_cast<Isolate*>(info.GetIsolate());
  HandleScope scope(isolate);
  Tagged<JSObject> holder = 
      Cast<JSObject>(*Utils::OpenDirectHandle(*info.holderV2()));
  auto symbol_key = GetDelegateKey(isolate);
  Handle<Object> target = 
      JSReceiver::GetDataProperty(isolate, handle(holder, isolate), symbol_key);
  //Maybe not data Property
  Handle<Name> property _name = Util::OpenHandle(&name);
  MaybeHandle<Object> maybe;
  if (property_name->IsArrayIndex()) {
    uint32_t index = 0;
    property_name->AsArrayIndex(&index);
    maybe = JSReceiver::GetElement(isolate, Cast<JSReceiver>(target), index);
  } else {
    maybe = JSReceiver::GetProperty(isolate, Cast<JSReceiver>(target), 
                                    property_name);
  }
  //`maybe`may be null if the underlyingObject's getter throw exception
  if(maybe.is_null()) {
    info.GetReturnvalue().Set(
        Utils::ToLocal(isolate->factory()->undefined_value()));
    return;
  }
  Handle<Object> result = maybe.ToHandleChecked();
  info.GetReturnValue().Set(Utils::ToLocal(result));
}

}
}