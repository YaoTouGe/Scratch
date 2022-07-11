/**
 * @file RenderTexutre.h
 * @author wangyudong
 * @brief Used as render target to do off-screen rendering and so on, just like FBO.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>

namespace Graphics
{
    class RenderTexture
    {
    public:
        typedef std::shared_ptr<RenderTexture>  SP;

        uint32_t GetHandle();
    };
}