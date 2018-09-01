#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

const int MAX = 3;
const int SUMS = MAX * 2 + 2;
const int MAX_VAL = 100;

struct Move{
    int pos;
    int score;
};

Move minimax(int world[][MAX], int sum[], int depth, int alpha, int beta, int turn);

int make_new_move(int world[][MAX], int sum[], int pos, int &turn);
bool undo(int world[][MAX], int sum[], int pos, int &turn);
int check_for_winner(int world[][MAX], int sum[]);

bool clear_board(int world[][MAX], int sum[]);

int prompt_for_move(int world[][MAX]);
bool game_over(int winner);
bool print_board(int world[][MAX]);
bool print_main_menu();

int main()
{
    int world[MAX][MAX];
    int sum[SUMS];
    Move best;//starts from 0
    bool done = false;
    int winner;//0: game is not over; +-1: the game has a winner; 2: tie
    char pve;
    int turn;

    do{
        clear_board(world, sum);
        print_main_menu();
        winner = 0;
        cout << "-->";
        cin >> pve;
        if(pve == '1'){
            print_board(world);
            do{
                cout << "Would you like to go first? (1 for yes/-1 for no)" << endl << "-->";
                cin >> turn;
            }while(turn != 1 && turn != -1);
            if(turn == -1){
                turn = 1;
                best = minimax(world, sum, 0, -MAX_VAL, MAX_VAL, 1);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }

            while(winner == 0){
                best.pos = prompt_for_move(world);
                if(best.pos == 0){
                    best = minimax(world, sum, 0, -MAX_VAL, MAX_VAL, turn);
                    winner = make_new_move(world, sum, best.pos, turn);
                }
                else{
                    best.pos --;
                    winner = make_new_move(world, sum, best.pos, turn);
                }
                print_board(world);
                if(winner != 0)
                    break;

                best = minimax(world, sum, 0, -MAX_VAL, MAX_VAL, turn);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }
            game_over(winner);
        }

        else if(pve == '0'){
            turn = 1;
            while(winner == 0){
                best = minimax(world, sum, 0, -MAX_VAL, MAX_VAL, turn);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }
            game_over(winner);
        }

        else
            done = true;
    }while(!done);

    return 0;
}

Move minimax(int world[][MAX], int sum[], int depth, int alpha, int beta, int turn){//pos starts from 0!!
    Move best;
    switch(check_for_winner(world, sum)){
    case 1:
        if(depth % 2 == 0){
            best.score = turn * MAX_VAL;
            return best;
        }
        else{
            best.score = -turn * MAX_VAL;
            return best;
        }
        break;

    case -1:
        if(depth % 2 == 0){
            best.score = -turn * MAX_VAL;
            return best;
        }
        else{
            best.score = turn * MAX_VAL;
            return best;
        }
        break;

    case 2:
        best.score = 0;
        return best;
        break;

    default:
        break;
    }

    vector<int> avail;
    int new_score;
    int minimizing = depth % 2;//如果为真，(本层)本节点结果取为下一层的所有节点的最小值
    if(minimizing){//even number depth
        best.score = MAX_VAL;
    }
    else{
        best.score = -MAX_VAL;
    }

    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(world[i][j] == 0)
                avail.push_back(MAX * i + j);
        }
    }

    for(unsigned int i = 0; i < avail.size(); i++){
        make_new_move(world, sum, avail[i], turn);
        new_score = minimax(world, sum, depth + 1, alpha, beta, turn).score;
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
        if(alpha > beta){
            break;
        }
    }
    return best;
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

    //if no winner is found, check if the board if full for a possible tie
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
        cout << "[num] Keys for Positions / [0] for Hint" << endl << "-->";
        cin >> input;
        if(input > 0){
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

bool game_over(int winner){
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
        cout << endl << "----------------" << endl;
    }
    cout << endl << "----------------------------------------------------------" << endl;
    return true;
}

bool print_main_menu(){
    cout << "------------------------------------------------" << endl
         << "           " << MAX << " x " << MAX << "tic-tac_toe game" << endl
         << "          [0] Player / [1] Player" << endl
         << "------------------------------------------------" << endl;
    return true;
}









