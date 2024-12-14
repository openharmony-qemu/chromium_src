// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/ohos/video_capture_device_ohos.h"

#include <cstddef>

#include <utility>

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/capture/video/ohos/ohos_capture_delegate.h"
#include "ohos_adapter_helper.h"

namespace media {
VideoCaptureDeviceOHOS::VideoCaptureDeviceOHOS(
    const VideoCaptureDeviceDescriptor& device_descriptor)
    : device_descriptor_(device_descriptor),
      capture_thread_("OHOSCaptureThread") {}

VideoCaptureDeviceOHOS::~VideoCaptureDeviceOHOS() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  // Check if the thread is running.
  // This means that the device has not been StopAndDeAllocate()d properly.
  DCHECK(!capture_thread_.IsRunning());
  capture_thread_.Stop();
}

void VideoCaptureDeviceOHOS::AllocateAndStart(
    const VideoCaptureParams& params,
    std::unique_ptr<VideoCaptureDevice::Client> client) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(!capture_impl_);
  if (capture_thread_.IsRunning())
    return;  // Wrong state.
  capture_thread_.Start();

  LOG(INFO) << "VideoCaptureDeviceOHOS::AllocateAndStart: "
            << ", device_id: " << device_descriptor_.device_id
            << ", facing: " << device_descriptor_.facing
            << ", width: " << params.requested_format.frame_size.width()
            << ", height: " << params.requested_format.frame_size.height()
            << ", frame_rate: " << params.requested_format.frame_rate
            << ", pixel_format: " << params.requested_format.pixel_format
            << ", buffer_type: " << static_cast<int>(params.buffer_type)
            << ", resolution_change_policy: "
            << static_cast<int>(params.resolution_change_policy)
            << ", power_line_frequency: "
            << static_cast<int>(params.power_line_frequency)
            << ", enable_face_detection: " << params.enable_face_detection;

  capture_impl_ = std::make_unique<OHOSCaptureDelegate>(
      device_descriptor_, capture_thread_.task_runner(), params);
  if (!capture_impl_) {
    LOG(ERROR) << "capture impl is null";
    return;
  }
  capture_thread_.task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSCaptureDelegate::AllocateAndStart,
                     capture_impl_->GetWeakPtr(), std::move(client)));
}

void VideoCaptureDeviceOHOS::StopAndDeAllocate() {
  LOG(INFO) << "StopAndDeAllocate";
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  if (!capture_thread_.IsRunning())
    return;  // Wrong state.
  capture_thread_.task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&OHOSCaptureDelegate::StopAndDeAllocate,
                                capture_impl_->GetWeakPtr()));
  capture_thread_.task_runner()->DeleteSoon(FROM_HERE, capture_impl_.release());
  capture_thread_.Stop();

  capture_impl_ = nullptr;
}

void VideoCaptureDeviceOHOS::TakePhoto(TakePhotoCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(capture_impl_);
  auto functor =
      base::BindOnce(&OHOSCaptureDelegate::TakePhoto,
                     capture_impl_->GetWeakPtr(), std::move(callback));
  if (!capture_thread_.IsRunning()) {
    // We have to wait until we get the device AllocateAndStart()ed.
    photo_requests_queue_.push_back(std::move(functor));
    return;
  }
  capture_thread_.task_runner()->PostTask(FROM_HERE, std::move(functor));
}

void VideoCaptureDeviceOHOS::GetPhotoState(GetPhotoStateCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto functor =
      base::BindOnce(&OHOSCaptureDelegate::GetPhotoState,
                     capture_impl_->GetWeakPtr(), std::move(callback));
  if (!capture_thread_.IsRunning()) {
    // We have to wait until we get the device AllocateAndStart()ed.
    photo_requests_queue_.push_back(std::move(functor));
    return;
  }
  capture_thread_.task_runner()->PostTask(FROM_HERE, std::move(functor));
}

void VideoCaptureDeviceOHOS::SetPhotoOptions(mojom::PhotoSettingsPtr settings,
                                             SetPhotoOptionsCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto functor = base::BindOnce(&OHOSCaptureDelegate::SetPhotoOptions,
                                capture_impl_->GetWeakPtr(),
                                std::move(settings), std::move(callback));
  if (!capture_thread_.IsRunning()) {
    // We have to wait until we get the device AllocateAndStart()ed.
    photo_requests_queue_.push_back(std::move(functor));
    return;
  }
  capture_thread_.task_runner()->PostTask(FROM_HERE, std::move(functor));
}

void VideoCaptureDeviceOHOS::MaybeSuspend() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto functor = base::BindOnce(&OHOSCaptureDelegate::MaybeSuspend,
                                capture_impl_->GetWeakPtr());
  if (!capture_thread_.IsRunning()) {
    // We have to wait until we get the device AllocateAndStart()ed.
    photo_requests_queue_.push_back(std::move(functor));
    return;
  }
  capture_thread_.task_runner()->PostTask(FROM_HERE, std::move(functor));
}

void VideoCaptureDeviceOHOS::Resume() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto functor =
      base::BindOnce(&OHOSCaptureDelegate::Resume, capture_impl_->GetWeakPtr());
  if (!capture_thread_.IsRunning()) {
    // We have to wait until we get the device AllocateAndStart()ed.
    photo_requests_queue_.push_back(std::move(functor));
    return;
  }
  capture_thread_.task_runner()->PostTask(FROM_HERE, std::move(functor));
}

}  // namespace media
