#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define CANVAS_WIDTH 60
#define CANVAS_HEIGHT 30
#define MAX_UNDO 15
#define MAX_LABEL 32
/* Shape types supported by the editor */
typedef enum {
    RECT,
    CIRCLE,
    LINE,
    TRIANGLE
} ShapeType;
/* Preset themes and their corresponding characters */
typedef enum {
    CLASSIC = '*',
    BLOCK = '#',
    TECH = '@',
    ARTISTIC = '%'
} Theme;
/* Main Object struct storing details of a drawn shape */
typedef struct Object {
    int id;
    ShapeType type;
    int x, y, x2, y2, x3, y3;
    int w, h, radius;
    char symbol;
    int filled;
    char label[MAX_LABEL];
    Theme theme;
    time_t created_at;
    int edit_count;
} Object;
/* Singly linked list Node structure */
typedef struct Node {
    Object* data;
    struct Node* next;
} Node;
/* Represents the 2D grid canvas */
typedef struct {
    char grid[CANVAS_HEIGHT][CANVAS_WIDTH];
} Canvas;
/* State snapshot for Undo/Redo mechanism */
typedef struct {
    Node* head;
    Canvas canvas;
} Snapshot;
/* Stack to hold historical snapshots */
typedef struct {
    Snapshot items[MAX_UNDO];
    int top;
} UndoRedoStack;
/* Axis-Aligned Bounding Box (AABB) coordinate bounds */
typedef struct {
    int min_x, min_y;
    int max_x, max_y;
} BoundingBox;
/* Canvas Functions */
void canvas_clear(Canvas *canvas);
void canvas_display(const Canvas *canvas);
void canvas_put(Canvas *canvas, int x, int y, char c);
/* Drawing Algorithms */
void draw_line_dda(Canvas *canvas, int x1, int y1, int x2, int y2, char symbol);
void draw_circle_bresenham(Canvas *canvas, int xc, int yc, int r, char symbol, int filled);
void draw_rectangle(Canvas *canvas, int x, int y, int w, int h, char symbol, int filled);
void draw_triangle(Canvas *canvas, int x1, int y1, int x2, int y2, int x3, int y3, char symbol, int filled);
void draw_object(Canvas *canvas, const Object *obj);
void redraw_all(Canvas *canvas, const Node *head);
/* List Operations */
Node* add_object(Node **head, ShapeType type, int x, int y, int x2, int y2, int x3, int y3, int w, int h, int radius, char symbol, int filled, const char *label, Theme theme);
int delete_object(Node **head, int id);
Object* find_object(Node *head, int id);
void move_object(Object *obj, int dx, int dy);
void resize_object(Object *obj, int w, int h, int r);
void clone_object(Node **head, int id);
void group_move(Node *head, int dx, int dy);
void free_list(Node *head);
Node* copy_list(const Node *source);
/* Collision/Overlap Detection */
BoundingBox get_bounding_box(const Object *obj);
int check_overlap(Object *a, Object *b);
void check_all_overlaps_for_new(Node *head, Object *new_obj);
/* Undo/Redo functions */
void push_snapshot(UndoRedoStack *stack, Node *head, const Canvas *canvas);
void clear_redo_stack(UndoRedoStack *stack);
int perform_undo(UndoRedoStack *u_stack, UndoRedoStack *r_stack, Node **head, Canvas *canvas);
int perform_redo(UndoRedoStack *u_stack, UndoRedoStack *r_stack, Node **head, Canvas *canvas);
/* File I/O */
int save_drawing(const char *filename, const Node *head, const Canvas *canvas);
int load_drawing(const char *filename, Node **head, Canvas *canvas);
/* Templates */
void generate_template(Node **head, int choice, int x, int y, Theme theme);
#endif /* GRAPHICS_H */
