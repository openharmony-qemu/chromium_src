// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on gl_factory_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "base/check.h"
#include "base/notreached.h"
#include "base/trace_event/trace_event.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_context_egl.h"
#include "ui/gl/gl_context_stub.h"
#include "ui/gl/gl_egl_api_implementation.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_share_group.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_surface_egl_ohos.h"
#include "ui/gl/gl_surface_stub.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/init/ozone_util.h"
#include "ui/gl/presenter.h"

namespace gl {
namespace init {

std::vector<GLImplementationParts> GetAllowedGLImplementations() {
  return std::vector<gl::GLImplementationParts>{
      gl::GLImplementationParts(gl::kGLImplementationEGLGLES2),
      gl::GLImplementationParts(gl::kGLImplementationEGLANGLE),
  };
}

bool GetGLWindowSystemBindingInfo(const GLVersionInfo& gl_info,
                                  GLWindowSystemBindingInfo* info) {
  switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
    default:
      return false;
  }
}

scoped_refptr<GLContext> CreateGLContext(GLShareGroup* share_group,
                                         GLSurface* compatible_surface,
                                         const GLContextAttribs& attribs) {
  TRACE_EVENT0("gpu", "gl::init::CreateGLContext");
  switch (GetGLImplementation()) {
    case kGLImplementationMockGL:
      return scoped_refptr<GLContext>(new GLContextStub(share_group));
    case kGLImplementationStubGL: {
      scoped_refptr<GLContextStub> stub_context =
          new GLContextStub(share_group);
      stub_context->SetUseStubApi(true);
      return stub_context;
    }
    case kGLImplementationDisabled:
      NOTREACHED();
      return nullptr;
    default:
      if (!compatible_surface) {
        LOG(ERROR) << "surface is nullptr.";
        return nullptr;
      }
      if (compatible_surface->GetHandle() ||
          compatible_surface->IsSurfaceless()) {
        return InitializeGLContext(new GLContextEGL(share_group),
                                   compatible_surface, attribs);
      }
  }
  return nullptr;
}

scoped_refptr<GLSurface> CreateViewGLSurface(GLDisplay* display,
                                             gfx::AcceleratedWidget window) {
  TRACE_EVENT0("gpu", "gl::init::CreateViewGLSurface");
  switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE:
      if (window != gfx::kNullAcceleratedWidget) {
        return InitializeGLSurface(
            NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
                display, window));
      } else {
        return InitializeGLSurface(new GLSurfaceStub());
      }
    default:
      NOTREACHED();
      return nullptr;
  }
}

scoped_refptr<Presenter> CreateSurfacelessViewGLSurface(
    GLDisplay* display,
    gfx::AcceleratedWidget window) {
  return nullptr;
}

scoped_refptr<GLSurface> CreateOffscreenGLSurfaceWithFormat(
    GLDisplay* display,
    const gfx::Size& size,
    GLSurfaceFormat format) {
  TRACE_EVENT0("gpu", "gl::init::CreateOffscreenGLSurfaceWithFormat");
  CHECK_NE(kGLImplementationNone, GetGLImplementation());
  switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE: {
      GLDisplayEGL* display_egl = display->GetAs<gl::GLDisplayEGL>();
      if (display_egl->IsEGLSurfacelessContextSupported() &&
          (size.width() == 0 && size.height() == 0)) {
        LOG(INFO) << "create offscreen glsurface by surfacelessEGL.";
        return InitializeGLSurfaceWithFormat(
            new SurfacelessEGL(display_egl, size), format);
      } else {
        LOG(INFO) << "create offscreen glsurface by pbufferGL.";
        return InitializeGLSurfaceWithFormat(
            new PbufferGLSurfaceEGL(display_egl, size), format);
      }
    }
    case kGLImplementationMockGL:
    case kGLImplementationStubGL:
      return new GLSurfaceStub;
    default:
      NOTREACHED();
      return nullptr;
  }
}

scoped_refptr<GLSurface> CreateOffscreenGLSurface(gl::GLDisplay* display,
                                                  const gfx::Size& size) {
  return CreateOffscreenGLSurfaceWithFormat(display, size, GLSurfaceFormat());
}

void SetDisabledExtensionsPlatform(const std::string& disabled_extensions) {
  GLImplementation implementation = GetGLImplementation();
  DCHECK_NE(kGLImplementationNone, implementation);
  switch (implementation) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE:
      SetDisabledExtensionsEGL(disabled_extensions);
      break;
    case kGLImplementationMockGL:
    case kGLImplementationStubGL:
      break;
    default:
      NOTREACHED();
  }
}

bool InitializeExtensionSettingsOneOffPlatform(GLDisplay* display) {
  GLImplementation implementation = GetGLImplementation();
  DCHECK_NE(kGLImplementationNone, implementation);
  switch (implementation) {
    case kGLImplementationEGLGLES2:
    case kGLImplementationEGLANGLE:
      return InitializeExtensionSettingsOneOffEGL(
          static_cast<GLDisplayEGL*>(display));
    case kGLImplementationMockGL:
    case kGLImplementationStubGL:
      return true;
    default:
      NOTREACHED();
      return false;
  }
}

}  // namespace init
}  // namespace gl
