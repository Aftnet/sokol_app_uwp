#include "sokol/sokol_gfx.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"

void init_handler()
{
    sg_context_desc app_context = sapp_sgcontext();
    sg_setup(&app_context);
}

void frame_handler()
{
    static sg_pass_action default_pass_action = { 0 };
    sg_begin_default_pass(&default_pass_action, sapp_width(), sapp_height());

    sg_end_pass();
    sg_commit();
}

void cleanup_handler()
{

}

void event_handler(sapp_event* event)
{

}

sapp_desc sokol_main(int argc, char* argv[])
{
    return (sapp_desc)
    {
        .width = 640,
        .height = 480,
        .init_cb = init_handler,
        .frame_cb = frame_handler,
        .cleanup_cb = cleanup_handler,
        .event_cb = event_handler
    };
}