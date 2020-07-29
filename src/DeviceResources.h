#pragma once

#include <windows.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Core.h>

#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <DirectXMath.h>


// Controls all the DirectX device resources.
class DeviceResources
{
public:
	// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
	interface IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	DeviceResources();
	void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
	void SetLogicalSize(winrt::Windows::Foundation::Size logicalSize);
	void SetCurrentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations currentOrientation);
	void SetDpi(float dpi);
	void ValidateDevice();
	void HandleDeviceLost();
	void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
	void Trim();
	void Present();

	// The size of the render target, in pixels.
	winrt::Windows::Foundation::Size GetOutputSize() const { return m_outputSize; }

	// The size of the render target, in dips.
	winrt::Windows::Foundation::Size GetLogicalSize() const { return m_logicalSize; }
	float GetDpi() const { return m_effectiveDpi; }

	// D3D Accessors.
	ID3D11Device3* GetD3DDevice() const { return m_d3dDevice.get(); }
	ID3D11DeviceContext3* GetD3DDeviceContext() const { return m_d3dContext.get(); }
	IDXGISwapChain3* GetSwapChain() const { return m_swapChain.get(); }
	D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
	ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_d3dDepthStencilView.get(); }
	D3D11_VIEWPORT GetScreenViewport() const { return m_screenViewport; }
	DirectX::XMFLOAT4X4 GetOrientationTransform3D() const { return m_orientationTransform3D; }

private:
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void UpdateRenderTargetSize();
	DXGI_MODE_ROTATION ComputeDisplayRotation();

	// Direct3D objects.
	winrt::com_ptr<ID3D11Device3> m_d3dDevice;
	winrt::com_ptr<ID3D11DeviceContext3> m_d3dContext;
	winrt::com_ptr<IDXGISwapChain3> m_swapChain;

	// Direct3D rendering objects. Required for 3D.
	winrt::com_ptr<ID3D11RenderTargetView1> m_d3dRenderTargetView;
	winrt::com_ptr<ID3D11DepthStencilView> m_d3dDepthStencilView;
	D3D11_VIEWPORT m_screenViewport;

	// Cached reference to the Window.
	winrt::agile_ref< winrt::Windows::UI::Core::CoreWindow> m_window;

	// Cached device properties.
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;
	winrt::Windows::Foundation::Size m_d3dRenderTargetSize;
	winrt::Windows::Foundation::Size m_outputSize;
	winrt::Windows::Foundation::Size m_logicalSize;
	winrt::Windows::Graphics::Display::DisplayOrientations m_nativeOrientation;
	winrt::Windows::Graphics::Display::DisplayOrientations m_currentOrientation;
	float m_dpi;

	// This is the DPI that will be reported back to the app. It takes into account whether the app supports high resolution screens or not.
	float m_effectiveDpi;

	// Transforms used for display orientation.
	DirectX::XMFLOAT4X4 m_orientationTransform3D;

	// The IDeviceNotify can be held directly as it owns the DeviceResources.
	IDeviceNotify* m_deviceNotify;

	// Swapchain Rotation Matrices (Z-rotation)
	static const DirectX::XMFLOAT4X4 m_rotation0;
	static const DirectX::XMFLOAT4X4 m_rotation90;
	static const DirectX::XMFLOAT4X4 m_rotation180;
	static const DirectX::XMFLOAT4X4 m_rotation270;
};