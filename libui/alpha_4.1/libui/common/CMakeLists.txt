target_sources(libui_impl_alpha4_1
	PRIVATE
		userbugs.c
		areaevents.c
		attrlist.c
		attrstr.c
		control.c
		matrix.c
		opentype.c
		tablemodel.c
		utf.c
)

target_sources(libui_alpha4_1
	PRIVATE
		attribute.c
		debug.c
		shouldquit.c
		tablevalue.c
)

target_include_directories(libui_impl_alpha4_1
	PRIVATE
		.
)

target_include_directories(libui_alpha4_1
	PRIVATE
		.
)