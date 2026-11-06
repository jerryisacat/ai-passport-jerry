idf_component_register(
    SRCS "src/jerryicat_launcher.c"
         "src/jerryicat_nav.c"
         "src/jerryicat_registry.c"
         "src/jerryicat_ui.c"
         "plays/plus-one/plus_one.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES bsp lvgl
)
