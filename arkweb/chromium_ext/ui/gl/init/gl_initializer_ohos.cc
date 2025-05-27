// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on gl_initializer_x11.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/trace_event/trace_event.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_display.h"
#include "ui/gl/gl_egl_api_implementation.h"
#include "ui/gl/gl_gl_api_implementation.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_display_initializer.h"
#include "ui/gl/init/gl_initializer.h"

namespace gl {
namespace init {

namespace {

bool InitializeStaticNativeEGLInternal() {
  base::NativeLibrary gles_library = LoadLibraryAndPrintError("libGLESv3.so");
  if (!gles_library) {
    return false;
  }
  base::NativeLibrary egl_library = LoadLibraryAndPrintError("libEGL.so");
  if (!egl_library) {
    base::UnloadNativeLibrary(gles_library);
    return false;
  }

  GLGetProcAddressProc get_proc_address =
      reinterpret_cast<GLGetProcAddressProc>(
          base::GetFunctionPointerFromNativeLibrary(egl_library,
                                                    "eglGetProcAddress"));
  if (!get_proc_address) {
    LOG(ERROR) << "eglGetProcAddress not found.";
    base::UnloadNativeLibrary(egl_library);
    base::UnloadNativeLibrary(gles_library);
    return false;
  }

  SetGLGetProcAddressProc(get_proc_address);
  AddGLNativeLibrary(egl_library);
  AddGLNativeLibrary(gles_library);

  return true;
}

bool InitializeStaticEGLInternal(GLImplementationParts implementation) {
  bool initialized = false;

  if (!initialized) {
    initialized = InitializeStaticNativeEGLInternal();
  }

  if (!initialized) {
    return false;
  }

  SetGLImplementationParts(implementation);

  InitializeStaticGLBindingsGL();
  InitializeStaticGLBindingsEGL();

  return true;
}

}  // namespace

GLDisplay* InitializeGLOneOffPlatform(gl::GpuPreference gpu_preference) {
  TRACE_EVENT0("gpu", "gl::init::InitializeGLOneOffPlatformohos");
  GLDisplayEGL* display = GetDisplayEGL(gpu_preference);
  switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE:
      if (!InitializeDisplay(display,
                             EGLDisplayPlatform(EGL_DEFAULT_DISPLAY))) {
        LOG(ERROR) << "GLDisplayEGL::Initialize failed.";
        return nullptr;
      }
      break;
    default:
      break;
  }
  return display;
}

bool InitializeStaticGLBindings(GLImplementationParts implementation) {
  TRACE_EVENT0("gpu", "gl::init::InitializeStaticGLBindingsohos");
  // Prevent reinitialization with a different implementation. Once the gpu
  // unit tests have initialized with kGLImplementationMock, we don't want to
  // later switch to another GL implementation.
  DCHECK_EQ(kGLImplementationNone, GetGLImplementation());

  switch (implementation.gl) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE:
      return InitializeStaticEGLInternal(implementation);
    case kGLImplementationMockGL:
    case kGLImplementationStubGL:
      SetGLImplementationParts(implementation);
      InitializeStaticGLBindingsGL();
      return true;
    default:
      NOTREACHED();
  }
}

void ShutdownGLPlatform(GLDisplay* display) {
  if (display) {
    display->Shutdown();
  }
  ClearBindingsEGL();
  ClearBindingsGL();
}

}  // namespace init
}  // namespace gl
