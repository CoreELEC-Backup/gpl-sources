enum theme {
    THEME_C64,
    THEME_C64C,
    THEME_C64_TRANSPARENT,
    THEME_C64C_TRANSPARENT,
    THEME_DARK_TRANSPARENT,
    THEME_LIGHT_TRANSPARENT
};

void
set_style(struct nk_context *ctx, enum theme theme)
{
    struct nk_color table[NK_COLOR_COUNT];
    if (theme == THEME_C64) {
        table[NK_COLOR_TEXT] = nk_rgba(250, 250, 250, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(123, 127, 130, 255); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 1);
        table[NK_COLOR_BUTTON] = nk_rgba(69, 59, 58, 255); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(165, 163, 160, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(48, 44, 45, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(89, 79, 78, 255); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 255); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(144, 141, 129, 255); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else if (theme == THEME_C64C) {
        table[NK_COLOR_TEXT] = nk_rgba(1, 1, 1, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(157, 152, 149, 255); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 10); 
        table[NK_COLOR_BUTTON] = nk_rgba(216, 209, 201, 255); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(240, 240, 240, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(255, 255, 255, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(109, 99, 98, 255); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 255); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(144, 141, 124, 255); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else if (theme == THEME_C64_TRANSPARENT) {
        table[NK_COLOR_TEXT] = nk_rgba(250, 250, 250, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(123, 127, 130, 200); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 1);
        table[NK_COLOR_BUTTON] = nk_rgba(69, 59, 58, 200); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(165, 163, 160, 200);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(48, 44, 45, 200);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(89, 79, 78, 200); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 200); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(144, 141, 129, 200); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else if (theme == THEME_C64C_TRANSPARENT) {
        table[NK_COLOR_TEXT] = nk_rgba(1, 1, 1, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(157, 152, 149, 200); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 10); 
        table[NK_COLOR_BUTTON] = nk_rgba(216, 209, 201, 200); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(240, 240, 240, 200);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(255, 255, 255, 200);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(109, 99, 98, 200); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 200); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(144, 141, 124, 200); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else if (theme == THEME_DARK_TRANSPARENT) {
        table[NK_COLOR_TEXT] = nk_rgba(250, 250, 250, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(80, 80, 80, 200); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 1); 
        table[NK_COLOR_BUTTON] = nk_rgba(32, 32, 32, 200); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(144, 144, 144, 200);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(64, 64, 64, 224);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(50, 50, 50, 200); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 200); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(16, 16, 16, 200); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else if (theme == THEME_LIGHT_TRANSPARENT) {
        table[NK_COLOR_TEXT] = nk_rgba(1, 1, 1, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
        table[NK_COLOR_HEADER] = nk_rgba(180, 180, 180, 200); // function keys
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 1); 
        table[NK_COLOR_BUTTON] = nk_rgba(220, 220, 220, 200); // regular keys
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(230, 230, 230, 224);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(255, 255, 255, 224);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(190, 190, 190, 200); // datasette
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(128, 0, 0, 200); // reset
        table[NK_COLOR_PROPERTY] = nk_rgba(160, 160, 160, 200); // hotkeys
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    } else {
        nk_style_default(ctx);
    }
}


