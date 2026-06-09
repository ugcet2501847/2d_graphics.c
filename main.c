#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_WIDTH 60
#define CANVAS_HEIGHT 20
#define MAX_SHAPES 50

// Enumeration for supported shape types
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Structures to store shape configurations
typedef struct { int x1, y1, x2, y2; } Line;
typedef struct { int x, y, w, h; } Rectangle;
typedef struct { int cx, cy, r; } Circle;
typedef struct { int x1, y1, x2, y2, x3, y3; } Triangle;

typedef struct {
    ShapeType type;
    int id; // Unique identification for modifying/deleting
    union {
        Line line;
        Rectangle rect;
        Circle circle;
        Triangle tri;
    } data;
} Shape;

// Global array tracking active canvas elements
Shape shape_list[MAX_SHAPES];
int total_shapes = 0;
int next_id = 1;

// Forward Declarations
void init_canvas(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]);
void display_canvas(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]);
void render_all_shapes(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]);

void draw_line(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x1, int y1, int x2, int y2);
void draw_rectangle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x, int y, int w, int h);
void draw_circle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int cx, int cy, int radius);
void draw_triangle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x1, int y1, int x2, int y2, int x3, int y3);

void menu_add_shape();
void menu_delete_shape();
void menu_modify_shape();

int main() {
    char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
    int choice;

    while (1) {
        // 1. Render data to canvas and print screen
        render_all_shapes(canvas);
        printf("\n=================== 2D GRAPHICS EDITOR ===================\n");
        display_canvas(canvas);
        
        // 2. Interactive Menu Interface
        printf("1. Add a Shape\n");
        printf("2. Delete a Shape\n");
        printf("3. Modify a Shape\n");
        printf("4. Exit Program\n");
        printf("Enter your choice (1-4): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input execution stalled.\n");
            break;
        }

        if (choice == 4) {
            printf("Exiting Graphics Editor. Goodbye!\n");
            break;
        }

        switch (choice) {
            case 1: menu_add_shape(); break;
            case 2: menu_delete_shape(); break;
            case 3: menu_modify_shape(); break;
            default: printf("\n[!] Invalid selection. Please choose options 1-4.\n");
        }
    }
    return 0;
}

// ==========================================
// Canvas State & Rendering Management
// ==========================================

void init_canvas(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

void display_canvas(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    // Print Top Boundary Ruler
    printf("   +");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n");

    // Print rows containing matrix indices
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Print Bottom Boundary Ruler
    printf("   +");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n");
}

void render_all_shapes(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    // Step 1: Blank slate reset
    init_canvas(canvas);

    // Step 2: Progressively stack active shapes down the pipeline
    for (int i = 0; i < total_shapes; i++) {
        Shape s = shape_list[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(canvas, s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s.data.rect.x, s.data.rect.y, s.data.rect.w, s.data.rect.h);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s.data.tri.x1, s.data.tri.y1, s.data.tri.x2, s.data.tri.y2, s.data.tri.x3, s.data.tri.y3);
                break;
        }
    }
}

// ==========================================
// Core Low-Level Math/Drawing Implementations
// ==========================================

void draw_line(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        if (x1 >= 0 && x1 < CANVAS_WIDTH && y1 >= 0 && y1 < CANVAS_HEIGHT) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void draw_rectangle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++) {
        if (i >= 0 && i < CANVAS_WIDTH) {
            if (y >= 0 && y < CANVAS_HEIGHT) canvas[y][i] = '*';
            if (y + h - 1 >= 0 && y + h - 1 < CANVAS_HEIGHT) canvas[y + h - 1][i] = '*';
        }
    }
    for (int j = y; j < y + h; j++) {
        if (j >= 0 && j < CANVAS_HEIGHT) {
            if (x >= 0 && x < CANVAS_WIDTH) canvas[j][x] = '*';
            if (x + w - 1 >= 0 && x + w - 1 < CANVAS_WIDTH) canvas[j][x + w - 1] = '*';
        }
    }
}

void draw_circle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int cx, int cy, int radius) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        // Multiplied by 2 on the X axis offset to fix character font stretching distortion
        int points[8][2] = {
            {cx + (x * 2), cy + y}, {cx - (x * 2), cy + y},
            {cx + (x * 2), cy - y}, {cx - (x * 2), cy - y},
            {cx + (y * 2), cy + x}, {cx - (y * 2), cy + x},
            {cx + (y * 2), cy - x}, {cx - (y * 2), cy - x}
        };

        for (int i = 0; i < 8; i++) {
            int px = points[i][0];
            int py = points[i][1];
            if (px >= 0 && px < CANVAS_WIDTH && py >= 0 && py < CANVAS_HEIGHT) {
                canvas[py][px] = '*';
            }
        }
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
        else { d = d + 4 * x + 6; }
    }
}

void draw_triangle(char canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

// ==========================================
// Active Object Modification Modules (CRUD)
// ==========================================

void menu_add_shape() {
    if (total_shapes >= MAX_SHAPES) {
        printf("Memory limit reached. Cannot add more elements.\n");
        return;
    }

    Shape s;
    s.id = next_id++;
    printf("\n--- Add Shape ---\n1. Line\n2. Rectangle\n3. Circle\n4. Triangle\nSelect type: ");
    int type;
    scanf("%d", &type);
    s.type = (ShapeType)type;

    if (type == 1) {
        printf("Enter x1 y1 x2 y2: ");
        scanf("%d %d %d %d", &s.data.line.x1, &s.data.line.y1, &s.data.line.x2, &s.data.line.y2);
    } else if (type == 2) {
        printf("Enter top-left x, y, width, height: ");
        scanf("%d %d %d %d", &s.data.rect.x, &s.data.rect.y, &s.data.rect.w, &s.data.rect.h);
    } else if (type == 3) {
        printf("Enter center cx, cy, and radius: ");
        scanf("%d %d %d", &s.data.circle.cx, &s.data.circle.cy, &s.data.circle.r);
    } else if (type == 4) {
        printf("Enter x1 y1 x2 y2 x3 y3: ");
        scanf("%d %d %d %d %d %d", &s.data.tri.x1, &s.data.tri.y1, &s.data.tri.x2, &s.data.tri.y2, &s.data.tri.x3, &s.data.tri.y3);
    } else {
        printf("Invalid shape type creation aborted.\n");
        return;
    }

    shape_list[total_shapes++] = s;
    printf("Shape added successfully with Assign-ID: %d\n", s.id);
}

void menu_delete_shape() {
    if (total_shapes == 0) {
        printf("Canvas is currently empty.\n");
        return;
    }

    printf("\nActive Elements Tracked:\n");
    for(int i=0; i<total_shapes; i++) {
        printf("ID: %d | Type Option [%d]\n", shape_list[i].id, shape_list[i].type);
    }

    printf("Enter the ID of the shape to delete: ");
    int target_id, found_idx = -1;
    scanf("%d", &target_id);

    for (int i = 0; i < total_shapes; i++) {
        if (shape_list[i].id == target_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) {
        printf("Shape ID reference not found.\n");
        return;
    }

    // Shift left to overwrite deleted entity space smoothly
    for (int i = found_idx; i < total_shapes - 1; i++) {
        shape_list[i] = shape_list[i + 1];
    }
    total_shapes--;
    printf("Shape %d deleted successfully.\n", target_id);
}

void menu_modify_shape() {
    if (total_shapes == 0) {
        printf("No active shapes to modify.\n");
        return;
    }

    printf("Enter the ID of the shape to modify: ");
    int target_id, found_idx = -1;
    scanf("%d", &target_id);

    for (int i = 0; i < total_shapes; i++) {
        if (shape_list[i].id == target_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) {
        printf("Target element lookup mismatch.\n");
        return;
    }

    Shape *s = &shape_list[found_idx];
    printf("Updating coordinates for Shape ID %d:\n", s->id);

    switch (s->type) {
        case SHAPE_LINE:
            printf("Enter new x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &s->data.line.x1, &s->data.line.y1, &s->data.line.x2, &s->data.line.y2);
            break;
        case SHAPE_RECTANGLE:
            printf("Enter new top-left x, y, width, height: ");
            scanf("%d %d %d %d", &s->data.rect.x, &s->data.rect.y, &s->data.rect.w, &s->data.rect.h);
            break;
        case SHAPE_CIRCLE:
            printf("Enter new center cx, cy, and radius: ");
            scanf("%d %d %d", &s->data.circle.cx, &s->data.circle.cy, &s->data.circle.r);
            break;
        case SHAPE_TRIANGLE:
            printf("Enter new x1 y1 x2 y2 x3 y3: ");
            scanf("%d %d %d %d %d %d", &s->data.tri.x1, &s->data.tri.y1, &s->data.tri.x2, &s->data.tri.y2, &s->data.tri.x3, &s->data.tri.y3);
            break;
    }
    printf("Shape %d updated successfully.\n", target_id);
}
