#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

const int MAX = 6;
const int SUMS = MAX * 2 + 2;
const int MAX_SCORE = 1000;
const int OPEN_LINE_SCORE = 10;
const int MISSING_ONE_SCORE = 100;
const int MAX_DEPTH = 6;

struct Move{
    int pos;
    int score;
};

Move minimax(int world[][MAX], int sum[], int depth, int alpha, int beta, int turn, int max_depth);
int evaluate(int world[][MAX], int sum[], int depth, int turn);

int prompt_and_move(int world[][MAX], int sum[], Move &best, int &turn);

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
    Move best;
    bool done = false;
    int winner;//0: game is not over; +-1: the game has a winner; 2: tie
    char players;
    int turn;

    do{
        clear_board(world, sum);
        print_main_menu();
        winner = 0;
        cout << "-->";
        cin >> players;
        switch(players){
        case '0':
        {
            turn = 1;
            while(winner == 0){
                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
                if(winner != 0)
                    break;
                best = minimax(world, sum, 0, -MAX_SCORE, MAX_SCORE, turn, MAX_DEPTH);
                winner = make_new_move(world, sum, best.pos, turn);
                print_board(world);
            }
            game_over(winner);
            break;
        }
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
            game_over(winner);
            break;
        }
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
            game_over(winner);
            break;
        }
        default:
            done = true;
            break;
        }
    }while(!done);

    return 0;
}

Move minimax(int world[][MAX], int sum[], int depth, int alpha, int beta, int turn, int max_depth){
    Move best;
    if(depth == max_depth || check_for_winner(world, sum)){
        best.score = evaluate(world, sum, depth, turn);
        return best;
    }

    vector<int> avail;
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
    //rows (same i)
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
    //cols (same j)
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
    //diags (i == j || i + j == MAX - 1)
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
         << "    [0] Player / [1] Player / [2] Player" << endl
         << "------------------------------------------------" << endl;
    return true;
}
