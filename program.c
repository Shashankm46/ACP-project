#include <stdio.h>
#include <stdlib.h>
#define WIDTH 80
#define HEIGHT 25
#define MAX_SHAPES 100
// Types of shapes
typedef enum {
    TYPE_LINE = 1,
    TYPE_RECTANGLE,
    TYPE_CIRCLE,
    TYPE_TRIANGLE
} ShapeType;
// Parameters for shapes
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3; // For triangle
    int radius; // For circle
} ShapeParams;
// Shape definition
typedef struct {
    int id;
    ShapeType type;
    ShapeParams params;
} Shape;
// Canvas and Shape list
char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;
// Function prototypes
void init_canvas(void);
void plot(int x, int y);
void draw_line(int x0, int y0, int x1, int y1);
void draw_rectangle(int x1, int y1, int x2, int y2);
void plot_circle_points(int xc, int yc, int x, int y);
void draw_circle(int xc, int yc, int r);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void render_canvas(void);
void display_canvas(void);
int get_int(const char *prompt, int min_val, int max_val);
void list_shapes(void);
void add_shape(void);
void delete_shape(void);
void modify_shape_menu(void);
// Initialize canvas with underscores
void init_canvas(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}
// Plot a pixel on the canvas with clipping
void plot(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}
// Bresenham's Line Algorithm
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;
    while (1) {
        plot(x0, y0);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
// Rectangle drawing function (edges only)
void draw_rectangle(int x1, int y1, int x2, int y2) {
    int min_x = x1 < x2 ? x1 : x2;
    int max_x = x1 > x2 ? x1 : x2;
    int min_y = y1 < y2 ? y1 : y2;
    int max_y = y1 > y2 ? y1 : y2;
    // Top and bottom borders
    for (int x = min_x; x <= max_x; x++) {
        plot(x, min_y);
        plot(x, max_y);
    }
    // Left and right borders
    for (int y = min_y; y <= max_y; y++) {
        plot(x1, y);
        plot(x2, y);
    }
}
// Midpoint Circle Algorithm helpers
void plot_circle_points(int xc, int yc, int x, int y) {
    plot(xc + x, yc + y);
    plot(xc - x, yc + y);
    plot(xc + x, yc - y);
    plot(xc - x, yc - y);
    plot(xc + y, yc + x);
    plot(xc - y, yc + x);
    plot(xc + y, yc - x);
    plot(xc - y, yc - x);
}
// Midpoint Circle Algorithm
void draw_circle(int xc, int yc, int r) {
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    
    plot_circle_points(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(xc, yc, x, y);
    }
}
// Triangle drawing function (connect three vertices with lines)
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}
// Render all shapes to canvas
void render_canvas(void) {
    init_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case TYPE_LINE:
                draw_line(s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case TYPE_RECTANGLE:
                draw_rectangle(s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case TYPE_CIRCLE:
                draw_circle(s.params.x1, s.params.y1, s.params.radius);
                break;
            case TYPE_TRIANGLE:
                draw_triangle(s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.x3, s.params.y3);
                break;
        }
    }
}
// Display canvas with row and column headers
void display_canvas(void) {
    // Print column header index in steps of 10
    printf("    ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) {
            printf("%d", (x / 10) % 10);
        } else {
            printf(" ");
        }
    }
    printf("\n    ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
    // Top border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    // Content rows
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }
    // Bottom border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
}
// Robust integer input scanner
int get_int(const char *prompt, int min_val, int max_val) {
    int val;
    char term;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d%c", &val, &term) != 2 || term != '\n') {
            // clear stdin buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a valid integer.\n");
        } else if (val < min_val || val > max_val) {
            printf("Value out of bounds. Must be between %d and %d.\n", min_val, max_val);
        } else {
            return val;
        }
    }
}
// List all active shapes
void list_shapes(void) {
    if (shape_count == 0) {
        printf("No shapes on the canvas.\n");
        return;
    }
    printf("\n============== ACTIVE SHAPES ==============\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("ID: %d | ", s.id);
        switch (s.type) {
            case TYPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case TYPE_RECTANGLE:
                printf("Rectangle: top-left (%d, %d), bottom-right (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case TYPE_CIRCLE:
                printf("Circle: center (%d, %d), radius %d\n", s.params.x1, s.params.y1, s.params.radius);
                break;
            case TYPE_TRIANGLE:
                printf("Triangle: vertices (%d, %d), (%d, %d), (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.x3, s.params.y3);
                break;
        }
    }
    printf("===========================================\n");
}
// Add a shape to the canvas
void add_shape(void) {
    if (shape_count >= MAX_SHAPES) {
        printf("Maximum shape limit (%d) reached. Please delete some first.\n", MAX_SHAPES);
        return;
    }
    printf("\nChoose shape type to add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int choice = get_int("Enter option (1-4): ", 1, 4);
    Shape s;
    s.id = next_id++;
    s.type = (ShapeType)choice;
    switch (s.type) {
        case TYPE_LINE:
            s.params.x1 = get_int("Enter start X (0-79): ", 0, WIDTH - 1);
            s.params.y1 = get_int("Enter start Y (0-24): ", 0, HEIGHT - 1);
            s.params.x2 = get_int("Enter end X (0-79): ", 0, WIDTH - 1);
            s.params.y2 = get_int("Enter end Y (0-24): ", 0, HEIGHT - 1);
            break;
        case TYPE_RECTANGLE:
            s.params.x1 = get_int("Enter top-left X (0-79): ", 0, WIDTH - 1);
            s.params.y1 = get_int("Enter top-left Y (0-24): ", 0, HEIGHT - 1);
            s.params.x2 = get_int("Enter bottom-right X (0-79): ", 0, WIDTH - 1);
            s.params.y2 = get_int("Enter bottom-right Y (0-24): ", 0, HEIGHT - 1);
            break;
        case TYPE_CIRCLE:
            s.params.x1 = get_int("Enter center X (0-79): ", 0, WIDTH - 1);
            s.params.y1 = get_int("Enter center Y (0-24): ", 0, HEIGHT - 1);
            s.params.radius = get_int("Enter radius (0-80): ", 0, 80);
            break;
        case TYPE_TRIANGLE:
            s.params.x1 = get_int("Enter vertex 1 X (0-79): ", 0, WIDTH - 1);
            s.params.y1 = get_int("Enter vertex 1 Y (0-24): ", 0, HEIGHT - 1);
            s.params.x2 = get_int("Enter vertex 2 X (0-79): ", 0, WIDTH - 1);
            s.params.y2 = get_int("Enter vertex 2 Y (0-24): ", 0, HEIGHT - 1);
            s.params.x3 = get_int("Enter vertex 3 X (0-79): ", 0, WIDTH - 1);
            s.params.y3 = get_int("Enter vertex 3 Y (0-24): ", 0, HEIGHT - 1);
            break;
    }
    shapes[shape_count++] = s;
    printf("Shape added successfully with ID %d.\n", s.id);
    render_canvas();
}
// Delete a shape from the canvas
void delete_shape(void) {
    if (shape_count == 0) {
        printf("No shapes to delete.\n");
        return;
    }
    list_shapes();
    int id = get_int("Enter ID of shape to delete: ", 1, next_id - 1);
    int found = 0;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            // Shift elements
            for (int j = i; j < shape_count - 1; j++) {
                shapes[j] = shapes[j + 1];
            }
            shape_count--;
            found = 1;
            break;
        }
    }
    if (found) {
        printf("Shape ID %d deleted successfully.\n", id);
        render_canvas();
    } else {
        printf("Shape ID %d not found.\n", id);
    }
}
// Modify an existing shape
void modify_shape_menu(void) {
    if (shape_count == 0) {
        printf("No shapes to modify.\n");
        return;
    }
    list_shapes();
    int id = get_int("Enter ID of shape to modify: ", 1, next_id - 1);
    int idx = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("Shape ID %d not found.\n", id);
        return;
    }
    Shape *s = &shapes[idx];
    printf("Modifying shape ID %d.\n", id);
    switch (s->type) {
        case TYPE_LINE:
            printf("Current Line: (%d, %d) to (%d, %d)\n", s->params.x1, s->params.y1, s->params.x2, s->params.y2);
            s->params.x1 = get_int("Enter new start X (0-79): ", 0, WIDTH - 1);
            s->params.y1 = get_int("Enter new start Y (0-24): ", 0, HEIGHT - 1);
            s->params.x2 = get_int("Enter new end X (0-79): ", 0, WIDTH - 1);
            s->params.y2 = get_int("Enter new end Y (0-24): ", 0, HEIGHT - 1);
            break;
        case TYPE_RECTANGLE:
            printf("Current Rectangle: top-left (%d, %d), bottom-right (%d, %d)\n", s->params.x1, s->params.y1, s->params.x2, s->params.y2);
            s->params.x1 = get_int("Enter new top-left X (0-79): ", 0, WIDTH - 1);
            s->params.y1 = get_int("Enter new top-left Y (0-24): ", 0, HEIGHT - 1);
            s->params.x2 = get_int("Enter new bottom-right X (0-79): ", 0, WIDTH - 1);
            s->params.y2 = get_int("Enter new bottom-right Y (0-24): ", 0, HEIGHT - 1);
            break;
        case TYPE_CIRCLE:
            printf("Current Circle: center (%d, %d), radius %d\n", s->params.x1, s->params.y1, s->params.radius);
            s->params.x1 = get_int("Enter new center X (0-79): ", 0, WIDTH - 1);
            s->params.y1 = get_int("Enter new center Y (0-24): ", 0, HEIGHT - 1);
            s->params.radius = get_int("Enter new radius (0-80): ", 0, 80);
            break;
        case TYPE_TRIANGLE:
            printf("Current Triangle: vertices (%d, %d), (%d, %d), (%d, %d)\n", s->params.x1, s->params.y1, s->params.x2, s->params.y2, s->params.x3, s->params.y3);
            s->params.x1 = get_int("Enter new vertex 1 X (0-79): ", 0, WIDTH - 1);
            s->params.y1 = get_int("Enter new vertex 1 Y (0-24): ", 0, HEIGHT - 1);
            s->params.x2 = get_int("Enter new vertex 2 X (0-79): ", 0, WIDTH - 1);
            s->params.y2 = get_int("Enter new vertex 2 Y (0-24): ", 0, HEIGHT - 1);
            s->params.x3 = get_int("Enter new vertex 3 X (0-79): ", 0, WIDTH - 1);
            s->params.y3 = get_int("Enter new vertex 3 Y (0-24): ", 0, HEIGHT - 1);
            break;
    }
    printf("Shape ID %d modified successfully.\n", id);
    render_canvas();
}
int main(void) {
    // Initial render of empty canvas
    render_canvas();
    while (1) {
        printf("\n===========================================\n");
        printf("            2D GRAPHICS EDITOR             \n");
        printf("===========================================\n");
        printf("1. View Picture (Canvas)\n");
        printf("2. List Active Shapes\n");
        printf("3. Add Shape\n");
        printf("4. Delete Shape\n");
        printf("5. Modify Shape\n");
        printf("6. Clear All Shapes\n");
        printf("7. Exit\n");
        printf("===========================================\n");
        int choice = get_int("Enter option (1-7): ", 1, 7);
        switch (choice) {
            case 1:
                display_canvas();
                break;
            case 2:
                list_shapes();
                break;
            case 3:
                add_shape();
                // Immediately display canvas after adding to let user see feedback
                display_canvas();
                break;
            case 4:
                delete_shape();
                display_canvas();
                break;
            case 5:
                modify_shape_menu();
                display_canvas();
                break;
            case 6:
                shape_count = 0;
                render_canvas();
                printf("All shapes cleared.\n");
                display_canvas();
                break;
            case 7:
                printf("Exiting editor. Goodbye!\n");
                return 0;
        }
    }
}