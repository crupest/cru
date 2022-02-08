#include "cru/platform/GraphicsBase.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

#include <dlib/matrix.h>
#include <dlib/numeric_constants.h>
#include <cmath>

using cru::platform::Point;

using matrix14 = dlib::matrix<float, 1, 4>;
using matrix13 = dlib::matrix<float, 1, 3>;
using matrix44 = dlib::matrix<float, 4, 4>;

matrix44 Identity() {
  matrix44 m;
  m = dlib::identity_matrix<float, 4>();
  return m;
}

matrix44 T1(float a, float b, float c) {
  auto m = Identity();

  m(0, 3) = -a;
  m(1, 3) = -b;
  m(2, 3) = -c;

  return m;
}

matrix44 T2(float theta) {
  auto m = Identity();

  m(0, 0) = -std::cos(theta);
  m(0, 2) = -std::sin(theta);
  m(2, 0) = std::sin(theta);
  m(2, 2) = -std::cos(theta);

  return m;
}

matrix44 T3(float phi) {
  auto m = Identity();

  m(1, 1) = std::sin(phi);
  m(1, 2) = -std::cos(phi);
  m(2, 1) = std::cos(phi);
  m(2, 2) = std::sin(phi);

  return m;
}

matrix44 T4(float alpha) {
  auto m = Identity();

  m(0, 0) = std::cos(alpha);
  m(1, 0) = std::sin(alpha);
  m(0, 1) = -std::sin(alpha);
  m(1, 1) = std::cos(alpha);

  return m;
}

matrix44 T5() {
  auto m = Identity();

  m(0, 0) = -1;

  return m;
}

struct Args {
  float a;
  float b;
  float c;
  float theta;
  float phi;
  float alpha;
};

matrix44 Tv(Args args) {
  return T1(args.a, args.b, args.c) * T2(args.theta) * T3(args.phi) *
         T4(args.alpha) * T5();
}

matrix14 Transform(matrix14 point, Args args) { return point * Tv(args); }

matrix14 Transform(matrix13 point, Args args) {
  return matrix14{point(0), point(1), point(2), 1} * Tv(args);
}

Point TransformTo2D(matrix14 point, float d) {
  return Point{point(1), point(2)};
}

const float length = 100;

matrix13 points[] = {
    {0, 0, 0},
    {length, 0, 0},
    {length, 0, 0},
    {length, length, 0},
    {length, length, 0},
    {0, length, 0},
    {0, length, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, length},
    {length, 0, 0},
    {length, 0, length},
    {length, length, 0},
    {length, length, length},
    {0, length, 0},
    {0, length, length},
    {0, 0, length},
    {length, 0, length},
    {length, 0, length},
    {length, length, length},
    {length, length, length},
    {0, length, length},
    {0, length, length},
    {0, 0, length},
};

const float pi = static_cast<float>(dlib::pi);

Args args{30, 40, 50, pi / 3.f, pi / 4.f, pi / 5.f};

int main() {
  std::vector<Point> points2d;

  for (auto p : points) {
    auto point2d = TransformTo2D(Transform(std::move(p), args), length);
    points2d.push_back(point2d);
  }

  auto application = cru::platform::bootstrap::CreateUiApplication();
  auto window = application->CreateWindow();

  auto brush = application->GetGraphicsFactory()->CreateSolidColorBrush(
      cru::platform::colors::black);

  window->SetClientSize(cru::platform::Size(400, 400));

  window->PaintEvent()->AddHandler([window, &brush, points2d](nullptr_t) {
    auto painter = window->BeginPaint();
    painter->PushState();
    painter->ConcatTransform(cru::platform::Matrix::Translation(200, 200));
    for (int i = 0; i < points2d.size(); i += 2) {
      painter->DrawLine(points2d[i], points2d[i + 1], brush.get(), 1.f);
    }
    painter->PopState();
  });

  window->SetVisibility(cru::platform::gui::WindowVisibilityType::Show);

  return application->Run();
}
