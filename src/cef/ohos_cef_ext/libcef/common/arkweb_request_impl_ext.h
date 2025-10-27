// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef OHOS_CEF_EXT_LIBCEF_COMMON_ARKWEB_REQUEST_IMPL_EX_H_
#define OHOS_CEF_EXT_LIBCEF_COMMON_ARKWEB_REQUEST_IMPL_EX_H_
#pragma once

#include <stdint.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "include/arkweb_request_ext.h"
#include "libcef/common/request_impl.h"
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
#include "base/synchronization/waitable_event.h"
#include "base/task/thread_pool.h"
#include "net/base/io_buffer.h"
#include "net/base/upload_data_stream.h"
#include "services/network/chunked_data_pipe_upload_data_stream.h"
#include "services/network/public/mojom/chunked_data_pipe_getter.mojom.h"
#endif

// Extended of CefRequestImpl
class ArkWebRequestImplExt : public ArkWebRequestExt, public CefRequestImpl {
 public:
  using CefRequest::Set;
  ArkWebRequestImplExt() = default;

  CefRefPtr<ArkWebRequestImplExt> AsArkWebRequestExt() override;

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER) || BUILDFLAG(ARKWEB_NETWORK_LOAD)
  CefRefPtr<ArkWebCefPostDataStream> GetUploadStream() override;
  bool IsRedirect() override;
  bool HasUserGesture() override;
  void SetFrameUrl(const CefString& frame_url) override;
  CefString GetFrameUrl() override;
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  bool IsMainFrame() override;
  void Set(const net::HttpRequestHeaders& headers) override;
  void Set(const net::RedirectInfo& redirect_info) override;
#endif

  void Set(const network::ResourceRequest* request,
           uint64_t identifier) override;

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  void SetDestination(network::mojom::RequestDestination destination);
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

 private:
  mutable base::Lock mutable_lock_;

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  network::mojom::RequestDestination destination_;
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  bool is_redirect_{false};
  bool has_user_gesture_{false};
  CefRefPtr<ArkWebCefPostDataStream> postdata_stream_;
  GURL frame_url_;
#endif
};

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
class ArkWebCefPostDataStreamImpl : public ArkWebCefPostDataStream {
 public:
  ArkWebCefPostDataStreamImpl();
  ~ArkWebCefPostDataStreamImpl();

  void SetReadCallback(
      CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) override;

  void Init(
      CefRefPtr<ArkWebCefPostDataStreamInitCallback> init_callback) override;
  void Read(
      void* buffer,
      int buf_len,
      CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) override;
  void AsyncRead(void* buffer,
                 int buf_len,
                 CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback>
                     read_callback) override;
  uint64_t GetSize() override;
  uint64_t GetPosition() override;
  bool IsChunked() override;
  bool HasNullSource() override;
  bool IsEOF() override;
  bool IsInMemory() override;
  void Set(network::ResourceRequestBody* body);
  void Reset() override;
  void GetChunkedDataPipeGetter(network::ResourceRequestBody* body) override;

 private:
  void OnStreamInitialized(int rv);
  void OnStreamRead(scoped_refptr<net::WrappedIOBuffer> buffer,
                    base::WaitableEvent* event,
                    int rv);
  void ReadAsync(void* buffer,
                 int buf_len,
                 CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback);
  void ReadAsyncOnTaskRunner(void* buffer,
                 int buf_len,
                 CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback); 
  void OnStreamReadAsync(
      scoped_refptr<net::WrappedIOBuffer> buffer,
      CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback,
      int rv);
  void OnStreamReadCompleteOnTaskRunner(
      scoped_refptr<net::WrappedIOBuffer> buffer,
      CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback,
      int rv);
  void ReadOnTaskRunner(void* buffer, int buf_len, base::WaitableEvent* event);

  scoped_refptr<base::SequencedTaskRunner> GetSharedSequencedAsyncRunner();

  CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback_;
  CefRefPtr<ArkWebCefPostDataStreamInitCallback> init_callback_;

  std::unique_ptr<net::UploadDataStream> upload_stream_;
  bool initialated_{false};
  bool is_data_pipe_{false};
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner({});
  int last_read_rv_ = -2;

  mutable base::Lock lock_;
  IMPLEMENT_REFCOUNTING(ArkWebCefPostDataStreamImpl);
};
#endif

#endif  // OHOS_CEF_EXT_LIBCEF_COMMON_ARKWEB_REQUEST_IMPL_EX_H_
