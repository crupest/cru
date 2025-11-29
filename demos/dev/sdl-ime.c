#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

int main() {
  SDL_Window* window;
  SDL_Rect area = {0, 0, 1, 1};
  int cursor = 0;

  window = SDL_CreateWindow("", 400, 200, 0);
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_SetTextInputArea(window, &area, cursor);
  SDL_StartTextInput(window);

  SDL_Event e;
  while (SDL_WaitEvent(&e)) {
    if (e.type == SDL_EVENT_TEXT_INPUT) {
      printf("%s: %s\n", "SDL_EVENT_TEXT_INPUT", e.text.text);
      fflush(stdout);
    } else if (e.type == SDL_EVENT_TEXT_EDITING) {
      printf("%s: %s, start %i, length %i\n", "SDL_EVENT_TEXT_EDITING",
             e.edit.text, e.edit.start, e.edit.length);
      fflush(stdout);
    } else if (e.type == SDL_EVENT_TEXT_EDITING_CANDIDATES) {
      printf("%s: num_candidates %i, selected %i\n",
             "SDL_EVENT_TEXT_EDITING_CANDIDATES",
             e.edit_candidates.num_candidates,
             e.edit_candidates.selected_candidate);
      fflush(stdout);
    } else if (e.type == SDL_EVENT_QUIT) {
      break;
    }
  }

  SDL_Quit();
}
