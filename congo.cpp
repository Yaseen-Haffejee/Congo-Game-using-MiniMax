#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string> 
#include <unordered_map>
using namespace std;

struct State{
    // State of the game is stored in an unordered_map.
    // We can find the position of any piece by using the .find() method
    unordered_multimap<string,string> boardByPiece;
    unordered_multimap<string,string> boardByPosition;
    unordered_multimap<string,string> whitePieces;
    unordered_multimap<string,string> blackPieces;

    // bool that will represent which players turn it is
    bool whitesTurn = true;

    State(unordered_multimap<string,string>&state,unordered_multimap<string,string>&state2,unordered_multimap<string,string>&w,unordered_multimap<string,string>&b,bool WhitesTurnToPlay){
        boardByPiece = state;
        boardByPosition = state2;
        whitePieces = w;
        blackPieces = b;
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
    State * newState = new State(StatesByPiece,StatesByPosition,wPieces,bPieces,whitePlay);
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
            auto it = s->boardByPosition.find(possible[i]);
            auto hasBlack = s->blackPieces.find(possible[i]);
            if(it->second == "Empty" || hasBlack!=s->blackPieces.end()){
                moves.push_back(it->first);
            }
        }
    }
    else{
        for(int i=0;i<size;i++){
            auto it = s->boardByPosition.find(possible[i]);
            auto hasWhite = s->whitePieces.find(possible[i]);
            if(it->second == "Empty" || hasWhite!=s->whitePieces.end()){
                moves.push_back(it->first);
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
                auto b = state->blackPieces.find("e3");
                if(b!=state->blackPieces.end()){
                    if(b->second == "L"){
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
                
                auto b = state->blackPieces.find("c3");
                if(b!=state->blackPieces.end()){
                    if(b->second == "L"){
                        possibleMoves.push_back("c3");
                    }
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
void ElephantMoves(State * s){
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
        return;
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
        ElephantMoves(initialPositions[i],s,multipleElephants);
    }
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
    
    // PrintPositions(states);
    for(State * state : states){
        ElephantMoves(state);
    }
    
  
}
void PrintMoves(vector<string>&moves,string& StartPos, bool& moreThanOne){
    int length = moves.size();
    if(length == 0){
        return;
    }
    sort(moves.begin(),moves.end());
    for(int i = 0; i < length;i++){
        if(i==length-1){
            cout<<StartPos<<moves[i];
        }
        else{
            cout<<StartPos<<moves[i]<<" ";
        }
       
    }
    if(!moreThanOne){
        cout<<"\n";
    }
    else{
        cout<<" ";
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
            cout<<StartPos<<moves[i];
        }
        else{
            cout<<StartPos<<moves[i]<<" ";
        }
       
    }
    cout<<"\n";
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