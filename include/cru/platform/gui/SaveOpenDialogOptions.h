#pragma once
#include "Base.h"

#include <string>
#include <vector>

namespace cru::platform::gui {
struct CRU_PLATFORM_GUI_API SaveDialogOptions {
  std::string title;
  std::string prompt;
  std::string message;
  std::vector<std::string> allowed_file_types;
  bool allow_all_file_types = false;
};

struct CRU_PLATFORM_GUI_API OpenDialogOptions : SaveDialogOptions {
  bool can_choose_files = true;
  bool can_choose_directories = false;
  bool allow_mulitple_selection = false;
};

template <typename T>
struct CRU_PLATFORM_GUI_API SaveDialogOptionsBuilderTemplate {
  T options;

  SaveDialogOptionsBuilderTemplate& SetTitle(std::string title) {
    options.title = std::move(title);
    return *this;
  }

  SaveDialogOptionsBuilderTemplate& SetPrompt(std::string prompt) {
    options.prompt = std::move(prompt);
    return *this;
  }

  SaveDialogOptionsBuilderTemplate& SetMessage(std::string message) {
    options.message = std::move(message);
    return *this;
  }

  SaveDialogOptionsBuilderTemplate& SetAllowedFileTypes(
      std::vector<std::string> allowed_file_types) {
    options.allowed_file_types = std::move(allowed_file_types);
    return *this;
  }

  SaveDialogOptionsBuilderTemplate& AddAllowedFileType(
      std::string allowed_file_type) {
    options.allowed_file_types.push_back(allowed_file_type);
    return *this;
  }

  SaveDialogOptionsBuilderTemplate& SetAllowAllFileTypes(
      bool allow_all_file_types) {
    options.allow_all_file_types = allow_all_file_types;
    return *this;
  }

  T Build() { return options; }
};

using SaveDialogOptionsBuilder =
    SaveDialogOptionsBuilderTemplate<SaveDialogOptions>;

struct CRU_PLATFORM_GUI_API OpenDialogOptionsBuilder
    : SaveDialogOptionsBuilderTemplate<OpenDialogOptions> {
  OpenDialogOptionsBuilder& SetCanChooseFiles(bool can_choose_files) {
    options.can_choose_files = can_choose_files;
    return *this;
  }

  OpenDialogOptionsBuilder& SetCanChooseDirectories(
      bool can_choose_directories) {
    options.can_choose_directories = can_choose_directories;
    return *this;
  }

  OpenDialogOptionsBuilder& SetAllowMultipleSelection(
      bool allow_mulitple_selection) {
    options.allow_mulitple_selection = allow_mulitple_selection;
    return *this;
  }
};
}  // namespace cru::platform::gui
