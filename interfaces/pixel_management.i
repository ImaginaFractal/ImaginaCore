#include <concepts>
#include <Imagina/types>

namespace Imagina {
	struct _GpuTexture;
	using GpuTexture = _GpuTexture *;

	interface IGraphics {
		GpuTexture CreateTexture();
		void DeleteTexture(GpuTexture texture);

		void SetTextureImage(GpuTexture texture, size_t width, size_t height, float *pixels);
		void UpdateTextureImage(GpuTexture texture, size_t width, size_t height, float *pixels);
	};

	interface IRasterizingInterface {
		bool GetPixel(HRReal &x, HRReal &y);
		void GetDdx(HRReal &x, HRReal &y);
		void GetDdy(HRReal &x, HRReal &y);
		void WriteResults(void *value);
	};

	interface IRasterizer {
		void Cancel();

		IRasterizingInterface GetRasterizingInterface();
		void FreeRasterizingInterface(IRasterizingInterface Interface);
	};
}