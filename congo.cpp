#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>


using namespace std;


typedef unordered_multimap<string,string> :: iterator iter;
struct State{
    // State of the game is stored in an unordered_map.
    // We can find the position of any piece by using the .find() method
    unordered_multimap<string,string> boardByPiece;
    unordered_multimap<string,string> boardByPosition;
    unordered_multimap<string,string> whitePieces;
    unordered_multimap<string,string> blackPieces;

    // bool that will represent which players turn it is
    bool whitesTurn = true;

    int moveNumber;

    State(unordered_multimap<string,string>&state,unordered_multimap<string,string>&state2,unordered_multimap<string,string>&w,unordered_multimap<string,string>&b,bool WhitesTurnToPlay,int move){
        boardByPiece = state;
        boardByPosition = state2;
        whitePieces = w;
        blackPieces = b;
        moveNumber = move;
        whitesTurn = WhitesTurnToPlay;
    }

};
//jus declaring the method for now. Completed after main function
void PrintPositions(vector<State *>&states);
void PrintMoves(vector<string>&moves, string& StartPos);
void PrintMoves(vector<string>&moves,string& StartPos, bool& moreThanOne);
void StringToState(string state,vector<State*>&states){
    // unordered multimap so searching is faster and we can have duplicate keys
    unordered_multimap<string,string> StatesByPiece;
    unordered_multimap<string,string> StatesByPosition;
    unordered_multimap<string,string> wPieces;
    unordered_multimap<string,string> bPieces;
    vector<string> alphabets = {"a","b","c","d","e","f","g"};
    // bool that will be changed to true if it whites turn to play
    bool whitePlay =false;
    int move;

    string :: iterator it;
    // pos will be used to access the array alphabets at an index
    int pos=0;
    // count will be used to be added to a string so we can have elements belonging to block a1,a2,a3 etc
    // we also decrease count as it corresponds to a rank/level
    int count=7;
    //string stream of the state of the board
    stringstream SS(state);
    string temp,state2;
    // i will be used to ensure that we only filter the state string for positions
    int i=0;
    // the >> moves onto the next word in the string state
    // we'll always have 3 words then. FIrst is the string representing the state, then it's the string representing whose turn it is and lastly the move number
    while(SS>>temp){
        if(i==0){
            state2 = temp;
            // we iterate through the entire position string character by character
            for(it=state2.begin();it!=state2.end();it++){
                // if we encounter a '/', we are moving onto a new rank so we decrease count by 1 and reset pos because we need to iterate through alphabets again
                // for the new rank
                if(*it == '/'){
                    count --;
                    pos = 0;
                }
                // We check if the current character is a digit, if it is then we just set that position on the board to Empty since there's no piece there
                else if(isdigit(*it)){
                    string p ="";
                    p+=*it;
                    // converting string to int
                    int bounds = stoi(p);
                    // loop as many times as bounds and add those empties to the map
                    for(int i=0;i<bounds;i++){
                        string key = alphabets[pos];
                        key+= to_string(count);
                        pos++;
                        // We make the value of the piece , P or p for example the key so we can search in linear time in the worst case using .find()
                        StatesByPiece.insert({"Empty", key});
                        StatesByPosition.insert({key,"Empty"});
                    }
                }
                // If it is an actual piece, then the process is the same
                else{
                    string key = alphabets[pos];
                    key+= to_string(count);
                    pos++;
                    string val= "";
                    val+=*it;
                    // if it is capitalized then it is a white piece
                    if(isupper(*it)){
                        wPieces.insert({key,val});
                    }
                    else{
                        bPieces.insert({key,val});
                    }
                    StatesByPiece.insert({val, key});
                    StatesByPosition.insert({key,val});
                }
            }
            // increment i once we finish the if statement because we are done processing the position string
            i++;
        }
        else{
            // check if the next work is w or b and set whitePlay accordingly
            if(temp == "w"){

                whitePlay = true;
            }
            else if(temp == "b"){
                continue;
            }
            else{
                // convert the last string to an int which represents which move it is
                move = stoi(temp);
            }
        }
    }
    // create a new state and add it to the states vector
    State * newState = new State(StatesByPiece,StatesByPosition,wPieces,bPieces,whitePlay,move);
    states.push_back(newState);


}

void setStates(vector<string>&inputs,vector<State*>&states){
    vector<string> :: iterator it;
    //loop through all the input strings and create a new state for each of them
    for(it=inputs.begin(); it!=inputs.end();it++){
        StringToState(*it,states);
    }
}

void ValidMoves(vector<string>&possible,vector<string>&moves,State * s){
    // loop through all the possibilities and only add it to Moves if the block is empty or it has a black or white piece there
    int size = possible.size();

    if(s->whitesTurn){
        for(int i=0;i<size;i++){
            // find the what piece is at the position
            auto it = s->boardByPosition.find(possible[i]);
            // check if in that position is a black piece that can be captured
            auto hasBlack = s->blackPieces.find(possible[i]);
            if(it->second == "Empty" || hasBlack!=s->blackPieces.end()){
                // add to valid moves
                moves.push_back(it->first);
            }
        }
    }
    else{
        for(int i=0;i<size;i++){
            auto it = s->boardByPosition.find(possible[i]);
            // check if white piece is there to be captured
            auto hasWhite = s->whitePieces.find(possible[i]);
            if(it->second == "Empty" || hasWhite!=s->whitePieces.end()){
                moves.push_back(it->first);
            }
        }

    }

}

void ValidPawnMoves(vector<string>&possible,vector<string>&moves, State * s, int& Currentrank){
    // loop through all the possibilities and only add it to Moves if the block is empty or it has a black or white piece there
    int size = possible.size();

    if(s->whitesTurn){
        for(int i=0;i<size;i++){
            // find the corresponding position on the board
            auto it = s->boardByPosition.find(possible[i]);
            // see if position is occupied by a black piece
            auto hasBlack = s->blackPieces.find(possible[i]);
            // store the rank of the move to be checked in variable rank
            int rank = stoi(string(1,it->first.at(1)));
            // if Currentrank is bigger than rank, then the white piece is moving down
            // if we are moving down, then we can't capture black pieces
            if(Currentrank > rank){
                // if block is empty or does not have black piece then this backward move is valid
                if(it->second == "Empty" ){
                    moves.push_back(it->first);
                }
            }
            else{
                // if block is empty or does have black piece then forward move is valid
                if(it->second == "Empty" || hasBlack!=s->blackPieces.end()){
                    moves.push_back(it->first);
                }
            }

        }
    }
    else{
        for(int i=0;i<size;i++){
            auto it = s->boardByPosition.find(possible[i]);
            auto hasWhite = s->whitePieces.find(possible[i]);
            int rank = stoi(string(1,it->first.at(1)));
             // if Currentrank is smaller than rank, then the black piece is moving up
            // if we are moving up, then we can't capture white pieces
            if(Currentrank < rank){
                // if block is empty or does not have white piece then this backward move is valid
                if(it->second == "Empty"){
                    moves.push_back(it->first);
                }
            }
            else{
                // if block is empty or does have white piece then forward move is valid
                if(it->second == "Empty" || hasWhite!=s->whitePieces.end()){
                    moves.push_back(it->first);
                }
            }
        }

    }
}
// Get the Lions Moves using this method
// Lions can only move left or right within 3 blocks namely ,C,D and E and within rank 1-3 if it's white or rank 7-4 if it's black
// They can only move from rank 3 to rank 5, i.e accross the river if by moving accross they can capture the opposing Lion.
vector<string> LionMoves(State * state){
    vector<string>possibleMoves;
    // The first thing is to determine which players move it is
    // if it's true then it is whites turn to play so we find the white lion
    if(state->whitesTurn){
        vector<string> possibilities;
        bool straightPath = false;
        // find the current position of the white lion
        auto LionPos = state->boardByPiece.find("L");
        auto blackLionPos = state->boardByPiece.find("l");
        // get the current rank and column that the LION is in individually.
        string column(1,LionPos->second.at(0));
        string rank(1,LionPos->second.at(1));

        string Blackcolumn(1,blackLionPos->second.at(0));
        string Blackrank(1,blackLionPos->second.at(1));
        //Check if there is a straight path between the LIONS
        if(Blackcolumn == column){
            int totalDistance = stoi(Blackrank) - stoi(rank);
            // if rank < blackrank then we know black lion is above the white lion
            bool obstructed = false;
            int r = stoi(rank);
            string pos = column+to_string(r+1);
            r++;
            while(!obstructed){
                string temp = pos;
                auto it = state->boardByPosition.find(temp);
                if(it->second == "Empty"){
                    pos = column + to_string(r);
                    r++;
                }
                else{
                    if(it->second == "l")
                        straightPath =true;
                    obstructed = true;
                }
            }
        }
        // if it is in d then it can move left,right,possibly up,possibly down and diagonally.
        if(column == "d"){

            // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string left = "c"+rank;
            string right = "e"+rank;
            string up = "";
            string down = "";
            string DiagonalLeftUp = "";
            string DiagonalRightUp = "";
            string DiagonalLeftDown = "";
            string DiagonalRightDown = "";
            if(Currentrank ==1 || Currentrank ==2){

                string nextRank = to_string(Currentrank+1);
                // set the diagaonal positions by increasing rank and shifting left and right
                DiagonalLeftUp = "c"+nextRank;
                DiagonalRightUp = "e"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==2){
                    string newRank = to_string(Currentrank - 1);
                    down = column+newRank;
                    DiagonalLeftDown = "c"+newRank;
                    DiagonalRightDown = "e"+newRank;
                    possibilities = {left,right,up,down,DiagonalLeftUp,DiagonalRightUp,DiagonalLeftDown,DiagonalRightDown};
                }
                else{
                    possibilities = {left,right,up,DiagonalLeftUp,DiagonalRightUp};
                }
                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);
            }
            else{
                // if the rank is 3
                // we need to check if the opposing lion is in the first row after the river. Positions c5,e5,d5
                vector<string>overRiver = {"d5"};
                string newRank = to_string(Currentrank -1);
                down = column+newRank;
                DiagonalLeftDown = "c"+newRank;
                DiagonalRightDown = "e"+newRank;
                possibilities = {left,right,down,DiagonalLeftDown,DiagonalRightDown};
                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);
            }

        }
        else if( column == "c"){
             // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string right = "d"+rank;
            string up = "";
            string down = "";
            string DiagonalRightUp = "";
            string DiagonalRightDown = "";
            if(Currentrank ==1 || Currentrank ==2){
                string nextRank = to_string(Currentrank+1);
                // set the diagaonal positions by increasing rank and shifting left and right
                DiagonalRightUp = "d"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==2){
                    string newRank = to_string(Currentrank - 1);
                    down = column+newRank;
                    DiagonalRightDown = "d"+newRank;
                    possibilities = {right,up,down,DiagonalRightUp,DiagonalRightDown};
                }
                else{
                    possibilities = {right,up,DiagonalRightUp};
                }
                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);

            }
            else{
                // if the rank is 3
                // we need to check if the opposing lion is in the first row after the river. Positions c5,e5,d5
                vector<string>overRiver = { "e5"};
                string newRank = to_string(Currentrank -1);
                down = column+newRank;
                DiagonalRightDown = "d"+newRank;
                possibilities = {right,down,DiagonalRightDown};
                // checking across the river if a lion is there
                auto b = state->blackPieces.find(overRiver[0]);
                if(b!=state->blackPieces.end()){
                    if(b->second == "l"){
                        auto between = state->boardByPosition.find("d4");
                        if(between->second == "Empty")
                            possibleMoves.push_back(overRiver[0]);
                    }
                }

                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);
            }

        }
        //column is e
        else{
            // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string left = "d"+rank;
            string up = "";
            string down = "";
            string DiagonalLeftUp = "";
            string DiagonalLeftDown = "";
            if(Currentrank ==1 || Currentrank ==2){
                string nextRank = to_string(Currentrank+1);
                // set the diagaonal positions by increasing rank and shifting left and right
                DiagonalLeftUp = "d"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==2){
                    string newRank = to_string(Currentrank - 1);
                    down = column+newRank;
                    DiagonalLeftDown = "d"+newRank;
                    possibilities = {left,up,down,DiagonalLeftUp,DiagonalLeftDown};
                }
                else{
                    possibilities = {left,up,DiagonalLeftUp};
                }
                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);

            }
            else{
                // if the rank is 3
                // we need to check if the opposing lion is in the first row after the river. Positions c5,e5,d5
                vector<string>overRiver = {"c5"};
                string newRank = to_string(Currentrank -1);
                down = column+newRank;
                DiagonalLeftDown = "d"+newRank;
                possibilities = {left,down,DiagonalLeftDown};
                // checking across the river if a lion is there

                auto b = state->blackPieces.find(overRiver[0]);
                if(b!=state->blackPieces.end()){
                    if(b->second == "l"){
                        auto between = state->boardByPosition.find("d4");
                        if(between->second=="Empty")
                            possibleMoves.push_back(overRiver[0]);

                    }
                }

                if(straightPath){
                    possibilities.push_back(blackLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);
            }


        }
        PrintMoves(possibleMoves,LionPos->second);
    }
    // black lions move
    else{
        vector<string> possibilities;
        bool straightPath = false;
        // find the current position of the white lion
        auto LionPos = state->boardByPiece.find("l");
        auto whiteLionPos = state->boardByPiece.find("L");
        // get the current rank and column that the LION is in individually.
        string column(1,LionPos->second.at(0));
        string rank(1,LionPos->second.at(1));

        string Whitecolumn(1,whiteLionPos->second.at(0));
        string Whiterank(1,whiteLionPos->second.at(1));
         //Check if there is a straight path between the LIONS
        if(Whitecolumn == column){
            if(Whitecolumn == column){
                int totalDistance = stoi(rank) - stoi(Whiterank);
                // if rank < blackrank then we know black lion is above the white lion
                bool obstructed = false;
                int r = stoi(Whiterank);
                string pos = Whitecolumn+to_string(r+1);
                r++;
                while(!obstructed){
                    string temp = pos;
                    auto it = state->boardByPosition.find(temp);
                    if(it->second == "Empty"){
                        pos = column + to_string(r);
                        r++;
                    }
                    else{
                        if(it->second == "l")
                            straightPath =true;
                        obstructed = true;
                    }
                }
            }
        }
        // if it is in d then it can move left,right,possibly up,possibly down and diagonally.
        if(column == "d"){

            // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string left = "c"+rank;
            string right = "e"+rank;
            string up = "";
            string down = "";
            string DiagonalLeftUp = "";
            string DiagonalRightUp = "";
            string DiagonalLeftDown = "";
            string DiagonalRightDown = "";
            if(Currentrank ==6 || Currentrank ==7){
                string nextRank = to_string(Currentrank-1);
                // set the diagaonal positions by decreasing rank and shifting left and right
                DiagonalLeftUp = "c"+nextRank;
                DiagonalRightUp = "e"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==6){
                    string newRank = to_string(Currentrank + 1);
                    down = column+newRank;
                    DiagonalLeftDown = "c"+newRank;
                    DiagonalRightDown = "e"+newRank;
                    possibilities = {left,right,up,down,DiagonalLeftUp,DiagonalRightUp,DiagonalLeftDown,DiagonalRightDown};
                }
                else{
                    possibilities = {left,right,up,DiagonalLeftUp,DiagonalRightUp};
                }
                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);

            }
            else{
                // if the rank is 5
                // we need to check if the opposing lion is in the first row after the river. Positions c5,e5,d5
                string newRank = to_string(Currentrank +1);
                down = column+newRank;
                DiagonalLeftDown = "c"+newRank;
                DiagonalRightDown = "e"+newRank;
                possibilities = {left,right,down,DiagonalLeftDown,DiagonalRightDown};
                // checking across the river if a lion is there
                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a white piece there
                ValidMoves(possibilities,possibleMoves,state);
            }

        }
        else if( column == "c"){
             // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string right = "d"+rank;
            string up = "";
            string down = "";
            string DiagonalRightUp = "";
            string DiagonalRightDown = "";
            if(Currentrank ==6 || Currentrank ==7){
                string nextRank = to_string(Currentrank-1);
                // set the diagaonal positions by increasing rank and shifting left and right
                DiagonalRightUp = "d"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==6){
                    string newRank = to_string(Currentrank + 1);
                    down = column+newRank;
                    DiagonalRightDown = "d"+newRank;
                    possibilities = {right,up,down,DiagonalRightUp,DiagonalRightDown};
                }
                else{
                    possibilities = {right,up,DiagonalRightUp};
                }
                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a white piece there
                ValidMoves(possibilities,possibleMoves,state);
            }
            else{
                // if the rank is 5
                // we need to check if the opposing lion is in the first row after the river. Positions c3,e3,d3
                string newRank = to_string(Currentrank +1);
                down = column+newRank;
                DiagonalRightDown = "d"+newRank;
                possibilities = {right,down,DiagonalRightDown};
                // checking across the river if a lion is there
                auto b = state->whitePieces.find("e3");
                if(b!=state->whitePieces.end()){
                    if(b->second == "L"){
                        auto between = state->boardByPosition.find("d4");
                        if(between->second == "Empty")
                            possibleMoves.push_back("e3");
                    }
                }

                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a white piece there
                ValidMoves(possibilities,possibleMoves,state);

            }

        }
        //column is e
        else{
            // convert rank to int because depending in the rank, certain additional moves will be available
            int Currentrank = stoi(rank);
            string left = "d"+rank;
            string up = "";
            string down = "";
            string DiagonalLeftUp = "";
            string DiagonalLeftDown = "";
            if(Currentrank ==6 || Currentrank ==7){
                string nextRank = to_string(Currentrank-1);
                // set the diagaonal positions by increasing rank and shifting left and right
                DiagonalLeftUp = "d"+nextRank;
                up = column+nextRank;
                // we add all these moves to possibilities
                if(Currentrank ==6){
                    string newRank = to_string(Currentrank + 1);
                    down = column+newRank;
                    DiagonalLeftDown = "d"+newRank;
                    possibilities = {left,up,down,DiagonalLeftUp,DiagonalLeftDown};
                }
                else{
                    possibilities = {left,up,DiagonalLeftUp};
                }
                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a white piece there
                ValidMoves(possibilities,possibleMoves,state);
            }
            else{
                // if the rank is 5
                // we need to check if the opposing lion is in the first row after the river. Positions c3,e3,d3
                string newRank = to_string(Currentrank +1);
                down = column+newRank;
                DiagonalLeftDown = "d"+newRank;
                possibilities = {left,down,DiagonalLeftDown};
                // checking across the river if a lion is there

                auto b = state->boardByPosition.find("c3");
                if(b->second == "L"){
                    auto between = state->boardByPosition.find("d4");
                    if(between->second == "Empty")
                        possibleMoves.push_back("c3");
                }
        
                if(straightPath){
                    possibilities.push_back(whiteLionPos->second);
                }
                // loop through all the possibilities and only add it to possibleMoves if the block is empty or it has a black piece there
                ValidMoves(possibilities,possibleMoves,state);
            }

        }

         PrintMoves(possibleMoves,LionPos->second);
    }

    return possibleMoves;
}

// Get the zebras moves using this method
// Zebras move in an L shape. Either 2 blocks horizontally and 1 vertically OR 2 Blocks vertically and 1 block horizontally
// Zebras capture a piece of the opposition, if the piece is on the ending block of the move
vector<string> ZebraMoves(State * state) {
    vector<string>possibleMoves;
    unordered_multimap<string,string> :: iterator zebraPos;
    //white zebra to move
    if(state->whitesTurn){
        zebraPos = state->boardByPiece.find("Z");
    }
    else{
        zebraPos = state->boardByPiece.find("z");
    }
    if(zebraPos == state->boardByPiece.end()){
        string p = "NULL";
        PrintMoves(possibleMoves,p);
    }
    else{
        vector<string>possibilities;
        //find the white zebra and get it's current rank and column
        // auto zebraPos = state->boardByPiece.find("Z");
        string column(1,zebraPos->second.at(0));
        string rank(1,zebraPos->second.at(1));
        int Currentrank = stoi(rank);
        //try shifting the rank up and down by 2 levels and the column left and right by two columns
        char doubleRight = static_cast<char>(zebraPos->second.at(0)+2);
        char doubleLeft = static_cast<char>(zebraPos->second.at(0)-2);
        int doubleUp = Currentrank +2;
        int doubleDown = Currentrank -2;
        bool moveLeftAndRight = (doubleRight <='g' && doubleLeft>='a' );
        bool moveUpAndDown = (doubleUp<=7 && doubleDown >=1);
        if(column == "a"){
            if(Currentrank ==7){
                //down down and right
                string move1 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                possibilities = {move1,move2};
            }
            else if(Currentrank ==6 || Currentrank == 2){
                // right,right and up
                string move1 = static_cast<char>(column[0]+2) + to_string(Currentrank+1);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                string move3;
                if(Currentrank == 6){
                    //down down and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                }
                else{
                    //up up and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                }

                possibilities = {move1,move2,move3};
            }
            else if(Currentrank==4 || Currentrank ==5 || Currentrank==3){
                // right,right and up
                string move1 = static_cast<char>(column[0]+2) + to_string(Currentrank+1);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                //down down and right
                string move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //up, up and right
                string move4 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                possibilities = {move1,move2,move3,move4};
            }
            else{
                // right right up
                string move1 = static_cast<char>(column[0]+2)+to_string(Currentrank+1);
                //up, up and right
                string move2 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                possibilities = {move1,move2};
            }

        }
        else if( column =="b"){
            if(Currentrank ==7){
                //down down and right
                string move1 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //down down and left
                string move2 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                // right,right and down
                string move3 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                possibilities = {move1,move2,move3};
            }
            else if(Currentrank ==6 || Currentrank == 2){
                // right,right and up
                string move1 = static_cast<char>(column[0]+2) + to_string(Currentrank+1);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                string move3,move4;
                if(Currentrank == 6){
                    //down down and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                    //down down and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                }
                else{
                    //up up and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                    //up up and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                }

                possibilities = {move1,move2,move3,move4};
            }
            else if(Currentrank==4 || Currentrank ==5 || Currentrank==3){
                // right,right and up
                string move1 = static_cast<char>(column[0]+2) + to_string(Currentrank+1);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                //down down and right
                string move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //down down and left
                string move4 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                //up, up and right
                string move5 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                //up up and left
                string move6 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                possibilities = {move1,move2,move3,move4,move5,move6};
            }
            else{
                // right right up
                string move1 = static_cast<char>(column[0]+2)+to_string(Currentrank+1);
                //up, up and right
                string move2 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                //up, up and left
                string move3 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                possibilities = {move1,move2,move3};
            }

        }
        else if(column=="c" || column =="d" || column =="e"){

            if(Currentrank ==7){
                //down down and right
                string move1 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //down down and left
                string move2 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                // right,right and down
                string move3 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                // left left and down
                string move4 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                possibilities = {move1,move2,move3,move4};
            }
            else if(Currentrank ==6 || Currentrank == 2){
                // right,right and up
                string move1 = static_cast<char>(column[0]+2) + to_string(Currentrank+1);
                // right,right and down
                string move2 = static_cast<char>(column[0]+2) + to_string(Currentrank-1);
                string move3,move4,move5,move6;
                if(Currentrank == 2){
                    // up up and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                    //up up and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                }
                else{
                    //down down and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                    //down down and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                }

                //left left and up
                move5 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                //left left and down
                move6 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);

                possibilities = {move1,move2,move3,move4,move5,move6};
            }
            else if(Currentrank== 5 || Currentrank==4|| Currentrank==3){
                // move right, right and either up or down
                string move1 = doubleRight+to_string(Currentrank+1);
                string move2 = doubleRight+to_string(Currentrank-1);
                // moving left,left, then either up or down
                string move3 = doubleLeft+to_string(Currentrank+1);
                string move4 = doubleLeft+to_string(Currentrank-1);
                //moving up,up,then either left or right
                string move5 = static_cast<char>(column[0] -1)+to_string(Currentrank+2);
                string move6 = static_cast<char>(column[0] +1)+to_string(Currentrank+2);
                //moving down,down, then either left or right
                string move7 = static_cast<char>(column[0] -1)+to_string(Currentrank-2);
                string move8 = static_cast<char>(column[0] +1)+to_string(Currentrank-2);

                possibilities = {move1,move2,move3,move4,move5,move6,move7,move8};
            }
            else{
                // right right up
                string move1 = static_cast<char>(column[0]+2)+to_string(Currentrank+1);
                //up, up and right
                string move2 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                //up, up and left
                string move3 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                //left left and up
                string move4 = static_cast<char>(column[0]-2)+to_string(Currentrank+1);
                possibilities = {move1,move2,move3,move4};
            }
        }
        else if(column =="f"){
            if (Currentrank == 7){
                // left left and down
                string move1 = static_cast<char>(column[0]+-2)+to_string(Currentrank-1);
                //down down and right
                string move2 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //down down and left
                string move3 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                possibilities = {move1,move2,move3};
            }
            else if(Currentrank ==6 || Currentrank == 2){
                //left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                //left left and down
                string move2 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                string move3,move4;
                if(Currentrank == 6){
                    //down down and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                    //down down and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                }
                else{
                    //up up and right
                    move3 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                    //up up and left
                    move4 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                }

                possibilities = {move1,move2,move3,move4};
            }
            else if(Currentrank ==5 || Currentrank ==4 || Currentrank ==3){
                // left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                // left left and down
                string move2 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                //up, up and right
                string move3 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                //up, up and left
                string move4 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                //down down and right
                string move5 = static_cast<char>(column[0]+1) + to_string(Currentrank-2);
                //down down and left
                string move6 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);

                possibilities = {move1,move2,move3,move4,move5,move6};

            }
            else{
                // left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                 //up, up and right
                string move2 = static_cast<char>(column[0]+1) + to_string(Currentrank+2);
                //up, up and left
                string move3 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                possibilities = {move1,move2,move3};

            }
        }
        else{
            if(Currentrank ==7){
                // left left and down
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                //down down and left
                string move2 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                possibilities = {move1,move2};
            }
            else if(Currentrank ==6 || Currentrank== 2){
                // left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                // left left and down
                string move2 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                string move3;
                if(Currentrank == 6){
                    //down down left
                    move3 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                }
                else{
                    //up up left
                    move3 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                }
                possibilities = {move1,move2,move3};
            }
            else if(Currentrank ==5 || Currentrank== 4 || Currentrank==3){
                // left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                // left left and down
                string move2 = static_cast<char>(column[0]-2) + to_string(Currentrank-1);
                //down down and left
                string move3 = static_cast<char>(column[0]-1) + to_string(Currentrank-2);
                //up, up and left
                string move4 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                possibilities = {move1,move2,move3,move4};

            }
            else{
                // left left and up
                string move1 = static_cast<char>(column[0]-2) + to_string(Currentrank+1);
                //up, up and left
                string move2 = static_cast<char>(column[0]-1) + to_string(Currentrank+2);
                possibilities = {move1,move2};

            }

        }

        ValidMoves(possibilities,possibleMoves,state);
        PrintMoves(possibleMoves,zebraPos->second);
    }
    return possibleMoves;
}

// Get the elephant moves using this method
// Since there can be more than on elephant, we need to find them all then call this method with their positions
// Elephants can move one or two squares vertically or horizontally
// If it moves two blocks and in the first block is an opponent piece, that piece is unaffected
vector<string> ElephantMoves(string currentPosition, State * state, bool& multipleElephants){
    vector<string> possibleMoves;
    vector<string>possibilities;
    //find the elephant and get it's current rank and column
    string column(1,currentPosition.at(0));
    string rank(1,currentPosition.at(1));
    int Currentrank = stoi(rank);

    if(column =="a" || column == "b" || column=="c" || column=="d" || column=="e"){
        if(Currentrank ==7){
            // Right
            string move1 = static_cast<char>(column[0]+1)+ rank;
            // right right
            string move2 = static_cast<char>(column[0]+2)+ rank;
            // down
            string move3 = column+to_string(Currentrank-1);
            // down down
            string move4 = column+to_string(Currentrank-2);
            possibilities = {move1,move2,move3,move4};
        }
        else if(Currentrank ==2 || Currentrank == 6){
            // Right
            string move1 = static_cast<char>(column[0]+1)+rank;
            // right Right
            string move2 = static_cast<char>(column[0]+2)+rank;
            string move5;
            // up
            string move3 = column + to_string(Currentrank +1);
            // down
            string move4 = column + to_string(Currentrank -1 );
            if(Currentrank == 6){
                // down down
                move5 = column + to_string(Currentrank - 2);
            }
            else{
                // up up
                move5 = column + to_string(Currentrank + 2);
            }
            possibilities = {move1,move2,move3,move4,move5};
        }
        else if(Currentrank == 5 || Currentrank==4 || Currentrank==3){
            // Right
            string move1 = static_cast<char>(column[0]+1)+rank;
            // right Right
            string move2 = static_cast<char>(column[0]+2)+rank;
            // up
            string move3 = column + to_string(Currentrank +1);
            // down
            string move4 = column + to_string(Currentrank -1 );
            // down down
            string move5 = column + to_string(Currentrank - 2);
            // up up
            string move6 =  column + to_string(Currentrank + 2);
            possibilities = {move1,move2,move3,move4,move5,move6};
        }
        else{
             // Right
            string move1 = static_cast<char>(column[0]+1)+rank;
            // right Right
            string move2 = static_cast<char>(column[0]+2)+rank;
            // up
            string move3 = column + to_string(Currentrank +1);
            // up up
            string move4 =  column + to_string(Currentrank + 2);
            possibilities = {move1,move2,move3,move4};
        }
        if(column == "b"){
            // left is the only additional move it can make
            string leftMove = static_cast<char>(column[0]-1)+rank;
            possibilities.push_back(leftMove);
        }
        else if(column == "c" || column == "d" || column == "e"){
            // left and left left are the only additional moves that can be made
            string leftMove = static_cast<char>(column[0]-1)+rank;
            string leftleftMove = static_cast<char>(column[0]-2)+rank;
            possibilities.push_back(leftMove);
            possibilities.push_back(leftleftMove);
        }

    }
    else if(column == "f" || column == "g"){
         if(Currentrank ==7){
            // left
            string move1 = static_cast<char>(column[0]-1)+ rank;
            // left left
            string move2 = static_cast<char>(column[0]-2)+ rank;
            // down
            string move3 = column+to_string(Currentrank-1);
            // down down
            string move4 = column+to_string(Currentrank-2);
            possibilities = {move1,move2,move3,move4};
        }
        else if(Currentrank ==2 || Currentrank == 6){
            // left
            string move1 = static_cast<char>(column[0]-1)+rank;
            // left left
            string move2 = static_cast<char>(column[0]-2)+rank;
            string move5;
            // up
            string move3 = column + to_string(Currentrank +1);
            // down
            string move4 = column + to_string(Currentrank -1 );
            if(Currentrank == 6){
                // down down
                move5 = column + to_string(Currentrank - 2);
            }
            else{
                // up up
                move5 = column + to_string(Currentrank + 2);
            }
            possibilities = {move1,move2,move3,move4,move5};
        }
        else if(Currentrank == 5 || Currentrank==4 || Currentrank==3){
            // left
            string move1 = static_cast<char>(column[0]-1)+rank;
            // left left
            string move2 = static_cast<char>(column[0]-2)+rank;
            // up
            string move3 = column + to_string(Currentrank +1);
            // down
            string move4 = column + to_string(Currentrank -1 );
            // down down
            string move5 = column + to_string(Currentrank - 2);
            // up up
            string move6 =  column + to_string(Currentrank + 2);
            possibilities = {move1,move2,move3,move4,move5,move6};
        }
        else{
             // left
            string move1 = static_cast<char>(column[0]-1)+rank;
            // left left
            string move2 = static_cast<char>(column[0]-2)+rank;
            // up
            string move3 = column + to_string(Currentrank +1);
            // up up
            string move4 =  column + to_string(Currentrank + 2);
            possibilities = {move1,move2,move3,move4};
        }
        if(column == "f"){
            // only additional move is right
            string rightMove = static_cast<char>(column[0]+1)+rank;
            possibilities.push_back(rightMove);
        }
    }
    ValidMoves(possibilities,possibleMoves,state);
    PrintMoves(possibleMoves,currentPosition,multipleElephants);
    return possibleMoves;
}
vector<string> ElephantMoves(State * s){
    vector<string>eMoves;
    unordered_multimap<string,string> :: iterator iter1,iter2;
    vector<string>initialPositions;
    bool multipleElephants = false;
    // check which players turn it is and find the relevant elephant
    if(s->whitesTurn){
        auto it = s->boardByPiece.equal_range("E");
        iter1 = it.first;
        iter2 = it.second;
    }
    else{
        auto it = s->boardByPiece.equal_range("e");
        iter1 = it.first;
        iter2 = it.second;
    }
    // No elephants
    if(iter1 == iter2){
        return eMoves;
    }
    // add the positions to the vector
    for(auto it = iter1; it!= iter2;it++){
       initialPositions.push_back(it->second);
    }
    int length = initialPositions.size();
    // if there's more than one element, then we have multiple 2 elephants of the same colour
    if(length>1){
        multipleElephants = true;
    }
    // sort the vector so we have alphabetical order
    sort(initialPositions.begin(),initialPositions.end());
    for(int i=0;i<length;i++){
        if(i == length-1){
            multipleElephants = false;
        }
        vector<string> e = ElephantMoves(initialPositions[i],s,multipleElephants);
        if(i==0){
            eMoves.insert(eMoves.begin(),e.begin(),e.end());
        }
        else{
            eMoves.insert(eMoves.end(),e.begin(),e.end());
        }
    }
    return eMoves;
}

// Get the Pawn moves using this method
// Since there can be more than one Pawn, we need to first find them then call this method for each of them
// Pawns can move forward one block up or diagonally
// White pawns can move back one or two steps back after passing the river. i.e rank 4
// Black pawns can move back one r two steps back after passing the river. i.e rank 4 and they are in rank 3
// When moving backwards you cannot capture an opposing piece
vector<string> PawnMoves(string currentPosition, State * state, bool& multiplePawns){
    vector<string>possibilities;
    vector<string>possibleMoves;
    // get the column and rank of the current piece
    string column(1,currentPosition.at(0));
    string rank(1,currentPosition.at(1));
    // convert rank to an int since we'll be using it to get moves etc
    int Currentrank = stoi(rank);

    // white pawns move
    if(state->whitesTurn){
        if(column != "g"){
            // there is no left diagonal if we are in column a

            // The pawn can only move back one it is past rank 4
            // So the only possible moves are up and diagonal right
            if(Currentrank <=4){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal right up
                string move2 = static_cast<char>(column[0]+1)+ to_string(Currentrank+1);
                possibilities = {move1,move2};
            }
            // if it is in 5 or 6, it can still move up and diagonal right, but also down one or two block
            else if(Currentrank == 5 || Currentrank == 6){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal right up
                string move2 = static_cast<char>(column[0]+1)+ to_string(Currentrank+1);
                // down
                string move3 = column + to_string(Currentrank -1 );
                // down down
                string move4 = column + to_string(Currentrank - 2);

                // back move is only valid if there are no pieces in the first back block
                if(state->boardByPosition.find(move3)->second == "Empty"){
                    possibilities = {move1,move2,move3,move4};
                }
                else{
                    possibilities = {move1,move2};
                }

            }
            //rank is 7, then we can only move down
            else{
                // down
                string move1 = column + to_string(Currentrank -1 );
                // down down
                string move2 = column + to_string(Currentrank - 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2};
                }
            }

            if(column != "a" && Currentrank != 7){
                // the only additional move is a diagonal left
                string leftDiag = static_cast<char>(column[0]-1)+to_string(Currentrank +1);
                possibilities.push_back(leftDiag);
            }
        }
        // column is g
        else{
            if(Currentrank <=4){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal left up
                string move2 = static_cast<char>(column[0]-1)+ to_string(Currentrank+1);
                possibilities = {move1,move2};
            }
            // if it is in 5 or 6, it can still move up and diagonal left, but also down one or two block
            else if(Currentrank == 5 || Currentrank == 6){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal left up
                string move2 = static_cast<char>(column[0]-1)+ to_string(Currentrank+1);
                // down
                string move3 = column + to_string(Currentrank -1 );
                // down down
                string move4 = column + to_string(Currentrank - 2);
                if(state->boardByPosition.find(move3)->second == "Empty"){
                    possibilities = {move1,move2,move3,move4};
                }
                else{
                    possibilities = {move1,move2};
                }
            }
            //rank is 7, then we can only move down
            else{
                // down
                string move1 = column + to_string(Currentrank -1 );
                // down down
                string move2 = column + to_string(Currentrank - 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2};
                }
            }
        }
    }
    // black pawns moves
    else{

        if(column != "g"){
            // The pawn can only move back one it is past rank 4
            // So the only possible moves are down and diagonal right down
            if(Currentrank >=4){
                // down
                string move1 = column + to_string(Currentrank -1);
                // diagonal right down
                string move2 = static_cast<char>(column[0]+1)+ to_string(Currentrank-1);
                possibilities = {move1,move2};
            }
            // if it is in 2 or 3, it can still move down and diagonal right down, but also up one or two block
            else if(Currentrank == 2 || Currentrank == 3){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal right down
                string move2 = static_cast<char>(column[0]+1)+ to_string(Currentrank-1);
                // down
                string move3 = column + to_string(Currentrank -1 );
                // up up
                string move4 = column + to_string(Currentrank + 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2,move3,move4};
                }
                else{
                    possibilities = {move2,move3};
                }

            }
            //rank is 1, then we can only move up
            else{
                // up
                string move1 = column + to_string(Currentrank + 1 );
                // up up
                string move2 = column + to_string(Currentrank + 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2};
                }
            }
            if(column != "a" && Currentrank != 1){
                // the only additional move is a diagonal left
                string leftDiag = static_cast<char>(column[0]-1)+to_string(Currentrank -1);
                possibilities.push_back(leftDiag);
            }
        }
        // column is g
        else{
            if(Currentrank >=4){
                // down
                string move1 = column + to_string(Currentrank -1);
                // diagonal left up
                string move2 = static_cast<char>(column[0]-1)+ to_string(Currentrank-1);
                possibilities = {move1,move2};
            }
            // if it is in 2 or 3, it can still move up and diagonal left, but also down one or two block
            else if(Currentrank == 2 || Currentrank == 3){
                // up
                string move1 = column + to_string(Currentrank +1);
                // diagonal left down
                string move2 = static_cast<char>(column[0]-1)+ to_string(Currentrank-1);
                // down
                string move3 = column + to_string(Currentrank -1 );
                // up up
                string move4 = column + to_string(Currentrank + 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2,move3,move4};
                }
                else{
                    possibilities = {move2,move3};
                }

            }
            //rank is 7, then we can only move up
            else{
                // up
                string move1 = column + to_string(Currentrank +1 );
                // up up
                string move2 = column + to_string(Currentrank + 2);
                if(state->boardByPosition.find(move1)->second == "Empty"){
                    possibilities = {move1,move2};
                }
            }
        }
    }
    ValidPawnMoves(possibilities,possibleMoves,state,Currentrank);
    PrintMoves(possibleMoves,currentPosition,multiplePawns);
    return possibleMoves;
}
vector<string> PawnMoves(State * s){
    vector<string>pMoves;
    unordered_multimap<string,string> :: iterator iter1,iter2;
    vector<string>initialPositions;
    bool multiplePawns = false;
    // check which players turn it is and find the relevant pawns
    if(s->whitesTurn){
        auto it = s->boardByPiece.equal_range("P");
        iter1 = it.first;
        iter2 = it.second;
    }
    else{
        auto it = s->boardByPiece.equal_range("p");
        iter1 = it.first;
        iter2 = it.second;
    }
    // No pawns
    if(iter1 == iter2){
        return pMoves;
    }
    // add the positions to the vector
    for(auto it = iter1; it!= iter2;it++){
       initialPositions.push_back(it->second);
    }
    int length = initialPositions.size();
    // if there's more than one element, then we have multiple 2 elephants of the same colour
    if(length>1){
        multiplePawns = true;
    }
    // sort the vector so we have alphabetical order
    sort(initialPositions.begin(),initialPositions.end());
    for(int i=0;i<length;i++){
        if(i == length-1){
            multiplePawns = false;
        }
        vector<string> p = PawnMoves(initialPositions[i],s,multiplePawns);
        if(i==0){
            pMoves.insert(pMoves.begin(),p.begin(),p.end());
        }
        else{
            pMoves.insert(pMoves.end(),p.begin(),p.end());
        }
    }
    return pMoves;
}
// updating the list where we store the configuration as with the piece as a key
// Parameters follow the order: board, piece we are going to replace, position of the piece we are replacing, string that will replace old string
void update_Piece_List(unordered_multimap<string,string>&board,string piece, string pos,string update){
    pair<iter,iter> Iterat = board.equal_range(piece);
    iter it1 = Iterat.first;
    // loop through and find the corresponding postion to pos
    while(it1 != Iterat.second){
        if ( it1->second == pos){
            // erase the old item and insert the new one
            board.erase(it1);
            board.insert({update,pos});
            break;
            // return it1;
        }
        it1++;
    }
}
// This method will return all the pieces corresponding to a players pieces that are in the river before they make any move
vector<pair<string,string>> get_river_pieces(State * incomingState){
    string init = "a";
    unordered_multimap<string,string> PieceList;
    vector<pair<string,string>> answers;
    // set piece list depending on which player must move now
    if(incomingState->whitesTurn){
        PieceList = incomingState->whitePieces;
    }
    else{
        PieceList = incomingState->blackPieces;
    }
    for(int i=0;i<7;i++){
        // find everything from a4-g4
        string RiverPos = static_cast<char>(init[0]+i)+to_string(4);
        auto iterator1 = PieceList.find(RiverPos);
        if (iterator1 != PieceList.end()){
            // if there is a piece in the river , add its position to the vector
            answers.push_back(make_pair(iterator1->first,iterator1->second));
        }
    }
    return answers;
}
// method that will adjust the state given a move
State * makeMove(State * state,string& move){
    // create the new state which we will adjust according to the move to be made
    State * newState =  new State(state->boardByPiece,state->boardByPosition,state->whitePieces,state->blackPieces,state->whitesTurn,state->moveNumber);
    // if the move being made is a white move, then for the new piece it needs to be a black move
    if(state->whitesTurn){
        newState->whitesTurn = false;
    }
    else{
        //if this move is a black move then the new state will be a white move
        newState->whitesTurn = true;
        // we also increment the number of moves played since an entire round will be completed
        newState->moveNumber ++;
    }

    // we need to find any black and white pieces that are in the river before a move is made.
    // we send the old state since that state tells us who is going to play this move
    vector<pair<string,string>> PiecesInRiver = get_river_pieces(state);
    string CurrentPosition = move.substr(0,2);
    string moveTo = move.substr(2,3);
    // find the current position on the board
    auto it = newState->boardByPosition.find(CurrentPosition);
    // find the position we are moving t0
    auto it2 = newState->boardByPosition.find(moveTo);
    //if the position we are moving to is empty
    if(it2->second == "Empty"){
        // set the new position equal to the piece in the in the current position
        it2->second = it->second;
        // set the value of the current position to empty since we are leaving that block
        it->second = "Empty";
        // get an iterator in the map where the pieces are the keys and we need to update that one as well
        update_Piece_List(newState->boardByPiece,it->second,it2->first,it2->second);
        update_Piece_List(newState->boardByPiece,it2->second,it->first,it->second);
        // updating the black and white piece lists
        if(isupper(it2->second.at(0))){
            // white piece so update that list
            auto wp = newState->whitePieces.find(CurrentPosition);
            // since the block becomes empty we remove it from the list
            newState->whitePieces.erase(wp);
            // insert the update position in the white piece list
            newState->whitePieces.insert({moveTo,it2->second});      
        }
        else{
            auto bp = newState->blackPieces.find(CurrentPosition);
            // since the block becomes empty we remove it from the list
            newState->blackPieces.erase(bp);
            // insert the update position in the black piece list
            newState->blackPieces.insert({moveTo,it2->second});
        }
    }
    // the state we are moving to contains a black piece or white piece
    else{
        // set the position moved to same as the current position
        string toBeMoved = it->second;
        string Replaced = it2->second;
        it2->second = toBeMoved;
        // set the value of the current position to empty since we are leaving that block
        it->second = "Empty";
        // get an iterator in the map where the pieces are the keys and we need to update that one as well
        update_Piece_List(newState->boardByPiece,Replaced,it2->first,it2->second);
        update_Piece_List(newState->boardByPiece,it2->second,it->first,it->second);
        // white move
         if(isupper(it2->second.at(0))){
            // white piece so update that list
            auto wp = newState->whitePieces.find(CurrentPosition);
            // since the block becomes empty we remove it from the list
            newState->whitePieces.erase(wp);
            // insert the update position in the white piece list
            newState->whitePieces.insert({moveTo,it2->second});
            // since we moved to a position that was occupied by a black piece , we need to remove it from the list
            auto bp = newState->blackPieces.find(moveTo);
            newState->blackPieces.erase(bp);
        }
        else{
            auto bp = newState->blackPieces.find(CurrentPosition);
            // since the block becomes empty we remove it from the list
            newState->blackPieces.erase(bp);
            // insert the update position in the white piece list
            newState->blackPieces.insert({moveTo,it2->second});
             // since we moved to a position that was occupied by a white piece , we need to remove it from the list
            auto wp = newState->whitePieces.find(moveTo);
            newState->whitePieces.erase(wp);
        }
    }
    // check there are pieces in the river
    if(PiecesInRiver.size()!=0){
        //loop trough the pices in the river
        for(int i=0;i<PiecesInRiver.size();i++){
            auto Piece = PiecesInRiver[i];
            // get the position of the piece in the previous state before move was made and after move was made in new state
            auto s1 = state->boardByPosition.find(Piece.first);
            auto s2 = newState->boardByPosition.find(Piece.first);
            //pieces need to drown since they are in the same position from state to state
            if(s1->second == s2->second){
                newState->boardByPosition.erase(newState->boardByPosition.find(Piece.first));
                newState->boardByPosition.insert({Piece.first,"Empty"});
                if(state->whitesTurn){
                    auto wp = newState->whitePieces.find(Piece.first);
                    newState->whitePieces.erase(wp);
                }
                else{
                    auto bp = newState->blackPieces.find(Piece.first);
                    newState->blackPieces.erase(bp);
                }
                update_Piece_List(newState->boardByPiece,Piece.second,Piece.first,"Empty");
            }
            else{
                // check if the piece moved in the river and if so, remove it
                if(moveTo.at(1) == CurrentPosition.at(1)){
                    newState->boardByPosition.erase(newState->boardByPosition.find(Piece.first));
                    newState->boardByPosition.insert({Piece.first,"Empty"});
                    if(state->whitesTurn){
                        auto wp = newState->whitePieces.find(moveTo);
                        newState->whitePieces.erase(wp);
                    }
                    else{
                        auto bp = newState->blackPieces.find(moveTo);
                        newState->blackPieces.erase(bp);
                    }
                    update_Piece_List(newState->boardByPiece,Piece.second,Piece.first,"Empty");
                }
            }
        }
    }
    return newState;
}
// method to convert a state to a string
string stateToString(State * s){
    string answer = "";
    vector<string> positions  = {"a7","b7","c7","d7","e7","f7","g7","a6","b6","c6","d6","e6","f6","g6","a5","b5","c5","d5","e5","f5","g5","a4","b4","c4","d4","e4","f4","g4","a3","b3","c3","d3","e3","f3","g3","a2","b2","c2","d2","e2","f2","g2","a1","b1","c1","d1","e1","f1","g1"};
    int len = positions.size();
    for(int i =0;i< len;i++){
        // find each position in the map
        auto it = s->boardByPosition.find(positions[i]);
        // check if the block is empty
        if(it->second == "Empty"){
            int l = answer.length();
            // if there's nothing in the string intially, add one blank space
            if(l==0){
                answer += to_string(1);
            }
            else{
                // if the string has other characters, get the last one and check if it is a digit
                char c =  answer.at(l-1);
                if(isdigit(c)){
                    // if it is a digit, increment it since we have two consecutive empty blocks now and add it to the string
                    string p = "";
                    p  +=c;
                    answer.pop_back();
                    answer+= to_string(stoi(p)+1);
                }
                // if it just a piece as the last character , then add one empty block
                else{
                    answer+=to_string(1);
                }
            }
        }
        // if it's not empty, then just add the Piece to the string
        else{
            answer+= it->second;
        }
        // if it is a g, we move onto the next rank so add /
        if(it->first.at(0)=='g'){
            if(i==len -1){
                continue;
            }
            answer+="/";
        }
    }
    // cout<<answer<<endl;
    string player;
    if(s->whitesTurn){
        player = "w";
    }
    else{
        player = "b";
    }
    answer+= " "+player+" "+to_string(s->moveNumber);
    return answer;
}
// method to check if a game is over or not given a state
string isGameOver(State * s){
    // find white and black lion
    auto WhiteLion = s->boardByPiece.find("L");
    auto BlackLion = s->boardByPiece.find("l");
    // if both are available we can continue
    if(WhiteLion!=s->boardByPiece.end() && BlackLion!=s->boardByPiece.end()){
        return "Continue";
    }
    // if only white lion is available
    if(WhiteLion!=s->boardByPiece.end() && BlackLion==s->boardByPiece.end()){
        return "White wins";
    }
    if(WhiteLion==s->boardByPiece.end() && BlackLion!=s->boardByPiece.end()){
        return "Black wins";
    }
}
// Method that will get all the moves for each piece, and merge them into one vector
vector<string> AllMoves(State * s){
    vector<string> everyPossibleMove;
    vector<string> elephant = ElephantMoves(s);
    vector<string> zebra = ZebraMoves(s);
    vector<string> pawn = PawnMoves(s);
    vector<string> lion = LionMoves(s);
    everyPossibleMove.insert(everyPossibleMove.begin(),lion.begin(),lion.end());
    everyPossibleMove.insert(everyPossibleMove.end(),elephant.begin(),elephant.end());
    everyPossibleMove.insert(everyPossibleMove.end(),zebra.begin(),zebra.end());
    everyPossibleMove.insert(everyPossibleMove.end(),pawn.begin(),pawn.end());
    return everyPossibleMove;
}

// Because we will not be able to search to the end of the game (the search tree is too large), we
// must use an evaluation function. This will allows us to search N moves into the future and
// evaluate the resulting position. An evaluation function is simply a function that accepts as input
// a board position, and returns a value that measures how good the position is for whoever it is to
// play. Importantly, the evaluation function is static — it does not consider moves into the future.
// There a 4 Cases:
// 1. If the board contains only a black and white lion and no other pieces, then it is a draw
// and the raw score is 0.
// 2. Otherwise, if the black lion is missing, then white has won and the raw score is 10000.
// 3. Otherwise, if the white lion is missing, then black has won and the raw score is -10000.
// 4. Otherwise, compute the value of the white pieces on the board.
// Then, do the same for the black pieces. In both cases, exclude the
// lions. The raw score is then the total value of white pieces minus the total value of black
// pieces.
// Piece Scores are as follows : Pawn : 100 , Elephant : 200 , Zebra : 300
// If it is whitesTurn to play, multiply raw score by 1 and if it blacks turn, multiply by -1
int Evaluation(State * state){
    int rawScore;
    auto blackLion = state->boardByPiece.find("l");
    auto whiteLion = state->boardByPiece.find("L");
    // Checking Case 1: 
    if(state->blackPieces.size() == 1 && state->whitePieces.size()==1){
        if(whiteLion!=state->boardByPiece.end() && blackLion!=state->boardByPiece.end()){
           rawScore = 0;
           return rawScore;
        }
    }
    // Checking Case 2: 
    if(whiteLion!=state->boardByPiece.end() && blackLion==state->boardByPiece.end()){
       rawScore = 10000;
    }
    // Checking Case 3: 
    else if(whiteLion==state->boardByPiece.end() && blackLion!=state->boardByPiece.end()){
        rawScore = -10000;
    }
    // Case 4:
    else{
        // loop through white pieces and add relative piece value to white score
        int whiteScore = 0;
        for(auto it = state->whitePieces.begin();it!= state->whitePieces.end();it++){
            if(it->second != "L"){
                if(it->second == "Z"){
                    whiteScore+= 300;
                }
                else if(it->second == "E"){
                    whiteScore+= 200;
                }
                else{
                    whiteScore += 100;
                }
            }
        }
        int blackScore = 0;
        for(auto it = state->blackPieces.begin();it!= state->blackPieces.end();it++){
            if(it->second != "l"){
                if(it->second == "z"){
                    blackScore+= 300;
                }
                else if(it->second == "e"){
                    blackScore+= 200;
                }
                else{
                    blackScore += 100;
                }
            }
        }
        rawScore = whiteScore - blackScore;
    }

    if(!state->whitesTurn){
        rawScore *= -1;
    }

    return rawScore;

}
int MiniMax(State* state, int depth){
    string outcome = isGameOver(state);
    if(outcome != "Continue" || depth <=0){
        return Evaluation(state);
    }
    int value = -100000000;
    vector<string> moves = AllMoves(state);
    for(string move : moves){
        State * nextState = makeMove(state,move);
        int eval = -1 * MiniMax(nextState,depth -1);
        // cout<<eval<<endl;
        value = max(value,eval);
    }
    return value;
}
int main(){
    vector<string> inputs;
    vector<State *>states;
    int n;
    cin>>n;
    cin.ignore();
    for(int i = 0; i < n;i++){
        string input;
        getline(cin,input);
        inputs.push_back(input);
    }
    setStates(inputs,states);
    int len = states.size();
    for(int i=0; i< len;i++){
        State * s  = states[i];
        int val = MiniMax(s,2);
        cout<<val<<endl;
    }


}
void PrintMoves(vector<string>&moves,string& StartPos, bool& moreThanOne){
    int length = moves.size();
    if(length == 0){
        // cout<<"\n";
        return;
    }
    sort(moves.begin(),moves.end());
    for(int i = 0; i < length;i++){
        if(i==length-1){
            string ans = StartPos+moves[i];
            // cout<<StartPos<<moves[i];
            moves[i] = ans;
        }
        else{
            string ans = StartPos+moves[i];
            // cout<<StartPos<<moves[i]<<" ";
            moves[i] = ans;
        }

    }
    if(!moreThanOne){
        // cout<<"\n";
    }
    else{
        // cout<<" ";
    }

}

void PrintMoves(vector<string>&moves, string& StartPos){
    int length = moves.size();
    if(length == 0){
        return;
    }
    sort(moves.begin(),moves.end());
    for(int i = 0; i < length;i++){
        if(i==length-1){
            string ans = StartPos+moves[i];
            // cout<<StartPos<<moves[i];
            moves[i] = ans;
        }
        else{
            string ans = StartPos+moves[i];
            // cout<<StartPos<<moves[i]<<" ";
            moves[i] = ans;
        }

    }
    // cout<<"\n";
}

void PrintPositions(vector<State *>&states){

    vector<string>outputs = {"white pawn: ","black pawn: ","white superpawn: ","black superpawn: ","white giraffe: ","black giraffe: ","white monkey: ","black monkey: ","white elephant: ","black elephant: ","white lion: ","black lion: ","white crocodile: ","black crocodile: ","white zebra: ","black zebra: ","side to play: "};
    vector<string>outputs2 = {"white pawn:","black pawn:","white superpawn:","black superpawn:","white giraffe:","black giraffe:","white monkey:","black monkey:","white elephant:","black elephant:","white lion:","black lion:","white crocodile:","black crocodile:","white zebra:","black zebra:","side to play:"};
    vector<string>values = {"P","p","S","s","G","g","M","m","E","e","L","l","C","c","Z","z"};
    int length = outputs.size();
    // Loop through every state first
    for(int j=0;j<states.size();j++){
        State* s = states[j];
        // For each state we need to print the values in outputs as well as the corresponding values in the board
        // i.e position of the piece on the board
        for(int i=0;i<length;i++){
            // if we are at the last element, print the colour whose move it is
            if(i == length-1){
                string turn = "black";
                if(s->whitesTurn){
                    turn = "white";
                }
                if(j==states.size()-1){
                    cout<<outputs[i]<<turn;
                }
                else{
                    cout<<outputs[i]<<turn<<endl;
                }

            }
            else{
                // get all the positions which corresponds to a certain piece
                auto it =  s->boardByPiece.equal_range(values[i]);

                vector<string> answers;
                // if the piece is not on the board
                if(it.first == it.second){
                    cout<<outputs2[i]<<endl;
                    continue;
                }
                // print the name of the piece
                cout<<outputs[i];
                for(auto iter = it.first;iter!=it.second;iter++){
                    //loop through the it and add them to answers
                    answers.push_back(iter->second);
                }
                // sort the vector so they are in alphabetical order
                sort(answers.begin(),answers.end());

                //loop through the vector and print out the positions
                for(int k=0;k<answers.size();k++){
                    //formatting the spaces when printing below
                    if(k == answers.size()-1){
                        cout<<answers[k];
                    }
                    else{
                        cout<<answers[k]<<" ";
                    }

                }
                cout<<"\n";
            }

        }
        cout<<"\n";
    }
}