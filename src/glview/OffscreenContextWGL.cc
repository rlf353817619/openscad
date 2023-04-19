#include "OffscreenContextWGL.h"

#include <iostream>
#include <sstream>

#undef NOGDI // To access ChoosePixelFormat, PIXELFORMATDESCRIPTOR, etc.
#include <windows.h>
#include <wingdi.h>
#define GLAD_WGL
#define GLAD_WGL_IMPLEMENTATION
#include <glad/wgl.h>

class OffscreenContextWGL : public OffscreenContext {

public:
  HWND window = nullptr;
  HDC devContext = nullptr;
  HGLRC renderContext = nullptr;

  OffscreenContextWGL(int width, int height) : OffscreenContext(width, height) {}
  ~OffscreenContextWGL() {
    wglMakeCurrent(nullptr, nullptr);
    if (this->renderContext) wglDeleteContext(this->renderContext);
    if (this->devContext) ReleaseDC(this->window, this->devContext);
    if (this->window) DestroyWindow(this->window);
  }

  std::string getInfo() const override {
    std::stringstream result;
    // should probably get some info from WGL context here?
    result << "GL context creator: WGL (new)\n"
          << "PNG generator: lodepng\n";

    return result.str();
  }

  bool makeCurrent() const override {
    wglMakeCurrent(this->devContext, this->renderContext);
    return true;
  }
};

std::shared_ptr<OffscreenContext> CreateOffscreenContextWGL(size_t width, size_t height,
							    size_t majorGLVersion, size_t minorGLVersion, bool compatibilityProfile)
{
  auto ctx = std::make_shared<OffscreenContextWGL>(width, height);

  WNDCLASSEX wndClass = {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_OWNDC,
    .lpfnWndProc = &DefWindowProc,
    .lpszClassName = L"OffscreenClass"
  };
  // FIXME: Check for ERROR_CLASS_ALREADY_EXISTS ?
  RegisterClassEx(&wndClass);
  // Create the window. Position and size it.
  // Style the window and remove the caption bar (WS_POPUP)
  ctx->window = CreateWindowEx(0, L"OffscreenClass", L"offscreen", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, 0, 0);
  ctx->devContext = GetDC(ctx->window);

  PIXELFORMATDESCRIPTOR pixelFormatDesc = {
    .nSize = sizeof(PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    // FIXME: Can we remove PFD_DOUBLEBUFFER for offscreen rendering?
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cDepthBits = 24,
    .cStencilBits = 8
  };
  int pixelFormat = ChoosePixelFormat(ctx->devContext, &pixelFormatDesc);
  SetPixelFormat(ctx->devContext, pixelFormat, &pixelFormatDesc);
  // FIXME: Use wglChoosePixelFormatARB() if appropriate

  const auto tmpRenderContext = wglCreateContext(ctx->devContext);
  if (tmpRenderContext == nullptr) {
    std::cerr << "wglCreateContext() failed: " << GetLastError() << std::endl;
    return nullptr;
  }
  wglMakeCurrent(ctx->devContext, tmpRenderContext);
  auto wglVersion = gladLoaderLoadWGL(ctx->devContext);
  std::cout << "GLAD: Loaded WGL " << GLAD_VERSION_MAJOR(wglVersion) << "." << GLAD_VERSION_MINOR(wglVersion) << std::endl;
  // FIXME: If version == 0, GLAD failed and we cannot use any extensions (or WGL at all?)
  // We need to check if wglCreateContextAttribsARB == 0. When is this function available? WGL 1.0 or is it an extension?
  
  int attributes[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, majorGLVersion,
    WGL_CONTEXT_MINOR_VERSION_ARB, minorGLVersion,
    WGL_CONTEXT_PROFILE_MASK_ARB,
    compatibilityProfile ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB,         
    0
  };  
  ctx->renderContext = wglCreateContextAttribsARB(ctx->devContext, nullptr, attributes);
  std::cout << "PPP" << std::endl;
  if (ctx->renderContext == nullptr) {
    std::cerr << "wglCreateContextAttribsARB() failed: " << GetLastError() << std::endl;
    return nullptr;
  }

  wglMakeCurrent(nullptr, nullptr);
  if (!wglDeleteContext(tmpRenderContext)) {
    std::cerr << "wglDeleteContext() failed: " << GetLastError() << std::endl;
    return nullptr;
  }
  return ctx;
}
