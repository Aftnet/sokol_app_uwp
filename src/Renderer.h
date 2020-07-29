#pragma once

#include "DeviceResources.h"

#include "sokol/sokol_gfx.h"
#include <memory>

//NEVER INSTANTIATE MORE THAN ONE AT A TIME
class Renderer
{
public:
	Renderer(const std::shared_ptr<DeviceResources>& deviceResources);
	~Renderer();

	bool Render();

private:
	// Cached pointer to device resources. Shared because of need to interface with C
	static std::shared_ptr<DeviceResources> m_deviceResources;

	// Sokol objects
	sg_buffer m_vbuf = { 0 };
	sg_buffer m_ibuf = { 0 };
	sg_shader m_shd = { 0 };
	sg_bindings m_bind = { 0 };
	sg_pipeline m_pip = { 0 };
	sg_pass_action m_sg_pass_action = { 0 };
};