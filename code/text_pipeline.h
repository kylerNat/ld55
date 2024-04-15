#pragma pack(push, 1)
struct text_render_info {
    real_2 l;
    real_2 u;
    real_2 lt;
    real_2 ut;
    real_4 color;
};
#pragma pack(pop)

void create_text_pipeline(pipeline_info* pipeline)
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
    VkShaderModule vert_shader = create_shader_module_from_file("data/shaders/text_vert.spv");
    VkShaderModule frag_shader = create_shader_module_from_file("data/shaders/text_frag.spv");

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
        .stride = sizeof(text_render_info),
        .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
    };

    VkVertexInputAttributeDescription attribute_descriptions[] = {
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(text_render_info, l),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(text_render_info, u),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(text_render_info, lt),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(text_render_info, ut),        },
        {    .binding = 0, .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(text_render_info, color),    },
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

pipeline_info* text_pipeline = add_pipeline(create_text_pipeline);

real_2 get_text_center(char* text, font_info font)
{
    int ascent, descent, line_gap = 0;
    stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &line_gap);
    real scale = stbtt_ScaleForPixelHeight(&font.info, font.size);

    real current_x = 0;
    real current_y = 0;
    real min_x = 0;
    real max_x = 0;
    real max_y = 0;
    real min_y = 0;
    for(char* c = text; *c; c++)
    {
        if(*c == '\n')
        {
            current_x = 0;
            current_y += font.size;
            continue;
        }
        stbtt_aligned_quad quad = {};
        stbtt_GetPackedQuad(font.char_data, font_resolution, font_resolution, *c,
                            &current_x, &current_y, &quad, false);
        min_x = min(min_x, quad.x0);
        max_x = max(max_x, quad.x1);
        min_y = min(min_y, current_y-descent*scale);
        max_y = max(max_y, current_y+ascent*scale);
    }
    return {0.5f*(max_x+min_x), 0.5f*(max_y+min_y)};
}

real_2 get_text_size(char* text, font_info font)
{
    int ascent, descent, line_gap = 0;
    stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &line_gap);
    real scale = stbtt_ScaleForPixelHeight(&font.info, font.size);

    real current_x = 0;
    real current_y = 0;
    real min_x = 0;
    real max_x = 0;
    real max_y = 0;
    real min_y = 0;
    for(char* c = text; *c; c++)
    {
        if(*c == '\n')
        {
            current_x = 0;
            current_y += font.size;
            continue;
        }
        stbtt_aligned_quad quad = {};
        stbtt_GetPackedQuad(font.char_data, font_resolution, font_resolution, *c,
                            &current_x, &current_y, &quad, false);
        min_x = min(min_x, quad.x0);
        max_x = max(max_x, quad.x1);
        // min_y = min(min_y, current_y+descent*scale);
        // max_y = max(max_y, current_y+ascent*scale);
        min_y = min(min_y, quad.y0);
        max_y = max(max_y, quad.y1);
    }
    real w = 1920.0f;
    real h = 1080.0f;
    return {2.0f*(max_x-min_x)/h, 2.0f*(max_y-min_y)/h};
}

void draw_text(char* text, real_2 p, real_4 color, real_2 alignment, font_info font)
{
    uint32 f = vkon.current_frame;

    uint32 start_index = (vkon.dynamic_vertex_buffers_used[f]+sizeof(text_render_info)-1)/sizeof(text_render_info);
    text_render_info* vertex_buffer = (text_render_info*) vkon.dynamic_vertex_buffers_mapped[f]+start_index;
    uint32 n_verts = 0;
    {
        real x_base = 0.5*p.x;
        real y_base = 0.5*p.y;

        real_2 center_offset = get_text_center(text, font);
        x_base -= (alignment.x+1)*center_offset.x;
        y_base -= (alignment.y+1)*center_offset.y;

        int ascent, descent, line_gap = 0;
        stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &line_gap);
        real scale = stbtt_ScaleForPixelHeight(&font.info, font.size);
        y_base += scale*ascent;

        real current_x = x_base;
        real current_y = y_base;
        for(unsigned char* c = (unsigned char*) text; *c; c++) {
            if(*c == '\n') {
                current_x = x_base;
                current_y += font.size;
                continue;
            }
            int char_index = *c;
            if(char_index >= 0x80) {
                char_index &= 0x1F;
                char_index *= 0x40;
                char_index += (*(++c))&0x3F;
            }
            stbtt_aligned_quad quad = {};
            stbtt_GetPackedQuad(font.char_data, font_resolution, font_resolution, char_index,
                                &current_x, &current_y, &quad, false);
            // quad.y0 -= 10.0;
            // quad.y1 -= 10.0;
            real w = 1920.0f;
            real h = 1080.0f;
            vertex_buffer[n_verts++] = {
                {p.x/vkon.aspect_ratio + quad.x0*2.0f/w,
                 p.y                   + quad.y0*2.0f/h},
                {p.x/vkon.aspect_ratio + quad.x1*2.0f/w,
                 p.y                   + quad.y1*2.0f/h},
                {quad.s0, quad.t0},
                {quad.s1, quad.t1},
                color,
            };
        }
    }
    vkon.dynamic_vertex_buffers_used[f] = (start_index+n_verts)*sizeof(text_render_info);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, text_pipeline->pipeline);
    vkCmdDraw(vkon.command_buffers[f], 4, n_verts, 0, start_index);
}

uint32 batched_text_start_index;
text_render_info* batched_text_vertex_buffer;
uint32 batched_text_n_verts;
void draw_text_batched_start()
{
    uint32 f = vkon.current_frame;

    batched_text_start_index = (vkon.dynamic_vertex_buffers_used[f]+sizeof(text_render_info)-1)/sizeof(text_render_info);
    batched_text_vertex_buffer = (text_render_info*) vkon.dynamic_vertex_buffers_mapped[f]+batched_text_start_index;
    batched_text_n_verts = 0;
}

void draw_text_batched(char* text, real_2 p, real_4 color, real_2 alignment, font_info font)
{
    {
        real x_base = 0.5*p.x;
        real y_base = 0.5*p.y;

        real_2 center_offset = get_text_center(text, font);
        x_base -= (alignment.x+1)*center_offset.x;
        y_base -= (alignment.y+1)*center_offset.y;

        int ascent, descent, line_gap = 0;
        stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &line_gap);
        real scale = stbtt_ScaleForPixelHeight(&font.info, font.size);
        y_base += scale*ascent;

        real current_x = x_base;
        real current_y = y_base;
        for(unsigned char* c = (unsigned char*) text; *c; c++) {
            if(*c == '\n') {
                current_x = x_base;
                current_y += font.size;
                continue;
            }
            int char_index = *c;
            if(char_index >= 0x80) {
                char_index &= 0x1F;
                char_index *= 0x40;
                char_index += (*(++c))&0x3F;
            }
            stbtt_aligned_quad quad = {};
            stbtt_GetPackedQuad(font.char_data, font_resolution, font_resolution, char_index,
                                &current_x, &current_y, &quad, false);
            // quad.y0 -= 10.0;
            // quad.y1 -= 10.0;
            real w = 1920.0f;
            real h = 1080.0f;
            batched_text_vertex_buffer[batched_text_n_verts++] = {
                {p.x/vkon.aspect_ratio + quad.x0*2.0f/w,
                 p.y                   + quad.y0*2.0f/h},
                {p.x/vkon.aspect_ratio + quad.x1*2.0f/w,
                 p.y                   + quad.y1*2.0f/h},
                {quad.s0, quad.t0},
                {quad.s1, quad.t1},
                color,
            };
        }
    }
}

void draw_text_batched_end()
{
    uint32 f = vkon.current_frame;

    vkon.dynamic_vertex_buffers_used[f] = (batched_text_start_index+batched_text_n_verts)*sizeof(text_render_info);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, text_pipeline->pipeline);
    vkCmdDraw(vkon.command_buffers[f], 4, batched_text_n_verts, 0, batched_text_start_index);
}
