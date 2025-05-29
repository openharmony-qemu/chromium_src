// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/fido/ohos/ohos_webauthn_api.h"

#include "base/native_library.h"
#include "base/no_destructor.h"
#include "device/fido/attestation_statement_formats.h"
#include "device/fido/ohos/type_conversions.h"

#include "device/fido/ohos/fido2_api_utils.h"

namespace device {

class OhosWebAuthnApiImpl : public OhosWebAuthnApi {
 public:
  OhosWebAuthnApiImpl();
  virtual ~OhosWebAuthnApiImpl();

  bool IsAvailable() override;

  GetClientCapabilitiesResult GetClientCapabilities() override;

  RegisterResult Register(
      CtapMakeCredentialRequest request,
      MakeCredentialOptions request_options) override;

  GetAssertionResult GetAssertion(
      CtapGetAssertionRequest request,
      CtapGetAssertionOptions options) override;

 private:
  bool is_available_ = false;

  using FuncGetClientCapabilityPtr =
      FIDO2_ErrorCode (*)(FIDO2_CapabilityArray** capability);
  using FuncGetPlatformAuthenticatorPtr =
      FIDO2_ErrorCode (*)(FIDO2_AuthenticatorMetadataArray** authenticators);
  using FuncRegisterPtr =
      FIDO2_ErrorCode (*)(const FIDO2_CredentialCreationOptions options,
        const FIDO2_TokenBinding tokenBinding,
        const char* origin,
        FIDO2_PublicKeyAttestationCredential** publicKeyAttestationCredential);
  using FuncAuthenticatePtr =
      FIDO2_ErrorCode (*)(const FIDO2_CredentialRequestOptions options,
        const FIDO2_TokenBinding tokenBinding,
        const char* origin,
        FIDO2_PublicKeyAssertionCredential** publicKeyAssertionCredential);
  using FuncCapabilityArrayDestroyPtr =
      void (*)(FIDO2_CapabilityArray *capability);
  using FuncAuthenticatorMetadataArrayDestroyPtr =
      void (*)(FIDO2_AuthenticatorMetadataArray *authenticators);
  using FuncPublicKeyAttestationCredentialDestroyPtr =
      void (*)(FIDO2_PublicKeyAttestationCredential *publicKeyAttestationCredential);
  using FuncPublicKeyAssertionCredentialDestroyPtr =
      void (*)(FIDO2_PublicKeyAssertionCredential *publicKeyAssertionCredential);
  using FuncInitCreationOptionsPtr =
      void (*)(FIDO2_CredentialCreationOptions *options);
  using FuncInitTokenBindingPtr =
      void (*)(FIDO2_TokenBinding *tokenBinding);
  using FuncInitRequestOptionsPtr =
      void (*)(FIDO2_CredentialRequestOptions *options);

#define FUNCTION_LIST(action) \
  action(FuncGetClientCapabilityPtr, \
         func_get_client_capability_ptr_, \
         HMS_FIDO2_getClientCapability) \
  action(FuncGetPlatformAuthenticatorPtr, \
         func_get_platform_authenticator_ptr_, \
         HMS_FIDO2_getPlatformAuthenticator) \
  action(FuncRegisterPtr, \
         func_register_ptr_, \
         HMS_FIDO2_register) \
  action(FuncAuthenticatePtr, \
         func_authenticate_ptr_, \
         HMS_FIDO2_authenticate) \
  action(FuncCapabilityArrayDestroyPtr, \
         func_capability_array_destroy_ptr_, \
         HMS_FIDO2_CapabilityArray_Destroy) \
  action(FuncAuthenticatorMetadataArrayDestroyPtr, \
         func_authenticator_metadata_array_destroy_ptr_, \
         HMS_FIDO2_AuthenticatorMetadataArray_Destroy) \
  action(FuncPublicKeyAttestationCredentialDestroyPtr, \
         func_public_key_attestation_credential_destroy_ptr_, \
         HMS_FIDO2_PublicKeyAttestationCredential_Destroy) \
  action(FuncPublicKeyAssertionCredentialDestroyPtr, \
         func_public_key_assertion_credential_destroy_ptr_, \
         HMS_FIDO2_PublicKeyAssertionCredential_Destroy) \
  action(FuncInitCreationOptionsPtr, \
         func_init_creation_options_ptr_, \
         HMS_FIDO2_initCreationOptions) \
  action(FuncInitTokenBindingPtr, \
         func_init_token_binding_ptr_, \
         HMS_FIDO2_initTokenBinding) \
  action(FuncInitRequestOptionsPtr, \
         func_init_request_options_ptr_, \
         HMS_FIDO2_initRequestOptions)

#define DECLARE_MEMBER(type, member_name, func_name) \
  type member_name = nullptr;

  FUNCTION_LIST(DECLARE_MEMBER)
#undef DECLARE_MEMBER
};

// static
OhosWebAuthnApi* OhosWebAuthnApi::Instance()
{
  static base::NoDestructor<OhosWebAuthnApiImpl> api;
  return api.get();
}

OhosWebAuthnApi::~OhosWebAuthnApi() = default;

bool OhosWebAuthnApi::IsAvailable()
{
  return false;
}

OhosWebAuthnApi::GetClientCapabilitiesResult OhosWebAuthnApi::GetClientCapabilities()
{
  return {};
}

OhosWebAuthnApi::RegisterResult OhosWebAuthnApi::Register(
    CtapMakeCredentialRequest request,
    MakeCredentialOptions request_options)
{
  return {MakeCredentialStatus::kAuthenticatorResponseInvalid, std::nullopt};
}

OhosWebAuthnApi::GetAssertionResult OhosWebAuthnApi::GetAssertion(
    CtapGetAssertionRequest request,
    CtapGetAssertionOptions options)
{
  return {GetAssertionStatus::kAuthenticatorResponseInvalid, std::nullopt};
}

OhosWebAuthnApiImpl::OhosWebAuthnApiImpl()
{
  base::FilePath path("libfido2_ndk.z.so");
  base::NativeLibraryLoadError error;
  base::NativeLibrary library = base::LoadNativeLibrary(path, &error);
  if (!library) {
    is_available_ = false;
    LOG(WARNING) << "load fido api failed, error:" << error.ToString();
    return;
  }
  is_available_ = true;

#define ASSIGN_FUNC_PTR_VALUE(type, member_name, func_name) \
  member_name = reinterpret_cast<type>( \
      base::GetFunctionPointerFromNativeLibrary(library, #func_name));

  FUNCTION_LIST(ASSIGN_FUNC_PTR_VALUE)
#undef ASSIGN_FUNC_PTR_VALUE

#define CHECK_AND_WARNING(type, member_name, func_name) \
  do {                                                  \
    if (!member_name) {                                 \
      LOG(WARNING) << "func " #member_name " is null";  \
    }                                                   \
  }                                                     \
  while (false);

  FUNCTION_LIST(CHECK_AND_WARNING)
#undef CHECK_AND_WARNING
}

OhosWebAuthnApiImpl::~OhosWebAuthnApiImpl() = default;

bool OhosWebAuthnApiImpl::IsAvailable()
{
  return is_available_;
}

NO_SANITIZE("cfi")
OhosWebAuthnApi::GetClientCapabilitiesResult OhosWebAuthnApiImpl::GetClientCapabilities()
{
  if (!func_get_client_capability_ptr_) {
    return {};
  }
  std::vector<std::pair<std::string, bool>> capabilities;
  FIDO2_CapabilityArray* capability_array_ptr = nullptr;
  do {
    LOG(INFO) << "before get_client_capability";
    FIDO2_ErrorCode error =
        func_get_client_capability_ptr_(&capability_array_ptr);
    LOG(INFO) << "after get_client_capability, error["
              << static_cast<int>(error) << ", " << !!capability_array_ptr << "]";
    if (error != FIDO2_SUCCESS || !capability_array_ptr) {
      break;
    }
    LOG(INFO) << "capability_array_ptr: "
              << Convert<std::string>(*capability_array_ptr);
    capabilities.reserve(capability_array_ptr->number);
    for (uint32_t i = 0; i < capability_array_ptr->number; i++) {
      const auto& capability = capability_array_ptr->capability[i];
      capabilities.push_back({
          Convert<std::string>(capability.capability),
          capability.isSupported});
    }
  } while (false);

  if (capability_array_ptr && func_capability_array_destroy_ptr_) {
    func_capability_array_destroy_ptr_(capability_array_ptr);
    capability_array_ptr = nullptr;
  }
  return capabilities;
}

NO_SANITIZE("cfi")
OhosWebAuthnApi::RegisterResult OhosWebAuthnApiImpl::Register(
    CtapMakeCredentialRequest request,
    MakeCredentialOptions request_options)
{
  LOG(INFO) << "OhosWebAuthnApiImpl::Register";
  std::pair<MakeCredentialStatus,
            std::optional<AuthenticatorMakeCredentialResponse>> ret{
      MakeCredentialStatus::kAuthenticatorResponseInvalid, std::nullopt};
  if (!func_register_ptr_) {
    LOG(INFO) << "exit OhosWebAuthnApiImpl::Register, no native api";
    return ret;
  }

  CredentialOptionsDataHolder data_holder(request);

  FIDO2_CredentialCreationOptions options;
  if (func_init_creation_options_ptr_) {
    func_init_creation_options_ptr_(&options);
  }
  Initialize(data_holder, request, request_options, &options);

  LOG(INFO) << "options: " << Convert<std::string>(options);

  FIDO2_TokenBinding token_binding;
  if (func_init_token_binding_ptr_) {
    func_init_token_binding_ptr_(&token_binding);
  }
  const char* origin = nullptr;
  if (request.extra) {
    origin = request.extra->common.origin.c_str();
  }

  FIDO2_PublicKeyAttestationCredential* credential_ptr = nullptr;
  do {
    LOG(INFO) << "before register";
    FIDO2_ErrorCode error =
        func_register_ptr_(options, token_binding, origin, &credential_ptr);
    LOG(INFO) << "after register, error["
              << static_cast<int>(error) << ", " << !!credential_ptr << "]";
    ret.first = MakeCredentialStatus::kAuthenticatorResponseInvalid;
    if (error != FIDO2_SUCCESS || !credential_ptr) {
      break;
    }
    LOG(INFO) << "credential : " << Convert<std::string>(*credential_ptr);
    ret.first = MakeCredentialStatus::kSuccess;
    ret.second = Convert<AuthenticatorMakeCredentialResponse>(*credential_ptr);
  } while (false);

  if (credential_ptr &&
      func_public_key_attestation_credential_destroy_ptr_) {
    func_public_key_attestation_credential_destroy_ptr_(credential_ptr);
  }

  LOG(INFO) << "exit OhosWebAuthnApiImpl::Register, ret{"
            << static_cast<int>(ret.first) << "}";
  return ret;
}

NO_SANITIZE("cfi")
OhosWebAuthnApi::GetAssertionResult OhosWebAuthnApiImpl::GetAssertion(
      CtapGetAssertionRequest request,
      CtapGetAssertionOptions options)
{
  std::pair<GetAssertionStatus,
            std::optional<AuthenticatorGetAssertionResponse>> ret{
      GetAssertionStatus::kAuthenticatorResponseInvalid, std::nullopt};
  if (!func_authenticate_ptr_) {
    return ret;
  }

  CredentialOptionsDataHolder data_holder(request);

  FIDO2_CredentialRequestOptions fido_options;
  if (func_init_request_options_ptr_) {
    func_init_request_options_ptr_(&fido_options);
  }
  Initialize(data_holder, request, options, &fido_options);
  LOG(INFO) << "fido_options: " << Convert<std::string>(fido_options);

  FIDO2_TokenBinding token_binding;
  if (func_init_token_binding_ptr_) {
    func_init_token_binding_ptr_(&token_binding);
  }
  const char* origin = nullptr;
  if (request.extra) {
    origin = request.extra->common.origin.c_str();
  }

  FIDO2_PublicKeyAssertionCredential* credential_ptr = nullptr;

  do {
    LOG(INFO) << "before authenticate";
    FIDO2_ErrorCode error =
        func_authenticate_ptr_(fido_options, token_binding, origin, &credential_ptr);
    ret.first = GetAssertionStatus::kAuthenticatorResponseInvalid;
    LOG(INFO) << "after authenticate, error["
              << static_cast<int>(error) << ", " << !!credential_ptr << "]";
    if (error != FIDO2_SUCCESS || !credential_ptr) {
      break;
    }
    LOG(INFO) << "credential:" << Convert<std::string>(*credential_ptr);
    ret.first = GetAssertionStatus::kSuccess;
    ret.second = Convert<AuthenticatorGetAssertionResponse>(*credential_ptr);
  } while (false);

  if (credential_ptr && func_public_key_assertion_credential_destroy_ptr_) {
    func_public_key_assertion_credential_destroy_ptr_(credential_ptr);
  }
  return ret;
}

} // namepsace device
