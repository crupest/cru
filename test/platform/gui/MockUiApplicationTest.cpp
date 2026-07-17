#include "cru/platform/gui/mock/UiApplication.h"

#include <catch2/catch_test_macros.hpp>

#include <cru/base/Base.h>
#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Font.h>
#include <cru/platform/graphics/Geometry.h>
#include <cru/platform/graphics/ImageFactory.h>
#include <cru/platform/graphics/TextLayout.h>
#include <cru/platform/gui/Window.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace {
class DestructionCounterObject : public cru::Object {
 public:
  explicit DestructionCounterObject(int* destruction_count)
      : destruction_count_(destruction_count) {}

  ~DestructionCounterObject() override { ++*destruction_count_; }

 private:
  int* destruction_count_;
};

class FakeGraphicsFactory
    : public virtual cru::platform::graphics::IGraphicsFactory {
 public:
  explicit FakeGraphicsFactory(int* destruction_count = nullptr)
      : destruction_count_(destruction_count) {}

  ~FakeGraphicsFactory() override {
    if (destruction_count_ != nullptr) {
      ++*destruction_count_;
    }
  }

  std::string GetPlatformId() const override { return "FakeGraphics"; }

  std::unique_ptr<cru::platform::graphics::ISolidColorBrush>
  CreateSolidColorBrush() override {
    return nullptr;
  }

  std::unique_ptr<cru::platform::graphics::IGeometryBuilder>
  CreateGeometryBuilder() override {
    return nullptr;
  }

  std::unique_ptr<cru::platform::graphics::IFont> CreateFont(std::string,
                                                             float) override {
    return nullptr;
  }

  std::unique_ptr<cru::platform::graphics::ITextLayout> CreateTextLayout(
      std::shared_ptr<cru::platform::graphics::IFont>, std::string) override {
    return nullptr;
  }

  cru::platform::graphics::IImageFactory* GetImageFactory() override {
    return nullptr;
  }

 private:
  int* destruction_count_;
};
}  // namespace

using cru::Exception;
using cru::platform::gui::IClipboard;
using cru::platform::gui::ICursorManager;
using cru::platform::gui::INativeWindow;
using cru::platform::gui::IUiApplication;
using cru::platform::gui::mock::MockClipboard;
using cru::platform::gui::mock::MockCursorManager;
using cru::platform::gui::mock::MockResource;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockUiApplicationFixture;

TEST_CASE("Mock UI application fixture owns the process singleton safely",
          "[platform][gui][mock][app][singleton][fixture]") {
  REQUIRE(IUiApplication::GetInstance() == nullptr);

  {
    MockUiApplicationFixture fixture;
    REQUIRE(IUiApplication::GetInstance() == fixture.GetApplication());
    REQUIRE(fixture.GetApplication()->GetPlatformId() ==
            MockResource::kPlatformId);
  }

  REQUIRE(IUiApplication::GetInstance() == nullptr);
}

TEST_CASE("Mock UI application fixture restores previous singleton",
          "[platform][gui][mock][app][singleton][fixture]") {
  MockUiApplication app;
  IUiApplication::ScopedRegistration registration(app);

  REQUIRE(IUiApplication::GetInstance() == &app);

  {
    MockUiApplicationFixture fixture;
    REQUIRE(IUiApplication::GetInstance() == fixture.GetApplication());
  }

  REQUIRE(IUiApplication::GetInstance() == &app);
}

TEST_CASE("Mock UI application direct construction does not install singleton",
          "[platform][gui][mock][app][singleton]") {
  MockUiApplication app;
  MockUiApplication second_app;

  REQUIRE(IUiApplication::GetInstance() == nullptr);
}

TEST_CASE("Mock UI application constructor can opt into singleton registration",
          "[platform][gui][mock][app][singleton]") {
  MockUiApplication app(nullptr, false, true);

  REQUIRE(IUiApplication::GetInstance() == &app);
}

TEST_CASE("Mock UI application constructor registration restores previous app",
          "[platform][gui][mock][app][singleton]") {
  MockUiApplication app;
  IUiApplication::ScopedRegistration registration(app);

  REQUIRE(IUiApplication::GetInstance() == &app);

  {
    MockUiApplication second_app(nullptr, false, true);
    REQUIRE(IUiApplication::GetInstance() == &second_app);
  }

  REQUIRE(IUiApplication::GetInstance() == &app);
}

TEST_CASE("UI application scoped registration installs and restores singleton",
          "[platform][gui][mock][app][singleton]") {
  MockUiApplication app;
  MockUiApplication second_app;

  REQUIRE(IUiApplication::GetInstance() == nullptr);

  {
    IUiApplication::ScopedRegistration registration(app);
    REQUIRE(IUiApplication::GetInstance() == &app);

    {
      IUiApplication::ScopedRegistration second_registration(second_app);
      REQUIRE(IUiApplication::GetInstance() == &second_app);
    }

    REQUIRE(IUiApplication::GetInstance() == &app);
  }

  REQUIRE(IUiApplication::GetInstance() == nullptr);
}

TEST_CASE("UI application singleton is isolated per thread",
          "[platform][gui][mock][app][singleton][thread]") {
  MockUiApplication app;
  IUiApplication::ScopedRegistration registration(app);

  IUiApplication* thread_initial = nullptr;
  IUiApplication* thread_registered = nullptr;
  std::thread thread([&] {
    thread_initial = IUiApplication::GetInstance();

    MockUiApplication thread_app;
    IUiApplication::ScopedRegistration thread_registration(thread_app);
    thread_registered = IUiApplication::GetInstance();
  });
  thread.join();

  REQUIRE(thread_initial == nullptr);
  REQUIRE(thread_registered != nullptr);
  REQUIRE(thread_registered != &app);
  REQUIRE(IUiApplication::GetInstance() == &app);
}

TEST_CASE("Timer auto canceler tolerates no installed UI application",
          "[platform][gui][mock][app][singleton][timer]") {
  REQUIRE(IUiApplication::GetInstance() == nullptr);

  cru::platform::gui::TimerAutoCanceler canceler(1);
}

TEST_CASE("Mock UI application exposes injected graphics factory and services",
          "[platform][gui][mock][app][services]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplicationFixture fixture(&graphics_factory);
  auto* app = fixture.GetApplication();

  REQUIRE(app->GetGraphicsFactory() == &graphics_factory);
  REQUIRE(dynamic_cast<ICursorManager*>(app->GetMockCursorManager()) ==
          app->GetCursorManager());
  REQUIRE(dynamic_cast<IClipboard*>(app->GetMockClipboard()) ==
          app->GetClipboard());
  REQUIRE(dynamic_cast<MockCursorManager*>(app->GetCursorManager()) ==
          app->GetMockCursorManager());
  REQUIRE(dynamic_cast<MockClipboard*>(app->GetClipboard()) ==
          app->GetMockClipboard());
}

TEST_CASE("Mock UI application graphics factory ownership is explicit",
          "[platform][gui][mock][app][graphics]") {
  int owned_destruction_count = 0;
  {
    MockUiApplicationFixture fixture(
        new FakeGraphicsFactory(&owned_destruction_count), true);
    REQUIRE(owned_destruction_count == 0);
  }
  REQUIRE(owned_destruction_count == 1);

  int borrowed_destruction_count = 0;
  FakeGraphicsFactory borrowed_graphics_factory(&borrowed_destruction_count);
  {
    MockUiApplicationFixture fixture(&borrowed_graphics_factory, false);
    REQUIRE(fixture.GetApplication()->GetGraphicsFactory() ==
            &borrowed_graphics_factory);
  }
  REQUIRE(borrowed_destruction_count == 0);
}

TEST_CASE(
    "Mock UI application run returns requested quit code once handlers run",
    "[platform][gui][mock][app][quit]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  std::vector<int> handler_order;

  app->AddOnQuitHandler([&handler_order] { handler_order.push_back(1); });
  REQUIRE(app->Run() == 0);
  REQUIRE(handler_order.empty());

  app->RequestQuit(7);
  app->RequestQuit(9);
  app->AddOnQuitHandler([&handler_order] { handler_order.push_back(2); });

  REQUIRE(app->HasQuitRequest());
  REQUIRE(app->GetRequestedQuitCode() == 9);
  REQUIRE(app->Run() == 9);
  REQUIRE(handler_order == std::vector<int>{1, 2});

  app->AddOnQuitHandler([&handler_order] { handler_order.push_back(3); });
  REQUIRE(app->Run() == 9);
  REQUIRE(handler_order == std::vector<int>{1, 2, 3});
}

TEST_CASE("Mock UI application cleans delete-later objects at run boundaries",
          "[platform][gui][mock][app][delete-later]") {
  int destruction_count = 0;
  {
    MockUiApplicationFixture fixture;
    fixture.GetApplication()->DeleteLater(
        new DestructionCounterObject(&destruction_count));
    REQUIRE(destruction_count == 0);
    REQUIRE(fixture.GetApplication()->Run() == 0);
    REQUIRE(destruction_count == 1);

    fixture.GetApplication()->DeleteLater(
        new DestructionCounterObject(&destruction_count));
    REQUIRE(destruction_count == 1);
  }
  REQUIRE(destruction_count == 2);
}

TEST_CASE("Mock UI application pumps immediate actions in FIFO order",
          "[platform][gui][mock][app][event-loop][MockEventLoop]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  std::vector<int> action_order;

  auto first_id =
      app->SetImmediate([&action_order] { action_order.push_back(1); });
  auto second_id =
      app->SetImmediate([&action_order] { action_order.push_back(2); });
  auto canceled_id =
      app->SetImmediate([&action_order] { action_order.push_back(99); });

  REQUIRE(first_id > 0);
  REQUIRE(second_id > first_id);
  REQUIRE(canceled_id > second_id);
  app->CancelTimer(canceled_id);
  app->CancelTimer(canceled_id);
  REQUIRE(action_order.empty());

  REQUIRE(app->PumpOnce());
  REQUIRE(action_order == std::vector<int>{1});
  REQUIRE(app->PumpOnce());
  REQUIRE(action_order == std::vector<int>{1, 2});
  REQUIRE_FALSE(app->PumpOnce());
}

TEST_CASE("Mock UI application timeout waits for manual clock advancement",
          "[platform][gui][mock][app][timer][MockTimer]") {
  using namespace std::chrono_literals;

  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int timeout_count = 0;

  auto timer_id = app->SetTimeout(10ms, [&timeout_count] { ++timeout_count; });

  REQUIRE(timer_id > 0);
  REQUIRE_FALSE(app->PumpOnce());
  REQUIRE(timeout_count == 0);

  app->AdvanceTimeBy(9ms);
  REQUIRE_FALSE(app->PumpOnce());
  REQUIRE(timeout_count == 0);

  app->AdvanceTimeBy(1ms);
  REQUIRE(app->PumpOnce());
  REQUIRE(timeout_count == 1);
  REQUIRE_FALSE(app->PumpOnce());
}

TEST_CASE("Mock UI application zero-delay timeout runs through pump",
          "[platform][gui][mock][app][timer][MockTimer]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int timeout_count = 0;

  app->SetTimeout(std::chrono::milliseconds::zero(),
                  [&timeout_count] { ++timeout_count; });

  REQUIRE(timeout_count == 0);
  REQUIRE(app->PumpOnce());
  REQUIRE(timeout_count == 1);
  REQUIRE_FALSE(app->PumpOnce());
}

TEST_CASE("Mock UI application interval repeats until canceled",
          "[platform][gui][mock][app][timer][MockTimer]") {
  using namespace std::chrono_literals;

  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  std::vector<int> ticks;
  long long interval_id = 0;
  interval_id = app->SetInterval(5ms, [&] {
    ticks.push_back(static_cast<int>(ticks.size() + 1));
    if (ticks.size() == 3) {
      app->CancelTimer(interval_id);
    }
  });

  app->AdvanceTimeBy(15ms);
  REQUIRE(app->PumpOnce());
  REQUIRE(app->PumpOnce());
  REQUIRE(app->PumpOnce());
  REQUIRE_FALSE(app->PumpOnce());
  REQUIRE(ticks == std::vector<int>{1, 2, 3});
}

TEST_CASE("Mock UI application invalid timer cancellation is a no-op",
          "[platform][gui][mock][app][timer][MockTimer]") {
  using namespace std::chrono_literals;

  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int timeout_count = 0;

  auto timeout_id = app->SetTimeout(1ms, [&timeout_count] { ++timeout_count; });
  app->CancelTimer(0);
  app->CancelTimer(-1);
  app->CancelTimer(timeout_id + 100);

  app->AdvanceTimeBy(1ms);
  REQUIRE(app->PumpOnce());
  REQUIRE(timeout_count == 1);
}

TEST_CASE("Mock UI application pump flushes delete-later work",
          "[platform][gui][mock][app][delete-later][DeleteLater]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int destruction_count = 0;

  app->SetImmediate([&] {
    app->DeleteLater(new DestructionCounterObject(&destruction_count));
    REQUIRE(destruction_count == 0);
  });

  REQUIRE(app->PumpOnce());
  REQUIRE(destruction_count == 1);
}

TEST_CASE("Mock UI application settle drains actions and delete-later work",
          "[platform][gui][mock][app][settle][delete-later]"
          "[MockSettle][DeleteLater]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int destruction_count = 0;
  std::vector<int> action_order;

  app->SetImmediate([&] {
    action_order.push_back(1);
    app->DeleteLater(new DestructionCounterObject(&destruction_count));
  });
  app->SetImmediate([&action_order] { action_order.push_back(2); });

  REQUIRE(app->PumpUntilIdle() == 2);
  REQUIRE(action_order == std::vector<int>{1, 2});
  REQUIRE(destruction_count == 1);
  REQUIRE(app->PumpUntilIdle() == 0);
}

TEST_CASE("Mock UI application delete-later flushes on destruction",
          "[platform][gui][mock][app][delete-later][DeleteLater]") {
  int destruction_count = 0;
  {
    MockUiApplicationFixture fixture;
    fixture.GetApplication()->DeleteLater(
        new DestructionCounterObject(&destruction_count));
    REQUIRE(destruction_count == 0);
  }
  REQUIRE(destruction_count == 1);
}

TEST_CASE(
    "MockDiagnostics WaitUntil SettleDiagnostics reports compact loop state",
    "[platform][gui][mock][app][settle][MockSettle]"
    "[MockDiagnostics][SettleDiagnostics]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  int action_count = 0;

  app->SetImmediate([&action_count] { ++action_count; });

  REQUIRE(app->WaitUntil([&action_count] { return action_count == 1; }, 2));
  REQUIRE(action_count == 1);
  REQUIRE_FALSE(app->WaitUntil([] { return false; }, 1));
  const auto diagnostic = std::string(app->GetLastDiagnostic());
  REQUIRE(diagnostic.find("WaitUntil") != std::string::npos);
  REQUIRE(diagnostic.find("current_time_ms=0") != std::string::npos);
  REQUIRE(diagnostic.find("max_iterations=1") != std::string::npos);
  REQUIRE(diagnostic.find("iterations=1") != std::string::npos);
  REQUIRE(diagnostic.find("queued_actions=0") != std::string::npos);
  REQUIRE(diagnostic.find("timers=0") != std::string::npos);
  REQUIRE(diagnostic.find("delete_later_pending=false") != std::string::npos);
}

TEST_CASE(
    "MockDiagnostics SettleDiagnostics reports pending uncanceled interval",
    "[platform][gui][mock][app][settle][timer][MockSettle]"
    "[MockDiagnostics][SettleDiagnostics]") {
  using namespace std::chrono_literals;

  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();
  const auto interval_id = app->SetInterval(10ms, [] {});

  try {
    app->Settle(3);
    FAIL("Settle should fail while a repeating interval remains pending.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("pending repeating interval") != std::string::npos);
    REQUIRE(message.find("max_iterations=3") != std::string::npos);
    REQUIRE(message.find("iterations=0") != std::string::npos);
    REQUIRE(message.find("timers=1") != std::string::npos);
    REQUIRE(message.find("id=" + std::to_string(interval_id)) !=
            std::string::npos);
    REQUIRE(message.find("due_ms=10") != std::string::npos);
    REQUIRE(message.find("interval_ms=10") != std::string::npos);
    REQUIRE(message.find("repeat=true") != std::string::npos);
  }
}

TEST_CASE(
    "Mock UI application manual registry and timer seams stay deterministic",
    "[platform][gui][mock][app][future-scope]") {
  MockUiApplicationFixture fixture;
  auto* app = fixture.GetApplication();

  REQUIRE(app->GetAllWindow().empty());
  REQUIRE(app->IsQuitOnAllWindowClosed());
  app->SetQuitOnAllWindowClosed(false);
  REQUIRE_FALSE(app->IsQuitOnAllWindowClosed());

  INativeWindow* fake_window = reinterpret_cast<INativeWindow*>(0x1);
  app->RegisterWindow(fake_window);
  REQUIRE(app->GetAllWindow() == std::vector<INativeWindow*>{fake_window});
  app->RegisterWindow(fake_window);
  REQUIRE(app->GetAllWindow() == std::vector<INativeWindow*>{fake_window});
  app->UnregisterWindow(fake_window);
  REQUIRE(app->GetAllWindow().empty());

  std::unique_ptr<INativeWindow> created_window(app->CreateWindow());
  REQUIRE(created_window != nullptr);
  REQUIRE_FALSE(created_window->IsCreated());
  REQUIRE(app->GetAllWindow().empty());
  app->CancelTimer(0);
  app->CancelTimer(-1);
  app->CancelTimer(123);
}
