#pragma once

#include "BasicTypes.h"
#include "Constants.h"

#ifdef _WIN32
#ifdef IM_BUILD_CORE
#define im_decl __declspec(dllexport)
#else
#define im_decl __declspec(dllimport)
#endif
#else
#define im_decl
#endif // FIXME: Move to somewhere else

namespace Imagina {
	template<intmax_t factor, typename type>
	type MulInt(type a) {
		return a * Constants::Integer<type, factor>();
	}

	struct GRRectangle {
		GRReal MinX, MinY, MaxX, MaxY;

		GRRectangle() = default;
		GRRectangle(GRReal MinX, GRReal MinY, GRReal MaxX, GRReal MaxY) : MinX(MinX), MinY(MinY), MaxX(MaxX), MaxY(MaxY) {}

		GRReal Width()	{ return MaxX - MinX; }
		GRReal Height()	{ return MaxY - MinY; }
	};

	template <typename real>
	struct Rectangle {
		real X, Y;
		real HalfWidth, HalfHeight;

		Rectangle() = default;
		Rectangle(real X, real Y, real HalfWidth, real HalfHeight) : X(X), Y(Y), HalfWidth(HalfWidth), HalfHeight(HalfHeight) {}

		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
		Rectangle(const Rectangle<real2> &rectangle) : X(rectangle.X), Y(rectangle.Y), HalfWidth(rectangle.HalfWidth), HalfHeight(rectangle.HalfHeight) {}

		real Width()	const { return MulInt<2>(HalfWidth); }
		real Height()	const { return MulInt<2>(HalfWidth); }
		real MinX()		const { return X - HalfWidth; }
		real MinY()		const { return Y - HalfHeight; }
		real MaxX()		const { return X + HalfWidth; }
		real MaxY()		const { return Y + HalfHeight; }

		real AspectRatio() const { return HalfWidth / HalfHeight; }

		void AspectRatio(real value) { HalfWidth = HalfHeight * value; }
		
        Rectangle TransformTo(Rectangle rectangle) {
            return Rectangle(
                X * rectangle.HalfWidth  + rectangle.X,
                Y * rectangle.HalfHeight + rectangle.Y,
                HalfWidth	* rectangle.HalfWidth,
                HalfHeight	* rectangle.HalfHeight);
        }
		Rectangle TransformFrom(Rectangle rectangle) {
            return Rectangle(
                (X - rectangle.X) / rectangle.HalfWidth,
                (Y - rectangle.Y) / rectangle.HalfHeight,
                HalfWidth	/ rectangle.HalfWidth,
                HalfHeight	/ rectangle.HalfHeight);
        }

	};

	using SRRectangle = Rectangle<SRReal>;
	using HRRectangle = Rectangle<HRReal>;
}