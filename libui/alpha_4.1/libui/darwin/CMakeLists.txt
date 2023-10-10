# 3 june 2016

target_sources(libui_impl_alpha4_1
	PRIVATE
		darwin/aat.m
		darwin/alloc.m
		darwin/area.m
		darwin/areaevents.m
		darwin/attrstr.m
		darwin/autolayout.m
		darwin/box.m
		darwin/button.m
		darwin/checkbox.m
		darwin/colorbutton.m
		darwin/combobox.m
		darwin/control.m
		darwin/datetimepicker.m
		darwin/debug.m
		darwin/draw.m
		darwin/drawtext.m
		darwin/editablecombo.m
		darwin/entry.m
		darwin/fontbutton.m
		darwin/fontmatch.m
		darwin/fonttraits.m
		darwin/fontvariation.m
		darwin/form.m
		darwin/future.m
		darwin/graphemes.m
		darwin/grid.m
		darwin/group.m
		darwin/image.m
		darwin/label.m
		darwin/main.m
		darwin/map.m
		darwin/menu.m
		darwin/multilineentry.m
		darwin/opentype.m
		darwin/progressbar.m
		darwin/radiobuttons.m
		darwin/scrollview.m
		darwin/separator.m
		darwin/slider.m
		darwin/spinbox.m
		darwin/stddialogs.m
		darwin/tab.m
		darwin/table.m
		darwin/tablecolumn.m
		darwin/text.m
		darwin/undocumented.m
		darwin/util.m
		darwin/window.m
		darwin/winmoveresize.m
)

target_include_directories(libui_impl_alpha4_1
	PRIVATE
		.
)

target_include_directories(libui_alpha4_1
	PRIVATE
		.
)

target_link_libraries(libui_impl_alpha4_1
	PRIVATE
		"-framework Foundation"
		"-framework Appkit"
)

target_link_libraries(libui_alpha4_1
	PRIVATE
	"-framework Foundation"
	"-framework Appkit"
)
