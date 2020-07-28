#pragma once

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.ApplicationModel.h>

#include <concrt.h>	// For create_task

namespace DX
{
	// Function that reads from a binary file asynchronously.
	/*inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		using namespace winrt::Windows::Storage;
		using namespace Concurrency;

		auto folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation();

		return winrt::create_task(folder.GetFileAsync(filename)).then([] (StorageFile const& file) 
		{
			return FileIO::ReadBufferAsync(file);
		}).then([] (Streams::IBuffer const& fileBuffer) -> std::vector<byte> 
		{
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer.Length());
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(winrt::array_view<byte>(returnBuffer.data(), fileBuffer.Length()));
			return returnBuffer;
		});
	}

	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}*/


}
