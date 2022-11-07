#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2

#define MAX_MAZE_SIZE 4000

int predecessor[MAX_MAZE_SIZE]; 

int move_if_valid(struct maze *m, struct queue *q, int r, int c, int move, int i) {
    int new_r = r + m_offsets[move][0];
    int new_c = c + m_offsets[move][1];

    if(maze_valid_move(m, new_r, new_c) && maze_get(m, new_r, new_c) != VISITED && maze_get(m, new_r, new_c) != '#') {
        queue_push(q ,maze_index(m, new_r, new_c));
        predecessor[maze_index(m, new_r, new_c)] = i;
    }
}

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
    struct queue *q = queue_init(MAX_MAZE_SIZE);

    int r_start, c_start;
    maze_start(m, &r_start, &c_start);
    int index_start = maze_index(m, r_start, c_start);
    
    int r_destination, c_destination;
    maze_destination(m, &r_destination, &c_destination);
    int index_destination = maze_index(m, r_destination, c_destination);

    queue_push(q, index_start);

    while(!queue_empty(q)) {
        int i = queue_pop(q);
        int r = maze_row(m, i);
        int c = maze_col(m, i);

        if(maze_at_destination(m, r, c)) {
            break;
        }

        if(maze_get(m, r, c) != VISITED) {
            maze_set(m, r, c, VISITED);
            for(int move = 0; move < N_MOVES; move++) {
                move_if_valid(m, q, r, c, move, i);
            }
        }
    }

    int path_length = 0;
    int i = index_destination;

    while(i != index_start) {
        i = predecessor[i];
        maze_set(m, maze_row(m, i), maze_col(m, i), 'x');
        path_length++;
    }

    queue_cleanup(q);
    return path_length;
}

int main(void) {
    /* read maze */
    struct maze *m = maze_read();
    if (!m) {
        printf("Error reading maze\n");
        return 1;
    }

    /* solve maze */
    int path_length = bfs_solve(m);
    if (path_length == ERROR) {
        printf("bfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("bfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
