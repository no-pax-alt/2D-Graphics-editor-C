#include "graphics.h"
/* Global tracking variables for the program session */
time_t program_start_time;
int total_undo_operations = 0;
int operation_count = 0;
Theme current_theme = CLASSIC;
/*
 * PURPOSE:  Reads an integer from stdin, validating the format and preventing crashes
 * PARAMS:   prompt - output message to display before input
 * RETURNS:  Validated integer value
 */
int read_int(const char *prompt) {
    char buf[128];
    int val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        if (sscanf(buf, "%d", &val) == 1) {
            return val;
        } else {
            printf("Error: Invalid integer format. Please try again.\n");
        }
    }
}
/*
 * PURPOSE:  Reads an integer within a specific inclusive range, re-prompting on violation
 * PARAMS:   prompt   - message
 *           min_val  - lower bound
 *           max_val  - upper bound
 * RETURNS:  Validated integer in range
 */
int read_int_range(const char *prompt, int min_val, int max_val) {
    char buf[128];
    int val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        if (sscanf(buf, "%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            } else {
                printf("Error: Value must be between %d and %d.\n", min_val, max_val);
            }
        } else {
            printf("Error: Invalid integer format.\n");
        }
    }
}
/*
 * PURPOSE:  Reads the first non-whitespace character, ignoring extra characters
 * PARAMS:   prompt - message
 * RETURNS:  Character input
 */
char read_char(const char *prompt) {
    char buf[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        int i = 0;
        while (buf[i] == ' ' || buf[i] == '\t') i++;
        if (buf[i] != '\n' && buf[i] != '\0') {
            return buf[i];
        }
    }
}
/*
 * PURPOSE:  Reads a line of string, removing trailing newlines and enforcing non-emptiness
 * PARAMS:   prompt  - message
 *           dest    - destination buffer pointer
 *           max_len - max buffer capacity
 * RETURNS:  void
 */
void read_string(const char *prompt, char *dest, int max_len) {
    char buf[256];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) {
            strncpy(dest, buf, max_len - 1);
            dest[max_len - 1] = '\0';
            return;
        } else {
            printf("Error: Input cannot be empty.\n");
        }
    }
}
/*
 * PURPOSE:  Helper for interactive modifying. Keeps current int if user hits ENTER
 * PARAMS:   prompt      - message
 *           current_val - existing value to retain
 *           out_val     - pointer to store changes
 * RETURNS:  1 if modified, 0 if kept
 */
int read_int_or_keep(const char *prompt, int current_val, int *out_val) {
    char buf[128];
    printf("%s [%d]: ", prompt, current_val);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) == 0) {
        return 0;
    }
    int val;
    if (sscanf(buf, "%d", &val) == 1) {
        *out_val = val;
        return 1;
    }
    printf("Invalid input. Keeping current value.\n");
    return 0;
}
/*
 * PURPOSE:  Helper for interactive modifying. Keeps current char if user hits ENTER
 * PARAMS:   prompt      - message
 *           current_val - existing char
 *           out_val     - storage pointer
 * RETURNS:  1 if modified, 0 if kept
 */
int read_char_or_keep(const char *prompt, char current_val, char *out_val) {
    char buf[128];
    printf("%s [%c]: ", prompt, current_val);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) == 0) {
        return 0;
    }
    int i = 0;
    while (buf[i] == ' ' || buf[i] == '\t') i++;
    if (buf[i] != '\0') {
        *out_val = buf[i];
        return 1;
    }
    return 0;
}
/*
 * PURPOSE:  Helper for interactive modifying. Keeps current string if user hits ENTER
 * PARAMS:   prompt      - message
 *           current_val - existing string
 *           dest        - destination buffer
 *           max_len     - capacity
 * RETURNS:  1 if modified, 0 if kept
 */
int read_string_or_keep(const char *prompt, const char *current_val, char *dest, int max_len) {
    char buf[256];
    printf("%s [%s]: ", prompt, current_val);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) == 0) {
        return 0;
    }
    strncpy(dest, buf, max_len - 1);
    dest[max_len - 1] = '\0';
    return 1;
}
/*
 * PURPOSE:  Interactively prompts the user to edit fields of an existing object
 * PARAMS:   head   - pointer to list head
 *           canvas - pointer to Canvas
 *           target_id - ID of the object to modify
 * RETURNS:  void
 */
void modify_object_interactive(Node *head, Canvas *canvas, int target_id) {
    Object *obj = find_object(head, target_id);
    if (!obj) {
        printf("Error: Object not found.\n");
        return;
    }
    
    printf("\nModifying Object #%d '%s'. Press ENTER to keep current value.\n", obj->id, obj->label);
    
    int temp_int;
    char temp_char;
    char temp_str[256];
    
    if (read_int_or_keep("Enter new X coordinate", obj->x, &temp_int)) {
        int dx = temp_int - obj->x;
        move_object(obj, dx, 0);
        obj->edit_count--; // compensate for move_object incrementing count
    }
    if (read_int_or_keep("Enter new Y coordinate", obj->y, &temp_int)) {
        int dy = temp_int - obj->y;
        move_object(obj, 0, dy);
        obj->edit_count--;
    }
    
    if (obj->type == RECT) {
        if (read_int_or_keep("Enter new Width", obj->w, &temp_int)) {
            obj->w = temp_int < 1 ? 1 : temp_int;
        }
        if (read_int_or_keep("Enter new Height", obj->h, &temp_int)) {
            obj->h = temp_int < 1 ? 1 : temp_int;
        }
    } else if (obj->type == CIRCLE) {
        if (read_int_or_keep("Enter new Radius", obj->radius, &temp_int)) {
            obj->radius = temp_int < 1 ? 1 : temp_int;
        }
    } else if (obj->type == LINE) {
        if (read_int_or_keep("Enter new X2 coordinate", obj->x2, &temp_int)) {
            obj->x2 = temp_int;
        }
        if (read_int_or_keep("Enter new Y2 coordinate", obj->y2, &temp_int)) {
            obj->y2 = temp_int;
        }
    } else if (obj->type == TRIANGLE) {
        if (read_int_or_keep("Enter new X2 coordinate", obj->x2, &temp_int)) {
            obj->x2 = temp_int;
        }
        if (read_int_or_keep("Enter new Y2 coordinate", obj->y2, &temp_int)) {
            obj->y2 = temp_int;
        }
        if (read_int_or_keep("Enter new X3 coordinate", obj->x3, &temp_int)) {
            obj->x3 = temp_int;
        }
        if (read_int_or_keep("Enter new Y3 coordinate", obj->y3, &temp_int)) {
            obj->y3 = temp_int;
        }
    }
    
    if (read_char_or_keep("Enter new Symbol character", obj->symbol, &temp_char)) {
        obj->symbol = temp_char;
    }
    
    if (read_int_or_keep("Enter new Filled flag (0=outline, 1=filled)", obj->filled, &temp_int)) {
        obj->filled = temp_int ? 1 : 0;
    }
    
    if (read_string_or_keep("Enter new Label", obj->label, temp_str, MAX_LABEL)) {
        strncpy(obj->label, temp_str, MAX_LABEL - 1);
        obj->label[MAX_LABEL - 1] = '\0';
    }
    
    printf("Preset themes: 1. CLASSIC(*), 2. BLOCK(#), 3. TECH(@), 4. ARTISTIC(%%)\n");
    if (read_int_or_keep("Enter new Theme choice (1-4)", (int)obj->theme, &temp_int)) {
        if (temp_int == 1) { obj->theme = CLASSIC; obj->symbol = '*'; }
        else if (temp_int == 2) { obj->theme = BLOCK; obj->symbol = '#'; }
        else if (temp_int == 3) { obj->theme = TECH; obj->symbol = '@'; }
        else if (temp_int == 4) { obj->theme = ARTISTIC; obj->symbol = '%'; }
    }
    
    obj->edit_count++;
    redraw_all(canvas, head);
    printf("Success: Object #%d modified.\n", obj->id);
}
/*
 * PURPOSE:  Gathers and prints a panel-bordered summary of editor stats
 * PARAMS:   head       - pointer to list head
 *           canvas     - pointer to Canvas
 *           undo_ops   - total undo counts
 *           start_time - program start timestamp
 *           op_cnt     - current session operation count
 * RETURNS:  void
 */
void display_dashboard(const Node *head, const Canvas *canvas, int undo_ops, time_t start_time, int op_cnt) {
    int total = 0;
    int rect_cnt = 0, circle_cnt = 0, line_cnt = 0, tri_cnt = 0;
    
    const Node *curr = head;
    while (curr) {
        total++;
        if (curr->data->type == RECT) rect_cnt++;
        else if (curr->data->type == CIRCLE) circle_cnt++;
        else if (curr->data->type == LINE) line_cnt++;
        else if (curr->data->type == TRIANGLE) tri_cnt++;
        curr = curr->next;
    }
    
    int filled_cells = 0;
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            if (canvas->grid[y][x] != '.') {
                filled_cells++;
            }
        }
    }
    double fill_pct = (double)filled_cells * 100.0 / (CANVAS_WIDTH * CANVAS_HEIGHT);
    
    int sym_freq[256] = {0};
    curr = head;
    while (curr) {
        unsigned char sym = (unsigned char)curr->data->symbol;
        sym_freq[sym]++;
        curr = curr->next;
    }
    char most_used_sym = '\0';
    int max_freq = 0;
    for (int i = 0; i < 256; i++) {
        if (sym_freq[i] > max_freq && i > 32 && i < 127) {
            max_freq = sym_freq[i];
            most_used_sym = (char)i;
        }
    }
    
    Object *most_edited = NULL;
    int max_edits = -1;
    curr = head;
    while (curr) {
        if (curr->data->edit_count > max_edits) {
            max_edits = curr->data->edit_count;
            most_edited = curr->data;
        }
        curr = curr->next;
    }
    
    time_t now = time(NULL);
    int diff_sec = (int)difftime(now, start_time);
    int hrs = diff_sec / 3600;
    int mins = (diff_sec % 3600) / 60;
    int secs = diff_sec % 60;
    
    printf("\n┌────────────────────────────────────────────────────────┐\n");
    printf("│                  STATISTICS DASHBOARD                  │\n");
    printf("├────────────────────────────────────────────────────────┤\n");
    printf("│ Total Objects:      %-35d│\n", total);
    printf("│   - Rectangles:     %-35d│\n", rect_cnt);
    printf("│   - Circles:        %-35d│\n", circle_cnt);
    printf("│   - Lines:          %-35d│\n", line_cnt);
    printf("│   - Triangles:      %-35d│\n", tri_cnt);
    printf("│ Canvas Fill:        %-34.2f%%│\n", fill_pct);
    
    if (max_freq > 0) {
        printf("│ Most-Used Symbol:   '%c' (used %d times)                  │\n", most_used_sym, max_freq);
    } else {
        printf("│ Most-Used Symbol:   None                               │\n");
    }
    
    if (most_edited && max_edits > 0) {
        char edit_str[64];
        snprintf(edit_str, sizeof(edit_str), "#%d '%s' (%d edits)", most_edited->id, most_edited->label, max_edits);
        printf("│ Most-Edited Shape:  %-35s│\n", edit_str);
    } else {
        printf("│ Most-Edited Shape:  None                               │\n");
    }
    
    printf("│ Undo Operations:    %-35d│\n", undo_ops);
    printf("│ Session Duration:   %02d:%02d:%02d                           │\n", hrs, mins, secs);
    
    int last_autosave = (op_cnt / 5) * 5;
    if (last_autosave > 0) {
        printf("│ Auto-Save Status:   Saved (autosave.gfx at op %-8d)│\n", last_autosave);
    } else {
        printf("│ Auto-Save Status:   Waiting for op 5                   │\n");
    }
    printf("└────────────────────────────────────────────────────────┘\n");
}
/*
 * PURPOSE:  Prints detailed metadata table for a selected object by ID
 * PARAMS:   head      - pointer to list head
 *           target_id - ID of shape
 * RETURNS:  void
 */
void display_inspector(Node *head, int target_id) {
    Object *obj = find_object(head, target_id);
    if (!obj) {
        printf("Error: Object not found for ID %d.\n", target_id);
        return;
    }
    
    char time_buf[64];
    struct tm *tm_info = localtime(&obj->created_at);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    
    BoundingBox bbox = get_bounding_box(obj);
    
    char overlap_buf[256] = "";
    Node *curr = head;
    int first = 1;
    while (curr) {
        if (curr->data != obj && check_overlap(obj, curr->data)) {
            char temp_str[16];
            snprintf(temp_str, sizeof(temp_str), "#%d", curr->data->id);
            if (!first) {
                strncat(overlap_buf, ", ", sizeof(overlap_buf) - strlen(overlap_buf) - 1);
            }
            strncat(overlap_buf, temp_str, sizeof(overlap_buf) - strlen(overlap_buf) - 1);
            first = 0;
        }
        curr = curr->next;
    }
    if (first) {
        strcpy(overlap_buf, "None");
    }
    
    const char *type_name = "RECTANGLE";
    if (obj->type == CIRCLE) type_name = "CIRCLE";
    else if (obj->type == LINE) type_name = "LINE";
    else if (obj->type == TRIANGLE) type_name = "TRIANGLE";
    
    const char *theme_name = "CLASSIC";
    if (obj->theme == BLOCK) theme_name = "BLOCK";
    else if (obj->theme == TECH) theme_name = "TECH";
    else if (obj->theme == ARTISTIC) theme_name = "ARTISTIC";
    printf("\n┌────────────────────────────────────────────────────────┐\n");
    printf("│                   OBJECT INSPECTOR                     │\n");
    printf("├────────────────────────────────────────────────────────┤\n");
    printf("│ ID:          %-42d│\n", obj->id);
    printf("│ Label:       %-42s│\n", obj->label);
    printf("│ Type:        %-42s│\n", type_name);
    
    if (obj->type == RECT) {
        printf("│ Coordinates: Top-Left: (%d, %d)                         │\n", obj->x, obj->y);
        printf("│ Size:        w=%-10d h=%-27d│\n", obj->w, obj->h);
    } else if (obj->type == CIRCLE) {
        printf("│ Coordinates: Center: (%d, %d)                           │\n", obj->x, obj->y);
        printf("│ Size:        r=%-40d│\n", obj->radius);
    } else if (obj->type == LINE) {
        printf("│ Coordinates: (%d, %d) to (%d, %d)                       │\n", obj->x, obj->y, obj->x2, obj->y2);
    } else if (obj->type == TRIANGLE) {
        printf("│ Coordinates: P1(%d,%d), P2(%d,%d), P3(%d,%d)            │\n", obj->x, obj->y, obj->x2, obj->y2, obj->x3, obj->y3);
    }
    
    printf("│ Symbol:      '%c'                                       │\n", obj->symbol);
    printf("│ Filled:      %-42s│\n", obj->filled ? "Yes" : "No");
    printf("│ Theme:       %-42s│\n", theme_name);
    printf("│ Created At:  %-42s│\n", time_buf);
    printf("│ Edit Count:  %-42d│\n", obj->edit_count);
    printf("│ BBox:        (%d, %d) to (%d, %d)                       │\n", bbox.min_x, bbox.min_y, bbox.max_x, bbox.max_y);
    printf("│ Overlaps:    %-42s│\n", overlap_buf);
    printf("└────────────────────────────────────────────────────────┘\n");
}
/*
 * PURPOSE:  Displays the 17-option main control panel
 * PARAMS:   void
 * RETURNS:  void
 */
void display_menu(void) {
    printf("\n┌─────────────────────────────────┐\n");
    printf("│   2D GRAPHICS EDITOR  v2.0      │\n");
    printf("├─────────────────────────────────┤\n");
    printf("│  1. Draw Shape                  │\n");
    printf("│  2. Move Shape                  │\n");
    printf("│  3. Resize Shape                │\n");
    printf("│  4. Delete Shape                │\n");
    printf("│  5. Clone Shape                 │\n");
    printf("│  6. Group Move                  │\n");
    printf("│  7. Object Inspector            │\n");
    printf("│  8. Show Object List            │\n");
    printf("│  9. Statistics Dashboard        │\n");
    printf("│ 10. Save Drawing                │\n" );
    printf("│ 11. Load Drawing                │\n");
    printf("│ 12. Generate Template           │\n");
    printf("│ 13. Change Theme                │\n");
    printf("│ 14. Undo                        │\n");
    printf("│ 15. Redo                        │\n");
    printf("│ 16. Display Canvas              │\n");
    printf("│ 17. Exit                        │\n");
    printf("└─────────────────────────────────┘\n");
}
/*
 * PURPOSE:  Main execution loop and menu routing
 * PARAMS:   void
 * RETURNS:  0 on clean exit
 */
int main(void) {
    Canvas canvas;
    Node *head = NULL;
    UndoRedoStack undo_stack = { .top = -1 };
    UndoRedoStack redo_stack = { .top = -1 };
    
    program_start_time = time(NULL);
    canvas_clear(&canvas);
    
    while (1) {
        display_menu();
        int choice = read_int_range("Select option (1-17): ", 1, 17);
        
        // Mutating operations flag
        int mutated = 0;
        
        switch (choice) {
            case 1: { // Draw Shape
                printf("\nShape Types: 1. Rectangle, 2. Circle, 3. Line, 4. Triangle\n");
                int type_choice = read_int_range("Select shape type: ", 1, 4);
                ShapeType type = RECT;
                int x = 0, y = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
                int w = 0, h = 0, r = 0;
                
                if (type_choice == 1) {
                    type = RECT;
                    x = read_int_range("Enter top-left X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y = read_int_range("Enter top-left Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                    w = read_int_range("Enter width (1-60): ", 1, CANVAS_WIDTH);
                    h = read_int_range("Enter height (1-30): ", 1, CANVAS_HEIGHT);
                } else if (type_choice == 2) {
                    type = CIRCLE;
                    x = read_int_range("Enter center X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y = read_int_range("Enter center Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                    r = read_int_range("Enter radius (1-30): ", 1, 30);
                } else if (type_choice == 3) {
                    type = LINE;
                    x = read_int_range("Enter start X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y = read_int_range("Enter start Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                    x2 = read_int_range("Enter end X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y2 = read_int_range("Enter end Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                } else if (type_choice == 4) {
                    type = TRIANGLE;
                    x = read_int_range("Enter vertex 1 X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y = read_int_range("Enter vertex 1 Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                    x2 = read_int_range("Enter vertex 2 X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y2 = read_int_range("Enter vertex 2 Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                    x3 = read_int_range("Enter vertex 3 X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                    y3 = read_int_range("Enter vertex 3 Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                }
                
                char sym = (char)current_theme;
                char use_custom = read_char("Use custom symbol instead of theme symbol? (y/n): ");
                if (use_custom == 'y' || use_custom == 'Y') {
                    sym = read_char("Enter drawing symbol character: ");
                }
                
                int filled = 0;
                if (type_choice != 3) { // lines cannot be filled
                    int filled_choice = read_int_range("Fill style: 0. Outline, 1. Filled: ", 0, 1);
                    filled = (filled_choice == 1);
                }
                
                char label[MAX_LABEL];
                read_string("Enter a label/name for this shape: ", label, MAX_LABEL);
                
                // Save state for undo before drawing
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                Node *new_node = add_object(&head, type, x, y, x2, y2, x3, y3, w, h, r, sym, filled, label, current_theme);
                if (new_node) {
                    redraw_all(&canvas, head);
                    printf("Success: Added shape #%d '%s'\n", new_node->data->id, new_node->data->label);
                    check_all_overlaps_for_new(head, new_node->data);
                }
                mutated = 1;
                break;
            }
            case 2: { // Move Shape
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int id = read_int("Enter Object ID to move: ");
                Object *obj = find_object(head, id);
                if (!obj) {
                    printf("Error: Object not found.\n");
                    break;
                }
                int dx = read_int("Enter translation dx (horizontal): ");
                int dy = read_int("Enter translation dy (vertical): ");
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                move_object(obj, dx, dy);
                redraw_all(&canvas, head);
                printf("Success: Moved shape #%d by (%d, %d)\n", id, dx, dy);
                mutated = 1;
                break;
            }
            case 3: { // Resize / Modify Shape
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int id = read_int("Enter Object ID to resize/modify: ");
                Object *obj = find_object(head, id);
                if (!obj) {
                    printf("Error: Object not found.\n");
                    break;
                }
                printf("Resize Options:\n1. Resize dimensions (width/height/radius)\n2. Interactive Modify (edit any field)\n");
                int res_opt = read_int_range("Select option (1-2): ", 1, 2);
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                if (res_opt == 1) {
                    if (obj->type == RECT) {
                        int nw = read_int_range("Enter new width (1-60): ", 1, CANVAS_WIDTH);
                        int nh = read_int_range("Enter new height (1-30): ", 1, CANVAS_HEIGHT);
                        resize_object(obj, nw, nh, 0);
                        printf("Success: Rectangle #%d resized.\n", id);
                    } else if (obj->type == CIRCLE) {
                        int nr = read_int_range("Enter new radius (1-30): ", 1, 30);
                        resize_object(obj, 0, 0, nr);
                        printf("Success: Circle #%d resized.\n", id);
                    } else {
                        printf("Note: Direct resizing is not supported for Lines and Triangles. Please use Interactive Modify instead.\n");
                    }
                } else {
                    modify_object_interactive(head, &canvas, id);
                }
                redraw_all(&canvas, head);
                mutated = 1;
                break;
            }
            case 4: { // Delete Shape
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int id = read_int("Enter Object ID to delete: ");
                Object *obj = find_object(head, id);
                if (!obj) {
                    printf("Error: Object not found.\n");
                    break;
                }
                printf("Delete object #%d '%s'? (y/n): ", obj->id, obj->label);
                char confirm = read_char("");
                if (confirm == 'y' || confirm == 'Y') {
                    push_snapshot(&undo_stack, head, &canvas);
                    clear_redo_stack(&redo_stack);
                    
                    delete_object(&head, id);
                    redraw_all(&canvas, head);
                    printf("Success: Object deleted.\n");
                    mutated = 1;
                } else {
                    printf("Operation cancelled.\n");
                }
                break;
            }
            case 5: { // Clone Shape
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int id = read_int("Enter Object ID to clone: ");
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                clone_object(&head, id);
                redraw_all(&canvas, head);
                mutated = 1;
                break;
            }
            case 6: { // Group Move
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int dx = read_int("Enter translation dx (horizontal) for all shapes: ");
                int dy = read_int("Enter translation dy (vertical) for all shapes: ");
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                group_move(head, dx, dy);
                redraw_all(&canvas, head);
                printf("Success: Moved all shapes.\n");
                mutated = 1;
                break;
            }
            case 7: { // Object Inspector
                if (!head) {
                    printf("Error: No shapes drawn yet.\n");
                    break;
                }
                int id = read_int("Enter Object ID to inspect: ");
                display_inspector(head, id);
                break;
            }
            case 8: { // Show Object List
                if (!head) {
                    printf("\nNo objects in drawing list.\n");
                    break;
                }
                printf("\n%-4s %-10s %-15s %-8s %-22s %-5s\n", "ID", "Type", "Label", "Symbol", "Coords/Size", "Edits");
                printf("─────────────────────────────────────────────────────────────────────────\n");
                Node *curr = head;
                while (curr) {
                    Object *o = curr->data;
                    const char *type_str = "Rect";
                    if (o->type == CIRCLE) type_str = "Circle";
                    else if (o->type == LINE) type_str = "Line";
                    else if (o->type == TRIANGLE) type_str = "Triangle";
                    
                    char coord_str[64];
                    if (o->type == RECT) {
                        snprintf(coord_str, sizeof(coord_str), "(%d,%d) w=%d h=%d", o->x, o->y, o->w, o->h);
                    } else if (o->type == CIRCLE) {
                        snprintf(coord_str, sizeof(coord_str), "(%d,%d) r=%d", o->x, o->y, o->radius);
                    } else if (o->type == LINE) {
                        snprintf(coord_str, sizeof(coord_str), "(%d,%d)->(%d,%d)", o->x, o->y, o->x2, o->y2);
                    } else {
                        snprintf(coord_str, sizeof(coord_str), "P1(%d,%d) P2(%d,%d) P3(%d,%d)", o->x, o->y, o->x2, o->y2, o->x3, o->y3);
                    }
                    
                    printf("%-4d %-10s %-15.15s '%c'      %-22s %-5d\n",
                           o->id, type_str, o->label, o->symbol, coord_str, o->edit_count);
                    curr = curr->next;
                }
                break;
            }
            case 9: { // Statistics Dashboard
                display_dashboard(head, &canvas, total_undo_operations, program_start_time, operation_count);
                break;
            }
            case 10: { // Save Drawing
                char filename[256];
                read_string("Enter filename to save drawing (e.g. house.gfx): ", filename, sizeof(filename));
                if (save_drawing(filename, head, &canvas)) {
                    printf("Success: Drawing saved to '%s'.\n", filename);
                } else {
                    printf("Error: Could not save drawing to '%s'.\n", filename);
                }
                break;
            }
            case 11: { // Load Drawing
                char filename[256];
                read_string("Enter filename to load drawing: ", filename, sizeof(filename));
                
                // Backup current state for undo
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                if (load_drawing(filename, &head, &canvas)) {
                    printf("Success: Drawing loaded from '%s'.\n", filename);
                } else {
                    printf("Error: Load failed. Existing drawing state kept intact.\n");
                    // Discard backup since no mutation occurred
                    free_list(undo_stack.items[undo_stack.top].head);
                    undo_stack.top--;
                }
                break;
            }
            case 12: { // Generate Template
                printf("\nTemplates:\n1. House\n2. Tree\n3. Rocket\n4. Arrow\n");
                int temp_choice = read_int_range("Select template: ", 1, 4);
                int bx = read_int_range("Enter base X coordinate (0-59): ", 0, CANVAS_WIDTH - 1);
                int by = read_int_range("Enter base Y coordinate (0-29): ", 0, CANVAS_HEIGHT - 1);
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                generate_template(&head, temp_choice, bx, by, current_theme);
                redraw_all(&canvas, head);
                printf("Success: Generated template shapes.\n");
                mutated = 1;
                break;
            }
            case 13: { // Change Theme
                printf("\nPreset Themes:\n");
                printf("1. CLASSIC ('*')\n");
                printf("2. BLOCK ('#')\n");
                printf("3. TECH ('@')\n");
                printf("4. ARTISTIC ('%%')\n");
                int theme_choice = read_int_range("Select theme (1-4): ", 1, 4);
                
                push_snapshot(&undo_stack, head, &canvas);
                clear_redo_stack(&redo_stack);
                
                if (theme_choice == 1) current_theme = CLASSIC;
                else if (theme_choice == 2) current_theme = BLOCK;
                else if (theme_choice == 3) current_theme = TECH;
                else if (theme_choice == 4) current_theme = ARTISTIC;
                
                // Prompt: should we update all existing shapes' symbols?
                char apply_all = read_char("Apply new theme to all existing shapes? (y/n): ");
                if (apply_all == 'y' || apply_all == 'Y') {
                    Node *curr = head;
                    while (curr) {
                        curr->data->theme = current_theme;
                        curr->data->symbol = (char)current_theme;
                        curr->data->edit_count++;
                        curr = curr->next;
                    }
                }
                redraw_all(&canvas, head);
                printf("Success: Theme changed.\n");
                mutated = 1;
                break;
            }
            case 14: { // Undo
                if (perform_undo(&undo_stack, &redo_stack, &head, &canvas)) {
                    total_undo_operations++;
                    operation_count++;
                    printf("Success: Undo performed.\n");
                } else {
                    printf("Error: Nothing to undo.\n");
                }
                break;
            }
            case 15: { // Redo
                if (perform_redo(&undo_stack, &redo_stack, &head, &canvas)) {
                    operation_count++;
                    printf("Success: Redo performed.\n");
                } else {
                    printf("Error: Nothing to redo.\n");
                }
                break;
            }
            case 16: { // Display Canvas
                canvas_display(&canvas);
                break;
            }
            case 17: { // Exit
                printf("Exiting... Goodbye!\n");
                free_list(head);
                clear_redo_stack(&undo_stack);
                clear_redo_stack(&redo_stack);
                return 0;
            }
        }
        
        if (mutated) {
            operation_count++;
            if (operation_count % 5 == 0) {
                if (save_drawing("autosave.gfx", head, &canvas)) {
                    printf("(Silently saved state to 'autosave.gfx')\n");
                }
            }
        }
        
        // Auto-display canvas and operation count after every selection (except when displaying list/inspecting/stats dashboard to prevent clutter, but we do it anyway per requirement)
        printf("\n[Operation Count: %d]\n", operation_count);
        canvas_display(&canvas);
    }
    
    return 0;
}
