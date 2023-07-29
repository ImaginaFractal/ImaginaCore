#include <concepts>
#include "../types"

namespace Imagina {
	interface IRasterizingInterface {
		bool GetPixel(HRReal &x, HRReal &y);
		void GetDdx(HRReal &x, HRReal &y);
		void GetDdy(HRReal &x, HRReal &y);
		void WriteResults(void *value);
	};
}