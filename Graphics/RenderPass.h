/**
 * @file RenderPass.h
 * @author wangyudong
 * @brief Abstraction of a render pass.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once
#include<memory>

namespace RenderSystem
{
    class RenderPass
    {
    public:
        typedef std::shared_ptr<RenderPass> SP;

        RenderPass() {}
        virtual void SetUp() = 0;
        virtual void BeforeRender() = 0;
        virtual void AftereRender() = 0;
        virtual void Render() = 0;

        virtual ~RenderPass() {}
        inline int Priority() { return mPriority; }

    protected:
        // lower is higher
        int mPriority = 0;
    };
}