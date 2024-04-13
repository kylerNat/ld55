layout(std140, binding = 0) uniform ubo {
    mat4 camera;
    mat3 camera_axes;
    vec3 camera_pos;
    float fov;
    float aspect_ratio;
    float time;
};
