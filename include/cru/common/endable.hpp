#pragma once
#include "base.hpp"

namespace cru {
// Although c++ has destructor called automatically. But there is
// occasion when an instance of class needs to end with a result
// and release all internal resources.
// eg. IGeometryBuild will end with building a Geometry and release
// some resources. IPainter will end drawing and release some
// resources and map the drawing result onto target.
// note: You can't call End twice. And most methods on the object
// is invalid to call after End is called. Get whether it is ended
// by IsEnded.
template<typename TResult>
struct IEndable : virtual Interface {
  // Get whether the object is ended.
  virtual bool IsEnded() const = 0;
  // End the object with a result.
  virtual TResult End() = 0;
};
}
