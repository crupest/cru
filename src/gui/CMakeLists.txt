add_library(CruGui
	Keyboard.cpp
	Menu.cpp
	UiApplication.cpp
)
target_compile_definitions(CruGui PRIVATE CRU_GUI_EXPORT_API)
target_link_libraries(CruGui PUBLIC CruGraphics)
