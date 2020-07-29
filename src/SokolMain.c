#include "sokol/sokol_app.h"

void init_handler()
{

}

void frame_handler()
{

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