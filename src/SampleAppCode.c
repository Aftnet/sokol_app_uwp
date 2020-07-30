#include "sokol/sokol_gfx.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"

#include "sokol/hhm.h"

//Only for OutputDebugStringA
#include <Windows.h>


/* a uniform block with a model-view-projection matrix */
typedef struct {
    hmm_mat4 mvp;
} vs_params_t;


static sg_buffer m_vbuf;
static sg_buffer m_ibuf;
static sg_bindings m_bind;
static sg_shader m_shd;
static sg_pipeline m_pip;
static sg_pass_action m_sg_pass_action = { 0 };

void init_handler()
{
    sg_desc desc = { 0 };
    desc.context = sapp_sgcontext();
    sg_setup(&desc);

    /* cube vertex buffer */
    float vertices[] = {
        /* positions        colors */
        -1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
         1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
         1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
        -1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

        -1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
         1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
         1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
        -1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

        -1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

        1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
        1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
        1.0,  1.0,  1.0,   1.0, 0.5, 0.0, 1.0,
        1.0, -1.0,  1.0,   1.0, 0.5, 0.0, 1.0,

        -1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
         1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
         1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

        -1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
        -1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
         1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
         1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0
    };

    sg_buffer_desc vbufDesc = { 0 };
    vbufDesc.size = sizeof(vertices);
    vbufDesc.content = vertices;
    m_vbuf = sg_make_buffer(&vbufDesc);

    /* cube indices */
    uint16_t indices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9, 10,  8, 10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20, 23, 22, 20
    };

    sg_buffer_desc ibufDesc = { 0 };
    ibufDesc.type = SG_BUFFERTYPE_INDEXBUFFER;
    ibufDesc.size = sizeof(indices);
    ibufDesc.content = indices;
    m_ibuf = sg_make_buffer(&ibufDesc);

    /* define the resource bindings */
    m_bind.vertex_buffers[0] = m_vbuf;
    m_bind.index_buffer = m_ibuf;

    /* create shader */
    sg_shader_desc shdDesc = { 0 };
    shdDesc.attrs[0].sem_name = "POSITION";
    shdDesc.attrs[1].sem_name = "COLOR";
    shdDesc.attrs[1].sem_index = 1;
    shdDesc.vs.uniform_blocks[0].size = sizeof(vs_params_t);
    shdDesc.vs.source =
        "cbuffer params: register(b0) {\n"
        "  float4x4 mvp;\n"
        "};\n"
        "struct vs_in {\n"
        "  float4 pos: POSITION;\n"
        "  float4 color: COLOR1;\n"
        "};\n"
        "struct vs_out {\n"
        "  float4 color: COLOR0;\n"
        "  float4 pos: SV_Position;\n"
        "};\n"
        "vs_out main(vs_in inp) {\n"
        "  vs_out outp;\n"
        "  outp.pos = mul(mvp, inp.pos);\n"
        "  outp.color = inp.color;\n"
        "  return outp;\n"
        "};\n";
    shdDesc.fs.source =
        "float4 main(float4 color: COLOR0): SV_Target0 {\n"
        "  return color;\n"
        "}\n";
    m_shd = sg_make_shader(&shdDesc);

    /* a pipeline object */
    sg_pipeline_desc pipDesc = { 0 };
    pipDesc.layout.buffers[0].stride = 28;
    pipDesc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pipDesc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;
    pipDesc.shader = m_shd;
    pipDesc.index_type = SG_INDEXTYPE_UINT16;
    pipDesc.depth_stencil.depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL;
    pipDesc.depth_stencil.depth_write_enabled = true;
    pipDesc.rasterizer.cull_mode = SG_CULLMODE_BACK;
    m_pip = sg_make_pipeline(&pipDesc);
}

void cleanup_handler()
{
    sg_destroy_pipeline(m_pip);
    sg_destroy_shader(m_shd);
    sg_destroy_buffer(m_ibuf);
    sg_destroy_buffer(m_vbuf);
    sg_shutdown();
}

void frame_handler()
{
    /* view-projection matrix */
    hmm_mat4 proj = HMM_Perspective(60.0f, (float)sapp_width() / (float)sapp_height(), 0.01, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

    /* model-view-proj matrix for vertex shader */
    static vs_params_t vs_params;
    static float rx = 0.0f;
    static float ry = 0.0f;
    rx += 1.0f;
    ry += 2.0f;

    hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
    vs_params.mvp = HMM_MultiplyMat4(view_proj, model);

    sg_begin_default_pass(&m_sg_pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(m_pip);
    sg_apply_bindings(&m_bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
    sg_draw(0, 36, 1);
    sg_end_pass();
    sg_commit();

    return true;
}

void event_handler(sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        switch (event->key_code)
        {
        case SAPP_KEYCODE_F:
            if (!sapp_is_fullscreen())
            {
                sapp_toggle_fullscreen();
            }
            break;
        case SAPP_KEYCODE_ESCAPE:
            if (sapp_is_fullscreen())
            {
                sapp_toggle_fullscreen();
            }
            break;
        case SAPP_KEYCODE_T:
            if (!event->key_repeat)
            {
                sapp_toggle_fullscreen();
            }
            break;
        case SAPP_KEYCODE_Q:
            sapp_request_quit();
            break;
        }
    }
}

void error_handler(const char* message)
{
    OutputDebugStringA(message);
}

sapp_desc sokol_main(int argc, char* argv[])
{
    return (sapp_desc)
    {
        .width = 640,
        .height = 480,
        .high_dpi = false,
        .fullscreen = false,
        .window_title = "Sokol_app sample",
        .init_cb = init_handler,
        .frame_cb = frame_handler,
        .cleanup_cb = cleanup_handler,
        .event_cb = event_handler
    };
}