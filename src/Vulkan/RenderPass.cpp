#include "RenderPass.hpp"

RenderPass::RenderPass(Device* device, Swapchain* swapchain) {
    // Getting all the necessary data
    vk::Format colorFormat = swapchain->getColorFormat();
    vk::Format depthFormat = device->getDepthFormat();
    vk::SampleCountFlagBits multiSamplingLevel = device->getMultiSamplingLevel();

    // Color attachment
    vk::AttachmentDescription colorAttachment(vk::AttachmentDescriptionFlags(),
                                              colorFormat,                               
                                              multiSamplingLevel,                        
                                              vk::AttachmentLoadOp::eClear,              
                                              vk::AttachmentStoreOp::eStore,             
                                              vk::AttachmentLoadOp::eDontCare,           
                                              vk::AttachmentStoreOp::eDontCare,          
                                              vk::ImageLayout::eUndefined,               
                                              vk::ImageLayout::eColorAttachmentOptimal);

    // Depth testing attachment
    vk::AttachmentDescription depthAttachment(vk::AttachmentDescriptionFlags(),                 
                                                     depthFormat,                                      
                                                     multiSamplingLevel,                               
                                                     vk::AttachmentLoadOp::eClear,                     
                                                     vk::AttachmentStoreOp::eDontCare,                 
                                                     vk::AttachmentLoadOp::eDontCare,                  
                                                     vk::AttachmentStoreOp::eDontCare,                 
                                                     vk::ImageLayout::eUndefined,                      
                                                     vk::ImageLayout::eDepthStencilAttachmentOptimal);
    // Multisampling attachment
    vk::AttachmentDescription multiSamplingAttachment(vk::AttachmentDescriptionFlags(), 
                                                      colorFormat,                      
                                                      vk::SampleCountFlagBits::e1,      
                                                      vk::AttachmentLoadOp::eDontCare,  
                                                      vk::AttachmentStoreOp::eStore,    
                                                      vk::AttachmentLoadOp::eDontCare,  
                                                      vk::AttachmentStoreOp::eDontCare, 
                                                      vk::ImageLayout::eUndefined,      
                                                      vk::ImageLayout::ePresentSrcKHR);

    // Attachment references
    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference multiSamplingReference(2, vk::ImageLayout::eColorAttachmentOptimal);

    // Compress all attachment descriptions into a vector
    std::vector<vk::AttachmentDescription> attachments = {
        colorAttachment,
        depthAttachment,
        multiSamplingAttachment
    };

    // Subpass description
    vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(),
                                   vk::PipelineBindPoint::eGraphics, 
                                   0,                                 
                                   nullptr,                           
                                   1,                                 
                                   &colorReference,         
                                   &multiSamplingReference, 
                                   &depthReference,  
                                   0,                                 
                                   nullptr);

    // Subpass dependencies
    vk::SubpassDependency subpassDependency(0,                                                                                    
                                            0,                                                                                    
                                            vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    
                                            vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    
                                            vk::AccessFlags(),                                                                    
                                            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, 
                                            vk::DependencyFlags(VK_DEPENDENCY_BY_REGION_BIT));

    // Render pass creation
    vk::RenderPassCreateInfo renderPassCreateInfo(vk::RenderPassCreateFlags(),
                                                  static_cast<uint32_t>(attachments.size()), 
                                                  attachments.data(),                        
                                                  1,                                         
                                                  &subpass,                                  
                                                  1,                                         
                                                  &subpassDependency);

    renderPass = device->getLogicalDevice()->createRenderPass(renderPassCreateInfo);

    #ifndef NDEBUG
        spdlog::info("Render pass successfully created.");
    #endif
} 

RenderPass::~RenderPass() {
    #ifndef NDEBUG
        spdlog::info("Render pass successfully destroyed.");
    #endif
}

vk::RenderPass* RenderPass::getRenderPass() {
    return &renderPass;
}
