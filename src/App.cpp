#include "App.h"

#include <ppltasks.h>

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
	winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<App>());
}