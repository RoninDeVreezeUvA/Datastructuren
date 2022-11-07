/* Do not edit this file. */

// Needed for getline()
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maze.h"

struct maze {
    int n;
    int start_index;
    int finish_index;
    char *data;
};

/* Move offsets: (row, column) We can only move in four directions.
 *
 *           (-1,0)
 *    (0, -1)      (0, 1)
 *           (1, 0)
 */
int m_offsets[N_MOVES][2] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

/* Creates a square maze structure of 'n' rows by 'n' columns filled with
 * walls. maze_init() is not part of the maze interface, it is a helper
 * function for maze_read().
 * Returns a pointer to the initialized maze or NULL if an error occured. */
struct maze *maze_init(int n) {
    if (n <= 0) {
        return NULL;
    }
    struct maze *m = malloc(sizeof(struct maze));
    if (!m) {
        return NULL;
    }
    m->n = n;
    m->data = calloc(1, (size_t)(m->n * m->n * (int) sizeof(char)));
    if (!m->data) {
        free(m);
        return NULL;
    }
    memset(m->data, WALL, (size_t)(m->n * m->n));

    // And finally set the default start and finish locations.
    m->start_index = maze_index(m, 1, 1); // upper left
    m->finish_index = maze_index(m, maze_size(m) - 2,
                                 maze_size(m) - 2); // lower right
    return m;
}

void maze_cleanup(struct maze *m) {
    free(m->data);
    free(m);
}

char maze_get(const struct maze *m, int r, int c) {
    assert(r >= 0 && r < m->n && c >= 0 && c < m->n);
    return m->data[r * m->n + c];
}

void maze_set(struct maze *m, int r, int c, char value) {
    assert(r >= 0 && r < m->n && c >= 0 && c < m->n);
    m->data[r * m->n + c] = value;
}

void maze_print(const struct maze *m, bool blocks) {
    for (int r = 0; r < m->n; r++) {
        for (int c = 0; c < m->n; c++) {
            if (blocks && maze_get(m, r, c) == WALL) {
                printf("\u2588");
            } else if (maze_at_start(m, r, c)) {
                putchar(START);
            } else if (maze_at_destination(m, r, c)) {
                putchar(FINISH);
            } else {
                putchar(maze_get(m, r, c));
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* Set RGB values in color array */
static void set_rgb(unsigned char color[], unsigned char r, unsigned char g,
                    unsigned char b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

/* To view the ppm file use a viewer such as geeqie or eog. Zoom in and
 * disable interpolation.
 * The maze cells are colored as follows:
 * Start:       green
 * Destination: orange
 * Coridor:     black
 * Wall:        white
 * Path:        red
 * Visited:     gray
 */
int maze_output_ppm(const struct maze *m, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return 1;
    }

    /* Write header */
    fprintf(fp, "P6\n%d %d\n255\n", (int) m->n, (int) m->n);

    /* Write RGB color data for every cell location. */
    for (int r = 0; r < m->n; r++) {
        for (int c = 0; c < m->n; c++) {
            unsigned char color[3] = { 0, 0, 0 }; // black
            if (maze_at_start(m, r, c)) {
                set_rgb(color, 0, 255, 0); // green
            } else if (maze_at_destination(m, r, c)) {
                set_rgb(color, 255, 165, 0); // orange
            } else if (maze_get(m, r, c) == WALL) {
                set_rgb(color, 255, 255, 255); // white
            } else if (maze_get(m, r, c) == PATH) {
                set_rgb(color, 255, 0, 0); // red
            } else if (maze_get(m, r, c) == VISITED) {
                set_rgb(color, 128, 128, 128); // gray
            }
            fwrite(color, 1, 3, fp);
        }
    }
    fclose(fp);
    return 0;
}

/* Detect and set start and finish locations in maze 'm'. */
static void check_for_start_and_dest(struct maze *m, int r, int c, char val) {
    if (val == START) {
        m->start_index = maze_index(m, r, c);
    } else if (val == FINISH) {
        m->finish_index = maze_index(m, r, c);
    }
}

/* Set 'val' character in 'm' at position 'r', 'c'. */
static void set_value(struct maze *m, int r, int c, char val) {
    if (val == WALL) {
        maze_set(m, r, c, WALL);
    } else {
        /* Should overwrite start and finish markers with FLOOR. */
        maze_set(m, r, c, FLOOR);
    }
}

struct maze *maze_read(void) {
    char *buf = NULL;
    size_t bufsize = 0;

    /* Read one line to get number of columns so we can allocate the maze. */
    int ncols = (int) getline(&buf, &bufsize, stdin) - 1;
    struct maze *m = maze_init(ncols);
    if (!m) {
        free(buf);
        return NULL;
    }

    int row = 0;
    do {
        if (row == ncols) { /* Error: more rows than columns */
            maze_cleanup(m);
            free(buf);
            return NULL;
        }

        int column = 0;
        while (buf[column] != '\n') {
            check_for_start_and_dest(m, row, column, buf[column]);
            set_value(m, row, column, buf[column]);
            column++;
        }
        row++;
    } while (getline(&buf, &bufsize, stdin) == ncols + 1); // ncols + \n

    if (row < ncols) { /* Error: more columns than rows */
        maze_cleanup(m);
        m = NULL;
    }

    free(buf);
    return m;
}

void maze_start(const struct maze *m, int *r, int *c) {
    *r = maze_row(m, m->start_index);
    *c = maze_col(m, m->start_index);
}

void maze_destination(const struct maze *m, int *r, int *c) {
    *r = maze_row(m, m->finish_index);
    *c = maze_col(m, m->finish_index);
}

bool maze_at_start(const struct maze *m, int r, int c) {
    return maze_index(m, r, c) == m->start_index;
}

bool maze_at_destination(const struct maze *m, int r, int c) {
    return maze_index(m, r, c) == m->finish_index;
}

bool maze_valid_move(const struct maze *m, int r, int c) {
    if (r > 0 && r < (m->n - 1) && c > 0 && c < (m->n - 1)) {
        return true;
    }
    return false;
}

int maze_size(const struct maze *m) {
    return m->n;
}

int maze_index(const struct maze *m, int r, int c) {
    return m->n * r + c;
}

int maze_row(const struct maze *m, int index) {
    return index / m->n;
}

int maze_col(const struct maze *m, int index) {
    return index % m->n;
}
