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
    unordered_multimap<string,string> board;

    // bool that will represent which players turn it is
    bool whitesTurn = true;

    State(unordered_multimap<string,string>&state, bool WhitesTurnToPlay){
        board = state;
        whitesTurn = WhitesTurnToPlay;
    }

};

void Print(vector<State *>&states){
    
    vector<string>outputs = {"white pawn: ","black pawn: ","white superpawn: ","black superpawn: ","white giraffe: ","black giraffe: ","white monkey: ","black monkey: ","white elephant: ","black elephant: ","white lion: ","black lion: ","white crocodile: ","black crocodile: ","white zebra: ","black zebra: ","side to play: "};
    // vector<string>outputs = {"white pawn:","black pawn:","white superpawn:","black superpawn:","white giraffe:","black giraffe:","white monkey:","black monkey:","white elephant:","black elephant:","white lion:","black lion:","white crocodile:","black crocodile:","white zebra:","black zebra:","side to play:"};
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
                auto it =  s->board.equal_range(values[i]);
                // print the name of the piece
                cout<<outputs[i];
                vector<string> answers;
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
                cout<<endl;
            }
           
        }
        cout<<"\n";
    }
}

void StringToState(string state,vector<State*>&states){
    // unordered multimap so searching is faster and we can have duplicate keys
    unordered_multimap<string,string> States;
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
                        States.insert({"Empty", key});
                    }
                }
                // If it is an actual piece, then the process is the same
                else{
                    string key = alphabets[pos];
                    key+= to_string(count);
                    pos++;
                    string val= "";
                    val+=*it;
                    States.insert({val, key});
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
    State * newState = new State(States,whitePlay);
    states.push_back(newState);
    
   
}

void setStates(vector<string>&inputs,vector<State*>&states){
    vector<string> :: iterator it;
    //loop through all the input strings and create a new state for each of them
    for(it=inputs.begin(); it!=inputs.end();it++){
        StringToState(*it,states);
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
    
    Print(states);
  
}