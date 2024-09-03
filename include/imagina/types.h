#pragma once

#include <exception>
#include "numerics.h"
#include "constants.h"
#include "math.h"

namespace Imagina {
	//template<intmax_t factor, typename type>
	//type MulInt(type a) {
	//	return a * Constants::Integer<type, factor>();
	//}

	struct GRRectangle {
		real_gr MinX, MinY, MaxX, MaxY;

		GRRectangle() = default;
		GRRectangle(real_gr MinX, real_gr MinY, real_gr MaxX, real_gr MaxY) : MinX(MinX), MinY(MinY), MaxX(MaxX), MaxY(MaxY) {}

		real_gr Width()	const { return MaxX - MinX; }
		real_gr Height()	const { return MaxY - MinY; }
	};

	template <typename real>
	struct Circle {
		real X, Y;
		real Radius;

		Circle() = default;
		Circle(real X, real Y, real Radius) : X(X), Y(Y), Radius(Radius) {}

		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
			Circle(const Circle<real2> &circle) : X(circle.X), Y(circle.Y), Radius(circle.Radius) {}

		real Diameter() const { return Radius * Constants::Two; }
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

		real Width()	const { return HalfWidth * Constants::Two; }
		real Height()	const { return HalfHeight * Constants::Two; }
		real MinX()		const { return X - HalfWidth; }
		real MinY()		const { return Y - HalfHeight; }
		real MaxX()		const { return X + HalfWidth; }
		real MaxY()		const { return Y + HalfHeight; }

		real AspectRatio() const { return HalfWidth / HalfHeight; }

		void AspectRatio(real value) { HalfWidth = HalfHeight * value; }
		
		Rectangle TransformTo(Rectangle rectangle) const {
			return Rectangle(
				X * rectangle.HalfWidth  + rectangle.X,
				Y * rectangle.HalfHeight + rectangle.Y,
				HalfWidth	* rectangle.HalfWidth,
				HalfHeight	* rectangle.HalfHeight);
		}
		Rectangle TransformFrom(Rectangle rectangle) const {
			return Rectangle(
				(X - rectangle.X) / rectangle.HalfWidth,
				(Y - rectangle.Y) / rectangle.HalfHeight,
				HalfWidth	/ rectangle.HalfWidth,
				HalfHeight	/ rectangle.HalfHeight);
		}

		Circle<real> Circumcircle() const {
			// FIXME //using std::hypot; // Computes the square root of the sum of the squares of x and y, without undue overflow or underflow at intermediate stages of the computation
			return Circle(X, Y, hypot(HalfHeight, HalfWidth));
		}

		Circle<real> ApproximateCircumcircle() const {
			return Circle(X, Y, manhattan_hypot(HalfHeight, HalfWidth));
		}
	};
	
	template <typename real>
	struct Location {
		real X, Y;
		real HalfHeight;

		Location() = default;
		Location(real X, real Y, real HalfHeight) : X(X), Y(Y), HalfHeight(HalfHeight) {}

		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
		Location(const Location<real2> &rectangle) : X(rectangle.X), Y(rectangle.Y), HalfHeight(rectangle.HalfHeight) {}

		real Height()	const { return HalfHeight * Constants::Two; }
		real MinY()		const { return Y - HalfHeight; }
		real MaxY()		const { return Y + HalfHeight; }

		real HalfWidth(real aspectRatio)	const { return HalfHeight * aspectRatio; }
		real Width(real aspectRatio)		const { return Height() * aspectRatio; }
		real MinX(real aspectRatio)			const { return X - HalfWidth(aspectRatio); }
		real MaxX(real aspectRatio)			const { return X + HalfWidth(aspectRatio); }

		Rectangle<real> ToRectangle(real aspectRatio) const {
			return Rectangle<real>(X, Y, HalfWidth(aspectRatio), HalfHeight);
		}
		
		Location ZoomIn(real_sr centerX, real_sr centerY) const {
			real_hr newHalfHeight = HalfHeight * 0.5; // TODO: replace "* 0.5"
			return Location(
				X + centerX * newHalfHeight,
				Y + centerY * newHalfHeight,
				newHalfHeight);
		}

		Location ZoomOut(real_sr centerX, real_sr centerY) const {
			return Location(
				X - centerX * HalfHeight,
				Y - centerY * HalfHeight,
				//MulInt<2>(HalfHeight));
				HalfHeight * Constants::Two);
		}

		bool operator==(const Location &location) const {
			return X == location.X && Y == location.Y && HalfHeight == location.HalfHeight;
		}
		bool operator!=(const Location &location) const {
			return X != location.X || Y != location.Y || HalfHeight != location.HalfHeight;
		}
	};

	using SRCircle = Circle<real_sr>;
	using HRCircle = Circle<real_hr>;

	using SRRectangle = Rectangle<real_sr>;
	using HRRectangle = Rectangle<real_hr>;

	using SRLocation = Location<real_sr>;
	using HRLocation = Location<real_hr>;

	class not_implemented : public std::exception {
	public:
		virtual const char *what() const noexcept override { return "Not implemented"; } // FIXME: Move to a cpp file.
	};
}