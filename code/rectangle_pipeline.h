#pragma pack(push, 1)
struct rectangle_render_info {
    real_3 p;
    real_2 r;
    real_4 color;
};
#pragma pack(pop)

void create_rectangle_pipeline(pipeline_info* pipeline)
{
    int error;

    //pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vkon.global_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule vert_shader = create_shader_module_from_file("data/shaders/rectangle_vert.spv");
    VkShaderModule frag_shader = create_shader_module_from_file("data/shaders/rectangle_frag.spv");

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

    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = sizeof(rectangle_render_info),
        .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
    };

    VkVertexInputAttributeDescription attribute_descriptions[] = {
        {    .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT,
             .offset = offsetof(rectangle_render_info, p),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(rectangle_render_info, r),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(rectangle_render_info, color),    },
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
        .renderPass = vkon.ui_render_pass,
        .subpass = 0,
    };

    error = vkCreateGraphicsPipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create graphics pipeline");

    vkDestroyShaderModule(vkon.device, frag_shader, 0);
    vkDestroyShaderModule(vkon.device, vert_shader, 0);
}

pipeline_info* rectangle_pipeline = add_pipeline(create_rectangle_pipeline);

void draw_rectangles(rectangle_render_info* rectangles, int n_rectangles)
{
    uint32 f = vkon.current_frame;

    size_t size = n_rectangles*sizeof(rectangle_render_info);
    uint32 start_index = (vkon.dynamic_vertex_buffers_used[f]+sizeof(rectangle_render_info)-1)/sizeof(rectangle_render_info);
    memcpy((rectangle_render_info*) vkon.dynamic_vertex_buffers_mapped[f]+start_index,
           rectangles,
           size);
    vkon.dynamic_vertex_buffers_used[f] = (start_index+n_rectangles)*sizeof(rectangle_render_info);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, rectangle_pipeline->pipeline);
    vkCmdDraw(vkon.command_buffers[f], 4, n_rectangles, 0, start_index);
}

void draw_rectangle(real_3 p, real_2 r, real_4 color)
{
    rectangle_render_info rectangles[] = {{p, r, color}};
    draw_rectangles(rectangles, 1);
}
