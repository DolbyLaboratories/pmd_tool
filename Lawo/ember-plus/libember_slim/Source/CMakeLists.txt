add_library(ember_slim_lib)

target_include_directories(ember_slim_lib
    PUBLIC
        .
)

target_sources(ember_slim_lib
    PRIVATE
        ber.c
        berio.c
        berreader.c
        bertag.c
        bytebuffer.c
        glow.c
        glowrx.c
        glowtx.c
        emberasyncreader.c
        emberframing.c
        emberinternal.c
        ember.c
)