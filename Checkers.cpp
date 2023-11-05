#include<iostream>
#include<math.h>
#include<limits.h>
#include <memory.h>
using namespace std;

int infi = 5000; //greatest integer
int Max_Move = 30; //defining the maximum no of valid moves for any pawn
int alphabeta_counter = 0;
int minimax_counter = 0;
int depth_search = 8;

struct game{
    int board[8][8];
    //-1 for non movable place
    //0 for vacant place
    //1 for white
    //2 for black
    //3 for white king
    //4 for black king
    bool chance;
    //0 for white 
    //1 for black
    int forced_capture[2];
};

struct motion{
    int x1, x2, y1, y2;
    //x1 y1 initial coordinates
    //x2 y2 final coordinates
};

struct tree{
    game state;
    motion move;
    int eval;
    int n;
    tree** child;
};

struct Move{
    int x_pos;
    int y_pos;
    bool capture;
    //flag to indicate if the move is a capture move or not
    //0 for normal move
    //1 for capture move
};


void print_board(tree* node)
{
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            if(i % 2 == j% 2){
                printf("* ");
            }
            if (node->state.board[i][j] == 0){
                printf("- ");
            }else if (node->state.board[i][j] == 1){
                printf("W ");
            }else if (node->state.board[i][j] == 2){
                printf("B ");
            }else if (node->state.board[i][j] == 3){
                printf("KW");
            }else if (node->state.board[i][j] == 4){
                printf("KB");
            }
        }
        printf("\n");
    }
}

bool further_capture(int board[8][8], int x, int y){
    int pawn_class = board[x][y];
    if(pawn_class == 1){
        if(x+2 <= 7 && y-2 >= 0){
            if(board[x+1][y-1] == 2 || board[x+1][y-1] == 4){
                if(board[x+2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x+2 <= 7 && y+2 <= 7){
            if(board[x+1][y+1] == 2 || board[x+1][y+1] == 4){
                if(board[x+2][y+2] == 0){
                    return 1;
                }
            }
        }
    } else if (pawn_class == 3){
		if(x-2 >= 0 && y-2 >= 0){
            if(board[x-1][y-1] == 2 || board[x-1][y-1] == 4){
                if(board[x-2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x-2 >= 0 && y+2 <= 7){
            if(board[x-1][y+1] == 2 || board[x-1][y+1] == 4){
                if(board[x-2][y+2] == 0){
                    return 1;
                }
            }
        }
        if(x+2 <= 7 && y-2 >= 0){
            if(board[x+1][y-1] == 2 || board[x+1][y-1] == 4){
                if(board[x+2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x+2 <= 7 && y+2 <= 7){
            if(board[x+1][y+1] == 2 || board[x+1][y+1] == 4){
                if(board[x+2][y+2] == 0){
                    return 1;
                }
            }
        }
	} else if(pawn_class == 4){
        if(x-2 >= 0 && y-2 >= 0){
            if(board[x-1][y-1] == 1 || board[x-1][y-1] == 3){
                if(board[x-2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x-2 >= 0 && y+2 <= 7){
            if(board[x-1][y+1] == 1 || board[x-1][y+1] == 3){
                if(board[x-2][y+2] == 0){
                    return 1;
                }
            }
        }
        if(x+2 <= 7 && y-2 >= 0){
            if(board[x+1][y-1] == 1 || board[x+1][y-1] == 3){
                if(board[x+2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x+2 <= 7 && y+2 <= 7){
            if(board[x+1][y+1] == 1 || board[x+1][y+1] == 3){
                if(board[x+2][y+2] == 0){
                    return 1;
                }
            }
        }
    } else if (pawn_class == 2){
		if(x-2 >= 0 && y-2 >= 0){
            if(board[x-1][y-1] == 1 || board[x-1][y-1] == 3){
                if(board[x-2][y-2] == 0){
                    return 1;
                }
            }
        }
        if(x-2 >= 0 && y+2 <= 7){
            if(board[x-1][y+1] == 1 || board[x-1][y+1] == 3){
                if(board[x-2][y+2] == 0){
                    return 1;
                }
            }
        }
	}

    return 0;
}

void commit_move(tree* node, motion move){
	// we cannot directly change game state as it is not a pointer (any change in this function would not affect outside this function)
	// so we need a pointer and this is a tree node
    int pawn_class = node->state.board[move.x1][move.y1];
    node->state.board[move.x1][move.y1] = 0;

    if(!node->state.chance){
        int difference = abs(move.y1-move.y2);
        if(difference == 2){
            int avg_x = (move.x1 + move.x2)/2;
            int avg_y = (move.y1 + move.y2)/2;
            node->state.board[avg_x][avg_y] = 0;
            if(move.x2 == 7){
                node->state.board[move.x2][move.y2] = pawn_class;
            }else{
                node->state.board[move.x2][move.y2] = pawn_class;
            }
            if(further_capture(node->state.board, move.x2, move.y2)){
                node->state.forced_capture[0] = move.x2;
                node->state.forced_capture[1] = move.y2;
				node->state.chance = 0;
            }else{
				node->state.forced_capture[0] = -1;
				node->state.forced_capture[1] = -1;				
                node->state.chance = 1;
            }
        } else if(difference == 1){
            if(move.x2 == 7){
                node->state.board[move.x2][move.y2] = pawn_class;
            }else{
                node->state.board[move.x2][move.y2] = pawn_class;
            }
            node->state.chance = 1;
			node->state.forced_capture[0] = -1;
			node->state.forced_capture[1] = -1;		
        }
        else{
            printf("INVALID!");
        }
    }else{
        int difference = abs(move.y1-move.y2);
        if(difference == 2){
            int avg_x = (move.x1 + move.x2)/2;
            int avg_y = (move.y1 + move.y2)/2;
            node->state.board[avg_x][avg_y] = 0;
            if(move.x2 == 0){
                node->state.board[move.x2][move.y2] = pawn_class;
            }else{
                node->state.board[move.x2][move.y2] = pawn_class;
            }
            if(further_capture(node->state.board, move.x2, move.y2)){
                node->state.forced_capture[0] = move.x2;
                node->state.forced_capture[1] = move.y2;
				node->state.chance = 1;
            }else{
				node->state.forced_capture[0] = -1;
				node->state.forced_capture[1] = -1;		
                node->state.chance = 0;
            }
        }
        else if(difference == 1){
            if(move.x2 == 0){
                node->state.board[move.x2][move.y2] = pawn_class;
            }else{
                node->state.board[move.x2][move.y2] = pawn_class;
            }
            node->state.chance = 0;
			node->state.forced_capture[0] = -1;
			node->state.forced_capture[1] = -1;		
        }else{
            printf("INVALID!");
        }
    }
}

/*int pawn_num(tree* node, int pawn_class){
    //keeps a check on the numbers of different pawn classes
    int white_npawn = 0;
    int black_npawn = 0;
    int white_kpawn = 0;
    int black_kpawn = 0;
    for(int i = 0; i<8 ; i++){
        for(int j = 0; j<8; j++){
            if(node->state.board[i][j] == 1){
                white_npawn += 1;
            }
            else if(node->state.board[i][j] == 2){
                black_npawn += 1;
            }
            else if(node->state.board[i][j] == 3){
                white_kpawn += 1;
            }
            else if(node->state.board[i][j] == 4){
                black_kpawn += 1;
            }
        }
    }

    if(pawn_class == 1){
        return white_npawn;
    }
    else if(pawn_class == 2){
        return black_npawn;
    }
    else if(pawn_class == 3){
        return white_kpawn;
    }
    else if(pawn_class == 4){
        return black_kpawn;
    }
}*/

bool in(motion* valid_moves, int size, int x1, int y1, int x2, int y2){
	for (int i=0; i<size; i++){
		if (valid_moves[i].x1 == x1 && valid_moves[i].y1 == y1 && valid_moves[i].x2 == x2 && valid_moves[i].y2 == y2) return 1;
	}
	return 0;
}

bool in_start(motion* valid_moves, int size, int x1, int y1){
	for (int i=0; i<size; i++){
		if (valid_moves[i].x1 == x1 && valid_moves[i].y1 == y1) return 1;
	}
	return 0;
}

int allcaptures(tree* node, motion* capture){
	int count = 0;
	for (int x=0; x<8; x++){
		for (int y=0; y<8; y++){
			int pawn_class = node->state.board[x][y];
			if (node->state.chance){
				// black to move
				if (pawn_class == 2){
					if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 1 || node->state.board[x-1][y-1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 1 || node->state.board[x-1][y+1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y+2;
						count++;
					}
				} else if (pawn_class == 4){
					if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 1 || node->state.board[x+1][y-1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 1 || node->state.board[x+1][y+1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y+2;
						count++;
					}
					if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 1 || node->state.board[x-1][y-1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 1 || node->state.board[x-1][y+1] == 3)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y+2;
						count++;
					}
				}
			} else {
				// white to move
				if (pawn_class == 1){
					if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 2 || node->state.board[x+1][y-1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 2 || node->state.board[x+1][y+1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y+2;
						count++;
					}
				} else if (pawn_class == 3){
					if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 2 || node->state.board[x+1][y-1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 2 || node->state.board[x+1][y+1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x+2;
						capture[count].y2 = y+2;
						count++;
					}
					if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 2 || node->state.board[x-1][y-1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y-2;
						count++;
					}
					if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 2 || node->state.board[x-1][y+1] == 4)){
						capture[count].x1 = x;
						capture[count].y1 = y;
						capture[count].x2 = x-2;
						capture[count].y2 = y+2;
						count++;
					}
				}
			}
		}
	}
	return count;
}

motion *get_valid_moves(tree* node, int x, int y){
    //this function returns the array of valid moves corresponding to a pawn
    //x and y are used to represnt the pawn location in the board

    //1. creating an array 
    
    motion* valid_moves = (motion*)malloc(Max_Move*sizeof(motion));

    for(int i = 0; i<Max_Move; i++){
        //assigning valid moves to -1 so that we can find the end of the array
        valid_moves[i].x1 = x;
        valid_moves[i].y1 = y;
        valid_moves[i].x2 = -1;
        valid_moves[i].y2 = -1;
    }

    //2. checking valid moves
    int count = 0;

	int *arr = node->state.forced_capture;
	motion* capture = (motion*)malloc(Max_Move*sizeof(motion)); // these are also captures but not included in forced capture because they are not further capture
	int capture_size = allcaptures(node, capture);
	
	if (arr[0] != -1 || arr[1] != -1){
		// we have arr[0] and arr[1] as x1 and y1
		if (x == arr[0] && y == arr[1]){
			int pawn_class = node->state.board[x][y];
			if (pawn_class == 1){
				if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 2 || node->state.board[x+1][y-1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 2 || node->state.board[x+1][y+1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y+2;
					count++;
				}
			} else if (pawn_class == 2){
				if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 1 || node->state.board[x-1][y-1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 1 || node->state.board[x-1][y+1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y+2;
					count++;
				}
			} else if (pawn_class == 3){
				if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 2 || node->state.board[x+1][y-1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 2 || node->state.board[x+1][y+1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y+2;
					count++;
				}
				if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 2 || node->state.board[x-1][y-1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 2 || node->state.board[x-1][y+1] == 4)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y+2;
					count++;
				}
			} else if (pawn_class == 4){
				if(node->state.board[x+2][y-2] == 0 && x+2<=7 && y-2>=0 && (node->state.board[x+1][y-1] == 1 || node->state.board[x+1][y-1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x+2][y+2] == 0 && x+2<=7 && y+2<=7 && (node->state.board[x+1][y+1] == 1 || node->state.board[x+1][y+1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x+2;
					valid_moves[count].y2 = y+2;
					count++;
				}
				if(node->state.board[x-2][y-2] == 0 && x-2>=0 && y-2>=0 && (node->state.board[x-1][y-1] == 1 || node->state.board[x-1][y-1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y-2;
					count++;
				}
				if(node->state.board[x-2][y+2] == 0 && x-2>=0 && y+2<=7 && (node->state.board[x-1][y+1] == 1 || node->state.board[x-1][y+1] == 3)){
					valid_moves[count].x1 = x;
					valid_moves[count].y1 = y;
					valid_moves[count].x2 = x-2;
					valid_moves[count].y2 = y+2;
					count++;
				}
			}
		} else {
			// the position passed is not coinciding with forced capture hence return 0 valid_moves
		}
		
	} else if (capture_size == 0 && arr[0] == -1 && arr[1] == -1){
		//a. Checking the neighbouring places of the pawn class to be a vacant place for it to be a valid move
		int pawn_class = node->state.board[x][y];
		if(pawn_class == 1){
			if(node->state.board[x+1][y-1] == 0 && x+1<=7 && y-1>=0){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x+1][y+1] == 0 && x+1<=7 && y+1<=7){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y+1;
				count++;
			}
		}
		else if(pawn_class == 3){
			if(node->state.board[x+1][y-1] == 0 && x+1<=7 && y-1>=0){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x+1][y+1] == 0 && x+1<=7 && y+1<=7){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y+1;
				count++;
			}
			if(node->state.board[x-1][y-1] == 0 && x-1>=0 && y-1>=0){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x-1][y+1] == 0 && x-1>=0 && y+1<=7){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y+1;
				count++;
			}
		}
		else if(pawn_class == 2){
			if(node->state.board[x-1][y-1] == 0 && x-1>=0 && y-1>=0){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x-1][y+1] == 0 && x-1>=0 && y+1<=7){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y+1;
				count++;
			}
		}
		else if(pawn_class == 4){
			if(node->state.board[x-1][y-1] == 0 && x-1>=0 && y-1>=0){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x-1][y+1] == 0 && x-1>=0 && y+1<=7){
				valid_moves[count].x2 = x-1;
				valid_moves[count].y2 = y+1;
				count++;
			}
			if(node->state.board[x+1][y-1] == 0 && x+1<=7 && y-1>=0){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y-1;
				count++;
			}
			if(node->state.board[x+1][y+1] == 0 && x+1<=7 && y+1<=7){
				valid_moves[count].x2 = x+1;
				valid_moves[count].y2 = y+1;
				count++;
			}
		}
		
	} else {
		for (int i=0; i<capture_size; i++){
			if (capture[i].x1 == x && capture[i].y1 == y){
				valid_moves[count].x2 = capture[i].x2;
				valid_moves[count].y2 = capture[i].y2;
				count++;
			}
		}
	}
    
	valid_moves[count].x2 = -1;
	valid_moves[count].y2 = -1;
    return valid_moves;
}


int count_valid_moves(tree* node, int x, int y){
    motion* valid_moves = get_valid_moves(node, x, y);
    int count = 0;
    for(int i = 0; i < Max_Move; i++){
        if(valid_moves[i].x2 == -1 && valid_moves[i].y2 == -1){
            break;
        }
        count++;
    }
    return count;
}

motion *total_get_valid_moves(tree* node){
    motion* total_valid_moves = (motion*)malloc(100*sizeof(motion)); //stores the total valid moves 
    int count = 0; //just like ith position of the array
    for(int i = 0; i<8 ; i++){
        for(int j = 0; j<8; j++){
			if (node->state.chance){
				if(node->state.board[i][j] == 2){
					total_valid_moves[count].x1 = i;
					total_valid_moves[count].y1 = j;
					motion* valid_moves = get_valid_moves(node,i, j);
					int moves = count_valid_moves(node, i, j);
					for(int k = 0; k<moves; k++){
						total_valid_moves[count].x1 = i;
						total_valid_moves[count].y1 = j;
						total_valid_moves[count].x2 = valid_moves[k].x2;
						total_valid_moves[count].y2 = valid_moves[k].y2;
						count++;
					}
				} else if(node->state.board[i][j] == 4){
					total_valid_moves[count].x1 = i;
					total_valid_moves[count].y1 = j;
					motion* valid_moves = get_valid_moves(node,i, j);
					int moves = count_valid_moves(node, i, j);
					for(int k = 0; k<moves; k++){
						total_valid_moves[count].x1 = i;
						total_valid_moves[count].y1 = j;
						total_valid_moves[count].x2 = valid_moves[k].x2;
						total_valid_moves[count].y2 = valid_moves[k].y2;
						count++;
					}
				}
			} else {
				if(node->state.board[i][j] == 1){
					motion* valid_moves = get_valid_moves(node,i, j);
					int moves = count_valid_moves(node, i, j);
					for(int k = 0; k<moves; k++){
						total_valid_moves[count].x1 = i;
						total_valid_moves[count].y1 = j;
						total_valid_moves[count].x2 = valid_moves[k].x2;
						total_valid_moves[count].y2 = valid_moves[k].y2;
						count++;
					}
				} else if(node->state.board[i][j] == 3){
					total_valid_moves[count].x1 = i;
					total_valid_moves[count].y1 = j;
					motion* valid_moves = get_valid_moves(node,i, j);
					int moves = count_valid_moves(node, i, j);
					for(int k = 0; k<moves; k++){
						total_valid_moves[count].x1 = i;
						total_valid_moves[count].y1 = j;
						total_valid_moves[count].x2 = valid_moves[k].x2;
						total_valid_moves[count].y2 = valid_moves[k].y2;
						count++;
					}
				} 
			}
        }
    }
    return total_valid_moves;
}

int total_count_valid_moves(tree* node){
    int total_white_moves = 0;
    int total_black_moves = 0;

    for(int i = 0; i<8 ; i++){
        for(int j = 0; j<8; j++){
            if(node->state.board[i][j] == 1){
                total_white_moves += count_valid_moves(node, i, j);
            }
            else if(node->state.board[i][j] == 2){
                total_black_moves += count_valid_moves(node, i, j);
            }
            else if(node->state.board[i][j] == 3){
                total_white_moves += count_valid_moves(node, i, j);
            }
            else if(node->state.board[i][j] == 4){
                total_black_moves += count_valid_moves(node, i, j);
            }
        }
    }

	if (node->state.chance){
		return total_black_moves;
	} else {
		return total_white_moves;
	}
}

int static_value(tree* node){
    // +infi represents if white wins the match
    // -infi represents if black wins the match
    // 0 neither white nor black is winning
    // [1, infi) represents if white is winning 1 being the lowest possibility and increasing form then on
    // (-infi, -1] represents if black is winning -1 being the lowest possibility and increasing form then on

    int win_var  = 0; //keeps a check on the possibility of win of either white or black, positive value of win_var means that white is winning and negative value means that black is winning

    int white_npawn = 0;    //number of white n pawns
    int black_npawn = 0;    //number of black n pawns
    int white_kpawn = 0;    //number of white k pawns
    int black_kpawn = 0;    //number of black k pawns

    int total_white_moves = 0;  //keeps a check on the total number of moves available for white 
    int total_black_moves = 0;  //keeps a check on the total number of moves available for black
	
	if (node->state.chance){
		total_black_moves = total_count_valid_moves(node);
	} else {
		total_white_moves = total_count_valid_moves(node);
	}

    //counting the no of pieces and the total number of moves by either black or white
    for(int i = 0; i<8 ; i++){
        for(int j = 0; j<8; j++){
            if(node->state.board[i][j] == 1){
                white_npawn += 1;
            }
            else if(node->state.board[i][j] == 2){
                black_npawn += 1;
            }
            else if(node->state.board[i][j] == 3){
                white_kpawn += 1;
            }
            else if(node->state.board[i][j] == 4){
                black_kpawn += 1;
            }
        }
    }

    //1. Checking the clear cut winner

    //a. No pawn left
    if(white_npawn != 0){
        if(black_npawn == 0){
            if(black_kpawn == 0){
                win_var = infi; // white is the clear cut winner
                return win_var;
            }
        }
        else if(black_kpawn != 0){
            if(white_npawn == 0){
                if(white_kpawn == 0){
                    win_var = -1*infi; // black is the clear cut winner
                    return win_var;
                }
            }
        }
    }

    //b no place to move
	if (node->state.chance){
		// black to move
		if(total_black_moves == 0){
			win_var = infi;
			return win_var;
		}
	} else {
		if(total_white_moves == 0){
			win_var = -1*infi;
			return win_var;
		}
	}

    if((white_npawn<=12 && white_npawn>8) || (black_npawn<=12 && black_npawn>8)){
        //start game 
        int npawn_val = 100;  //n pawn has a piece value of 100
        int kpawn_val = 200;  //k pawn has a piece value of 200
		
		win_var += (white_npawn - black_npawn) * npawn_val;
		win_var += (white_kpawn - black_kpawn) * kpawn_val;

        //a
        int bonus = 0; 
        //pieces get point for staying in their home row
        //home row for white is first row 
        //home row for black is last row

        for(int j = 1; j<=7; j = j + 2){
            if(node->state.board[0][j] == 1 || node->state.board[0][j] == 3){
                bonus += 5;
            }
            if(node->state.board[7][j-1] == 2 || node->state.board[7][j-1] == 4){
                bonus -= 5;
            }
        }
        
        win_var += bonus;

    }
    else if ((white_npawn<=8 && white_npawn>4) || (black_npawn<=8 && black_npawn>4)){
        //mid game
        int npawn_val = 100;  //n pawn has a piece value of 100
        int kpawn_val = 200;  //k pawn has a piece value of 200

		win_var += (white_npawn - black_npawn) * npawn_val;
		win_var += (white_kpawn - black_kpawn) * kpawn_val;
		
        int bonus = 0; 
        //pieces get point for staying in the center row
        //row 3 and 4 is the center row
        
        for(int j = 0; j<=7; j = j+ 2){
            if(node->state.board[3][j] == 1 || node->state.board[3][j] == 3){
                //3rd row for white
                bonus += 10;
            }
            if(node->state.board[3][j] == 2 || node->state.board[3][j] == 4){
                //3rd row for black
                bonus -= 10;
            }
            if(node->state.board[4][j+1] == 1 || node->state.board[4][j+1] == 3){
                //4th row for white
                bonus += 10;
            }
            if(node->state.board[4][j+1] == 2 || node->state.board[4][j+1] == 4){
                //4th row for black
                bonus -= 10;
            }
        }
        win_var += bonus;

        bonus = 0;
        //a. Keeping pieces together will make it harder for the opponent to move
        for(int i = 1; i<=6; i++){
            for(int j = 1; j<=6; j++){
                if(node->state.board[i][j] == 1 || node->state.board[i][j] == 3){
                    //if white pawns are together
                    if(node->state.board[i-1][j-1] == 1 || node->state.board[i][j] == 3){
                        bonus += 5;
                    }
                    if(node->state.board[i-1][j+1] == 1 || node->state.board[i][j] == 3){
                        bonus += 5;
                    }
                }
                else if(node->state.board[i][j] == 2 || node->state.board[i][j] == 4){
                    //if black pawns are together
                    if(node->state.board[i+1][j-1] == 2 || node->state.board[i][j] == 4){
                        bonus -= 5;
                    }
                    if(node->state.board[i+1][j+1] == 2 || node->state.board[i][j] == 4){
                        bonus -= 5;
                    }
                }
            }
        }
        win_var += bonus;  
    }
    else if ((white_npawn<=4 && white_npawn>=0) || (black_npawn<=4 && black_npawn>=0)){
        //end game
        int npawn_val = 100;  //n pawn has a piece value of 100
        int kpawn_val = 200;  //k pawn has a piece value of 200

		win_var += (white_npawn - black_npawn) * npawn_val;
		win_var += (white_kpawn - black_kpawn) * kpawn_val;
		
        int bonus = 0;
        //a. Keeping pieces together will make it harder for the opponent to move
        for(int i = 1; i<=6; i++){
            for(int j = 1; j<=6; j++){
                if(node->state.board[i][j] == 1 || node->state.board[i][j] == 3){
                    //if white pawns are together
                    if(node->state.board[i-1][j-1] == 1 || node->state.board[i][j] == 3){
                        bonus += 5;
                    }
                    if(node->state.board[i-1][j+1] == 1 || node->state.board[i][j] == 3){
                        bonus += 5;
                    }
                }
                else if(node->state.board[i][j] == 2 || node->state.board[i][j] == 4){
                    //if black pawns are together
                    if(node->state.board[i+1][j-1] == 2 || node->state.board[i][j] == 4){
                        bonus -= 5;
                    }
                    if(node->state.board[i+1][j+1] == 2 || node->state.board[i][j] == 4){
                        bonus -= 5;
                    }
                }
            }
        }

        //this will help the ai to defeat the human at the end of the game to reduce the no of moves made by the human
        if (node->state.chance){
			if (total_white_moves < 6){
				// if it is greater than 6 than no effect else
				bonus += -1*(5 - total_white_moves)*10;
			}
		} else {
			if (total_black_moves < 6){
				// if it is greater than 6 than no effect else
				bonus += (5 - total_black_moves)*10;
			}
		}
		
		// the more the distant the pieces are more difficult is to capture or stalemate
		int distance = 0; // irrespective from where we are starting
		int pieces[12][2];
		int count = 0;
		// the piece count of white is lower than white should be far away from black piece and vice versa
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				// storing white pieces in pieces array
				if (node->state.board[i][j] == 1 || node->state.board[i][j] == 3){
					pieces[count][0] = i;
					pieces[count][1] = j;
					count++;
				}
			}
		}
		
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				// storing white pieces in pieces array
				if (node->state.board[i][j] == 2 || node->state.board[i][j] == 4){
					for (int k=0; k<count; k++){
						distance += abs(pieces[k][0]-i) + abs(pieces[k][1]-j);
					}
				}
			}
		}
		
		if (npawn_val*white_npawn + kpawn_val*white_kpawn < npawn_val*black_npawn + kpawn_val*black_kpawn){
			// as k pawn is worth more than the white pawn
			// black has more piece 
			bonus = bonus + distance*7; // each distance is assigned a value of 2
		} else if (npawn_val*white_npawn + kpawn_val*white_kpawn > npawn_val*black_npawn + kpawn_val*black_kpawn) {
			bonus = bonus - distance*7;
		} else {
			// both have equal material cannot decide
		}
		
        win_var += bonus;
		
    }
    return win_var;
}

void gen_moves(tree* node){
	// it will directly append the move in the child of the node
	// it not only generates the move but also represent the game state after that move
	
	int n = total_count_valid_moves(node); // no of total avaliable moves
	motion* all_moves = total_get_valid_moves(node); // an array of motion of size n
	
	if (n==0){
		node->child = NULL;
		node->n = 0;
		return;
	}
	
	node->child = (tree**)malloc(n*sizeof(tree*));
	node->n = n;
	for (int i=0; i<n; i++){
		// an empty location
		tree* temp = (tree*)malloc(sizeof(tree));
		temp->n = 0; // initially setting 0 child of temp
		temp->child = NULL; // intially setting to NULL after next depth this function would update this child
		temp->eval = 0; // eval is assigned 0
		// now assigning the node state to this temp so that after the move it's state has changed
		temp->move.x1 = all_moves[i].x1;
		temp->move.x2 = all_moves[i].x2;
		temp->move.y1 = all_moves[i].y1;
		temp->move.y2 = all_moves[i].y2;
		temp->state.chance = node->state.chance;
		memcpy(&temp->state.forced_capture, &node->state.forced_capture, sizeof(node->state.forced_capture));
		memcpy(&temp->state.board, &node->state.board, sizeof(node->state.board));
		commit_move(temp, all_moves[i]); // this would update the game state
		node->child[i] = temp; // assigning the child
	}
}

int minimax(tree* node, bool ismax, int depth){
	// instead of using two variables one for the particular depth we want to achieve and our current depth we could use
	// just variable and recursively pass it as depth--
	if (depth==0){
		// base condition
		// we are at the leaf node
		minimax_counter++;
		return static_value(node);
	}
	
	if (ismax){
		// we are at the maximiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			minimax_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		int max = minimax(node->child[0],!node->child[0]->state.chance,depth-1);
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],!node->child[i]->state.chance,depth-1);
			if (temp > max){
				max = temp;
			}
		}
		
		node->eval = max; // assigning the evaluation to the node helpfull for finding further best moves
		return max;
		
	} else {
		// we are the minimiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			minimax_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		int min = minimax(node->child[0],!node->child[0]->state.chance,depth-1);
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],!node->child[i]->state.chance,depth-1);
			if (temp < min){
				min = temp;
			}
		}
		
		node->eval = min; // assigning the evaluation to the node helpfull for finding further best moves
		return min;
	}
}

int minimax(tree* node, int depth){
	// A maximum of 9 depth can be reached in a game like tic-tac-toe
	if (depth==0){
		return static_value(node);
	}
	
	if (node->state.chance == 0){
		// The player to move is O and hence it is the maximising player
		// we are at the maximiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			minimax_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		int max = minimax(node->child[0],!node->child[0]->state.chance,depth-1);
		// root node's move will store the best move
		node->move = node->child[0]->move; // intially this is the best move
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],!node->child[i]->state.chance,depth-1);
			if (temp > max){
				max = temp;
				node->move = node->child[i]->move; // updating the best move
			}
		}
		
		node->eval = max; // assigning the evaluation to the node helpfull for finding further best moves
		return max;
		
	} else {
		// The player to move is X and hence it is the minimiser player
		// we are the minimiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			minimax_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		int min = minimax(node->child[0],!node->child[0]->state.chance,depth-1);
		node->move = node->child[0]->move; // intially this is the best move
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],!node->child[i]->state.chance,depth-1);
			if (temp < min){
				min = temp;
				node->move = node->child[i]->move;
			}
		}
		
		node->eval = min; // assigning the evaluation to the node helpfull for finding further best moves
		return min;
	}	
}

int alphabeta(tree* node, bool ismax, int depth, int alpha=-2147483648, int beta=2147483647){
	// alpha is an integer so its initial value is -2^31 and beta is also an integer so its initial value is 2^31-1
	if (depth==0){
		// base condition
		// we are at the leaf node
		alphabeta_counter++;
		return static_value(node);
	}
	
	if (ismax){
		// we are at the maximiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			alphabeta_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		if (node->n == 1){
			// there is only one move now
			commit_move(node, node->child[0]->move);
			free(node->child[0]);
			node->n = 0;
			node->child = NULL;
			return alphabeta(node, !node->state.chance, depth-1, alpha, beta);
		}
		
		
		int max = alphabeta(node->child[0],!node->child[0]->state.chance,depth-1,alpha,beta);
		if (max > alpha) alpha = max;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
			// but whatever it be it would not affect our final answer
			return max;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],!node->child[i]->state.chance,depth-1,alpha,beta);
			if (temp > alpha) alpha = temp;
			if (temp > max) max = temp;
			if (alpha >= beta){
				// condition for pruning of the tree
				//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
				// but whatever it be it would not affect our final answer
				return max;
			}
		}
		
		node->eval = max;
		return max; // no pruning of the tree happened
		
	} else {
		// we are at the minimising player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			alphabeta_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		if (node->n == 1){
			// there is only one move now
			commit_move(node, node->child[0]->move);
			free(node->child[0]);
			node->n = 0;
			node->child = NULL;
			return alphabeta(node, !node->state.chance, depth-1, alpha, beta);
		}
		
		int min = alphabeta(node->child[0],!node->child[0]->state.chance,depth-1,alpha,beta);
		if (min < beta) beta = min;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
			// but whatever it be it would not affect our final answer
			return min;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],!node->child[i]->state.chance,depth-1,alpha,beta);
			if (temp < beta) beta = temp;
			if (temp < min) min = temp;
			if (alpha >= beta){
				// condition for pruning of the tree
				//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
				// but whatever it be it would not affect our final answer
				return min;
			}
		}
		
		node->eval = min;
		return min; // no pruning of the tree happened
	}
}

int alphabeta(tree* node, int depth){
	// A maximum of 9 depth can be reached in a game like tic-tac-toe
	int alpha = -2147483648;
	int beta = 2147483647;
	if (depth==0){
		return static_value(node);
	}
	
	if (node->state.chance == 0){
		// The player to move is O and hence it is the maximising player
		// we are at the maximiser player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			alphabeta_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		if (node->n == 1){
			// there is only one move now
			commit_move(node, node->child[0]->move);
			node->move.x1 = node->child[0]->move.x1;
			node->move.y1 = node->child[0]->move.y1;
			node->move.x2 = node->child[0]->move.x2;
			node->move.y2 = node->child[0]->move.y2;
			free(node->child[0]);
			node->n = 0;
			node->child = NULL;
			return alphabeta(node, !node->state.chance, depth-1, alpha, beta);
		}
		
		// now we cannot directly change to ismax to 0 because currently white is playing because this can be forced_capture moves
		
		int max = alphabeta(node->child[0],!node->child[0]->state.chance,depth-1,alpha,beta);
		node->move = node->child[0]->move; // setting initially this as the best move
		if (max > alpha) alpha = max;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
			// but whatever it be it would not affect our final answer
			return max;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],!node->child[i]->state.chance,depth-1,alpha,beta);
			if (temp > alpha) alpha = temp;
			if (temp > max){
				max = temp;
				node->move = node->child[i]->move; // updating the best move
			}
			if (alpha >= beta){
				// condition for pruning of the tree
				//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
				// but whatever it be it would not affect our final answer
				return max;
			}
		}
		
		node->eval = max;
		return max; // no pruning of the tree happened
		
		
	} else {
		// The player to move is X and hence it is the minimiser player
		// we are at the minimising player
		gen_moves(node); // generate all the possible move for that position
		// There could be a possibility that there is no possible move i.e. the game ended in the previous depth
		// If node->child is NULL then it means that either one player definately wins or the game is draw (stalemate case in CHESS)
		if (node->n == 0){
			// this could be a draw or a win
			alphabeta_counter++; // this variable calculate static calculations so it needs to be incremented
			return static_value(node); // consider this node to be a leaf node
		}
		
		if (node->n == 1){
			// there is only one move now
			commit_move(node, node->child[0]->move);
			node->move.x1 = node->child[0]->move.x1;
			node->move.y1 = node->child[0]->move.y1;
			node->move.x2 = node->child[0]->move.x2;
			node->move.y2 = node->child[0]->move.y2;
			free(node->child[0]);
			node->n = 0;
			node->child = NULL;
			return alphabeta(node, !node->state.chance, depth-1, alpha, beta);
		}
		
		int min = alphabeta(node->child[0],!node->child[0]->state.chance,depth-1,alpha,beta);
		node->move = node->child[0]->move; // setting initially this as the best move
		if (min < beta) beta = min;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
			// but whatever it be it would not affect our final answer
			return min;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],!node->child[i]->state.chance,depth-1,alpha,beta);
			if (temp < beta) beta = temp;
			if (temp < min){
				min = temp; // updating the best move
				node->move = node->child[i]->move;
			}
			if (alpha >= beta){
				// condition for pruning of the tree
				//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
				// but whatever it be it would not affect our final answer
				return min;
			}
		}
		
		node->eval = min;
		return min; // no pruning of the tree happened
	}	
}

void free_node(tree* node){
	if (node->child == NULL){
		free(node);
		return;
	} else {
		for (int i=node->n-1; i>=0; i--){
			free_node(node->child[i]); // depth first freeing
		}
		free(node); // free the current node
		return;
	}
}

void free_root(tree* root){
	// free the root but not the root node
	if (root->child == NULL){
		root->n = 0;
		root->eval = 0;
		root->child = NULL;
	} else {
		for (int i=root->n-1; i>=0; i--){
			free_node(root->child[i]); // depth first freeing
		}
		root->n = 0;
		root->eval = 0;
		root->child = NULL;
	}	
}

tree* init_board(){
    tree* root = (tree*)malloc(sizeof(tree));
    root->state.board[0][0] = -1;
	root->state.board[0][1] = 0;
	root->state.board[0][2] = -1;
	root->state.board[0][3] = 4;
	root->state.board[0][4] = -1;
	root->state.board[0][5] = 0;
	root->state.board[0][6] = -1;
	root->state.board[0][7] = 0;
	
	root->state.board[1][0] = 3;
	root->state.board[1][1] = -1;
	root->state.board[1][2] = 0;
	root->state.board[1][3] = -1;
	root->state.board[1][4] = 0;
	root->state.board[1][5] = -1;
	root->state.board[1][6] = 0;
	root->state.board[1][7] = -1;
	
	root->state.board[2][0] = -1;
	root->state.board[2][1] = 0;
	root->state.board[2][2] = -1;
	root->state.board[2][3] = 0;
	root->state.board[2][4] = -1;
	root->state.board[2][5] = 0;
	root->state.board[2][6] = -1;
	root->state.board[2][7] = 0;
	
	root->state.board[3][0] = 0;
	root->state.board[3][1] = -1;
	root->state.board[3][2] = 0;
	root->state.board[3][3] = -1;
	root->state.board[3][4] = 0;
	root->state.board[3][5] = -1;
	root->state.board[3][6] = 0;
	root->state.board[3][7] = -1;
	
	root->state.board[4][0] = -1;
	root->state.board[4][1] = 0;
	root->state.board[4][2] = -1;
	root->state.board[4][3] = 4;
	root->state.board[4][4] = -1;
	root->state.board[4][5] = 0;
	root->state.board[4][6] = -1;
	root->state.board[4][7] = 0;
	
	root->state.board[5][0] = 0;
	root->state.board[5][1] = -1;
	root->state.board[5][2] = 4;
	root->state.board[5][3] = -1;
	root->state.board[5][4] = 0;
	root->state.board[5][5] = -1;
	root->state.board[5][6] = 0;
	root->state.board[5][7] = -1;
	
	root->state.board[6][0] = -1;
	root->state.board[6][1] = 0;
	root->state.board[6][2] = -1;
	root->state.board[6][3] = 0;
	root->state.board[6][4] = -1;
	root->state.board[6][5] = 0;
	root->state.board[6][6] = -1;
	root->state.board[6][7] = 0;
	
	root->state.board[7][0] = 0;
	root->state.board[7][1] = -1;
	root->state.board[7][2] = 0;
	root->state.board[7][3] = -1;
	root->state.board[7][4] = 0;
	root->state.board[7][5] = -1;
	root->state.board[7][6] = 0;
	root->state.board[7][7] = -1;


	root->state.chance = 0;
	root->state.forced_capture[0] = -1;
	root->state.forced_capture[1] = -1;

	root->eval = 0;
	root->n = 0;
	root->child = NULL;
	root->move.x1 = -1;
    root->move.y1 = -1;
    root->move.x2 = -1;
    root->move.y2 = -1;
    
	return root;
}

int main(){
	tree* root = init_board();	
	int prev_eval;
	motion prev_best;
		
	while (static_value(root) < 5000 && static_value(root) > -5000){
		// 5000 is the max value from the static_value function
		print_board(root);
		if (root->state.chance){
			// black's turn and AI play's for black
			alphabeta_counter = 0;
			minimax_counter = 0;
			alphabeta(root, depth_search);
			if (root->eval == prev_eval){
				printf("Best Move! Evaluation: %d\n",root->eval);
				cout << "Computer's suggested move was: " << prev_best.x1 << prev_best.y1 << prev_best.x2 << prev_best.y2 << endl;
			} else if (root->eval > prev_eval){
				printf("Brilliant Move! Evaluation: %d\n",root->eval);
				cout << "Computer's suggested move was: " << prev_best.x1 << prev_best.y1 << prev_best.x2 << prev_best.y2 << endl;
			} else if  (prev_eval - root->eval < 20){
				printf("Good Move! Evaluation: %d\n",root->eval);
				cout << "Computer's suggested move was: " << prev_best.x1 << prev_best.y1 << prev_best.x2 << prev_best.y2 << endl;
			} else if  (prev_eval - root->eval < 70){
				printf("Mistake! Evaluation: %d\n",root->eval);
				cout << "Computer's suggested move was: " << prev_best.x1 << prev_best.y1 << prev_best.x2 << prev_best.y2 << endl;
			} else {
				printf("Blunder! Evaluation: %d\n",root->eval);
				cout << "Computer's suggested move was: " << prev_best.x1 << prev_best.y1 << prev_best.x2 << prev_best.y2 << endl;
			}
			cout << "Position evaluated: " << alphabeta_counter << endl;
			cout << "Computer move is: " << root->move.x1 << root->move.y1 << root->move.x2 << root->move.y2 << endl;
			commit_move(root, root->move);
			free_root(root);
		} else {
			// white turn and user plays
			motion m1;
			alphabeta_counter = 0;
			minimax_counter = 0;
			alphabeta(root, depth_search);
			prev_eval = root->eval; // store it in root
			prev_best.x1 = root->move.x1;
			prev_best.y1 = root->move.y1;
			prev_best.x2 = root->move.x2;
			prev_best.y2 = root->move.y2;
			cout << "Current Evaluation: " << prev_eval << endl;
			int total_move_size = total_count_valid_moves(root); // no of total avaliable moves
			motion* all_moves = total_get_valid_moves(root); // an array of motion of size n
			cout << "Avaliable moves are: " << endl;
			for (int i=0; i<total_move_size; i++){
				cout << all_moves[i].x1 << all_moves[i].y1 << all_moves[i].x2 << all_moves[i].y2 << endl;
			}
			free_root(root);
			cout << "Enter your move: ";
			char c1, c2, c3, c4;
			scanf(" %c%c%c%c",&c1,&c2,&c3,&c4);
			m1.x1 = c1 - '0';
			m1.y1 = c2 - '0';
			m1.x2 = c3 - '0';
			m1.y2 = c4 - '0';
			while (!in(all_moves, total_move_size, m1.x1, m1.y1, m1.x2, m1.y2)){
				cout << "Invalid move! Please enter a valid move: ";
				scanf(" %c%c%c%c",&c1,&c2,&c3,&c4);
				m1.x1 = c1 - '0';
				m1.y1 = c2 - '0';
				m1.x2 = c3 - '0';
				m1.y2 = c4 - '0';
			}
			commit_move(root, m1);	
			free_root(root);
		}	
	}
	
	print_board(root);
	cout << "Game Ended! ";
	if (static_value(root) > 0){
		cout << "White won the game" << endl;
	} else {
		cout << "Black won the game" << endl;
	}
}