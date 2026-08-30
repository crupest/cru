#pragma once
#include "Base.h"

#include <concepts>
#include <string>
#include <vector>

namespace cru::platform::gui {
struct CRU_PLATFORM_GUI_API SaveDialogOptions {
  std::string title;
  /**
   * Used as the text label of ok button on Windows.
   */
  std::string prompt;
  /**
   * Not used on Windows. Better not use this.
   */
  std::string message;
  std::vector<std::string> allowed_file_types;
  bool allow_all_file_types = false;
};

struct CRU_PLATFORM_GUI_API OpenDialogOptions : SaveDialogOptions {
  bool can_choose_files = true;
  /**
   * On Windows, if this is true, only directories can be chosen even if
   * can_choose_files is true.
   */
  bool can_choose_directories = false;
  bool allow_multiple_selection = false;
};

template <typename Self, typename T>
struct SaveDialogOptionsBuilderTemplate {
  T options;

  Self& SetTitle(std::string title) {
    options.title = std::move(title);
    return GetSelf();
  }

  Self& SetPrompt(std::string prompt) {
    options.prompt = std::move(prompt);
    return GetSelf();
  }

  Self& SetMessage(std::string message) {
    options.message = std::move(message);
    return GetSelf();
  }

  Self& SetAllowedFileTypes(std::vector<std::string> allowed_file_types) {
    options.allowed_file_types = std::move(allowed_file_types);
    return GetSelf();
  }

  Self& AddAllowedFileType(std::string allowed_file_type) {
    options.allowed_file_types.push_back(allowed_file_type);
    return GetSelf();
  }

  Self& SetAllowAllFileTypes(bool allow_all_file_types) {
    options.allow_all_file_types = allow_all_file_types;
    return GetSelf();
  }

  T Build() { return options; }

 private:
  Self& GetSelf()
    requires(std::derived_from<Self, SaveDialogOptionsBuilderTemplate>)
  {
    return *static_cast<Self*>(this);
  }
};

struct CRU_PLATFORM_GUI_API SaveDialogOptionsBuilder
    : public SaveDialogOptionsBuilderTemplate<SaveDialogOptionsBuilder,
                                              SaveDialogOptions> {};

struct CRU_PLATFORM_GUI_API OpenDialogOptionsBuilder
    : SaveDialogOptionsBuilderTemplate<OpenDialogOptionsBuilder,
                                       OpenDialogOptions> {
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
      bool allow_multiple_selection) {
    options.allow_multiple_selection = allow_multiple_selection;
    return *this;
  }
};
}  // namespace cru::platform::gui
