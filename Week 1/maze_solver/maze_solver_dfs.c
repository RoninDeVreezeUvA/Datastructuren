#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2

#define MAX_MAZE_SIZE 4000

int predecessor[MAX_MAZE_SIZE]; 

/* Checks if the given move is possile (next tile is not visited and not wall)
   If the tile is free it is added to the stack and set to visited.
   The predecessor of the tile is saved in order to recover the solution path.
*/
void check_neighbour(struct maze *m, struct stack *s, 
                             int move, int current_index) {
    // Calculate the new row and column
    int new_r = maze_row(m, current_index) + m_offsets[move][0];
    int new_c = maze_col(m, current_index) + m_offsets[move][1];

    // Get the char at the next position
    char new_position = maze_get(m, new_r, new_c);

    // Check if the next position is a unvisited and free to enter
    if(maze_valid_move(m, new_r, new_c) && new_position == FLOOR) {
        // Add tile to stack and set to visited
        stack_push(s, maze_index(m, new_r, new_c));
        maze_set(m, new_r, new_c, VISITED);

        // Save the last location in order to recover the path later
        predecessor[maze_index(m, new_r, new_c)] = current_index;
    }
}

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int dfs_solve(struct maze *m) {
    if(m == NULL) {
        perror("Pointer to maze struct is NULL in dfs_solve");
        return ERROR;
    }

    // Create a new stack
    struct stack *s = stack_init(MAX_MAZE_SIZE);
    if(s == NULL) {
        perror("Could not initialize stack struct in dfs_solve");
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

    // Add the start tile to the stack and set it to visited
    if(stack_push(s, index_start) == 1) {
        perror("Could not push element onto stack in dfs_solve");
        return ERROR;
    }
    maze_set(m, r_start, c_start, VISITED);

    while(!stack_empty(s)) {
        // Get the first element and its row and column
        int i = stack_pop(s);
        if(i == -1) {
            perror("Could not pop element from stack in dfs_solve");
            return ERROR;
        }
        int r = maze_row(m, i);
        int c = maze_col(m, i);

        
        if(maze_at_destination(m, r, c)) {
            static int path_length = 0;
            int i = index_destination;

            while(i != index_start) {
                i = predecessor[i];
                maze_set(m, maze_row(m, i), maze_col(m, i), 'x');
                path_length++;
            }

            stack_cleanup(s);
            return path_length;
        }

        // Check each neighbour and add it to the stack if it is valid
        for(int move = 0; move < N_MOVES; move++) {
            check_neighbour(m, s, move, i);
        }
    }

    // Destination was never reached, so no path was found
    stack_cleanup(s);
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
    int path_length = dfs_solve(m);
    if (path_length == ERROR) {
        printf("dfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("dfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}