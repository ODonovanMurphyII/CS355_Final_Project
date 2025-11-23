#ifndef GUI_H
#define GUI_H

#include <ncurses.h>
#include "core.h"

/* Initialize ncurses + BIOS-like color pairs */
void gui_init();

/* Shutdown ncurses */
void gui_shutdown();

/* Draw top header (BIOS style) */
void gui_draw_header(const char *title);

/* Draw bottom help/footer */
void gui_draw_footer(const char *legend);

/* Draw the main frames (left file list area and right info area) */
void gui_draw_frame();

/* Draw the file list in the left pane.
   selected_index: zero-based index of the highlighted item.
   head: linked list returned by get_directory_information()
*/
void gui_draw_file_list(file_info *head, int selected_index);

/* Draw file info in the right pane for the selected file */
void gui_draw_file_info(file_info *selected);

/* Show a centered popup menu. options is an array of strings (count entries).
   Returns selected index [0..count-1] or -1 if cancelled (ESC).
*/
int gui_popup_menu(const char *title, const char *options[], int count);

/* Prompt for a password in a centered box. The result is stored in out (maxlen).
   Returns 0 on success, -1 on cancel (ESC).
*/
int gui_prompt_password(char *out, size_t maxlen);

/* Small centered message popup (OK on any key) */
void gui_message(const char *fmt, ...);

#endif /* GUI_H */
