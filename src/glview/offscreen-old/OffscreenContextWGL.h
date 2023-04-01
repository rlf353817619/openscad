#pragma once

#include <memory>

#include "OffscreenContext.h"

namespace offscreen_old {

std::shared_ptr<OffscreenContext> CreateOffscreenContextWGL(
    unsigned int width, unsigned int height, unsigned int majorGLVersion, 
    unsigned int minorGLVersion, bool compatibilityProfile);

}  // namespace offscreen_old
