// Copyright (c) 2008-2009 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "arkweb_request_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "cef/libcef/common/request_impl.h"
#include "net/base/elements_upload_data_stream.h"
#include "net/base/upload_bytes_element_reader.h"
#include "net/base/upload_file_element_reader.h"

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
#include "base/datashare_uri_utils.h"
#include "base/files/file.h"
#include "base/task/thread_pool.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/io_buffer.h"
#include "services/network/chunked_data_pipe_upload_data_stream.h"
#include "services/network/data_pipe_element_reader.h"
#endif

namespace {
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
// A subclass of net::UploadBytesElementReader which owns
// ResourceRequestBody.
class BytesElementReader : public net::UploadBytesElementReader {
 public:
  BytesElementReader(network::ResourceRequestBody* resource_request_body,
                     const network::DataElementBytes& element)
      : net::UploadBytesElementReader(std::span(element.bytes())),
        resource_request_body_(resource_request_body) {}

  BytesElementReader(const BytesElementReader&) = delete;
  BytesElementReader& operator=(const BytesElementReader&) = delete;

  ~BytesElementReader() override {}

 private:
  scoped_refptr<network::ResourceRequestBody> resource_request_body_;
};

// A subclass of net::UploadFileElementReader which owns
// ResourceRequestBody.
// This class is necessary to ensure the BlobData and any attached shareable
// files survive until upload completion.
class FileElementReader : public net::UploadFileElementReader {
 public:
  FileElementReader(network::ResourceRequestBody* resource_request_body,
                    base::TaskRunner* task_runner,
                    const network::DataElementFile& element,
                    base::File&& file)
      : net::UploadFileElementReader(task_runner,
                                     std::move(file),
                                     element.path(),
                                     element.offset(),
                                     element.length(),
                                     element.expected_modification_time()),
        resource_request_body_(resource_request_body) {}

  FileElementReader(const FileElementReader&) = delete;
  FileElementReader& operator=(const FileElementReader&) = delete;

  ~FileElementReader() override {}

 private:
  scoped_refptr<network::ResourceRequestBody> resource_request_body_;
};

std::unique_ptr<net::UploadDataStream> CreateUploadDataStream(
    network::ResourceRequestBody* body,
    std::vector<base::File>& opened_files,
    base::SequencedTaskRunner* file_task_runner) {
  // In the case of a chunked upload, there will just be one element.
  if (body->elements()->size() == 1) {
    if (body->elements()->begin()->type() ==
        network::mojom::DataElementDataView::Tag::kChunkedDataPipe) {
      auto& element = body->elements_mutable()
                          ->at(0)
                          .As<network::DataElementChunkedDataPipe>();
      const bool has_null_source = element.read_only_once().value();
      auto upload_data_stream =
          std::make_unique<network::ChunkedDataPipeUploadDataStream>(
              body, element.ReleaseChunkedDataPipeGetter(), has_null_source,
              true);
      if (element.read_only_once()) {
        upload_data_stream->EnableCache();
      }
      body->elements_mutable()->clear();
      return upload_data_stream;
    }
  }

  auto opened_file = opened_files.begin();
  std::vector<std::unique_ptr<net::UploadElementReader>> element_readers;
  for (const auto& element : *body->elements()) {
    switch (element.type()) {
      case network::mojom::DataElementDataView::Tag::kBytes:
        element_readers.push_back(std::make_unique<BytesElementReader>(
            body, element.As<network::DataElementBytes>()));
        break;
      case network::mojom::DataElementDataView::Tag::kFile:
        DCHECK(opened_file != opened_files.end());
        element_readers.push_back(std::make_unique<FileElementReader>(
            body, file_task_runner, element.As<network::DataElementFile>(),
            std::move(*opened_file++)));
        break;
      case network::mojom::DataElementDataView::Tag::kDataPipe: {
        element_readers.push_back(
            std::make_unique<network::DataPipeElementReader>(
                body, element.As<network::DataElementDataPipe>()
                          .CloneDataPipeGetter()));
        break;
      }
      case network::mojom::DataElementDataView::Tag::kChunkedDataPipe: {
        // This shouldn't happen, as the traits logic should ensure that if
        // there's a chunked pipe, there's one and only one element.
        NOTREACHED();
      }
    }
  }
  DCHECK(opened_file == opened_files.end());

  return std::make_unique<net::ElementsUploadDataStream>(
      std::move(element_readers), body->identifier());
}
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)
}  // namespace

CefRefPtr<ArkWebRequestImplExt> ArkWebRequestImplExt::AsArkWebRequestExt() {
  return this;
}

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER) || BUILDFLAG(ARKWEB_NETWORK_LOAD)
CefRefPtr<ArkWebCefPostDataStream> ArkWebRequestImplExt::GetUploadStream() {
  base::AutoLock lock_scope(mutable_lock_);
  return postdata_stream_;
}

bool ArkWebRequestImplExt::IsRedirect() {
  base::AutoLock lock_scope(mutable_lock_);
  return is_redirect_;
}

bool ArkWebRequestImplExt::HasUserGesture() {
  base::AutoLock lock_scope(mutable_lock_);
  return has_user_gesture_;
}

CefString ArkWebRequestImplExt::GetFrameUrl() {
  base::AutoLock lock_scope(mutable_lock_);
  return frame_url_.spec();
}

void ArkWebRequestImplExt::SetFrameUrl(const CefString& frame_url) {
  base::AutoLock lock_scope(mutable_lock_);
  const GURL& frame_gurl = GURL(frame_url.ToString());
  if (frame_url_ != frame_gurl) {
    frame_url_ = frame_gurl;
  }
}
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
bool ArkWebRequestImplExt::IsMainFrame() {
  base::AutoLock lock_scope(mutable_lock_);
  return (destination_ == network::mojom::RequestDestination::kDocument);
}

void ArkWebRequestImplExt::Set(const net::HttpRequestHeaders& headers) {
  CefRequestImpl::Set(headers);
}

void ArkWebRequestImplExt::Set(const net::RedirectInfo& redirect_info) {
  CefRequestImpl::Set(redirect_info);
}
#endif

void ArkWebRequestImplExt::Set(const network::ResourceRequest* request,
                               uint64_t identifier) {
  CefRequestImpl::Set(request, identifier);

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  if (request->request_body) {
    postdata_stream_ = ArkWebCefPostDataStream::Create();
    static_cast<ArkWebCefPostDataStreamImpl*>(postdata_stream_.get())
        ->Set(request->request_body.get());
  }
  has_user_gesture_ = request->has_user_gesture;
  is_redirect_ = false;
#endif
}

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
void ArkWebRequestImplExt::SetDestination(
    network::mojom::RequestDestination destination) {
  base::AutoLock lock_scope(mutable_lock_);
  destination_ = destination;
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
// CefPostDataStream ----------------------------------------------------------

// static
CefRefPtr<ArkWebCefPostDataStream> ArkWebCefPostDataStream::Create() {
  CefRefPtr<ArkWebCefPostDataStream> postdata_stream(
      new ArkWebCefPostDataStreamImpl());
  return postdata_stream;
}

// ArkWebCefPostDataStreamImpl
// ------------------------------------------------------

ArkWebCefPostDataStreamImpl::ArkWebCefPostDataStreamImpl() {}

ArkWebCefPostDataStreamImpl::~ArkWebCefPostDataStreamImpl() {
  if (is_data_pipe_ && upload_stream_) {
    content::GetIOThreadTaskRunner({})->DeleteSoon(FROM_HERE,
                                                   std::move(upload_stream_));
  }
}

void ArkWebCefPostDataStreamImpl::Reset() {
  read_callback_ = nullptr;
  init_callback_ = nullptr;
}

void ArkWebCefPostDataStreamImpl::GetChunkedDataPipeGetter(
    network::ResourceRequestBody* body) {
  LOG(INFO) << "scheme_handler get chunked data pip getter initialated_: "
            << initialated_;
  if (body && upload_stream_ && !initialated_) {
    body->SetToChunkedDataPipe(
        static_cast<network::ChunkedDataPipeUploadDataStream*>(
            upload_stream_.get())
            ->ReleaseChunkedDataPipeGetter(),
        network::ResourceRequestBody::ReadOnlyOnce(HasNullSource()));
  }
}

void ArkWebCefPostDataStreamImpl::Set(network::ResourceRequestBody* body) {
  std::vector<base::File> open_files;
  if (body) {
    for (const auto& element : *(body->elements())) {
      if (element.type() == network::DataElement::Tag::kFile) {
        const auto& file_element = element.As<network::DataElementFile>();
        std::string real_path =
            base::GetRealPath(base::FilePath(file_element.path()));
        LOG(INFO) << "scheme_handler file path: " << file_element.path()
                  << " real path: " << real_path;
        open_files.push_back(
            base::File(base::FilePath(real_path),
                       base::File::FLAG_OPEN | base::File::FLAG_READ));
      }
      if (element.type() == network::DataElement::Tag::kDataPipe ||
          element.type() == network::DataElement::Tag::kChunkedDataPipe) {
        is_data_pipe_ = true;
      }
    }
    scoped_refptr<base::SequencedTaskRunner> task_runner =
        base::ThreadPool::CreateSequencedTaskRunner(
            {base::MayBlock(), base::TaskPriority::USER_VISIBLE});
    upload_stream_ =
        CreateUploadDataStream(body, open_files, task_runner.get());
  }
}

void ArkWebCefPostDataStreamImpl::SetReadCallback(
    CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) {
  read_callback_ = read_callback;
}

void ArkWebCefPostDataStreamImpl::OnStreamInitialized(int rv) {
  if (init_callback_) {
    init_callback_->OnInitComplete(rv);
  } else {
    LOG(ERROR) << "scheme_handler_init_callback is nullptr.";
  }
}

void ArkWebCefPostDataStreamImpl::Init(
    CefRefPtr<ArkWebCefPostDataStreamInitCallback> init_callback) {
  initialated_ = true;
  init_callback_ = init_callback;
  if (upload_stream_) {
    int rv = upload_stream_->Init(
        base::BindOnce(&ArkWebCefPostDataStreamImpl::OnStreamInitialized,
                       base::Unretained(this)),
        net::NetLogWithSource());
    if (rv == net::ERR_IO_PENDING) {
      return;
    }
    OnStreamInitialized(rv);
  } else {
    OnStreamInitialized(-2);
  }
}

void ArkWebCefPostDataStreamImpl::ReadOnTaskRunner(void* buffer,
                                                   int buf_len,
                                                   base::WaitableEvent* event) {
  scoped_refptr<net::WrappedIOBuffer> upload_buffer =
      base::MakeRefCounted<net::WrappedIOBuffer>(base::span(
          static_cast<const char*>(buffer), static_cast<size_t>(buf_len)));
  if (upload_stream_) {
    int rv = upload_stream_->Read(
        upload_buffer.get(), buf_len,
        base::BindOnce(&ArkWebCefPostDataStreamImpl::OnStreamRead,
                       base::Unretained(this), upload_buffer, event));
    if (rv == net::ERR_IO_PENDING) {
      last_read_rv_ = rv;
      return;
    }
    last_read_rv_ = rv;
    event->Signal();
  } else {
    last_read_rv_ = -2;
    event->Signal();
  }
}

void ArkWebCefPostDataStreamImpl::ReadAsync(
    void* buffer,
    int buf_len,
    CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) {
  scoped_refptr<net::WrappedIOBuffer> upload_buffer =
      base::MakeRefCounted<net::WrappedIOBuffer>(base::span(
          static_cast<const char*>(buffer), static_cast<size_t>(buf_len)));
  if (upload_stream_) {
    int rv = upload_stream_->Read(
        upload_buffer.get(), buf_len,
        base::BindOnce(&ArkWebCefPostDataStreamImpl::OnStreamReadAsync,
                       base::Unretained(this), upload_buffer, read_callback));
    if (rv == net::ERR_IO_PENDING) {
      return;
    }
    OnStreamReadAsync(upload_buffer, read_callback, rv);
  } else {
    OnStreamReadAsync(upload_buffer, read_callback, -2);
  }
}

void ArkWebCefPostDataStreamImpl::ReadAsyncOnTaskRunner(
    void* buffer,
    int buf_len,
    CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback) {
  scoped_refptr<net::WrappedIOBuffer> upload_buffer =
      base::MakeRefCounted<net::WrappedIOBuffer>(base::span(
          static_cast<const char*>(buffer), static_cast<size_t>(buf_len)));

  if (upload_stream_) {
    int rv = upload_stream_->Read(
        upload_buffer.get(), buf_len,
        base::BindOnce(&ArkWebCefPostDataStreamImpl::OnStreamReadCompleteOnTaskRunner,
                       base::Unretained(this), upload_buffer, read_callback));
    if (rv == net::ERR_IO_PENDING) {
      return;
    }
    OnStreamReadCompleteOnTaskRunner(upload_buffer, read_callback, rv);
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    OnStreamReadCompleteOnTaskRunner(upload_buffer, read_callback, -2);
  }
}

void ArkWebCefPostDataStreamImpl::Read(
    void* buffer,
    int buf_len,
    CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) {
  if (base::SequencedTaskRunner::HasCurrentDefault() || IsInMemory() ||
      IsChunked()) {
    ReadAsync(buffer, buf_len, read_callback);
  } else {
    if (sequenced_task_runner_) {
      base::WaitableEvent* completion = new base::WaitableEvent();
      sequenced_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&ArkWebCefPostDataStreamImpl::ReadOnTaskRunner, this,
                         buffer, buf_len, completion));
      completion->Wait();
      delete completion;
      read_callback->OnReadComplete((char*)buffer, last_read_rv_);
    } else {
      read_callback->OnReadComplete((char*)buffer, -2);
    }
  }
}

scoped_refptr<base::SequencedTaskRunner>
ArkWebCefPostDataStreamImpl::GetSharedSequencedAsyncRunner() {
  static base::NoDestructor<scoped_refptr<base::SequencedTaskRunner>>
      sequenced_async_task_runner(
          scoped_refptr(base::ThreadPool::CreateSequencedTaskRunner({})));
  return *sequenced_async_task_runner;
}

void ArkWebCefPostDataStreamImpl::AsyncRead(
    void* buffer,
    int buf_len,
    CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback) {
  scoped_refptr<base::SequencedTaskRunner> sequenced_async_task_runner =
      GetSharedSequencedAsyncRunner();
  if (sequenced_async_task_runner) {
    sequenced_async_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&ArkWebCefPostDataStreamImpl::ReadAsyncOnTaskRunner,
                       this, buffer, buf_len, read_callback));
  } else {
    LOG(ERROR) << "sequenced_async_task_runner is nullptr";
    read_callback->OnAsyncReadComplete((char*)buffer, -2);
  }
}

void ArkWebCefPostDataStreamImpl::OnStreamReadCompleteOnTaskRunner(
    scoped_refptr<net::WrappedIOBuffer> buffer,
    CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback,
    int rv) {
  if (read_callback) {
    read_callback->OnAsyncReadComplete(buffer->data(), rv);
  }
}

void ArkWebCefPostDataStreamImpl::OnStreamReadAsync(
    scoped_refptr<net::WrappedIOBuffer> buffer,
    CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback,
    int rv) {
  if (read_callback) {
    read_callback->OnReadComplete(buffer->data(), rv);
  }
}

void ArkWebCefPostDataStreamImpl::OnStreamRead(
    scoped_refptr<net::WrappedIOBuffer> buffer,
    base::WaitableEvent* event,
    int rv) {
  last_read_rv_ = rv;
  if (event) {
    event->Signal();
  }
}

uint64_t ArkWebCefPostDataStreamImpl::GetSize() {
  if (upload_stream_) {
    return upload_stream_->size();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return 0;
  }
}

uint64_t ArkWebCefPostDataStreamImpl::GetPosition() {
  if (upload_stream_) {
    return upload_stream_->position();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return 0;
  }
}

bool ArkWebCefPostDataStreamImpl::IsChunked() {
  if (upload_stream_) {
    return upload_stream_->is_chunked();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return false;
  }
}

bool ArkWebCefPostDataStreamImpl::HasNullSource() {
  if (upload_stream_) {
    return upload_stream_->has_null_source();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return false;
  }
}

bool ArkWebCefPostDataStreamImpl::IsEOF() {
  if (upload_stream_) {
    return upload_stream_->IsEOF();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return false;
  }
}

bool ArkWebCefPostDataStreamImpl::IsInMemory() {
  if (upload_stream_) {
    return upload_stream_->IsInMemory();
  } else {
    LOG(ERROR) << "scheme_handler upload stream is nullptr.";
    return false;
  }
}
#endif
