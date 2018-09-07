/* Author: Robert Shi
 * Program: Tic-Tac-Toe
 * Purpose: 0/1/2 player Tic-Tac-Toe game
 * Notes: TESTED
 */
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//IMPORTANT DEFINITIONS
//1. Cell numbers AND positions:
    //We mark each cell in the game board with a Cell number from 1 to MAX*MAX
        //starting from the top-left corner of the board and follow the rule: from
        //left to right, top to bottom
    //e.g. This is how we name each cell on a 3 by 3 game board
            //|1||2||3|
            //---------
            //|4||5||6|
            //---------
            //|7||8||9|
            //---------
    //On the other hand, POSITIONS START FROM 0 to MAX*MAX-1. Because arrays
        //indexes start from 0, this makes things easier for me

//2. int turn:
    //This value indicates the current player, the one who will make the next move
    //We use 1 to indicate "X" and -1 for "O" throughout the program.

//3. Lines:
    //When a player fills up a line, s/he wins the game.
    //e.g. cells with cell number 1,2,and 3 in the graph above forms a line
    //the number of lines of a game board is related to its dimensions

//4. int world[MAX][MAX]:
    //Values in this array indicates whether a cell is taken or not.
    //1 means this cell is taken by X, -1 for O. 0 indicates an empty cell

//5. int sums[]:
    //This array stores sum of each line. Since we are using +-1 to indicate the owner
        //of a cell, this array allows faster access to the array to check if there is
        //a winner or if there is a place to win (without looping through the whole board)

//6. Favorability:
    //Same as the intuitive favorability when we play games. The better chance you have
    //to win, the higher favorability a board situation has.
//-/////////////////////////////////////////////////////////////////////////////////////////////////////

const int MAX = 3;              //board size MAX*MAX
const int SUMS = MAX * 2 + 2;   //number of sums/lines where a player can win the game by
                                    //filling it up.
const int MAX_SCORE = 1000;     //the AI gets this board rating when it surely wins
const int OPEN_LINE_SCORE = 10; //the value of an open line where a player can still win
                                    //the game by filling it up
const int MAX_DEPTH = 6;        //maximum number of levels that the AI is allowed to go down
                                    //do not assign an odd number to this value
                                    //computing time increases significantly when this value
                                        //is increased
struct Move{
    int pos;    //number of the cell to play
    int score;  //board rating score that the current player get after making this move
};
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//The "hint" function. Determines where is the best place to go
//returns a Move object that tells other functions where to go by using the member value "pos"
//The "score" member value is used only inside this function for the computer to decide where to go
//The goal of the search that this function does is to maximize the favorability for current player
    //at depth 0

Move minimax(int world[][MAX],//the board situation
             int sum[],       //the sum array contains the sum of each line
             int depth,       //current depth
             int alpha,       //for AB pruning
             int beta,        //for AB pruning
             int turn,        //who's turn is this? 1 for X and -1 for O
             int max_depth);  //maximum number of levels allowed for this function to recur
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//board evaluating function, returns the board rating score
//if current player surely wins, returns MAX_SCORE
//if current player surely loses, returns -MAX_SCORE
//if nobody is winning, evaluates and return a score that indicates favorability
//in general, the score is positive if the situation is more favorable for current player
//and is negative if the situation is more favorable for the opponent

int evaluate(int world[][MAX], //board situation
             int sum[],        //the sum array contains the sum of each line
             int depth,        //current depth
             int turn);        //who's turn is this? 1 for X and -1 for O
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//prompt the user to input an integer that corresponds to a cell number
    //and make the move over that place by making change to variable "best"
//Then, check for a possible winner and return the winner's code: 1 for X,
    //-1 for O, 0 for game not over, 2 for tied (no winner and board is full)
//if the user inputs a 0, the computer would help the user decide and make a move automatically

int prompt_and_move(int world[][MAX],//board situation
                    int sum[],       //the sum array contains the sum of each line
                    Move &best,      //the "best" Move to make decided by human player/AI
                    int &turn);      //who's turn is this? 1 for X and -1 for O

//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//make a new move for the current player at position pos
//Then, check for a possible winner and return the winner's code: 1 for X,
    //-1 for O, 0 for game not over, 2 for tied (no winner and board is full)

int make_new_move(int world[][MAX],//board situation
                  int sum[],       //the sum array contains the sum of each line
                  int pos,         //number of the cell (position) to play
                  int &turn);      //who's turn is this? 1 for X and -1 for O
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//undo a move and modify the sum array accordingly
//will check for input validity. if the position given does not have a chess
    //that is played by the right player, returns false. otherwise, undo that
    //move and return true

bool undo(int world[][MAX], //board situation
          int sum[],        //the sum array contains the sum of each line
          int pos,          //position to undo
          int &turn);       //who's turn is this? 1 for X and -1 for O
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//checks for a winner and return winner's code: 1 for X, -1 for 0, 0 for game
    //not over, 2 for tied (no winner and board is full)

int check_for_winner(int world[][MAX],//board situation
                     int sum[]);      //the sum array contains the sum of each line
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//clear the board for a new game
//both world array and sum array will be set to all zeros

bool clear_board(int world[][MAX], //game board
                 int sum[]);       //sum array
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//prompt the user to input the cell number where s/he would like to make
    //the next move
//will check for input validity. The user will be asked to input again until the
    //the function gets a valid input
//Then, check for a winner and return winner's code: 1 for X, -1 for 0, 0 for game
    //not over, 2 for tied (no winner and board is full)

int prompt_for_move(int world[][MAX]);//game board
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//prints the game over message according to the winner's code (see above)
//returns true all the time unless encountered an unexpected error

bool print_game_over(int winner);//winner's code
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//prints the game board
//returns true all the time unless encountered an unexpected error

bool print_board(int world[][MAX]);//game board
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
//prints main menu
//returns true all the time unless encountered an unexpected error

bool print_main_menu();
//-/////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //--------------------------------init--------------------------------------
    int world[MAX][MAX];//game board
    int sum[SUMS];      //the sum array contains the sum of each line
    Move best;          //temporary space that holds the move to make
    bool done = false;  //false until the user chooses to exit the program
    int winner;         //winner's code
                            //0: game is not over yet; +-1: the game has a winner; 2: tied
    char players;       //number of players, user input
    int turn;           //temporary space that holds current player's code

    //-------------------------------GAME----------------------------------
    do{
        clear_board(world, sum);
        print_main_menu();
        winner = 0;
        cout << "-->";
        cin >> players;
        switch(players){
        //----0 players-----
        case '0':
        {
            turn = 1;
            while(winner == 0){
                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
                if(winner != 0)
                    break;
                //these 3 lines are repeated deliberately so that we can change the
                    //intelligence of one AI player and test the results
                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }
            print_game_over(winner);
            break;
        }
        //----1 player-----
        case '1':
        {
            print_board(world);
            do{
                cout << "Would you like to go first? (1 for yes/-1 for no)" << endl << "-->";
                cin >> turn;
            }while(turn != 1 && turn != -1);
            if(turn == -1){
                turn = 1;
                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, 1, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }

            while(winner == 0){
                winner = prompt_and_move(world, sum, best, turn);
                print_board(world);
                if(winner != 0)
                    break;

                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }
            print_game_over(winner);
            break;
        }

        //----2 players-----
        case '2':
        {
            print_board(world);
            turn = 1;
            while(winner == 0){
                winner = prompt_and_move(world, sum, best, turn);
                print_board(world);
                if(winner != 0)
                    break;

                winner = prompt_and_move(world, sum, best, turn);
                print_board(world);
            }
            print_game_over(winner);
            break;
        }

        //exit the program
        default:
            done = true;
            break;
        }
    }while(!done);

    return 0;
}
//-//////////////////////////////////////////////////////////////////////////////////////////////////////////////
Move minimax(int world[][MAX], int sum[], int depth, int alpha, int beta, int turn, int max_depth){
    Move best;
    if(depth == max_depth || check_for_winner(world, sum)){
        best.score = evaluate(world, sum, depth, turn);
        return best;
    }

    vector<int> avail;//list of available cells
    int new_score;
    int minimizing = depth % 2;//true means this node should take the minimum value of all of its child nodes
    if(minimizing){//even number depth
        best.score = MAX_SCORE;
    }
    else{
        best.score = -MAX_SCORE;
    }

    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(world[i][j] == 0)
                avail.push_back(MAX * i + j);
        }
    }

    //if all places are equivalent, make a move at the first available cell
    best.pos = avail[0];

    for(unsigned int i = 0; i < avail.size(); i++){
        make_new_move(world, sum, avail[i], turn);
        new_score = minimax(world, sum, depth + 1, alpha, beta, turn, max_depth).score;
        if(minimizing && new_score < best.score){
            best.score = new_score;
            beta = new_score;
            best.pos = avail[i];
        }
        else if(!minimizing && new_score > best.score){
            best.score = new_score;
            alpha = new_score;
            best.pos = avail[i];
        }
        undo(world, sum, avail[i], turn);
        if(alpha >= beta){
            break;
        }
    }
    return best;
}

int evaluate(int world[][MAX], int sum[], int depth, int turn){
    //check for game over
    switch(check_for_winner(world, sum)){
    case 1:
        if(depth % 2 == 0){
            return turn * MAX_SCORE;
        }
        else{
            return -turn * MAX_SCORE;
        }
        break;

    case -1:
        if(depth % 2 == 0){
            return -turn * MAX_SCORE;
        }
        else{
            return turn * MAX_SCORE;
        }
        break;

    case 2:
        return 0;
        break;

    default:
        break;
    }

    //check for immediate win for ai player
    for(int i = 0; i < SUMS; i++){
        if(sum[i] == turn * MAX)
            return MAX_SCORE;
    }

    //check if ai player is checkmated
    vector<int> niche;
    for(int i = 0; i < SUMS; i++){
        if(sum[i] == -turn * MAX){
            niche.push_back(sum[i]);
        }
    }
    if(niche.size() >= 2){
        bool taken = false;
        int first;
        int pos;
        for(unsigned int n = 0; n < niche.size(); n++){
            if(niche[n] < MAX){
                for(int j = 0; j < MAX; j++){
                    if(world[n][j] == 0){
                        pos = n * MAX + j;
                        break;
                    }
                }
            }
            else if(niche[n] < 2 * MAX){
                n -= MAX;
                for(int i = 0; i < MAX; i++){
                    if(world[i][n] == 0){
                        pos = i * MAX + n;
                        break;
                    }
                }
            }
            else{
                for(int i = 0; i < MAX; i++){
                    if(world[i][i] == 0){
                        pos = i * MAX + i;
                        break;
                    }
                }
            }
            if(taken){
                if(first != pos)
                    return -MAX_SCORE;
            }
            else{
                first = pos;
                taken = true;
            }
        }
    }

    //evaluate the board
    int val = 0;
    int total;
    bool locked;
    //rows (cells that have the same i value)
    for(int i = 0; i < MAX; i++){
        total = 0;
        locked = false;
        for(int j = 0; j < MAX; j++){
            if(abs(total + world[i][j]) < abs(total)){
                locked = true;
                break;
            }
            else{
                total += world[i][j];
            }
        }
        if(!locked && total != 0){
            val += OPEN_LINE_SCORE * total / abs(total);
        }
    }
    //cols (cells that have the same j value)
    for(int j = 0; j < MAX; j++){
        total = 0;
        locked = false;
        for(int i = 0; i < MAX; i++){
            if(abs(total + world[i][j]) < abs(total)){
                locked = true;
                break;
            }
            else{
                total += world[i][j];
            }
        }
        if(!locked && total != 0){
            val += OPEN_LINE_SCORE * total / abs(total);
        }
    }

    //diagonal 1 (i == j): top left to bottom right
    for(int i = 0; i < MAX; i++){
        total = 0;
        if(abs(total + world[i][i]) < abs(total)){
            break;
        }
        else{
            total += world[i][i];
        }
        if(total != 0)
            val += OPEN_LINE_SCORE * total / abs(total);
    }

    //diagonal 2 (i + j == MAX - 1): top right to bottom left
    for(int i = 0; i < MAX; i++){
        total = 0;
        int j = MAX - 1 - i;
        if(abs(total + world[i][j]) < abs(total)){
            break;
        }
        else{
            total += world[i][j];
        }
        if(total != 0)
            val += OPEN_LINE_SCORE * total / abs(total);
    }
    val *= turn;
    return val;
}

int prompt_and_move(int world[][MAX], int sum[], Move &best, int &turn){
    best.pos = prompt_for_move(world);
    if(best.pos == 0){
        best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
        return make_new_move(world, sum, best.pos, turn);
    }
    else{
        best.pos --;
        return make_new_move(world, sum, best.pos, turn);
    }
}

int make_new_move(int world[][MAX], int sum[], int pos, int &turn){
    int row = pos / MAX;
    int col = pos % MAX;
    world[row][col] = turn;
    sum[row] += turn;
    sum[MAX + col] += turn;
    if(row == col)
        sum[SUMS - 2] += turn;
    if(row + col == MAX - 1)
        sum[SUMS - 1] += turn;

    turn *= -1;
    return check_for_winner(world, sum);
}

bool undo(int world[][MAX], int sum[], int pos, int &turn){
    int row = pos / MAX;
    int col = pos % MAX;
    if(world[row][col] != -turn)
        return false;
    make_new_move(world, sum, pos, turn);
    world[row][col] = 0;
    return true;
}

int check_for_winner(int world[][MAX], int sum[]){
    //look for a winner
    for(int i = 0; i < SUMS; i++){
        if(sum[i] == MAX)
            return 1;
        if(sum[i] == -MAX)
            return -1;
    }

    //if no winner is found, check if the board is full for a possible tie
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(world[i][j] == 0)
                return 0;//game is not over as there are still empty spaces
        }
    }
    return 2;//tie
}

bool clear_board(int world[][MAX], int sum[]){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            world[i][j] = 0;
        }
        sum[2 * i] = 0;
        sum[2 * i + 1] = 0;
    }
    sum[SUMS - 2] = 0;
    sum[SUMS - 1] = 0;
    return true;
}

int prompt_for_move(int world[][MAX]){
    int input;
    bool done = false;
    do{
        cout << "[num] Keys for Positions / [0] for Hint / [u]ndo" << endl << "-->";
        cin >> input;
        if(input > 0 && input <= MAX * MAX){
            int pos = input - 1;
            int row = pos / MAX;
            int col = pos % MAX;
            if(world[row][col] == 0)
                done = true;
        }
        else if(input == 0)
            done = true;
    }while(!done);
    return input;
}

bool print_game_over(int winner){
    switch (winner) {
    case 1:
        cout << "----------------------------------------------------------" << endl
             << "                 Game Over. X Wins!" << endl
             << "----------------------------------------------------------" << endl;
        break;
    case -1:
        cout << "----------------------------------------------------------" << endl
             << "                 Game Over. O Wins!" << endl
             << "----------------------------------------------------------" << endl;
        break;
    case 2:
        cout << "----------------------------------------------------------" << endl
             << "                          Draw!" << endl
             << "----------------------------------------------------------" << endl;
        break;
    default:
        break;
    }

    return true;
}

bool print_board(int world[][MAX]){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(world[i][j] == 1){
                cout << "|X|";
            }
            else if(world[i][j] == -1){
                cout << "|O|";
            }
            else{
                cout << "| |";
            }
        }
        cout << endl;
        for(int j = 0; j < MAX; j++)
            cout << "---";
        cout << endl;
    }
    cout << "----------------------------------------------------------" << endl;
    return true;
}

bool print_main_menu(){
    cout << "------------------------------------------------" << endl
         << "           " << MAX << " x " << MAX << " tic-tac-toe game" << endl
         << "    [0] Player / [1] Player / [2] Players" << endl
         << "------------------------------------------------" << endl;
    return true;
}
