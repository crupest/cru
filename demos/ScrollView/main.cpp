#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/controls/ScrollView.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/Window.h"

using cru::platform::gui::IUiApplication;
using cru::ui::controls::ScrollView;
using cru::ui::controls::TextBlock;
using cru::ui::controls::Window;

int main() {
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  Window window;
  ScrollView scroll_view;
  window.AddChild(&scroll_view);

  auto text_block = TextBlock::Create(
      R"([Verse 1]
The snow glows white on the mountain tonight
Not a footprint to be seen
A kingdom of isolation
And it looks like I'm the queen
The wind is howling like this swirling storm inside
Couldn't keep it in, Heaven knows I tried

[Pre-Chorus]
Don't let them in, don't let them see
Be the good girl you always have to be
Conceal, don't feel, don't let them know
Well, now they know

[Chorus]
Let it go, let it go
Can't hold it back anymore
Let it go, let it go
Turn away and slam the door
I don't care what they're going to say
Let the storm rage on
The cold never bothered me anyway

[Verse 2]
It's funny how some distance
Makes everything seem small
And the fears that once controlled me
Can't get to me at all

[Pre-Chorus]
It's time to see what I can do
To test the limits and break through
No right, no wrong, no rules for me
I'm free

[Chorus]
Let it go, let it go
I'm one with the wind and sky
Let it go, let it go
You'll never see me cry
Here I stand, and here I'll stay
Let the storm rage on...

[Bridge]
My power flurries through the air into the ground
My soul is spiraling in frozen fractals all around
And one thought crystallizes like an icy blast:
I'm never going back, the past is in the past!

[Chorus]
Let it go, let it go
And I'll rise like the break of dawn
Let it go, let it go
That perfect girl is gone
Here I stand in the light of day
Let the storm rage on!
The cold never bothered me anyway)",
      true);

  scroll_view.SetChild(text_block.get());

  window.GetWindowHost()->GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);

  return application->Run();
}
