#pragma once

#include <memory>

#include "OffscreenContext.h"

namespace offscreen_old {

std::shared_ptr<OffscreenContext> CreateOffscreenContextNSOpenGL(
    unsigned int width, unsigned int height, unsigned int majorGLVersion, 
    unsigned int minorGLVersion);

}  // namespace offscreen_old
