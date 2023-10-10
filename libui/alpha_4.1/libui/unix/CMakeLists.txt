
set(GTK_INCLUDES
	/usr/include/gtk-3.0
	/usr/include/at-spi2-atk/2.0
	/usr/include/at-spi-2.0
	/usr/include/dbus-1.0 
	/usr/lib/i386-linux-gnu/dbus-1.0/include 
	/usr/include/gio-unix-2.0/ 
	/usr/include/mirclient 
	/usr/include/mircommon 
	/usr/include/mircookie 
	/usr/include/cairo 
	/usr/include/pango-1.0 
	/usr/include/harfbuzz 
	/usr/include/atk-1.0 
	/usr/include/pixman-1 
	/usr/include/freetype2 
	/usr/include/libpng12 
	/usr/include/gdk-pixbuf-2.0 
	/usr/include/glib-2.0 
	/usr/lib/i386-linux-gnu/glib-2.0/include
	/usr/lib/x86_64-linux-gnu/glib-2.0/include
)

set(GTK_CFLAGS
	-std=c99
	-fPIC
	-Wno-switch
	-fvisibility=hidden
)

target_compile_options(libui_impl_alpha4_1
	PUBLIC ${GTK_CFLAGS}
)

target_compile_options(libui_alpha4_1
	PUBLIC ${GTK_CFLAGS}
)

target_include_directories(libui_impl_alpha4_1
	PRIVATE
		${GTK_INCLUDES}
		.
)

target_include_directories(libui_alpha4_1
	PRIVATE
		${GTK_INCLUDES}
		.
)

target_sources(libui_impl_alpha4_1
	PRIVATE
		debug.c
		graphemes.c
)

target_sources(libui_alpha4_1
	PRIVATE
		alloc.c
		area.c
		attrstr.c
		box.c
		button.c
		cellrendererbutton.c
		checkbox.c
		child.c
		colorbutton.c
		combobox.c
		control.c
		datetimepicker.c
		draw.c
		drawmatrix.c
		drawpath.c
		drawtext.c
		editablecombo.c
		entry.c
		fontbutton.c
		fontmatch.c
		form.c
		future.c
		grid.c
		group.c
		image.c
		label.c
		main.c
		menu.c
		multilineentry.c
		opentype.c
		progressbar.c
		radiobuttons.c
		separator.c
		slider.c
		spinbox.c
		stddialogs.c
		tab.c
		table.c
		tablemodel.c
		text.c
		util.c
		window.c
)