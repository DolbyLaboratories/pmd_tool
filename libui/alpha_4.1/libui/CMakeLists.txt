find_package(PkgConfig)

# ---- Libui libs ----

add_library(libui_impl_alpha4_1)

target_include_directories(libui_impl_alpha4_1
	PUBLIC
		include
		..
)

add_library(libui_alpha4_1)

target_include_directories(libui_alpha4_1
	PUBLIC
		include
		..
)

target_link_libraries(libui_alpha4_1
	PRIVATE
		libui_impl_alpha4_1
)


# ---- Libui API ----

target_include_directories(libui_impl_alpha4_1
	PUBLIC
		include
		..
)

target_include_directories(libui_alpha4_1
	PUBLIC
		include
		..
)

if(${OS} MATCHES win32)
	target_include_directories(libui_impl_alpha4_1
		PUBLIC
			include/windows
	)
	target_include_directories(libui_alpha4_1
		PUBLIC
			include/windows
	)
elseif(${OS} MATCHES osx)
	target_include_directories(libui_impl_alpha4_1
		PUBLIC
			include/osx
	)
	target_include_directories(libui_alpha4_1
		PUBLIC
			include/osx
	)
else()
	target_include_directories(libui_impl_alpha4_1
		PUBLIC
			include/unix
	)
	target_include_directories(libui_alpha4_1
		PUBLIC
			include/unix
	)
endif()


# ---- Libui dependencies ----

set(DEPS "")

if(${OS} MATCHES win32)
	set(DEPS
		kernel32
		user32
		gdi32
		ole32
		oleaut32
		uuid
		comdlg32
		comctl32
		uxtheme
		msimg32
		d2d1
		dwrite
		oleacc
		windowscodecs
	)
elseif(${OS} MATCHES linux)
	set(DEPS
		gtk-3
		gdk-3
		pangocairo-1.0
		pango-1.0
		atk-1.0
		cairo-gobject
		cairo
		gdk_pixbuf-2.0
		gio-2.0
		gobject-2.0
		glib-2.0
		m
		dl
	)

	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	pkg_check_modules(GLIB REQUIRED glib-2.0)

	target_link_libraries(libui_impl_alpha4_1
		PRIVATE ${GTK3_LIBRARIES}
		PRIVATE ${GLIB_LIBRARIES}
	)

	target_link_libraries(libui_alpha4_1
		PRIVATE ${GTK3_LIBRARIES}
		PRIVATE ${GLIB_LIBRARIES}
	)

	target_link_options(libui_impl_alpha4_1
		PRIVATE "--no-undefined --no-allow-shlib-undefined"
	)

	target_link_options(libui_alpha4_1
		PRIVATE "--no-undefined --no-allow-shlib-undefined"
	)

	target_compile_options(libui_impl_alpha4_1
		PRIVATE ${GTK3_CFLAGS} ${GLIB_CFLAGS}
	)

	target_compile_options(libui_alpha4_1
		PRIVATE ${GTK3_CFLAGS} ${GLIB_CFLAGS}
	)

	target_link_directories(libui_impl_alpha4_1
		PRIVATE ${GTK3_LIBRARY_DIRS} ${GLIB_LIBRARY_DIRS}
	)

	target_link_directories(libui_alpha4_1
		PRIVATE ${GTK3_LIBRARY_DIRS} ${GLIB_LIBRARY_DIRS}
	)

	target_include_directories(libui_impl_alpha4_1
		PRIVATE
			${GTK3_INCLUDE_DIRS}
			${GLIB_INCLUDE_DIRS}
	)

	target_include_directories(libui_alpha4_1
		PRIVATE
			${GTK3_INCLUDE_DIRS}
			${GLIB_INCLUDE_DIRS}
	)
endif()

target_link_libraries(libui_impl_alpha4_1
	PRIVATE
		${DEPS}
)

target_link_libraries(libui_alpha4_1
	PRIVATE
		${DEPS}
)

# ---- Libui sources ----

add_subdirectory(common)

if(${OS} MATCHES win32)
	add_subdirectory(windows)
elseif(${OS} MATCHES osx)
	add_subdirectory(darwin)
else()
	add_subdirectory(unix)
endif()