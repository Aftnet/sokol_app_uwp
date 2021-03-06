#define SOKOL_IMPL
#define SOKOL_D3D11
#include "sokol_app.h"

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
	// DPI scaling behavior constants
	static const bool m_supportHighResolutions;
	static const float m_dpiThreshold;
	static const float m_widthThreshold;
	static const float m_heightThreshold;

	// Swapchain Rotation Matrices (Z-rotation)
	static const DirectX::XMFLOAT4X4 m_rotation0;
	static const DirectX::XMFLOAT4X4 m_rotation90;
	static const DirectX::XMFLOAT4X4 m_rotation180;
	static const DirectX::XMFLOAT4X4 m_rotation270;

	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void UpdateRenderTargetSize();
	DXGI_MODE_ROTATION ComputeDisplayRotation();
	float ConvertDipsToPixels(float dips, float dpi);
	bool SdkLayersAvailable();

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
};

// NEVER INSTANTIATE MORE THAN ONE AT A TIME
class Renderer
{
public:
	Renderer(const std::shared_ptr<DeviceResources>& deviceResources);
	~Renderer();

	bool Render();

private:
	// Cached pointer to device resources. Shared because of need to interface with C
	static std::shared_ptr<DeviceResources> m_deviceResources;
};

std::shared_ptr<DeviceResources> Renderer::m_deviceResources;

// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
public:
	App();

	// IFrameworkViewSource Methods
	winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView() { return *this; }

	// IFrameworkView Methods.
	virtual void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
	virtual void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
	virtual void Load(winrt::hstring const& entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	// Application lifecycle event handlers.
	void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
	void OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
	void OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);

	// Window event handlers.
	void OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args);
	void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);
	void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args);

	// DisplayInformation event handlers.
	void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
	void OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
	void OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);

private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<Renderer> m_renderer;
	bool m_windowClosed;
	bool m_windowVisible;
};






// High resolution displays can require a lot of GPU and battery power to render.
// High resolution phones, for example, may suffer from poor battery life if
// games attempt to render at 60 frames per second at full fidelity.
// The decision to render at full fidelity across all platforms and form factors
// should be deliberate.
const bool DeviceResources::m_supportHighResolutions = false;

// The default thresholds that define a "high resolution" display. If the thresholds
// are exceeded and SupportHighResolutions is false, the dimensions will be scaled
// by 50%.
const float DeviceResources::m_dpiThreshold = 192.0f;		// 200% of standard desktop display.
const float DeviceResources::m_widthThreshold = 1920.0f;	// 1080p width.
const float DeviceResources::m_heightThreshold = 1080.0f;	// 1080p height.


const DirectX::XMFLOAT4X4 DeviceResources::m_rotation0(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

// 90-degree Z-rotation
const DirectX::XMFLOAT4X4 DeviceResources::m_rotation90(
	0.0f, 1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

// 180-degree Z-rotation
const DirectX::XMFLOAT4X4 DeviceResources::m_rotation180(
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

// 270-degree Z-rotation
const DirectX::XMFLOAT4X4 DeviceResources::m_rotation270(
	0.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);


// Constructor for DeviceResources.
DeviceResources::DeviceResources() :
	m_screenViewport(),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_d3dRenderTargetSize(),
	m_outputSize(),
	m_logicalSize(),
	m_nativeOrientation(winrt::Windows::Graphics::Display::DisplayOrientations::None),
	m_currentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations::None),
	m_dpi(-1.0f),
	m_effectiveDpi(-1.0f),
	m_deviceNotify(nullptr)
{
	CreateDeviceResources();
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DeviceResources::CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (SdkLayersAvailable())
	{
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	winrt::com_ptr<ID3D11Device> device;
	winrt::com_ptr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		device.put(),				// Returns the Direct3D device created.
		&m_d3dFeatureLevel,			// Returns feature level of device created.
		context.put()				// Returns the device immediate context.
	);

	if (FAILED(hr))
	{
		// If the initialization fails, fall back to the WARP device.
		// For more information on WARP, see: 
		// https://go.microsoft.com/fwlink/?LinkId=286690
		winrt::check_hresult(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				device.put(),
				&m_d3dFeatureLevel,
				context.put()
			)
		);
	}

	// Store pointers to the Direct3D 11.3 API device and immediate context.
	m_d3dDevice = device.as<ID3D11Device3>();
	m_d3dContext = context.as<ID3D11DeviceContext3>();
}

// These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowSizeDependentResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView = nullptr;
	m_d3dDepthStencilView = nullptr;
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	UpdateRenderTargetSize();

	// The width and height of the swap chain must be based on the window's
	// natively-oriented width and height. If the window is not in the native
	// orientation, the dimensions must be reversed.
	DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

	bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	m_d3dRenderTargetSize.Width = swapDimensions ? m_outputSize.Height : m_outputSize.Width;
	m_d3dRenderTargetSize.Height = swapDimensions ? m_outputSize.Width : m_outputSize.Height;

	if (m_swapChain != nullptr)
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = m_swapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			lround(m_d3dRenderTargetSize.Width),
			lround(m_d3dRenderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			winrt::check_hresult(hr);
		}
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SCALING scaling = m_supportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = lround(m_d3dRenderTargetSize.Width);		// Match the size of the window.
		swapChainDesc.Height = lround(m_d3dRenderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;								// Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;									// Use double-buffering to minimize latency.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Microsoft Store apps must use this SwapEffect.
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = scaling;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		winrt::com_ptr<IDXGIDevice3> dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();

		winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
		winrt::check_hresult(dxgiDevice->GetAdapter(dxgiAdapter.put()));

		winrt::com_ptr<IDXGIFactory4> dxgiFactory;
		winrt::check_hresult(dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), dxgiFactory.put_void()));

		winrt::com_ptr<IDXGISwapChain1> swapChain;
		winrt::check_hresult(dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice.get(), m_window.get().as<::IUnknown>().get(), &swapChainDesc, nullptr, swapChain.put()));

		m_swapChain = swapChain.as<IDXGISwapChain3>();

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		winrt::check_hresult(dxgiDevice->SetMaximumFrameLatency(1));
	}

	// Set the proper orientation for the swap chain, and generate 2D and
	// 3D matrix transformations for rendering to the rotated swap chain.
	// Note the rotation angle for the 2D and 3D transforms are different.
	// This is due to the difference in coordinate spaces.  Additionally,
	// the 3D matrix is specified explicitly to avoid rounding errors.

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		m_orientationTransform3D = m_rotation0;
		break;

	case DXGI_MODE_ROTATION_ROTATE90:
		m_orientationTransform3D = m_rotation270;
		break;

	case DXGI_MODE_ROTATION_ROTATE180:
		m_orientationTransform3D = m_rotation180;
		break;

	case DXGI_MODE_ROTATION_ROTATE270:
		m_orientationTransform3D = m_rotation90;
		break;

	default:
		throw std::invalid_argument("Unexpected display rotation");
	}

	winrt::check_hresult(m_swapChain->SetRotation(displayRotation));

	// Create a render target view of the swap chain back buffer.
	winrt::com_ptr<ID3D11Texture2D1> backBuffer;
	winrt::check_hresult(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	winrt::check_hresult(m_d3dDevice->CreateRenderTargetView1(backBuffer.get(), nullptr, m_d3dRenderTargetView.put()));

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(m_d3dRenderTargetSize.Width),
		lround(m_d3dRenderTargetSize.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	winrt::com_ptr<ID3D11Texture2D1> depthStencil;
	winrt::check_hresult(m_d3dDevice->CreateTexture2D1(&depthStencilDesc, nullptr, depthStencil.put()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	winrt::check_hresult(
		m_d3dDevice->CreateDepthStencilView(
			depthStencil.get(),
			&depthStencilViewDesc,
			m_d3dDepthStencilView.put()
		)
	);

	// Set the 3D rendering viewport to target the entire window.
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		m_d3dRenderTargetSize.Width,
		m_d3dRenderTargetSize.Height
	);

	m_d3dContext->RSSetViewports(1, &m_screenViewport);
}

// Determine the dimensions of the render target and whether it will be scaled down.
void DeviceResources::UpdateRenderTargetSize()
{
	m_effectiveDpi = m_dpi;

	// To improve battery life on high resolution devices, render to a smaller render target
	// and allow the GPU to scale the output when it is presented.
	if (!m_supportHighResolutions && m_dpi > m_dpiThreshold)
	{
		float width = ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
		float height = ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

		// When the device is in portrait orientation, height > width. Compare the
		// larger dimension against the width threshold and the smaller dimension
		// against the height threshold.
		if (max(width, height) > m_widthThreshold && min(width, height) > m_heightThreshold)
		{
			// To scale the app we change the effective DPI. Logical size does not change.
			m_effectiveDpi /= 2.0f;
		}
	}

	// Calculate the necessary render target size in pixels.
	m_outputSize.Width = ConvertDipsToPixels(m_logicalSize.Width, m_effectiveDpi);
	m_outputSize.Height = ConvertDipsToPixels(m_logicalSize.Height, m_effectiveDpi);

	// Prevent zero size DirectX content from being created.
	m_outputSize.Width = max(m_outputSize.Width, 1);
	m_outputSize.Height = max(m_outputSize.Height, 1);
}

// This method is called when the CoreWindow is created (or re-created).
void DeviceResources::SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
{
	auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

	m_window = window;
	m_logicalSize = winrt::Windows::Foundation::Size(window.Bounds().Width, window.Bounds().Height);
	m_nativeOrientation = currentDisplayInformation.NativeOrientation();
	m_currentOrientation = currentDisplayInformation.CurrentOrientation();
	m_dpi = currentDisplayInformation.LogicalDpi();

	CreateWindowSizeDependentResources();
}

// This method is called in the event handler for the SizeChanged event.
void DeviceResources::SetLogicalSize(winrt::Windows::Foundation::Size logicalSize)
{
	if (m_logicalSize != logicalSize)
	{
		m_logicalSize = logicalSize;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DpiChanged event.
void DeviceResources::SetDpi(float dpi)
{
	if (dpi != m_dpi)
	{
		m_dpi = dpi;

		// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
		auto window = m_window.get();
		m_logicalSize = winrt::Windows::Foundation::Size(window.Bounds().Width, window.Bounds().Height);

		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the OrientationChanged event.
void DeviceResources::SetCurrentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations currentOrientation)
{
	if (m_currentOrientation != currentOrientation)
	{
		m_currentOrientation = currentOrientation;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DeviceResources::ValidateDevice()
{
	// The D3D Device is no longer valid if the default adapter changed since the device
	// was created or if the device has been removed.

	// First, get the information for the default adapter from when the device was created.

	winrt::com_ptr<IDXGIDevice3> dxgiDevice = m_d3dDevice.as< IDXGIDevice3>();

	winrt::com_ptr<IDXGIAdapter> deviceAdapter;
	winrt::check_hresult(dxgiDevice->GetAdapter(deviceAdapter.put()));

	winrt::com_ptr<IDXGIFactory4> deviceFactory;
	winrt::check_hresult(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

	winrt::com_ptr<IDXGIAdapter1> previousDefaultAdapter;
	winrt::check_hresult(deviceFactory->EnumAdapters1(0, previousDefaultAdapter.put()));

	DXGI_ADAPTER_DESC1 previousDesc;
	winrt::check_hresult(previousDefaultAdapter->GetDesc1(&previousDesc));

	// Next, get the information for the current default adapter.

	winrt::com_ptr<IDXGIFactory4> currentFactory;
	winrt::check_hresult(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

	winrt::com_ptr<IDXGIAdapter1> currentDefaultAdapter;
	winrt::check_hresult(currentFactory->EnumAdapters1(0, currentDefaultAdapter.put()));

	DXGI_ADAPTER_DESC1 currentDesc;
	winrt::check_hresult(currentDefaultAdapter->GetDesc1(&currentDesc));

	// If the adapter LUIDs don't match, or if the device reports that it has been removed,
	// a new D3D device must be created.

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(m_d3dDevice->GetDeviceRemovedReason()))
	{
		// Release references to resources related to the old device.
		dxgiDevice = nullptr;
		deviceAdapter = nullptr;
		deviceFactory = nullptr;
		previousDefaultAdapter = nullptr;

		// Create a new device and swap chain.
		HandleDeviceLost();
	}
}

// Recreate all device resources and set them back to the current state.
void DeviceResources::HandleDeviceLost()
{
	m_swapChain = nullptr;

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceLost();
	}

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

// Register our DeviceNotify to be informed on device lost and creation.
void DeviceResources::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
{
	m_deviceNotify = deviceNotify;
}

// Call this method when the app suspends. It provides a hint to the driver that the app 
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DeviceResources::Trim()
{
	m_d3dDevice.as<IDXGIDevice3>()->Trim();
}

// Present the contents of the swap chain to the screen.
void DeviceResources::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	DXGI_PRESENT_PARAMETERS parameters = { 0 };
	HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	m_d3dContext->DiscardView1(m_d3dRenderTargetView.get(), nullptr, 0);

	// Discard the contents of the depth stencil.
	m_d3dContext->DiscardView1(m_d3dDepthStencilView.get(), nullptr, 0);

	// If the device was removed either by a disconnection or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}
	else
	{
		winrt::check_hresult(hr);
	}
}

// This method determines the rotation between the display device's native orientation and the
// current display orientation.
DXGI_MODE_ROTATION DeviceResources::ComputeDisplayRotation()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	// Note: NativeOrientation can only be Landscape or Portrait even though
	// the DisplayOrientations enum has other values.
	switch (m_nativeOrientation)
	{
	case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
		switch (m_currentOrientation)
		{
		case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;
		}
		break;

	case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
		switch (m_currentOrientation)
		{
		case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;
		}
		break;
	}
	return rotation;
}

// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
inline float DeviceResources::ConvertDipsToPixels(float dips, float dpi)
{
	static const float dipsPerInch = 96.0f;
	return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

// Check for SDK Layer support.
inline bool DeviceResources::SdkLayersAvailable()
{
#if defined(_DEBUG)
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
		nullptr,                    // Any feature level will do.
		0,
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		nullptr,                    // No need to keep the D3D device reference.
		nullptr,                    // No need to know the feature level.
		nullptr                     // No need to keep the D3D device context reference.
	);

	return SUCCEEDED(hr);
#else
	return false;
#endif
}








Renderer::Renderer(const std::shared_ptr<DeviceResources>& deviceResources)
{
	_sapp_call_init();
}

Renderer::~Renderer()
{
	_sapp_call_cleanup();
}

bool Renderer::Render()
{
	_sapp_call_frame();
	return true;
}






App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView.Activated({ this, &App::OnActivated });

	winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &App::OnSuspending });
	winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &App::OnResuming });

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DeviceResources>();
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
{
	window.SizeChanged({ this, &App::OnWindowSizeChanged });
	window.VisibilityChanged({ this, &App::OnVisibilityChanged });
	window.Closed({ this, &App::OnWindowClosed });

	auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

	currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });
	currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });
	winrt::Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

	m_deviceResources->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(winrt::hstring const& entryPoint)
{
	if (m_renderer == nullptr)
	{
		m_renderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources));
	}
}

// This method is called after the window becomes active.
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

			if (m_renderer->Render())
			{
				m_deviceResources->Present();
			}
		}
		else
		{
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
{
	// Run() won't start until the CoreWindow is activated.
	winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Activate();
}

void App::OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	auto deferral = args.SuspendingOperation().GetDeferral();
	concurrency::create_task([this, deferral]()
		{
			m_deviceResources->Trim();

			// Insert your code here.
			deferral.Complete();
		});
}

void App::OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void App::OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args)
{
	m_deviceResources->SetLogicalSize(winrt::Windows::Foundation::Size(sender.Bounds().Width, sender.Bounds().Height));
	//m_main->CreateWindowSizeDependentResources();
}

void App::OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
	m_windowVisible = args.Visible();
}

void App::OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args)
{
	m_windowClosed = true;
}

// DisplayInformation event handlers.

void App::OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
	// Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
	// if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
	// you should always retrieve it using the GetDpi method.
	// See DeviceResources.cpp for more details.

	m_deviceResources->SetDpi(sender.LogicalDpi());
	//m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
	m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
	//m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
	m_deviceResources->ValidateDevice();
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	sapp_desc desc = sokol_main(0, nullptr);
	_sapp_init_state(&desc);

	winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<App>());
}