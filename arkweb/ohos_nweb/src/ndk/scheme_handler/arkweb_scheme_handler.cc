/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"

#include <unordered_set>

#include "base/logging.h"
#include "ohos_nweb/src/cef_delegate/nweb_application.h"
#include "ohos_nweb/src/cef_delegate/nweb_scheme_handler_factory.h"
#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/response.h"
#include "ohos_nweb/src/ndk/scheme_handler/scheme_handler.h"

namespace {
bool IsArkWebNetErrorValid(ArkWeb_NetError code) {
    static const std::unordered_set<ArkWeb_NetError> validCodes = {
        ARKWEB_NET_OK,
        ARKWEB_ERR_IO_PENDING,
        ARKWEB_ERR_FAILED,
        ARKWEB_ERR_ABORTED,
        ARKWEB_ERR_INVALID_ARGUMENT,
        ARKWEB_ERR_INVALID_HANDLE,
        ARKWEB_ERR_FILE_NOT_FOUND,
        ARKWEB_ERR_TIMED_OUT,
        ARKWEB_ERR_FILE_TOO_LARGE,
        ARKWEB_ERR_UNEXPECTED,
        ARKWEB_ERR_ACCESS_DENIED,
        ARKWEB_ERR_NOT_IMPLEMENTED,
        ARKWEB_ERR_INSUFFICIENT_RESOURCES,
        ARKWEB_ERR_OUT_OF_MEMORY,
        ARKWEB_ERR_UPLOAD_FILE_CHANGED,
        ARKWEB_ERR_SOCKET_NOT_CONNECTED,
        ARKWEB_ERR_FILE_EXISTS,
        ARKWEB_ERR_FILE_PATH_TOO_LONG,
        ARKWEB_ERR_FILE_NO_SPACE,
        ARKWEB_ERR_FILE_VIRUS_INFECTED,
        ARKWEB_ERR_BLOCKED_BY_CLIENT,
        ARKWEB_ERR_NETWORK_CHANGED,
        ARKWEB_ERR_BLOCKED_BY_ADMINISTRATOR,
        ARKWEB_ERR_SOCKET_CONNECTED,
        ARKWEB_ERR_UPLOAD_STREAM_REWIND_NOT_SUPPORTED,
        ARKWEB_ERR_CONTEXT_SHUT_DOWN,
        ARKWEB_ERR_BLOCKED_BY_RESPONSE,
        ARKWEB_ERR_CLEARTEXT_NOT_PERMITTED,
        ARKWEB_ERR_BLOCKED_BY_CSP,
        ARKWEB_ERR_H2_OR_QUIC_REQUIRED,
        ARKWEB_ERR_BLOCKED_BY_ORB,
        ARKWEB_ERR_CONNECTION_CLOSED,
        ARKWEB_ERR_CONNECTION_RESET,
        ARKWEB_ERR_CONNECTION_REFUSED,
        ARKWEB_ERR_CONNECTION_ABORTED,
        ARKWEB_ERR_CONNECTION_FAILED,
        ARKWEB_ERR_NAME_NOT_RESOLVED,
        ARKWEB_ERR_INTERNET_DISCONNECTED,
        ARKWEB_ERR_SSL_PROTOCOL_ERROR,
        ARKWEB_ERR_ADDRESS_INVALID,
        ARKWEB_ERR_ADDRESS_UNREACHABLE,
        ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_NEEDED,
        ARKWEB_ERR_TUNNEL_CONNECTION_FAILED,
        ARKWEB_ERR_NO_SSL_VERSIONS_ENABLED,
        ARKWEB_ERR_SSL_VERSION_OR_CIPHER_MISMATCH,
        ARKWEB_ERR_SSL_RENEGOTIATION_REQUESTED,
        ARKWEB_ERR_PROXY_AUTH_UNSUPPORTED,
        ARKWEB_ERR_BAD_SSL_CLIENT_AUTH_CERT,
        ARKWEB_ERR_CONNECTION_TIMED_OUT,
        ARKWEB_ERR_HOST_RESOLVER_QUEUE_TOO_LARGE,
        ARKWEB_ERR_SOCKS_CONNECTION_FAILED,
        ARKWEB_ERR_SOCKS_CONNECTION_HOST_UNREACHABLE,
        ARKWEB_ERR_ALPN_NEGOTIATION_FAILED,
        ARKWEB_ERR_SSL_NO_RENEGOTIATION,
        ARKWEB_ERR_WINSOCK_UNEXPECTED_WRITTEN_BYTES,
        ARKWEB_ERR_SSL_DECOMPRESSION_FAILURE_ALERT,
        ARKWEB_ERR_SSL_BAD_RECORD_MAC_ALERT,
        ARKWEB_ERR_PROXY_AUTH_REQUESTED,
        ARKWEB_ERR_PROXY_CONNECTION_FAILED,
        ARKWEB_ERR_MANDATORY_PROXY_CONFIGURATION_FAILED,
        ARKWEB_ERR_PRECONNECT_MAX_SOCKET_LIMIT,
        ARKWEB_ERR_SSL_CLIENT_AUTH_PRIVATE_KEY_ACCESS_DENIED,
        ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_NO_PRIVATE_KEY,
        ARKWEB_ERR_PROXY_CERTIFICATE_INVALID,
        ARKWEB_ERR_NAME_RESOLUTION_FAILED,
        ARKWEB_ERR_NETWORK_ACCESS_DENIED,
        ARKWEB_ERR_TEMPORARILY_THROTTLED,
        ARKWEB_ERR_HTTPS_PROXY_TUNNEL_RESPONSE_REDIRECT,
        ARKWEB_ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED,
        ARKWEB_ERR_MSG_TOO_BIG,
        ARKWEB_ERR_WS_PROTOCOL_ERROR,
        ARKWEB_ERR_ADDRESS_IN_USE,
        ARKWEB_ERR_SSL_HANDSHAKE_NOT_COMPLETED,
        ARKWEB_ERR_SSL_BAD_PEER_PUBLIC_KEY,
        ARKWEB_ERR_SSL_PINNED_KEY_NOT_IN_CERT_CHAIN,
        ARKWEB_ERR_CLIENT_AUTH_CERT_TYPE_UNSUPPORTED,
        ARKWEB_ERR_SSL_DECRYPT_ERROR_ALERT,
        ARKWEB_ERR_WS_THROTTLE_QUEUE_TOO_LARGE,
        ARKWEB_ERR_SSL_SERVER_CERT_CHANGED,
        ARKWEB_ERR_SSL_UNRECOGNIZED_NAME_ALERT,
        ARKWEB_ERR_SOCKET_SET_RECEIVE_BUFFER_SIZE_ERROR,
        ARKWEB_ERR_SOCKET_SET_SEND_BUFFER_SIZE_ERROR,
        ARKWEB_ERR_SOCKET_RECEIVE_BUFFER_SIZE_UNCHANGEABLE,
        ARKWEB_ERR_SOCKET_SEND_BUFFER_SIZE_UNCHANGEABLE,
        ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_BAD_FORMAT,
        ARKWEB_ERR_ICANN_NAME_COLLISION,
        ARKWEB_ERR_SSL_SERVER_CERT_BAD_FORMAT,
        ARKWEB_ERR_CT_STH_PARSING_FAILED,
        ARKWEB_ERR_CT_STH_INCOMPLETE,
        ARKWEB_ERR_UNABLE_TO_REUSE_CONNECTION_FOR_PROXY_AUTH,
        ARKWEB_ERR_CT_CONSISTENCY_PROOF_PARSING_FAILED,
        ARKWEB_ERR_SSL_OBSOLETE_CIPHER,
        ARKWEB_ERR_WS_UPGRADE,
        ARKWEB_ERR_READ_IF_READY_NOT_IMPLEMENTED,
        ARKWEB_ERR_NO_BUFFER_SPACE,
        ARKWEB_ERR_SSL_CLIENT_AUTH_NO_COMMON_ALGORITHMS,
        ARKWEB_ERR_EARLY_DATA_REJECTED,
        ARKWEB_ERR_WRONG_VERSION_ON_EARLY_DATA,
        ARKWEB_ERR_TLS13_DOWNGRADE_DETECTED,
        ARKWEB_ERR_SSL_KEY_USAGE_INCOMPATIBLE,
        ARKWEB_ERR_INVALID_ECH_CONFIG_LIST,
        ARKWEB_ERR_ECH_NOT_NEGOTIATED,
        ARKWEB_ERR_ECH_FALLBACK_CERTIFICATE_INVALID,
        ARKWEB_ERR_CERT_COMMON_NAME_INVALID,
        ARKWEB_ERR_CERT_DATE_INVALID,
        ARKWEB_ERR_CERT_AUTHORITY_INVALID,
        ARKWEB_ERR_CERT_CONTAINS_ERRORS,
        ARKWEB_ERR_CERT_NO_REVOCATION_MECHANISM,
        ARKWEB_ERR_CERT_UNABLE_TO_CHECK_REVOCATION,
        ARKWEB_ERR_CERT_REVOKED,
        ARKWEB_ERR_CERT_INVALID,
        ARKWEB_ERR_CERT_WEAK_SIGNATURE_ALGORITHM,
        ARKWEB_ERR_CERT_NON_UNIQUE_NAME,
        ARKWEB_ERR_CERT_WEAK_KEY,
        ARKWEB_ERR_CERT_NAME_CONSTRAINT_VIOLATION,
        ARKWEB_ERR_CERT_VALIDITY_TOO_LONG,
        ARKWEB_ERR_CERTIFICATE_TRANSPARENCY_REQUIRED,
        ARKWEB_ERR_CERT_SYMANTEC_LEGACY,
        ARKWEB_ERR_CERT_KNOWN_INTERCEPTION_BLOCKED,
        ARKWEB_ERR_SSL_OBSOLETE_VERSION_OR_CIPHER,
        ARKWEB_ERR_CERT_END,
        ARKWEB_ERR_INVALID_URL,
        ARKWEB_ERR_DISALLOWED_URL_SCHEME,
        ARKWEB_ERR_UNKNOWN_URL_SCHEME,
        ARKWEB_ERR_INVALID_REDIRECT,
        ARKWEB_ERR_TOO_MANY_REDIRECTS,
        ARKWEB_ERR_UNSAFE_REDIRECT,
        ARKWEB_ERR_UNSAFE_PORT,
        ARKWEB_ERR_INVALID_RESPONSE,
        ARKWEB_ERR_INVALID_CHUNKED_ENCODING,
        ARKWEB_ERR_METHOD_UNSUPPORTED,
        ARKWEB_ERR_UNEXPECTED_PROXY_AUTH,
        ARKWEB_ERR_EMPTY_RESPONSE,
        ARKWEB_ERR_RESPONSE_HEADERS_TOO_BIG,
        ARKWEB_ERR_PAC_SCRIPT_FAILED,
        ARKWEB_ERR_REQUEST_RANGE_NOT_SATISFIABLE,
        ARKWEB_ERR_MALFORMED_IDENTITY,
        ARKWEB_ERR_CONTENT_DECODING_FAILED,
        ARKWEB_ERR_NETWORK_IO_SUSPENDED,
        ARKWEB_ERR_SYN_REPLY_NOT_RECEIVED,
        ARKWEB_ERR_ENCODING_CONVERSION_FAILED,
        ARKWEB_ERR_UNRECOGNIZED_FTP_DIRECTORY_LISTING_FORMAT,
        ARKWEB_ERR_NO_SUPPORTED_PROXIES,
        ARKWEB_ERR_HTTP2_PROTOCOL_ERROR,
        ARKWEB_ERR_INVALID_AUTH_CREDENTIALS,
        ARKWEB_ERR_UNSUPPORTED_AUTH_SCHEME,
        ARKWEB_ERR_ENCODING_DETECTION_FAILED,
        ARKWEB_ERR_MISSING_AUTH_CREDENTIALS,
        ARKWEB_ERR_UNEXPECTED_SECURITY_LIBRARY_STATUS,
        ARKWEB_ERR_MISCONFIGURED_AUTH_ENVIRONMENT,
        ARKWEB_ERR_UNDOCUMENTED_SECURITY_LIBRARY_STATUS,
        ARKWEB_ERR_RESPONSE_BODY_TOO_BIG_TO_DRAIN,
        ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_LENGTH,
        ARKWEB_ERR_INCOMPLETE_HTTP2_HEADERS,
        ARKWEB_ERR_PAC_NOT_IN_DHCP,
        ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_DISPOSITION,
        ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_LOCATION,
        ARKWEB_ERR_HTTP2_SERVER_REFUSED_STREAM,
        ARKWEB_ERR_HTTP2_PING_FAILED,
        ARKWEB_ERR_CONTENT_LENGTH_MISMATCH,
        ARKWEB_ERR_INCOMPLETE_CHUNKED_ENCODING,
        ARKWEB_ERR_QUIC_PROTOCOL_ERROR,
        ARKWEB_ERR_RESPONSE_HEADERS_TRUNCATED,
        ARKWEB_ERR_QUIC_HANDSHAKE_FAILED,
        ARKWEB_ERR_HTTP2_INADEQUATE_TRANSPORT_SECURITY,
        ARKWEB_ERR_HTTP2_FLOW_CONTROL_ERROR,
        ARKWEB_ERR_HTTP2_FRAME_SIZE_ERROR,
        ARKWEB_ERR_HTTP2_COMPRESSION_ERROR,
        ARKWEB_ERR_PROXY_AUTH_REQUESTED_WITH_NO_CONNECTION,
        ARKWEB_ERR_HTTP_1_1_REQUIRED,
        ARKWEB_ERR_PROXY_HTTP_1_1_REQUIRED,
        ARKWEB_ERR_PAC_SCRIPT_TERMINATED,
        ARKWEB_ERR_INVALID_HTTP_RESPONSE,
        ARKWEB_ERR_CONTENT_DECODING_INIT_FAILED,
        ARKWEB_ERR_HTTP2_RST_STREAM_NO_ERROR_RECEIVED,
        ARKWEB_ERR_HTTP2_PUSHED_STREAM_NOT_AVAILABLE,
        ARKWEB_ERR_HTTP2_CLAIMED_PUSHED_STREAM_RESET_BY_SERVER,
        ARKWEB_ERR_TOO_MANY_RETRIES,
        ARKWEB_ERR_HTTP2_STREAM_CLOSED,
        ARKWEB_ERR_HTTP2_CLIENT_REFUSED_STREAM,
        ARKWEB_ERR_HTTP2_PUSHED_RESPONSE_DOES_NOT_MATCH,
        ARKWEB_ERR_HTTP_RESPONSE_CODE_FAILURE,
        ARKWEB_ERR_QUIC_UNKNOWN_CERT_ROOT,
        ARKWEB_ERR_QUIC_GOAWAY_REQUEST_CAN_BE_RETRIED,
        ARKWEB_ERR_TOO_MANY_ACCEPT_CH_RESTARTS,
        ARKWEB_ERR_INCONSISTENT_IP_ADDRESS_SPACE,
        ARKWEB_ERR_CACHED_IP_ADDRESS_SPACE_BLOCKED_BY_LOCAL_NETWORK_ACCESS_POLICY,
        ARKWEB_ERR_CACHE_MISS,
        ARKWEB_ERR_CACHE_READ_FAILURE,
        ARKWEB_ERR_CACHE_WRITE_FAILURE,
        ARKWEB_ERR_CACHE_OPERATION_UNSUPPORTED,
        ARKWEB_ERR_CACHE_OPEN_FAILURE,
        ARKWEB_ERR_CACHE_CREATE_FAILURE,
        ARKWEB_ERR_CACHE_RACE,
        ARKWEB_ERR_CACHE_CHECKSUM_READ_FAILURE,
        ARKWEB_ERR_CACHE_CHECKSUM_MISMATCH,
        ARKWEB_ERR_CACHE_LOCK_TIMEOUT,
        ARKWEB_ERR_CACHE_AUTH_FAILURE_AFTER_READ,
        ARKWEB_ERR_CACHE_ENTRY_NOT_SUITABLE,
        ARKWEB_ERR_CACHE_DOOM_FAILURE,
        ARKWEB_ERR_CACHE_OPEN_OR_CREATE_FAILURE,
        ARKWEB_ERR_INSECURE_RESPONSE,
        ARKWEB_ERR_NO_PRIVATE_KEY_FOR_CERT,
        ARKWEB_ERR_ADD_USER_CERT_FAILED,
        ARKWEB_ERR_INVALID_SIGNED_EXCHANGE,
        ARKWEB_ERR_INVALID_WEB_BUNDLE,
        ARKWEB_ERR_TRUST_TOKEN_OPERATION_FAILED,
        ARKWEB_ERR_TRUST_TOKEN_OPERATION_SUCCESS_WITHOUT_SENDING_REQUEST,
        ARKWEB_ERR_FTP_FAILED,
        ARKWEB_ERR_FTP_SERVICE_UNAVAILABLE,
        ARKWEB_ERR_FTP_TRANSFER_ABORTED,
        ARKWEB_ERR_FTP_FILE_BUSY,
        ARKWEB_ERR_FTP_SYNTAX_ERROR,
        ARKWEB_ERR_FTP_COMMAND_UNSUPPORTED,
        ARKWEB_ERR_FTP_BAD_COMMAND_SEQUENCE,
        ARKWEB_ERR_PKCS12_IMPORT_BAD_PASSWORD,
        ARKWEB_ERR_PKCS12_IMPORT_FAILED,
        ARKWEB_ERR_IMPORT_CA_CERT_NOT_CA,
        ARKWEB_ERR_IMPORT_CERT_ALREADY_EXISTS,
        ARKWEB_ERR_IMPORT_CA_CERT_FAILED,
        ARKWEB_ERR_IMPORT_SERVER_CERT_FAILED,
        ARKWEB_ERR_PKCS12_IMPORT_INVALID_MAC,
        ARKWEB_ERR_PKCS12_IMPORT_INVALID_FILE,
        ARKWEB_ERR_PKCS12_IMPORT_UNSUPPORTED,
        ARKWEB_ERR_KEY_GENERATION_FAILED,
        ARKWEB_ERR_PRIVATE_KEY_EXPORT_FAILED,
        ARKWEB_ERR_SELF_SIGNED_CERT_GENERATION_FAILED,
        ARKWEB_ERR_CERT_DATABASE_CHANGED,
        ARKWEB_ERR_CERT_VERIFIER_CHANGED,
        ARKWEB_ERR_DNS_MALFORMED_RESPONSE,
        ARKWEB_ERR_DNS_SERVER_REQUIRES_TCP,
        ARKWEB_ERR_DNS_SERVER_FAILED,
        ARKWEB_ERR_DNS_TIMED_OUT,
        ARKWEB_ERR_DNS_CACHE_MISS,
        ARKWEB_ERR_DNS_SEARCH_EMPTY,
        ARKWEB_ERR_DNS_SORT_ERROR,
        ARKWEB_ERR_DNS_SECURE_RESOLVER_HOSTNAME_RESOLUTION_FAILED,
        ARKWEB_ERR_DNS_NAME_HTTPS_ONLY,
        ARKWEB_ERR_DNS_REQUEST_CANCELED,
        ARKWEB_ERR_DNS_NO_MATCHING_SUPPORTED_ALPN
    };
    return validCodes.count(code) > 0;
}
}

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define ARKWEB_EXPORT __attribute__((visibility("default")))

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_Create(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_RequestHeaderList** request_header_list) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }
  *request_header_list = new ArkWeb_RequestHeaderList_(resource_request);
}

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_Destroy(
    ArkWeb_RequestHeaderList* request_header_list) {
  if (request_header_list) {
    delete request_header_list;
  } else {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
  }
}

ARKWEB_EXPORT int32_t OH_ArkWebRequestHeaderList_GetSize(
    const ArkWeb_RequestHeaderList* request_header_list) {
  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return -1;
  }

  return request_header_list->GetSize();
}

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_GetHeader(
    const ArkWeb_RequestHeaderList* request_header_list,
    int32_t index,
    char** key,
    char** value) {
  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }

  if (!key) {
    LOG(ERROR) << "scheme_handler key is nullptr";
    return;
  }

  if (!value) {
    LOG(ERROR) << "scheme_handler value is nullptr";
    return;
  }

  request_header_list->GetHeader(index, key, value);
}

ARKWEB_EXPORT int32_t
OH_ArkWebResourceRequest_SetUserData(ArkWeb_ResourceRequest* resource_request,
                                     void* user_data) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!user_data) {
    LOG(ERROR) << "scheme_handler set a nullptr.";
    return ARKWEB_INVALID_PARAM;
  }
  resource_request->user_data = user_data;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void* OH_ArkWebResourceRequest_GetUserData(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return nullptr;
  }
  return resource_request->user_data;
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetMethod(
    const ArkWeb_ResourceRequest* resource_request,
    char** method) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!method) {
    LOG(ERROR) << "scheme_handler method is nullptr";
    return;
  }
  resource_request->GetMethod(method);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetUrl(
    const ArkWeb_ResourceRequest* resource_request,
    char** url) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!url) {
    LOG(ERROR) << "scheme_handler url is nullptr";
    return;
  }
  resource_request->GetUrl(url);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetHttpBodyStream(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_HttpBodyStream** http_body_stream) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return;
  }
  resource_request->GetHttpBodyStream(http_body_stream);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_DestroyHttpBodyStream(
    ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return;
  }

  http_body_stream->Reset();
  http_body_stream = nullptr;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceRequest_GetResourceType(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return -1;
  }

  return resource_request->GetRequestResourceType();
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetFrameUrl(
    const ArkWeb_ResourceRequest* resource_request,
    char** frame_url) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!frame_url) {
    LOG(ERROR) << "scheme_handler frame_url is nullptr";
    return;
  }
  resource_request->GetFrameUrl(frame_url);
}

ARKWEB_EXPORT int32_t OH_ArkWebHttpBodyStream_SetReadCallback(
    ArkWeb_HttpBodyStream* http_body_stream,
    ArkWeb_HttpBodyStreamReadCallback readCallback) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  http_body_stream->SetReadCallback(readCallback);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebHttpBodyStream_SetAsyncReadCallback(
    ArkWeb_HttpBodyStream* http_body_stream,
    ArkWeb_HttpBodyStreamAsyncReadCallback readCallback) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  http_body_stream->SetAsyncReadCallback(readCallback);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t
OH_ArkWebHttpBodyStream_SetUserData(ArkWeb_HttpBodyStream* http_body_stream,
                                    void* user_data) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  http_body_stream->SetUserData(user_data);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void* OH_ArkWebHttpBodyStream_GetUserData(
    const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return nullptr;
  }

  return http_body_stream->GetUserData();
}

ARKWEB_EXPORT int32_t
OH_ArkWebHttpBodyStream_Init(ArkWeb_HttpBodyStream* http_body_stream,
                             ArkWeb_HttpBodyStreamInitCallback initCallback) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  http_body_stream->Init(initCallback);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebHttpBodyStream_Read(
    const ArkWeb_HttpBodyStream* http_body_stream,
    uint8_t* buffer,
    int buf_len) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return;
  }

  http_body_stream->Read(buffer, buf_len);
}

ARKWEB_EXPORT void OH_ArkWebHttpBodyStream_AsyncRead(
    const ArkWeb_HttpBodyStream* http_body_stream,
    uint8_t* buffer,
    int buf_len) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return;
  }
  if (buf_len < 0) {
    LOG(ERROR) << "buf_len smaller then zero";
    return;
  }

  http_body_stream->AsyncRead(buffer, buf_len);
}

ARKWEB_EXPORT uint64_t
OH_ArkWebHttpBodyStream_GetSize(const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return 0;
  }

  return http_body_stream->GetSize();
}

ARKWEB_EXPORT uint64_t OH_ArkWebHttpBodyStream_GetPosition(
    const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return 0;
  }

  return http_body_stream->GetPosition();
}

ARKWEB_EXPORT bool OH_ArkWebHttpBodyStream_IsChunked(
    const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return false;
  }

  return http_body_stream->IsChunked();
}

ARKWEB_EXPORT bool OH_ArkWebHttpBodyStream_IsEof(
    const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return false;
  }

  return http_body_stream->IsEOF();
}

ARKWEB_EXPORT bool OH_ArkWebHttpBodyStream_IsInMemory(
    const ArkWeb_HttpBodyStream* http_body_stream) {
  if (!http_body_stream) {
    LOG(ERROR) << "scheme_handler http body stream is nullptr";
    return false;
  }

  return http_body_stream->IsInMemory();
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceRequest_Destroy(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  delete resource_request;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetReferrer(
    const ArkWeb_ResourceRequest* resource_request,
    char** referrer) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!referrer) {
    LOG(ERROR) << "scheme_handler referrer is nullptr";
    return;
  }

  resource_request->GetReferrer(referrer);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetRequestHeaders(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_RequestHeaderList** request_header_list) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }
  *request_header_list = new ArkWeb_RequestHeaderList(resource_request);
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_IsRedirect(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->IsRedirect();
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_IsMainFrame(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->IsMainFrame();
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_HasGesture(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->HasUserGesture();
}

ARKWEB_EXPORT int32_t OH_ArkWeb_RegisterCustomSchemes(const char* scheme,
                                                      int32_t option) {
  if (!scheme) {
    LOG(ERROR) << "scheme_handler scheme is nulltpr.";
    return ARKWEB_INVALID_PARAM;
  }

  CefRefPtr<OHOS::NWeb::NWebApplication> application =
      OHOS::NWeb::NWebApplication::GetDefault();
  if (!application) {
    LOG(ERROR) << "scheme_handler application is nulltpr.";
    return ARKWEB_ERROR_UNKNOWN;
  }

  bool is_registered =
      application->RegisterCustomSchemes(std::string(scheme), option);
  if (is_registered) {
    return ARKWEB_NET_OK;
  } else {
    return ARKWEB_SCHEME_REGISTER_FAILED;
  }
}

ARKWEB_EXPORT bool OH_ArkWebServiceWorker_SetSchemeHandler(
    const char* scheme,
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme) {
    LOG(ERROR) << "scheme_handler scheme is nullptr.";
    return false;
  }

  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return false;
  }

  CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme(
          std::string(scheme));
  if (!factory) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return false;
  }

  factory->SetSchemeHandlerForSW(scheme_handler);
  return true;
}

ARKWEB_EXPORT bool OH_ArkWeb_SetSchemeHandler(
    const char* scheme,
    const char* web_tag,
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme) {
    LOG(ERROR) << "scheme_handler scheme is nullptr.";
    return false;
  }

  if (!web_tag) {
    LOG(ERROR) << "scheme_handler web tag is nullptr.";
    return false;
  }

  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return false;
  }
  CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme(
          std::string(scheme));
  if (!factory) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return false;
  }

  factory->SetSchemeHandler(std::string(web_tag), scheme_handler);
  return true;
}

ARKWEB_EXPORT int32_t OH_ArkWebServiceWorker_ClearSchemeHandlers() {
  OHOS::NWeb::NWebSchemeHandlerFactory::ClearServiceWorkerSchemeHandler();
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWeb_ClearSchemeHandlers(const char* web_tag) {
  if (!web_tag) {
    LOG(ERROR) << "scheme_handler web tag is nulltpr.";
    return ARKWEB_INVALID_PARAM;
  }

  OHOS::NWeb::NWebSchemeHandlerFactory::ClearSchemeHandlers(
      std::string(web_tag));
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_CreateSchemeHandler(
    ArkWeb_SchemeHandler** scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return;
  }
  *scheme_handler = new ArkWeb_SchemeHandler();
}

ARKWEB_EXPORT void OH_ArkWeb_DestroySchemeHandler(
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return;
  }

  delete scheme_handler;
}

ARKWEB_EXPORT int32_t
OH_ArkWebSchemeHandler_SetUserData(ArkWeb_SchemeHandler* scheme_handler,
                                   void* user_data) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_INVALID_PARAM;
  }

  if (!user_data) {
    LOG(ERROR) << "scheme_handler set a nullptr.";
    return ARKWEB_INVALID_PARAM;
  }

  scheme_handler->user_data = user_data;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void* OH_ArkWebSchemeHandler_GetUserData(
    const ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return nullptr;
  }

  return scheme_handler->user_data;
}

ARKWEB_EXPORT int32_t OH_ArkWebSchemeHandler_SetOnRequestStart(
    ArkWeb_SchemeHandler* scheme_handler,
    ArkWeb_OnRequestStart on_request_start) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_INVALID_PARAM;
  }

  scheme_handler->on_request_start = on_request_start;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t
OH_ArkWebSchemeHandler_SetOnRequestStop(ArkWeb_SchemeHandler* scheme_handler,
                                        ArkWeb_OnRequestStop on_request_stop) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_INVALID_PARAM;
  }

  scheme_handler->on_request_stop = on_request_stop;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_CreateResponse(ArkWeb_Response** response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr.";
    return;
  }
  *response = new ArkWeb_Response();
}

ARKWEB_EXPORT void OH_ArkWeb_DestroyResponse(ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  delete response;
  response = nullptr;
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetUrl(ArkWeb_Response* response,
                                               const char* url) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!url) {
    LOG(ERROR) << "scheme_handler set a nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetUrl(url);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetUrl(const ArkWeb_Response* response,
                                            char** url) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!url) {
    LOG(ERROR) << "scheme_handler url is nullptr";
    return;
  }
  response->GetUrl(url);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetError(ArkWeb_Response* response,
                                                 ArkWeb_NetError error_code) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetError(error_code);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT ArkWeb_NetError
OH_ArkWebResponse_GetError(const ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_ERR_FAILED;
  }
  return response->GetError();
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetStatus(ArkWeb_Response* response,
                                                  int status) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetStatus(status);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int OH_ArkWebResponse_GetStatus(const ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return -1;
  }
  return response->GetStatus();
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetStatusText(ArkWeb_Response* response,
                                                      const char* status_text) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!status_text) {
    LOG(ERROR) << "scheme_handler set a nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetStatusText(status_text);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetStatusText(
    const ArkWeb_Response* response,
    char** status_text) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!status_text) {
    LOG(ERROR) << "scheme_handler status text is nullptr";
    return;
  }
  response->GetStatusText(status_text);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetMimeType(ArkWeb_Response* response,
                                                    const char* mime_type) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!mime_type) {
    LOG(ERROR) << "scheme_handler set a nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetMimeType(mime_type);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetMimeType(
    const ArkWeb_Response* response,
    char** mime_type) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!mime_type) {
    LOG(ERROR) << "scheme_handler mime type is nullptr";
    return;
  }
  response->GetMimeType(mime_type);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetCharset(ArkWeb_Response* response,
                                                   const char* charset) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!charset) {
    LOG(ERROR) << "scheme_handler set a nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetCharset(charset);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetCharset(const ArkWeb_Response* response,
                                                char** charset) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!charset) {
    LOG(ERROR) << "scheme_handler charset is nullptr";
    return;
  }
  response->GetCharset(charset);
}

ARKWEB_EXPORT int32_t
OH_ArkWebResponse_SetHeaderByName(ArkWeb_Response* response,
                                  const char* name,
                                  const char* value,
                                  bool overwrite) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!name) {
    LOG(ERROR) << "scheme_handler name is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!value) {
    LOG(ERROR) << "scheme_handler value is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  response->SetHeaderByName(name, value, overwrite);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetHeaderByName(
    const ArkWeb_Response* response,
    const char* name,
    char** value) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!name) {
    LOG(ERROR) << "scheme_handler name is nullptr";
    return;
  }

  if (!value) {
    LOG(ERROR) << "scheme_handler value is nullptr";
    return;
  }
  response->GetHeaderByName(name, value);
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_Destroy(
    const ArkWeb_ResourceHandler* resource_handler) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  delete resource_handler;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidReceiveResponse(
    const ArkWeb_ResourceHandler* resource_handler,
    const ArkWeb_Response* response) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  resource_handler->DidReceiveResponse(response);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidReceiveData(
    const ArkWeb_ResourceHandler* resource_handler,
    const uint8_t* buffer,
    int64_t buf_len) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }

  if (!buffer) {
    LOG(ERROR) << "scheme_handler buffer is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  resource_handler->DidReceiveData(buffer, buf_len);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidFinish(
    const ArkWeb_ResourceHandler* resource_handler) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  resource_handler->DidFinish();
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidFailWithError(
    const ArkWeb_ResourceHandler* resource_handler,
    ArkWeb_NetError error_code) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  resource_handler->DidFailWithError(error_code, false);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidFailWithErrorV2(
    const ArkWeb_ResourceHandler* resource_handler,
    ArkWeb_NetError error_code,
    bool completeIfNoResponse) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler resource handler is nullptr";
    return ARKWEB_INVALID_PARAM;
  }
  if (!IsArkWebNetErrorValid(error_code) || error_code == ARKWEB_NET_OK) {
    LOG(ERROR) << "The error code is invalid or ARKWEB_NET_OK";
    return ARKWEB_INVALID_PARAM;
  }
  resource_handler->DidFailWithError(error_code, completeIfNoResponse);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_ReleaseString(char* string) {
  if (string) {
    delete string;
  }
}

ARKWEB_EXPORT void OH_ArkWeb_ReleaseByteArray(uint8_t* byte_array) {
  if (byte_array) {
    delete byte_array;
  }
}

ARKWEB_EXPORT int32_t
OH_ArkWebSchemeHandler_SetFromEts(ArkWeb_SchemeHandler* scheme_handler,
                                  bool fromEts) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_INVALID_PARAM;
  }

  scheme_handler->fromEts = fromEts;
  return ARKWEB_NET_OK;
}

#ifdef __cplusplus
}
#endif __cplusplus
