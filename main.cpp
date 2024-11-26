//
//  main.cpp
//  Pawn_to_Queen_Puzzle -> https://www.youtube.com/shorts/3dbOU5jV5gI
//
//  Created by Uygar Utku Baran on 10/12/24.
//

/*
 NOTES:
    1) BFS is used to find the first result that leads to the Queen in the final destination. Then we backtrack using pointers to the results and print them.
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <queue>
#include <stack>
#include <climits>
#include <tuple>
#include <chrono> // For timing the program

#define INDEXIFY(_e, _x, _y) (_e + _x + (_y * NUM_COL))
#define IS_PAWN_MOVE(_x, _y) ((_x == 0) && (_y == DOWN))

using namespace std;

class PawnToQueenPuzzle
{
public:
    static int solve()
    {
        // Maps board state to a tuple of <empty square index, number of moves to get to the state, previous state>
        unordered_map<string, tuple<int, int, string>> states;
        
        // Reserve 1,000,000 spaces
        states.reserve(1000000);
    
        // Insert the initial state, initial index of the empty square, the number of moves made, and empty string to indicate that there is nothing before the initial state of the board into the hashmap.
        states.emplace(initialState, make_tuple(INITIAL_EMPTY_INDEX, 0, ""));
        
        // For BFS, use a queue to keep track of the next moves to go to.
        queue<string> q;
        q.push(initialState);
    
        // For keeping track of the current state
        string curState;
    
        while (!q.empty())
        {
            // Get the front of the queue and pop it
            curState = q.front();
            q.pop();
            
            // Get the current states' value tuple: <empty square index, number of moves up to now, previous board>
            tuple<int, int, string> curVal = states.at(curState);
            
            // Check if we won (note that this could be also done even before we get to the move...
            // basically while we are checking the next moves)
            if (curState.at(INITIAL_EMPTY_INDEX) == QUEEN)
            {
                // Backtrack your movements and print all the way back to the beginning
                displaySolution(curState, states);
                return get<1>(curVal); // Return the 2nd element of the tuple
            }
            
            // Get the next moves
            vector<pair<string, int>> nextMoves = getNextMoves(curState, get<0>(curVal));
            
            // Go through each of the next moves
            for (auto& [nextState, emptyInd] : nextMoves)
            {
                // If the move has not been played yet, play it (by adding it to the hashmap)
                // and add it to the queue to be explored later
                if (!states.count(nextState))
                {
                    q.push(nextState);
                    // Add the next state as key along with empty square index, number of moves
                    // played to get to the next state, and the soon to be previous state of the board
                    states.emplace(nextState, make_tuple(emptyInd, get<1>(curVal) + 1, curState));
                }
            }
        }
        
        return 0;
    } 

private:
    /*
     INITIAL STATE OF THE BOARD:
     {
       'K', 'K', 'K', 'K',
       'B', 'B', 'B', 'B',
       'R', 'R', 'R', 'R',
       '0', 'i', 'i', 'P'
     };
     
     NOTE: K = Knight, B = Bishop, R = Rook, P = Pawn, 0 = Empty, i = Invalid, Q = Queen
    */
    inline static const string initialState = "KKKKBBBBRRRR0iiP";
    static constexpr int INITIAL_EMPTY_INDEX = 12;
                        
    static constexpr int NUM_ROW          =    4; // Decides y direction (vertical) segments for calculation
    static constexpr int NUM_COL          =    4; // Decides x direction (horizontal) segments for calculation
    static constexpr int BOARD_SIZE       =    NUM_ROW * NUM_COL; // 16
    static constexpr int  INVALID_MOVE    =  -13; // Random negative integer
    
    // Directions
    static constexpr int  UP          =  -1;
    static constexpr int  DOWN        =   1;
    static constexpr int  LEFT        =  -1;
    static constexpr int  RIGHT       =   1;
    
    // Moves of Rook, Queen, and Pawn - Straight moves
    static constexpr int  STRAIGHT_MOVES[][2] = { {LEFT,  0},  // LEFT
                                                     {RIGHT, 0},  // RIGHT
                                                     {0,    UP},  // UP
                                                     {0,  DOWN}   // DOWN
                                              };
    
    // Moves of Bishop and Queen - Diagonal moves
    static constexpr int  DIAG_MOVES[][2] = { {LEFT,    UP},  // LEFT_UP
                                                 {RIGHT,   UP},  // RIGHT_UP
                                                 {LEFT,  DOWN},  // LEFT_DOWN
                                                 {RIGHT, DOWN}   // RIGHT_DOWN
                                              };
    
    // Moves of Knight
    static constexpr int  KNIGHT_MOVES[][2] =   {    {LEFT,    UP*2}, // LEFT1_UP2
                                                     {RIGHT,   UP*2}, // RIGHT1_UP2
                                                     {RIGHT*2,   UP}, // RIGHT2_UP1
                                                     {RIGHT*2, DOWN}, // RIGHT2_DOWN1
                                                     {LEFT,  DOWN*2}, // LEFT1_DOWN2
                                                     {RIGHT, DOWN*2}, // RIGHT1_DOWN2
                                                     {LEFT*2,    UP}, // LEFT2_UP1
                                                     {LEFT*2,  DOWN}  // LEFT2_DOWN1
                                              };
    
    static constexpr int NUM_STRAIGHT_MOVES = sizeof(STRAIGHT_MOVES) / sizeof(STRAIGHT_MOVES[0]);
    static constexpr int NUM_DIAG_MOVES     = sizeof(DIAG_MOVES) / sizeof(DIAG_MOVES[0]);
    static constexpr int NUM_KNIGHT_MOVES   = sizeof(KNIGHT_MOVES) / sizeof(KNIGHT_MOVES[0]);
    
    // Indices that are invalid: 'i'
    static constexpr int INVALID_INDEX[2] = {13, 14};
    
    enum PIECE
    {
        INVALID = 'i',
        EMPTY   = '0',
        PAWN    = 'P',
        KNIGHT  = 'K',
        BISHOP  = 'B',
        ROOK    = 'R',
        QUEEN   = 'Q'
    };
    
    static inline void swap(char* _a, char* _b)
    {
        char _c = *_a;
        *_a = *_b;
        *_b = _c;
    }
    
    // Returns all the possible next states along with where the empty square is.
    static vector<pair<string, int>> getNextMoves(string& state, int emptySquare)
    {
        vector<pair<string, int>> nextStates;
        char otherPiece;
        int moveIndex;
        
        // Check straight moves which could be the Queen, Rook, or Pawn
        for (int i = 0; i < NUM_STRAIGHT_MOVES; i++)
        {
            if (isMoveWithinBoundry(emptySquare, STRAIGHT_MOVES[i][0], STRAIGHT_MOVES[i][1]))
            {
                moveIndex = INDEXIFY(emptySquare, STRAIGHT_MOVES[i][0], STRAIGHT_MOVES[i][1]);
                
                // Get the piece which the move will lead to from the empty square.
                otherPiece = state[moveIndex];
                
                // If the other piece is a Queen or Rook, or if it is a pawn and the move is DOWN (relative to the empty square) and the piece is a pawn, that's a valid move.
                if ( (otherPiece == QUEEN) || (otherPiece == ROOK) ||
                     ((otherPiece == PAWN) && IS_PAWN_MOVE(STRAIGHT_MOVES[i][0], STRAIGHT_MOVES[i][1])) )
                {
                    // Check if the Pawn gets promoted to a Queen
                    if ((otherPiece == PAWN) && (emptySquare < NUM_COL))
                    {
                        // Replace the Pawn with a Queen before swapping it with empty square (promoting it)
                        state[moveIndex] = QUEEN;
                        
                        // Swap the empty square with the other piece's index
                        swap( &(state[emptySquare]), &(state[moveIndex]) );
                        
                        // Save the state - moveIndex is the new empty square
                        nextStates.emplace_back(state, moveIndex);
                        
                        // Move the pieces back to where they were on the board
                        swap( &(state[emptySquare]), &(state[moveIndex]) );
                        
                        // Turn index back to Pawn
                        state[moveIndex] = PAWN;
                    }
                    else
                    {
                        // Swap the empty square with the other piece's index
                        swap( &(state[emptySquare]), &(state[moveIndex]) );
                        
                        // Save the state - moveIndex is the new empty square
                        nextStates.emplace_back(state, moveIndex);
                        
                        // Move the pieces back to where they were on the board
                        swap( &(state[emptySquare]), &(state[moveIndex]) );
                    }
                }
            }
        }
        
        // Check diagonal moves which could be the Queen or Bishop
        for (int i = 0; i < NUM_DIAG_MOVES; i++)
        {
            if (isMoveWithinBoundry(emptySquare, DIAG_MOVES[i][0], DIAG_MOVES[i][1]))
            {
                moveIndex = INDEXIFY(emptySquare, DIAG_MOVES[i][0], DIAG_MOVES[i][1]);
                
                // Get the piece which the move will lead to from the empty square.
                otherPiece = state[moveIndex];
                
                if ( (otherPiece == QUEEN) || (otherPiece == BISHOP) )
                {
                    // Swap the empty square with the other piece's index
                    swap( &(state[emptySquare]), &(state[moveIndex]) );
                    
                    // Save the state - moveIndex is the new empty square
                    nextStates.emplace_back(state, moveIndex);
                    
                    // Move the pieces back to where they were on the board
                    swap( &(state[emptySquare]), &(state[moveIndex]) );
                }
            }
        }
        
        // Check Knight moves
        for (int i = 0; i < NUM_KNIGHT_MOVES; i++)
        {
            if (isMoveWithinBoundry(emptySquare, KNIGHT_MOVES[i][0], KNIGHT_MOVES[i][1]))
            {
                moveIndex = INDEXIFY(emptySquare, KNIGHT_MOVES[i][0], KNIGHT_MOVES[i][1]);
                
                // Get the piece which the move will lead to from the empty square.
                otherPiece = state[moveIndex];
                
                if (otherPiece == KNIGHT)
                {
                    // Swap the empty square with the other piece's index
                    swap( &(state[emptySquare]), &(state[moveIndex]) );
                    
                    // Save the state - moveIndex is the new empty square
                    nextStates.emplace_back(state, moveIndex);
                    
                    // Move the pieces back to where they were on the board
                    swap( &(state[emptySquare]), &(state[moveIndex]) );
                }
            }
        }
        
        return nextStates;
    }
    
    static bool isMoveWithinBoundry(int emptyInd, int x, int y)
    {
        // Check horizontally to ensure that the horizontal move does not move us to up/down in the array due to indexing.
        if (((emptyInd + x) < 0) || ((emptyInd / NUM_COL) != ((emptyInd + x) / NUM_COL)))
            return false;
        
        // Check if the overall index is within boundaries.
        int newIndex = INDEXIFY(emptyInd, x, y);
        if ((newIndex == INVALID_INDEX[0]) || (newIndex == INVALID_INDEX[1]) ||
            (newIndex < 0) || (newIndex > (BOARD_SIZE-1)))
        {
            return false;
        }
        
        return true;
    }
    
    static void displayBoard(const string& board)
    {
        cout << "=======";
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (i == INVALID_INDEX[0] || i == INVALID_INDEX[1])
            {
                cout << "  ";
                continue;
            }
            
            if ((i % NUM_COL) == 0)
                cout << endl;
            
            cout << board[i] << " ";
        }
        
        cout << endl << "=======" << endl;
    }
    
    static void displaySolution(const string& finalBoard, const unordered_map<string, tuple<int, int, string>>& states)
    {
        string curState = finalBoard;
        while (true)
        {
            auto [emptyInd, numMoves, prevState] = states.at(curState);
            if (prevState.empty())
                break;
            
            cout << endl << "MOVE " << numMoves << ":"<< endl;
            displayBoard(curState);
            curState = prevState;
        }
        
        // Print the initial board
        cout << endl << "INITIAL BOARD:" << endl;
        displayBoard(initialState);
    }
};

int main(int argc, const char * argv[]) {
    // Get the starting time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run the puzzle
    PawnToQueenPuzzle::solve();
    
    // Get the ending time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Output the duration in seconds
    std::cout << std::endl << "Time taken to run the program: " << duration.count() << " seconds" << std::endl;
    
    return 0;
}
