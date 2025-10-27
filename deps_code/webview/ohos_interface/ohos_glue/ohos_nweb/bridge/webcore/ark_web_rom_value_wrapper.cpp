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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_rom_value_wrapper.h"

#include "ohos_nweb/ctocpp/ark_web_rom_value_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebRomValueType = OHOS::NWeb::NWebRomValue::Type;

ArkWebRomValueWrapper::ArkWebRomValueWrapper(ArkWebRefPtr<ArkWebRomValue> ark_web_rom_value)
    : ark_web_rom_value_(ark_web_rom_value)
{}

ArkWebRomValueType ArkWebRomValueWrapper::GetType()
{
    return static_cast<ArkWebRomValueType>(ark_web_rom_value_->GetType());
}

void ArkWebRomValueWrapper::SetType(ArkWebRomValueType type)
{
    ark_web_rom_value_->SetType(static_cast<unsigned char>(type));
}

int ArkWebRomValueWrapper::GetInt()
{
    return ark_web_rom_value_->GetInt();
}

void ArkWebRomValueWrapper::SetInt(int value)
{
    ark_web_rom_value_->SetInt(value);
}

bool ArkWebRomValueWrapper::GetBool()
{
    return ark_web_rom_value_->GetBool();
}

void ArkWebRomValueWrapper::SetBool(bool value)
{
    ark_web_rom_value_->SetBool(value);
}

double ArkWebRomValueWrapper::GetDouble()
{
    return ark_web_rom_value_->GetDouble();
}

void ArkWebRomValueWrapper::SetDouble(double value)
{
    ark_web_rom_value_->SetDouble(value);
}

std::string ArkWebRomValueWrapper::GetString()
{
    auto stValue = ark_web_rom_value_->GetString();
    auto value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetString(const std::string& value)
{
    auto stValue = ArkWebStringClassToStruct(value);
    ark_web_rom_value_->SetString(stValue);
    ArkWebStringStructRelease(stValue);
}

const char* ArkWebRomValueWrapper::GetBinary(int& length)
{
    return ark_web_rom_value_->GetBinary(length);
}

void ArkWebRomValueWrapper::SetBinary(int length, const char* value)
{
    ark_web_rom_value_->SetBinary(length, value);
}

std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebRomValue>> ArkWebRomValueWrapper::GetDictValue()
{
    auto stValue = ark_web_rom_value_->GetDictValue();
    auto value = ArkWebRomValueMapStructToClass(stValue);
    ArkWebRomValueMapStructRelease(stValue);
    return value;
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebRomValue>> ArkWebRomValueWrapper::GetListValue()
{
    auto stValue = ark_web_rom_value_->GetListValue();
    auto value = ArkWebRomValueVectorStructToClass(stValue);
    ArkWebRomValueVectorStructRelease(stValue);
    return value;
}

std::shared_ptr<OHOS::NWeb::NWebRomValue> ArkWebRomValueWrapper::NewChildValue()
{
    ArkWebRefPtr<ArkWebRomValue> ark_web_rom_value = ark_web_rom_value_->NewChildValue();
    if (CHECK_REF_PTR_IS_NULL(ark_web_rom_value)) {
        return nullptr;
    }

    return std::make_shared<ArkWebRomValueWrapper>(ark_web_rom_value);
}

void ArkWebRomValueWrapper::SaveDictChildValue(const std::string& key)
{
    auto stKey = ArkWebStringClassToStruct(key);
    ark_web_rom_value_->SaveDictChildValue(stKey);
    ArkWebStringStructRelease(stKey);
}

void ArkWebRomValueWrapper::SaveListChildValue()
{
    ark_web_rom_value_->SaveListChildValue();
}

int64_t ArkWebRomValueWrapper::GetInt64()
{
    return ark_web_rom_value_->GetInt64();
}

void ArkWebRomValueWrapper::SetInt64(int64_t value)
{
    ark_web_rom_value_->SetInt64(value);
}

std::vector<uint8_t> ArkWebRomValueWrapper::GetBinary()
{
    auto stValue = ark_web_rom_value_->GetBinary();
    auto value = ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetBinary(const std::vector<uint8_t>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(value);
    ark_web_rom_value_->SetBinary(stValue);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stValue);
}

std::vector<bool> ArkWebRomValueWrapper::GetBoolArray()
{
    auto stValue = ark_web_rom_value_->GetBoolArray();
    auto value = ArkWebBasicVectorStructToClass<bool, ArkWebBooleanVector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebBooleanVector>(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetBoolArray(const std::vector<bool>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<bool, ArkWebBooleanVector>(value);
    ark_web_rom_value_->SetBoolArray(stValue);
    ArkWebBasicVectorStructRelease<ArkWebBooleanVector>(stValue);
}

std::vector<int64_t> ArkWebRomValueWrapper::GetInt64Array()
{
    auto stValue = ark_web_rom_value_->GetInt64Array();
    auto value = ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebInt64Vector>(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetInt64Array(const std::vector<int64_t>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>(value);
    ark_web_rom_value_->SetInt64Array(stValue);
    ArkWebBasicVectorStructRelease<ArkWebInt64Vector>(stValue);
}

std::vector<double> ArkWebRomValueWrapper::GetDoubleArray()
{
    auto stValue = ark_web_rom_value_->GetDoubleArray();
    auto value = ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebDoubleVector>(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetDoubleArray(const std::vector<double>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<double, ArkWebDoubleVector>(value);
    ark_web_rom_value_->SetDoubleArray(stValue);
    ArkWebBasicVectorStructRelease<ArkWebDoubleVector>(stValue);
}

std::vector<std::string> ArkWebRomValueWrapper::GetStringArray()
{
    auto stValue = ark_web_rom_value_->GetStringArray();
    auto value = ArkWebStringVectorStructToClass(stValue);
    ArkWebStringVectorStructRelease(stValue);
    return value;
}

void ArkWebRomValueWrapper::SetStringArray(const std::vector<std::string>& value)
{
    auto stValue = ArkWebStringVectorClassToStruct(value);
    ark_web_rom_value_->SetStringArray(stValue);
    ArkWebStringVectorStructRelease(stValue);
}

std::string ArkWebRomValueWrapper::GetErrMsg()
{
    auto stMsg = ark_web_rom_value_->GetErrMsg();
    auto msg = ArkWebStringStructToClass(stMsg);
    ArkWebStringStructRelease(stMsg);
    return msg;
}

void ArkWebRomValueWrapper::SetErrMsg(const std::string& msg)
{
    auto stMsg = ArkWebStringClassToStruct(msg);
    ark_web_rom_value_->SetErrMsg(stMsg);
    ArkWebStringStructRelease(stMsg);
}

std::string ArkWebRomValueWrapper::GetErrName()
{
    auto stName = ark_web_rom_value_->GetErrName();
    auto name = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return name;
}

void ArkWebRomValueWrapper::SetErrName(const std::string& name)
{
    auto stName = ArkWebStringClassToStruct(name);
    ark_web_rom_value_->SetErrName(stName);
    ArkWebStringStructRelease(stName);
}

} // namespace OHOS::ArkWeb
