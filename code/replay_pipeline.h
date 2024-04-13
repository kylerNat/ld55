void create_replay_pipeline(pipeline_info* pipeline)
{
    int error;

    //pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vkon.postprocess_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule vert_shader = create_shader_module_from_file("data/shaders/fullscreen_image_vert.spv");
    VkShaderModule frag_shader = create_shader_module_from_file("data/shaders/fullscreen_image_frag.spv");

    VkPipelineShaderStageCreateInfo vert_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo frag_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_info, frag_info};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = len(dynamic_states),
        .pDynamicStates = dynamic_states,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = 0,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = (VK_COLOR_COMPONENT_R_BIT
                           | VK_COLOR_COMPONENT_G_BIT
                           | VK_COLOR_COMPONENT_B_BIT
                           | VK_COLOR_COMPONENT_A_BIT),
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blending,
        .pDynamicState = &dynamic_state,
        .layout = pipeline->pipeline_layout,
        .renderPass = vkon.replay_render_pass,
        .subpass = 0,
    };

    error = vkCreateGraphicsPipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create graphics pipeline");

    vkDestroyShaderModule(vkon.device, frag_shader, 0);
    vkDestroyShaderModule(vkon.device, vert_shader, 0);
}

pipeline_info* replay_pipeline = add_pipeline(create_replay_pipeline);

//draw replay///////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
struct replay_frame_render_info {
    real_2 p;
    real scale;
    uint32 frame_index;
};
#pragma pack(pop)

void create_draw_replay_pipeline(pipeline_info* pipeline)
{
    int error;

    VkDescriptorSetLayout descriptor_set_layouts[] = {
        vkon.global_descriptor_set_layout,
        vkon.replay_descriptor_set_layout,
    };

    //pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = len(descriptor_set_layouts),
        .pSetLayouts = descriptor_set_layouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule vert_shader = create_shader_module_from_file("data/shaders/draw_replay_frame_vert.spv");
    VkShaderModule frag_shader = create_shader_module_from_file("data/shaders/draw_replay_frame_frag.spv");

    VkPipelineShaderStageCreateInfo vert_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader,
        .pName = "main",
    };

    VkSpecializationMapEntry map_entries[] = {
        {0, 0, sizeof(uint)},
    };
    uint specialization_data[] = {vkon.max_replay_frames};
    VkSpecializationInfo frag_specialization = {
        .mapEntryCount = len(map_entries),
        .pMapEntries = map_entries,
        .dataSize = sizeof(specialization_data),
        .pData = specialization_data,
    };

    VkPipelineShaderStageCreateInfo frag_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader,
        .pName = "main",
        .pSpecializationInfo = &frag_specialization,
    };

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_info, frag_info};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = len(dynamic_states),
        .pDynamicStates = dynamic_states,
    };

    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = sizeof(replay_frame_render_info),
        .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
    };

    VkVertexInputAttributeDescription attribute_descriptions[] = {
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(replay_frame_render_info, p),              },
        {    .binding = 0, .format = VK_FORMAT_R32_SFLOAT,
             .offset = offsetof(replay_frame_render_info, scale),          },
        {    .binding = 0, .format = VK_FORMAT_R32_UINT,
             .offset = offsetof(replay_frame_render_info, frame_index),    },
    };

    for(int i = 0; i < len(attribute_descriptions); i++) attribute_descriptions[i].location = i;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = len(attribute_descriptions),
        .pVertexAttributeDescriptions = attribute_descriptions,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = 0,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = (VK_COLOR_COMPONENT_R_BIT
                           | VK_COLOR_COMPONENT_G_BIT
                           | VK_COLOR_COMPONENT_B_BIT
                           | VK_COLOR_COMPONENT_A_BIT),
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blending,
        .pDynamicState = &dynamic_state,
        .layout = pipeline->pipeline_layout,
        .renderPass = vkon.render_pass,
        .subpass = 0,
    };

    error = vkCreateGraphicsPipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create graphics pipeline");

    vkDestroyShaderModule(vkon.device, frag_shader, 0);
    vkDestroyShaderModule(vkon.device, vert_shader, 0);
}

pipeline_info _draw_replay_pipeline = {};
pipeline_info* draw_replay_pipeline = &_draw_replay_pipeline;

void draw_replay_frames(replay_frame_render_info* replay_frames, int n_replay_frames)
{
    uint32 f = vkon.current_frame;

    vkCmdBindDescriptorSets(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, draw_replay_pipeline->pipeline_layout, 1, 1, &vkon.replay_descriptor_set, 0, 0);

    size_t size = n_replay_frames*sizeof(replay_frame_render_info);
    uint32 start_index = (vkon.dynamic_vertex_buffers_used[f]+sizeof(replay_frame_render_info)-1)/sizeof(replay_frame_render_info);
    memcpy((replay_frame_render_info*) vkon.dynamic_vertex_buffers_mapped[f]+start_index,
           replay_frames,
           size);
    vkon.dynamic_vertex_buffers_used[f] = (start_index+n_replay_frames)*sizeof(replay_frame_render_info);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, draw_replay_pipeline->pipeline);
    vkCmdDraw(vkon.command_buffers[f], 4, n_replay_frames, 0, start_index);
}

void export_gif(char* filename, int first_frame, int n_frames)
{
    MsfGifState gif_state = {};
    msf_gif_begin(&gif_state, vkon.replay_resolution.x, vkon.replay_resolution.y);
    int bit_depth = 16;

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0,0,0},
        .imageExtent = {vkon.replay_resolution.x, vkon.replay_resolution.y, 1},
    };

    VkDeviceSize image_size = vkon.replay_resolution.x*vkon.replay_resolution.y*4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_vk_buffer(image_size,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &staging_buffer,
                     &staging_buffer_memory);
    void* data;
    vkMapMemory(vkon.device, staging_buffer_memory, 0, image_size, 0, &data);

    VkAccessFlags dst_access_mask = VK_ACCESS_TRANSFER_READ_BIT;
    VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    VkPipelineStageFlags dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkAccessFlags src_access_mask = 0;
    VkImageLayout old_layout = {};
    VkPipelineStageFlags src_stage = 0;
    switch(vkon.replay_mode) {
        case 0: {
            src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        } break;
        case 1: {
            src_access_mask = VK_ACCESS_SHADER_READ_BIT;
            old_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } break;
    }

    for(int i = 0; i < n_frames; i++) {
        VkCommandBuffer command_buffer = begin_single_time_command_buffer();

        int index = (first_frame+i+vkon.max_replay_frames)%vkon.max_replay_frames;

        {
            VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = src_access_mask,
                .dstAccessMask = dst_access_mask,
                .oldLayout = old_layout,
                .newLayout = new_layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vkon.replay_images[index],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };
            vkCmdPipelineBarrier(command_buffer,
                                 src_stage,
                                 dst_stage,
                                 0,
                                 0, 0,
                                 0, 0,
                                 1, &barrier);
        }

        vkCmdCopyImageToBuffer(command_buffer, vkon.replay_images[index], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging_buffer, 1, &region);

        {
            VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = dst_access_mask,
                .dstAccessMask = src_access_mask,
                .oldLayout = new_layout,
                .newLayout = old_layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vkon.replay_images[index],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };
            vkCmdPipelineBarrier(command_buffer,
                                 dst_stage,
                                 src_stage,
                                 0,
                                 0, 0,
                                 0, 0,
                                 1, &barrier);
        }

        end_single_time_command_buffer();

        msf_gif_frame(&gif_state, (uint8*) data, vkon.replay_centiseconds, bit_depth, 4*vkon.replay_resolution.x);
    }
    vkUnmapMemory(vkon.device, staging_buffer_memory);
    vkDestroyBuffer(vkon.device, staging_buffer, 0);
    vkFreeMemory(vkon.device, staging_buffer_memory, 0);

    MsfGifResult result = msf_gif_end(&gif_state);
    if(result.data)
    {
        FILE* f = fopen(filename, "wb");
        if(f)
        {
            fwrite(result.data, result.dataSize, 1, f);
            fclose(f);
        }
        else log_warning("could not open ", filename, " to save gif recording\n");
    }
    else log_warning("could not create gif recording\n");
    msf_gif_free(result);
}
