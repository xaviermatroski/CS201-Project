#include <iostream>
using namespace std;

int minimax_counter = 0; // this variable keeps a track of the no of times a static_value function is accessed by minimax
int alphabeta_counter = 0; // this variables keeps a track of the no of times a static_value function is accessed by alphabeta_counter
// these 2 variables to compare by how much alpha beta reduces static_value function operations

// All the static values are integers
struct game{
	int board[9]; // board is a 9 element array (instead of making 3x3 array i made 1x9 array)
	// 0 in the board represent an empty place
	// 1 in the board represent that the place is occupied by O
	// -1 in the board represent that the place is occupied by X
	bool xmove; // xmove is true when the player to move is X and is false when player to move is O
};

struct tree{
	game state; // state is a variable representing the game state
	int move; // it is used for representing the move which lead to that state
	// for the root node it is the best possible move avaliable not the move that lead to that state
	// move is a 0 to 8 number representing the index on which to move
	int eval; // It represents the evaluation of that node after backtracking
	int n; // n is the number of the children
	tree** child; // an array of pointers to the child
};

void print_board(tree* node){
	// replace -1 with X, 0 with . and 1 with O
	for (int i=0; i<9; i++){
		if (i%3 == 0){
			printf("\n");
		}
		if (node->state.board[i] == -1){
			printf("X ");
		} else if (node->state.board[i] == 0){
			printf(". ");
		} else {
			printf("O ");
		}
	}
}

void commit_move(tree* node, int move){
	// we cannot directly change game state as it is not a pointer (any change in this function would not affect outside this function)
	// so we need a pointer and this is a tree node
	if (node->state.xmove == 0){
		// O move
		node->state.board[move] = 1;
		node->state.xmove = 1; // now it's other player's turn
	} else {
		// X move
		node->state.board[move] = -1;
		node->state.xmove = 0; // now it's other player's turn
	}
}

bool board_full(tree* node){
	// this function as the name suggest returns whether the board is full or not
	for (int i=0; i<9; i++){
		if (node->state.board[i] == 0) return 0;
	}
	return 1;
}

int static_value(tree* node){
	// it gives +1 if O is the winner (note it is winner not winning)
	// 0 if no one is the winner
	// -1 if X is the winner
	
	// firstly checking whether O is the winner or not
	
	// checking row wise
	if (node->state.board[0] == 1 && node->state.board[1] == 1 && node->state.board[2] == 1){
		return 1;
	} else if (node->state.board[3] == 1 && node->state.board[4] == 1 && node->state.board[5] == 1){
		return 1;
	} else if (node->state.board[6] == 1 && node->state.board[7] == 1 && node->state.board[8] == 1){
		return 1;
	}
	
	// checking column wise
	if (node->state.board[0] == 1 && node->state.board[3] == 1 && node->state.board[6] == 1){
		return 1;
	} else if (node->state.board[1] == 1 && node->state.board[4] == 1 && node->state.board[7] == 1){
		return 1;
	} else if (node->state.board[2] == 1 && node->state.board[5] == 1 && node->state.board[8] == 1){
		return 1;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (node->state.board[0] == 1 && node->state.board[4] == 1 && node->state.board[8] == 1){
		return 1;
	} else if (node->state.board[2] == 1 && node->state.board[4] == 1 && node->state.board[6] == 1){
		return 1;
	}
	
	// secondly checking whether X is the winner or not
	
	// checking row wise
	if (node->state.board[0] == -1 && node->state.board[1] == -1 && node->state.board[2] == -1){
		return -1;
	} else if (node->state.board[3] == -1 && node->state.board[4] == -1 && node->state.board[5] == -1){
		return -1;
	} else if (node->state.board[6] == -1 && node->state.board[7] == -1 && node->state.board[8] == -1){
		return -1;
	}
	
	// checking column wise
	if (node->state.board[0] == -1 && node->state.board[3] == -1 && node->state.board[6] == -1){
		return -1;
	} else if (node->state.board[1] == -1 && node->state.board[4] == -1 && node->state.board[7] == -1){
		return -1;
	} else if (node->state.board[2] == -1 && node->state.board[5] == -1 && node->state.board[8] == -1){
		return -1;
	}
	
	// checking diagonally
	// there are only two diagonals
	if (node->state.board[0] == -1 && node->state.board[4] == -1 && node->state.board[8] == -1){
		return -1;
	} else if (node->state.board[2] == -1 && node->state.board[4] == -1 && node->state.board[6] == -1){
		return -1;
	}
	
	// now if all above are not the case then no one is the winner
	return 0;
}

bool is_over(tree* node){
	return static_value(node) == 0 ? 0 : 1; 
}

void gen_moves(tree* node){
	// it will directly append the move in the child of the node
	// it not only generates the move but also represent the game state after that move
	// all empty location are the moves in case of tic-tac-toe
	// all empty location are the moves only and only if the game has not ended yet
	// VERY IMPORTANT: 
	// Checking whether game has ended or not is very important because there can be a situation
	// where the other player has already won but our player still have moves (empty squares) in such a case 
	// our node would not be in the leaf node and our game would continue evaluating and if any case
	// our player also win by one extra move (like consider X already won after one move O is also winning now static
	// value gives O a preference like if X and O both are winning then it gives O as the winner as it first checks for 
	// whether is the winner or not then for X) so in that case our algo will select that move giving that the position
	// is in our favour
	
	// most important part of the code
	if (static_value(node)!=0){
		// do not generate any moves
		node->n = 0;
		node->child = NULL;
		return;
	}
	
	int size = 0;
	int i,j;
	// calculating the size of the moves array
	for (i=0; i<9; i++){
		if (node->state.board[i]==0){
			// an empty location
			size++;
		}
	}
	// size of the move array == size
	node->child = (tree**)malloc(size*sizeof(tree*));
	node->n = size;
	for (i=0; i<9; i++){
		if (node->state.board[i]==0){
			// an empty location
			tree* temp = (tree*)malloc(sizeof(tree));
			for (j=0; j<9; j++){
				// copying the board array
				temp->state.board[j] = node->state.board[j];
			}
			if (node->state.xmove){
				// X to move in the ith position
				temp->state.board[i] = -1;
				// now change xmove of the temp because now X has moved and after this O has to move
				temp->state.xmove = 0;
			} else {
				// O to move in the ith position
				temp->state.board[i] = 1;
				// now change xmove of the temp because now O has moved and after this X has to move
				temp->state.xmove = 1;
			}
			temp->n = 0; // initially setting 0 child of temp
			temp->child = NULL; // intially setting to NULL after next depth this function would update this child
			temp->move = i;
			size--;
			node->child[size] = temp; // assigning the child
		}
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
		
		int max = minimax(node->child[0],0,depth-1);
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],0,depth-1);
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
		
		int min = minimax(node->child[0],1,depth-1);
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],1,depth-1);
			if (temp < min){
				min = temp;
			}
		}
		
		node->eval = min; // assigning the evaluation to the node helpfull for finding further best moves
		return min;
	}
}

int minimax(tree* node, int depth = 9){
	// A maximum of 9 depth can be reached in a game like tic-tac-toe
	if (depth==0){
		return static_value(node);
	}
	
	if (node->state.xmove == 0){
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
		
		int max = minimax(node->child[0],0,depth-1);
		// root node's move will store the best move
		node->move = node->child[0]->move; // intially this is the best move
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],0,depth-1);
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
		
		int min = minimax(node->child[0],1,depth-1);
		node->move = node->child[0]->move; // intially this is the best move
		for (int i=1; i < node->n; i++){
			int temp = minimax(node->child[i],1,depth-1);
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
		
		int max = alphabeta(node->child[0],0,depth-1,alpha,beta);
		if (max > alpha) alpha = max;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
			// but whatever it be it would not affect our final answer
			return max;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],0,depth-1,alpha,beta);
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
		
		int min = alphabeta(node->child[0],1,depth-1,alpha,beta);
		if (min < beta) beta = min;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
			// but whatever it be it would not affect our final answer
			return min;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],1,depth-1,alpha,beta);
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

int alphabeta(tree* node, int depth = 9){
	// A maximum of 9 depth can be reached in a game like tic-tac-toe
	int alpha = -2147483648;
	int beta = 2147483647;
	if (depth==0){
		return static_value(node);
	}
	
	if (node->state.xmove == 0){
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
		
		int max = alphabeta(node->child[0],0,depth-1,alpha,beta);
		node->move = node->child[0]->move; // setting initially this as the best move
		if (max > alpha) alpha = max;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = max; // We are not evaluating node->eval because it can even be greater than max
			// but whatever it be it would not affect our final answer
			return max;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],0,depth-1,alpha,beta);
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
		
		int min = alphabeta(node->child[0],1,depth-1,alpha,beta);
		node->move = node->child[0]->move; // setting initially this as the best move
		if (min < beta) beta = min;
		if (alpha >= beta){
			// condition for pruning of the tree
			//node->eval = min; // We are not evaluating node->eval because it can even be lesser than min
			// but whatever it be it would not affect our final answer
			return min;
		}
		
		for (int i=1; i < node->n; i++){
			int temp = alphabeta(node->child[i],1,depth-1,alpha,beta);
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

tree* init_board(){
	game g1;
	g1.xmove = 0;
	g1.board[0] = 0;
	g1.board[1] = 0;
	g1.board[2] = 0;
	g1.board[3] = 0;
	g1.board[4] = 0;
	g1.board[5] = 0;
	g1.board[6] = 0;
	g1.board[7] = 0;
	g1.board[8] = 0;
	g1.xmove = 0; // considering that always O moves first
	tree* root = (tree*)malloc(sizeof(tree));
	root->state = g1;
	root->n = 0;
	root->child = NULL;
	return root;
}

int main(){
	// setting up all the variables
	tree* root = init_board();
	int x,y;
	int prev_eval = 0; // it is set to 0 because in the starting of the game no one is in the lead
	int fplayer = 0;
	cout << "Who would make the first move? 0 for AI, 1 for you: ";
	scanf("%d",&fplayer);
	if (fplayer == 0){
		// first player is AI
		// loop while the game does not end
		while (static_value(root) == 0 && board_full(root) == 0){
			// AI making it's move
			alphabeta_counter = 0;
			alphabeta(root);
			
			// O is the AI and X is the player
			// So if current eval is more than prev_eval then it is a blunder
			// and if it is the reverse then it is brilliant which can never occur :(
			if (prev_eval == root->eval){
				// The best move as predicted by the AI
				cout << "\nBest Move!" << endl;
			} else if (prev_eval < root->eval){
				// Blunder leading to the losing of the game
				cout << "\nBlunder!!!!" << endl;
			} else {
				// A move which the AI couldn't see
				cout << "\nBrilliant!!!!!!!!!!!!" << endl;
			}
			prev_eval = root->eval;
			
			cout << "AI's move is (" << root->move/3 << "," << root->move%3 << ")\nEvaluation: " << root->eval << ", Nodes Analysed: " << alphabeta_counter << endl; 
			commit_move(root,root->move);
			print_board(root);
			cout << endl;
			
			// Checking whether after AI's move game ended or not
			if (static_value(root) != 0 || board_full(root) == 1) break;
			
			// User making it's move
			cout << "\nEnter your move: ";
			scanf("%d %d",&x,&y);
			// looping while correct co-ordinates are not specified
			while (root->state.board[3*x+y] != 0){
				cout << "Please enter correct Co-ordinate: ";
				scanf("%d %d",&x,&y);
			}
			commit_move(root,3*x+y);
			print_board(root);
			cout << endl;
			
			// Clearing all previous evaluations
			root->child = NULL;
			root->n = 0;
		}

	} else {
		// first move is made by the user
		// nothing much new just in the above code swap AI and player
		alphabeta(root);
		cout << "Evaluation: " << root->eval << ", Nodes Analysed: " << alphabeta_counter << endl;
		cout << endl;
		print_board(root);
		cout << endl;
		while (static_value(root) == 0 && board_full(root) == 0){
			// User making it's move
			cout << "\nEnter your move: ";
			scanf("%d %d",&x,&y);
			// looping while correct co-ordinates are not specified
			while (root->state.board[3*x+y] != 0){
				cout << "Please enter correct Co-ordinate: ";
				scanf("%d %d",&x,&y);
			}
			commit_move(root,3*x+y);
			print_board(root);
			cout << endl;
			
			// Clearing all previous evaluations
			root->child = NULL;
			root->n = 0;
			
			// Checking whether after AI's move game ended or not
			if (static_value(root) != 0 || board_full(root) == 1) break;
			
			// AI making it's move
			alphabeta_counter = 0;
			alphabeta(root);
			
			// X is the AI and O is the player
			// So if current eval is less than prev_eval then it is a blunder
			// and if it is the reverse then it is brilliant which can never occur :(
			if (prev_eval == root->eval){
				// The best move as predicted by the AI
				cout << "\nBest Move!" << endl;
			} else if (prev_eval > root->eval){
				// Blunder leading to the losing of the game
				cout << "\nBlunder!!!!" << endl;
			} else {
				// A move which the AI couldn't see
				cout << "\nBrilliant!!!!!!!!!!!!" << endl;
			}
			prev_eval = root->eval;
			
			cout << "AI's move is (" << root->move/3 << "," << root->move%3 << ")\nEvaluation: " << root->eval << ", Nodes Analysed: " << alphabeta_counter << endl; 
			commit_move(root,root->move);
			print_board(root);
			cout << endl;			
		}
	}
	
	// The game ended printing the winner
	if (static_value(root) == -1) cout << "\nX is the winner! AI lost" << endl;
	else if (static_value(root) == 1) cout << "\nO is the winner! AI won" << endl;
	else cout << "\nIt's a draw!" << endl;
	cout << "Game Over" << endl;
}
