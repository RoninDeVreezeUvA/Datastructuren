#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2
#define DEBUG 0
#define debug_print(fmt) \
            do { if (DEBUG) fprintf(stderr, fmt); } while (0)
/*  Bron: https://stackoverflow.com/questions/1644868/
    define-macro-for-debug-printing-in-c */

#define MAX_MAZE_SIZE 4000

// Store all the predecessors to reconstruct the path later
int predecessor[MAX_MAZE_SIZE*MAX_MAZE_SIZE]; 

/* Checks if the given move is possile (next tile is not visited and not wall)
   If the tile is free it is added to the stack and set to visited.
   The predecessor of the tile is saved in order to recover the solution path.
*/
int check_neighbour(struct maze *m, struct queue *q, 
                             int move, int current_index) {
    // Calculate the new row and column and get the char at the position
    int new_r = maze_row(m, current_index) + m_offsets[move][0];
    int new_c = maze_col(m, current_index) + m_offsets[move][1];
    char new_position = maze_get(m, new_r, new_c);

    // Check if the next position is a unvisited and free to enter
    if(maze_valid_move(m, new_r, new_c) && new_position == FLOOR) {
        // Add tile to queue and set to visited
        if(queue_push(q, maze_index(m, new_r, new_c)) == 1) {
            debug_print("Could not push to queue in check_neighbour");
            return ERROR;
        }
        maze_set(m, new_r, new_c, VISITED);

        // Save the last location in order to recover the path later
        predecessor[maze_index(m, new_r, new_c)] = current_index;
    }

    return 0;
}

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
    if(m == NULL) {
        debug_print("Pointer to maze struct is NULL in bfs_solve");
        return ERROR;
    }

    // Create a new stack
    struct queue *q = queue_init(MAX_MAZE_SIZE);
    if(q == NULL) {
        debug_print("Could not initialize queue struct in bfs_solve");
        return ERROR;
    }

    // Calculate the index of the start tile
    int r_start, c_start;
    maze_start(m, &r_start, &c_start);
    int index_start = maze_index(m, r_start, c_start);
    
    // Calculate the index of the destination tile
    int r_destination, c_destination;
    maze_destination(m, &r_destination, &c_destination);
    int index_destination = maze_index(m, r_destination, c_destination);

    // Add the start tile to the queue and set it to visited
    if(queue_push(q, index_start) == 1) {
        debug_print("Could not push element onto queue in bfs_solve");
        return ERROR;
    }
    maze_set(m, r_start, c_start, VISITED);

    while(!queue_empty(q)) {
        // Get the first element and its row and column
        int i = queue_pop(q);
        if(i == -1) {
            debug_print("Could not pop element from queue in bfs_solve");
            return ERROR;
        }

        int r = maze_row(m, i);
        int c = maze_col(m, i);

        // If the end is reached, calculate the path lenght
        if(maze_at_destination(m, r, c)) {
            static int path_length = 0;
            int i = index_destination;

            // While not at the end, move to the predecessor
            while(i != index_start) {
                i = predecessor[i];
                maze_set(m, maze_row(m, i), maze_col(m, i), 'x');
                path_length++;
            }

            queue_cleanup(q);
            return path_length;
        }

        // Check each neighbour and add it to the queue if it is valid
        for(int move = 0; move < N_MOVES; move++) {
            if(check_neighbour(m, q, move, i) == ERROR) {
                debug_print("Could not check neighbour in bfs_solve");
                return ERROR;
            }
        }
    }

    // Destination was never reached, so no path was found
    queue_cleanup(q);
    return NOT_FOUND;    
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
