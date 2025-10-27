/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_rom_value_impl.h"

#include "ohos_nweb/cpptoc/ark_web_rom_value_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebRomValueType = OHOS::NWeb::NWebRomValue::Type;

ArkWebRomValueImpl::ArkWebRomValueImpl(std::shared_ptr<OHOS::NWeb::NWebRomValue> nweb_rom_value)
    : nweb_rom_value_(nweb_rom_value)
{}

unsigned char ArkWebRomValueImpl::GetType()
{
    return static_cast<unsigned char>(nweb_rom_value_->GetType());
}

void ArkWebRomValueImpl::SetType(unsigned char type)
{
    nweb_rom_value_->SetType(static_cast<ArkWebRomValueType>(type));
}

int ArkWebRomValueImpl::GetInt()
{
    return nweb_rom_value_->GetInt();
}

void ArkWebRomValueImpl::SetInt(int value)
{
    nweb_rom_value_->SetInt(value);
}

bool ArkWebRomValueImpl::GetBool()
{
    return nweb_rom_value_->GetBool();
}

void ArkWebRomValueImpl::SetBool(bool value)
{
    nweb_rom_value_->SetBool(value);
}

double ArkWebRomValueImpl::GetDouble()
{
    return nweb_rom_value_->GetDouble();
}

void ArkWebRomValueImpl::SetDouble(double value)
{
    nweb_rom_value_->SetDouble(value);
}

ArkWebString ArkWebRomValueImpl::GetString()
{
    return ArkWebStringClassToStruct(nweb_rom_value_->GetString());
}

void ArkWebRomValueImpl::SetString(const ArkWebString& value)
{
    nweb_rom_value_->SetString(ArkWebStringStructToClass(value));
}

const char* ArkWebRomValueImpl::GetBinary(int& length)
{
    return nweb_rom_value_->GetBinary(length);
}

void ArkWebRomValueImpl::SetBinary(int length, const char* value)
{
    nweb_rom_value_->SetBinary(length, value);
}

ArkWebRomValueMap ArkWebRomValueImpl::GetDictValue()
{
    return ArkWebRomValueMapClassToStruct(nweb_rom_value_->GetDictValue());
}

ArkWebRomValueVector ArkWebRomValueImpl::GetListValue()
{
    return ArkWebRomValueVectorClassToStruct(nweb_rom_value_->GetListValue());
}

ArkWebRefPtr<ArkWebRomValue> ArkWebRomValueImpl::NewChildValue()
{
    std::shared_ptr<OHOS::NWeb::NWebRomValue> nweb_rom_value = nweb_rom_value_->NewChildValue();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_rom_value)) {
        return nullptr;
    }

    return new ArkWebRomValueImpl(nweb_rom_value);
}

void ArkWebRomValueImpl::SaveDictChildValue(const ArkWebString& key)
{
    nweb_rom_value_->SaveDictChildValue(ArkWebStringStructToClass(key));
}

void ArkWebRomValueImpl::SaveListChildValue()
{
    nweb_rom_value_->SaveListChildValue();
}

int64_t ArkWebRomValueImpl::GetInt64()
{
    return nweb_rom_value_->GetInt64();
}

void ArkWebRomValueImpl::SetInt64(int64_t value)
{
    nweb_rom_value_->SetInt64(value);
}

ArkWebUint8Vector ArkWebRomValueImpl::GetBinary()
{
    return ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(nweb_rom_value_->GetBinary());
}

void ArkWebRomValueImpl::SetBinary(const ArkWebUint8Vector& value)
{
    nweb_rom_value_->SetBinary(ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(value));
}

ArkWebBooleanVector ArkWebRomValueImpl::GetBoolArray()
{
    return ArkWebBasicVectorClassToStruct<bool, ArkWebBooleanVector>(nweb_rom_value_->GetBoolArray());
}

void ArkWebRomValueImpl::SetBoolArray(const ArkWebBooleanVector& value)
{
    nweb_rom_value_->SetBoolArray(ArkWebBasicVectorStructToClass<bool, ArkWebBooleanVector>(value));
}

ArkWebInt64Vector ArkWebRomValueImpl::GetInt64Array()
{
    return ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>(nweb_rom_value_->GetInt64Array());
}

void ArkWebRomValueImpl::SetInt64Array(const ArkWebInt64Vector& value)
{
    nweb_rom_value_->SetInt64Array(ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>(value));
}

ArkWebDoubleVector ArkWebRomValueImpl::GetDoubleArray()
{
    return ArkWebBasicVectorClassToStruct<double, ArkWebDoubleVector>(nweb_rom_value_->GetDoubleArray());
}

void ArkWebRomValueImpl::SetDoubleArray(const ArkWebDoubleVector& value)
{
    nweb_rom_value_->SetDoubleArray(ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>(value));
}

ArkWebStringVector ArkWebRomValueImpl::GetStringArray()
{
    return ArkWebStringVectorClassToStruct(nweb_rom_value_->GetStringArray());
}

void ArkWebRomValueImpl::SetStringArray(const ArkWebStringVector& value)
{
    nweb_rom_value_->SetStringArray(ArkWebStringVectorStructToClass(value));
}

ArkWebString ArkWebRomValueImpl::GetErrMsg()
{
    return ArkWebStringClassToStruct(nweb_rom_value_->GetErrMsg());
}

void ArkWebRomValueImpl::SetErrMsg(const ArkWebString& msg)
{
    nweb_rom_value_->SetErrMsg(ArkWebStringStructToClass(msg));
}

ArkWebString ArkWebRomValueImpl::GetErrName()
{
    return ArkWebStringClassToStruct(nweb_rom_value_->GetErrName());
}

void ArkWebRomValueImpl::SetErrName(const ArkWebString& name)
{
    nweb_rom_value_->SetErrName(ArkWebStringStructToClass(name));
}

} // namespace OHOS::ArkWeb
