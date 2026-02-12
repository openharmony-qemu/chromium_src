/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "src/objects/property-descriptor.h"

namespace v8 {
namespace internal {
namespace longque {
const char* name = "__Longque__";

// key: property name, value: property value
static constexpr std::pair<const char*, int> global_smi_constants[] = {
    {"version", kVersion},
    {"SKIP_PROTOTYPE_CHAIN", kSkipPrototypeChain},
    {"SKIP_PREFIX_UNDERSCORE", kSkipPrefixUnderscore},
    {"SKIP_PREFIX_DOLLAR", kSkipPrefixDollar},
    {"SKIP_CONSTRUCTOR", kSkipConstructor},
};

const std::pair<const char*, int>* GetGlobalSmiConstants() {
  return global_smi_constants;
}

size_t GetGlobalSmiConstantsCount() {
  return arraysize(global_smi_constants);
}
} // namespace longque

class DelegateBuilder {
 public:
  DelegateBuilder(Isolate* isolate, Handle<JSObject> underlying_object,
                  Handle<JSObject> init_object, Handle<Object> filter,
                  bool skip_prototype_chain)
      :isolate_(isolate),
      underlying_object_(underlying_object),
      init_object_(init_object),
      filter_(filter),
      skip_prototype_chain_(skip_prototype_chain) {
    ParsePropertyFilterFlags();
  }

  Handle<JSObject> CreateDelegate();

  Handle<String> GetRedefinedProperty() const { return redefined_property_; }

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
  static Handle<Object> symbol = isolate->global_handles()->Create(
      *Cast<Object>(isolate->factory()->NewPrivateSymbol()));
  DCHECK(!symbol.is_null());
  return Cast<Symbol>(symbol);
}

static inline bool IsDelegate(Isolate* isolate, Handle<JSObject> object) {
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
  desc.set_writable(false);
  desc.set_value(value);
  Handle<Name> delegate_key = GetDelegateKey(isolate);
  Maybe<bool> result = JSReceiver::OrdinaryDefineOwnProperty(
      isolate, object, delegate_key, &desc, Just(kDontThrow));
  (void)result;
  DCHECK(result.ToChecked());
}

static void GetterForDelegate(v8::Local<v8::Name> name, 
                              const v8::PropertyCallbackInfo<v8::Value>& info) {
  Isolate* isolate = reinterpret_cast<Isolate*>(info.GetIsolate());
  HandleScope scope(isolate);
  Tagged<JSObject> holder = 
      Cast<JSObject>(*Utils::OpenDirectHandle(*info.HolderV2()));
  auto symbol_key = GetDelegateKey(isolate);
  Handle<Object> target = 
      JSReceiver::GetDataProperty(isolate, handle(holder, isolate), symbol_key);
  //Maybe not data property
  Handle<Name> property_name = Utils::OpenHandle(*name);
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
    info.GetReturnValue().Set(
        Utils::ToLocal(isolate->factory()->undefined_value()));
    return;
  }
  Handle<Object> result = maybe.ToHandleChecked();
  info.GetReturnValue().Set(Utils::ToLocal(result));
}

static void SetterForDelegate(Local<v8::Name> property, Local<v8::Value> value,
                              const PropertyCallbackInfo<v8::Boolean>& info) {
  Isolate* isolate = reinterpret_cast<Isolate*>(info.GetIsolate());
  HandleScope scope(isolate);
  Tagged<JSObject> holder = 
      Cast<JSObject>(*Utils::OpenDirectHandle(*info.HolderV2()));
  auto symbol_key = GetDelegateKey(isolate);
  Handle<Object> target = 
      JSReceiver::GetDataProperty(isolate, handle(holder, isolate), symbol_key);
  AllowGarbageCollection allow_gc;
  // Handle exception if needed
  (void)i::Object::SetPropertyOrElement(isolate, Cast<JSReceiver>(target),
                                        Utils::OpenHandle(*property),
                                        Utils::OpenHandle(*value));
  info.GetReturnValue().Set(v8::True(info.GetIsolate()));
}

void DelegateBuilder::ParsePropertyFilterFlags() {
  if (filter_.is_null() || IsUndefined(*filter_)) {
    return;
  }
  DCHECK(IsSmi(*filter_));
  int flags = Smi::ToInt(*filter_);
  if (flags & longque::kSkipPrefixUnderscore) {
    skip_prefix_underscore = true;
  }
  if (flags & longque::kSkipPrefixDollar) {
    skip_prefix_dollar = true;
  }
  if (flags & longque::kSkipConstructor) {
    skip_constructor = true;
  }
}

inline bool DelegateBuilder::ShouldDefineAccessor(
    Tagged<String> str_key) const {
  Handle<Object> str_handle = handle(str_key, isolate_);
  if (skip_prefix_underscore &&
      str_key->HasOneBytePrefix(base::CStrVector("_"))) {
    return false;
  }
  if (skip_prefix_dollar && str_key->HasOneBytePrefix(base::CStrVector("$"))) {
    return false;
  }
  if (skip_constructor &&
      str_key == ReadOnlyRoots(isolate_).constructor_string()) {
    return false;
  }
  return true;
}

inline void DelegateBuilder::MayDefineAccessor(Handle<JSObject> object,
                                               Tagged<Object> key) {
  DCHECK(!IsSymbol(key));
  Tagged<String> str_key = Cast<String>(key);
  if (!ShouldDefineAccessor(str_key)) {
    return;
  }
  Handle<String> name = handle(str_key, isolate_);
  Handle<AccessorInfo> accessor = Accessors::MakeAccessor(
      isolate_, name, GetterForDelegate, SetterForDelegate);
  MaybeHandle<Object> maybe =
      JSObject::SetAccessor(object, name, accessor, NONE);
  if (maybe.is_null() || IsUndefined(*maybe.ToHandleChecked())) {
    redefined_property_ = name;
    return;
  }                                         
}

static inline Tagged<JSReceiver> NextHolder(Isolate* isolate, Tagged<Map> map) {
  DisallowGarbageCollection no_gc;
  if (map->prototype(isolate) == ReadOnlyRoots(isolate).null_value()) {
    return JSReceiver();  
  }
  return Cast<JSReceiver>(map->prototype(isolate));
}

inline bool IsPrototypeChainContainsNonJSObject(Isolate* isolate,
                                                Handle<JSReceiver> receiver) {
  Handle<JSReceiver> holder = receiver;
  do {
    if (!IsJSObject(*holder)) {
        // For example: JSProxy, WasmObject
      return true;
    }
    holder = handle(NextHolder(isolate, holder->map()), isolate);
  } while (!(*holder).is_null());
  return false;
}

Handle<JSObject> DelegateBuilder::GetOrCreateInitObject() const {
  if (!init_object_.is_null()) {
    return init_object_;
  }
  return isolate_->factory()->NewJSObject(isolate_->object_function());
}

Handle<JSObject> DelegateBuilder::CreateDelegate() {
  Handle<JSObject> result = GetOrCreateInitObject();
  SetDelegateKey(isolate_, result, underlying_object_);
  Handle<JSReceiver> holder = underlying_object_;
  do {
    MaybeHandle<FixedArray> maybe = KeyAccumulator::GetKeys(
        isolate_, holder, KeyCollectionMode::kOwnOnly, ENUMERABLE_STRINGS,
        GetKeysConversion::kConvertToString);
    Handle<FixedArray> prop_names = maybe.ToHandleChecked();
    int length = prop_names->length();
    for (int i = 0; i < length; i++) {
      Handle<Object> key = Handle<Object>(prop_names->get(i), isolate_);
      DCHECK(IsString(*key));
      MayDefineAccessor(result, *key);
    }
    if (skip_prototype_chain_) {
      break;
    }
    holder = handle(NextHolder(isolate_, holder->map()), isolate_);
  } while (!(*holder).is_null());
  return result;
}

BUILTIN(CreateDelegate) {
  HandleScope scope(isolate);
  constexpr int kUnderlyingObjectIndex = 1;
  constexpr int kInitObject = 2;
  constexpr int kPropertyFilterIndex = 3;

  // 1. Check parameter underlyingObject.
  Handle<Object> underlying_object = args.at(kUnderlyingObjectIndex);
  if (!IsJSObject(*underlying_object)) {
    if (!IsJSReceiver(*underlying_object)) {
      return ThrowTypeError(
        isolate, "The 1st parameter (underlyingObject) is not an object");
    }
    return ThrowTypeError(
        isolate, 
        "The 1st parameter (underlyingObject) is an unsupported "
        "exotic object (such as proxy)"); 
  }

  // 2. Check parameter initObject.
  Handle<Object> init_object = args.at(kInitObject);
  Handle<JSObject> real_init_object{};
  if (!IsUndefined(*init_object)) {
    // Check user_defined initObject.
    if (!IsJSObject(*init_object)) {
        if (!IsJSReceiver(*init_object)) {
            return ThrowTypeError(
                isolate, "The 2nd parameter (initObject) is not an object");
        }
      return ThrowTypeError(isolate, 
                            "The 2nd parameter (initObject) is an unsupported "
                            "exotic object (such as proxy)");
    }
    if (IsDelegate(isolate, Cast<JSObject>(init_object))) {
      return ThrowTypeError(
          isolate, 
          "The 2nd parameter (initObject) should not be a delegate object");
    }
    if (!JSObject::IsExtensible(isolate, Cast<JSObject>(init_object))) {
      return ThrowTypeError(isolate, 
                            "The 2nd parameter (initObject) is not extensible");
    }
    real_init_object = Cast<JSObject>(init_object);
  }

  // 3. Check parameter propertyFilterFlags.
  Handle<Object> filter = args.at(kPropertyFilterIndex);
  bool skip_prototype_chain = false;
  if (!IsUndefined(*filter)) {
    // Check user-defined propertyFilterFlags.
    if (!IsSmi(*filter)) {
      return ThrowTypeError(
          isolate, "The 3rd parameter (propertyFilterFlags) is invalid");
    }
    int flags = Smi::ToInt(*filter);
    if ((flags & longque::kSkipPrototypeChain)) {
      skip_prototype_chain = true;
    }
  }

  // 4. Check prototype chain of underlyingObject if needed.
  // If underlyingObject is a non-JSObject or it's prototype chain (if we don't
  // skip prototype chain) contains any non-JSObject, throw a TypeError
  // exception.
  if (!skip_prototype_chain) {
    if (IsPrototypeChainContainsNonJSObject(
            isolate, Cast<JSReceiver>(underlying_object))) {
      return ThrowTypeError(isolate,
                            "The 1st parameter (underlyingObject)'s prototype "
                            "chain contains unsupported "
                            "exotic object (such as proxy)"); 
    }
  }

  DelegateBuilder builder(isolate, Cast<JSObject>(underlying_object),
                          real_init_object, filter, skip_prototype_chain);
  Handle<Object> result = builder.CreateDelegate();
  Handle<String> redefined_property = builder.GetRedefinedProperty();
  if (!redefined_property.is_null()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate,
        NewTypeError(MessageTemplate::kRedefineDisallowed, redefined_property));
  }
  return *result;
}
}  // namespace internal
}  // namespace v8