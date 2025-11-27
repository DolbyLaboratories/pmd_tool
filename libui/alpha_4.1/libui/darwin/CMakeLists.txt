# 3 june 2016

target_sources(libui_impl_alpha4_1
	PRIVATE
		aat.m
		alloc.m
		area.m
		areaevents.m
		attrstr.m
		autolayout.m
		box.m
		button.m
		checkbox.m
		colorbutton.m
		combobox.m
		control.m
		datetimepicker.m
		debug.m
		draw.m
		drawtext.m
		editablecombo.m
		entry.m
		fontbutton.m
		fontmatch.m
		fonttraits.m
		fontvariation.m
		form.m
		future.m
		graphemes.m
		grid.m
		group.m
		image.m
		label.m
		main.m
		map.m
		menu.m
		multilineentry.m
		opentype.m
		progressbar.m
		radiobuttons.m
		scrollview.m
		separator.m
		slider.m
		spinbox.m
		stddialogs.m
		tab.m
		table.m
		tablecolumn.m
		text.m
		undocumented.m
		util.m
		window.m
		winmoveresize.m
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
