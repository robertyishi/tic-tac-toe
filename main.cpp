#include <iostream>
#include <iomanip>

using namespace std;

int points[3][3]={{3,2,3},
                 {2,4,2},
                 {3,2,3}};

bool hint(int world[3][3], int turn);
bool win(int world[3][3], int turn);
bool defend(int world[3][3], int turn);
bool attack(int world[3][3], int turn);
bool world_is_full(int world[3][3]);

int* check_for_two(int world[3][3], int turn, int win);
int find(int world[3][3], int n);
int check_for_winner(int world[3][3]);
bool print(int world[3][3]);

int main()
{
    int world[3][3]={{0,0,0},
                     {0,0,0},
                     {0,0,0}};//0 empty, 1 cross, -1 circle
    int winner = 0;
    char answer;
    int i, j;

    do{
        print(world);
        cout << "-->";
        cin >> answer;
        switch(answer){
        case 'h':
            hint(world, 1);
            break;

        case 'i':
            cin >> i >> j;
            world[i][j] = 1;
            break;

        default:
            break;
        }

        winner = check_for_winner(world);
        if(world_is_full(world))
            break;

        print(world);
        hint(world, -1);
        winner = check_for_winner(world);
    }while(winner == 0);

    print(world);
    cout << "-------------------------------------------------" << endl << winner << " wins!" << endl;

    return 0;
}

bool hint(int world[3][3], int turn){
    if(win(world, turn))
        return true;
    if(defend(world, turn))
        return true;
    if(attack(world, turn))
        return true;
    return false;
}

bool win(int world[3][3], int turn){
    int *niche = check_for_two(world, turn, true);
    if(niche == NULL)
        return false;
    *niche = turn;
    return true;
}

bool defend(int world[3][3], int turn){
    int *niche = check_for_two(world, turn, -1);
    if(niche == NULL)
        return false;
    *niche = turn;
    return true;
}

bool attack(int world[3][3], int turn){
    int score = 0;
    int col, row;
    for(int i =0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(world[i][j] == 0 && points[i][j] > score){
                row = i;
                col = j;
                score = points[i][j];
            }
        }
    }
    if(score == 0){
        return false;
    }
    world[row][col] = turn;
    return true;
}

bool world_is_full(int world[3][3]){
    for(int i =0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(world[i][j] == 0){
                return false;
            }
        }
    }
    return true;
}

int* check_for_two(int world[3][3], int turn, int win){
    int sum;
    int *niche = NULL;

    //check for rows
    for(int i = 0; i < 3; i++){
        sum = 0;
        for(int j = 0; j < 3; j++){
            sum += world[i][j];
        }
        if(sum * turn * win == 2){
            if(world[i][0] == 0)
                niche = world[0] + i * 3;
            else if(world[i][1] == 0)
                niche = world[0] + i * 3 + 1;
            else
                niche = world[0] + i * 3 + 2;
            break;
        }
    }

    //check for columns
    for(int j = 0; j < 3; j++){
        sum = 0;
        for(int i = 0; i < 3; i++){
            sum += world[i][j];
        }
        if(sum * turn * win == 2){
            if(world[0][j] == 0)
                niche = world[0] + j;
            else if(world[1][j] == 0)
                niche = world[0] + j + 3;
            else
                niche = world[0] + j + 6;
            break;
        }
    }

    //check for diagonals
    sum = 0;
    for(int i = 0; i < 3; i++){
        sum += world[i][i];
    }
    if(sum * turn * win == 2){
        if(world[0][0] == 0)
            niche = world[0];
        else if(world[1][1] == 0)
            niche = world[0] + 4;
        else
            niche = world[0] + 8;
    }

    sum = 0;
    for(int i = 0; i < 3; i++){
        sum += world[i][2 - i];
    }
    if(sum * turn * win == 2){
        if(world[0][2] == 0)
            niche = world[0] + 2;
        else if(world[1][1] == 0)
            niche = world[0] + 4;
        else
            niche = world[0] + 6;
    }

    return niche;
}

int find(int world[3][3], int n){
    int sum;
    //search rows
    for(int i = 0; i < 3; i++){
        sum = 0;
        for(int j = 0; j < 3; j++){
            sum += world[i][j];
        }
        if(sum == n){
            return i + 1;
        }
    }

    //search cols
    for(int j = 0; j < 3; j++){
        sum = 0;
        for(int i = 0; i < 3; i++){
            sum += world[i][j];
        }
        if(sum == n){
            return j + 4;
        }
    }

    //search diagonals
    sum = 0;
    for(int i = 0; i < 3; i++){
        sum += world[i][i];
    }
    if (sum == n)
        return 7;
    sum = 0;
    for(int i = 0; i < 3; i++){
        sum += world[i][2 - i];
    }
    if(sum == n)
        return 8;

    //if not found
    return 0;
}

int check_for_winner(int world[3][3]){
    if(find(world, 3))
        return 1;
    else if(find(world, -3))
        return -1;
    else
        return 0;
}

bool print(int world[3][3]){
    for(int i =0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            cout << setw(3) << world[i][j];
        }
        cout << endl;
    }
    cout << endl << "------------------------------------" << endl;
    return true;
}




