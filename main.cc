/*
    Class: CSE340 
    Professor: James Gordon
    Author: Antonio Gil
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <queue>

using namespace std;

/*Global Vectors*/
vector<string> LHS;
vector<vector<string>> RHS;

// read grammar
void ReadGrammar()
{
    LexicalAnalyzer lexical;
    Token token;

    while(true) {
        bool valid = false;
        token = lexical.GetToken();
        //if # (END) is reached in the input, end parsing
        if(token.token_type == HASH) {
            break;
        }
        //Push non-terminal symbols to LHS vector
        else if(token.token_type == ID && lexical.peek(1).token_type == ARROW) {
            LHS.push_back(token.lexeme);
            valid = true;
        }
        //Empty Rule
        else if(token.token_type == ARROW && lexical.peek(1).token_type == STAR) {
            RHS.push_back({"#"});
            valid = true;
        }
        //Push rule (right-hand side) to RHS vector
        else if(token.token_type == ID && lexical.peek(1).token_type != ARROW) {
            //Initiate tmpVector, tmpToken, and push the first symbol in the rule
            vector<string> tmpVector;
            tmpVector.push_back(token.lexeme);
            Token tmpToken = lexical.GetToken();
            //Iterate through the rule until an ending star is found
            while(tmpToken.token_type != STAR) {
                if(tmpToken.token_type == ID){
                    //Push each symbol to the tmpVector
                    tmpVector.push_back(tmpToken.lexeme);
                } 
                tmpToken = lexical.GetToken();
            }
            //Push the tmpVector (rule) to the RHS vector
            RHS.push_back(tmpVector);
            valid = true;
        }
        //If current token is an ARROW with an ID following, continue parsing
        else if(token.token_type == ARROW && lexical.peek(1).token_type == ID) {
            valid = true;
        }
        //If current token is a STAR with an ID or HASH following, continue parsing
        else if(token.token_type == STAR && lexical.peek(1).token_type == ID || lexical.peek(1).token_type == HASH) {
            valid = true;
        }
        //Account for empty space in the input
        else if(token.lexeme == "") {
            valid = true;
        }
        //Check if the current syntax is valid
        if(!valid) {
            cout << "Syntax Error!";
            break;
        }
    }
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    //Ordered Terminal Symbols
        vector<string> OrderedTerminal;
        //Iterate over the RHS vectors
        for(const auto &currentVector : RHS) {
            //Iterate over the elements in each RHS vector
            for(const auto &element : currentVector) {
                //Check each vector and add any terminal symbols not already included (excluding #)
                if(element != "#" && find(LHS.begin(), LHS.end(), element) == LHS.end() && find(OrderedTerminal.begin(), OrderedTerminal.end(), element) == OrderedTerminal.end()) {
                    OrderedTerminal.push_back(element);
                }
            }
        }
        //Printed Ordered Terminal Vector
        for(const auto &element : OrderedTerminal) {
            cout << element << " ";
        }
    //Ordered Non-Terminal Symbols
        vector<string> OrderedNonTerminal;
        //Iterate over the LHS vector
        for(int i = 0; i < LHS.size(); i++) {
            //Add each element to the ordered non terminal vector if not already included
            if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), LHS[i]) == OrderedNonTerminal.end()) {
                OrderedNonTerminal.push_back(LHS[i]);
            }
            //Check each corresponding rule for non terminal symbols
            if(i < RHS.size()) {
                for(const auto &element : RHS[i]) {
                    //Any non terminal symbols not already in the ordered non terminal vector are added
                    if(find(LHS.begin(), LHS.end(), element) != LHS.end() && find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), element) == OrderedNonTerminal.end()) {
                        OrderedNonTerminal.push_back(element);
                    }
                }
            }
        }
        //Printed Ordered Non-Terminal Vector
        for(const auto &element : OrderedNonTerminal) {
            cout << element << " ";
        }
}

// Task 2
void RemoveUselessSymbols()
{
    //Ordered Terminal Symbols
        vector<string> OrderedTerminal;
        //Iterate over the RHS vectors
        for(const auto &currentVector : RHS) {
            //Iterate over the elements in each RHS vector
            for(const auto &element : currentVector) {
                //Check each vector and add any terminal symbols not already included (excluding #)
                if(find(LHS.begin(), LHS.end(), element) == LHS.end() && find(OrderedTerminal.begin(), OrderedTerminal.end(), element) == OrderedTerminal.end()) {
                    OrderedTerminal.push_back(element);
                }
            }
        }
    //Ordered Non-Terminal Symbols
        vector<string> OrderedNonTerminal;
        //Iterate over the LHS vector
        for(int i = 0; i < LHS.size(); i++) {
            //Add each element to the ordered non terminal vector if not already included
            if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), LHS[i]) == OrderedNonTerminal.end()) {
                OrderedNonTerminal.push_back(LHS[i]);
            }
            //Check each corresponding rule for non terminal symbols
            if(i < RHS.size()) {
                for(const auto &element : RHS[i]) {
                    //Any non terminal symbols not already in the ordered non terminal vector are added
                    if(find(LHS.begin(), LHS.end(), element) != LHS.end() && find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), element) == OrderedNonTerminal.end()) {
                        OrderedNonTerminal.push_back(element);
                    }
                }
            }
        }
    //Map of original grammar <LHS : RHS> & initialization
        multimap<string, vector<string>> grammar;
        for(int i = 0; i < LHS.size(); ++i) {
            grammar.insert({LHS[i], RHS[i]});
        }
    //Map of useful rules <LHS : RHS>
        multimap<string, vector<string>> useful;
    //Rules with generating Symbols
        bool change = true;
        while(true) {
            change = false;
            //Iterate over every pair in the grammar map
            for(const auto& pair : grammar) {
                bool genRule = true;
                bool found = false;
                //Check if the rule is already in the useful map
                for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                    if(itr -> first == pair.first && itr -> second == pair.second) {
                        found = true;
                        break;
                    }
                }
                //If rule is found, continue to next rule
                if(found) {
                    continue;
                }
                //Iterate over the symbols in each rule
                for(const auto& value : pair.second) {
                    //Search the current symbol in the rule for a match in generating symbols
                    if(find(OrderedTerminal.begin(), OrderedTerminal.end(), value) == OrderedTerminal.end()) {
                        genRule = false;
                        break;
                    }
                }
                //If rule found to be generating add it to the useful map and push the LHS to generating vector
                if(genRule == true) {
                    useful.insert({pair.first, pair.second});
                    OrderedTerminal.push_back(pair.first);
                    change = true;
                }
            }
            //Exit while loop if no changes made
            if(!change) {
                break;
            }
        }
    //Reachable Symbols
        //If starting symbol not in grammar, the grammar is empty, otherwise parse grammar for reachable symbols
        string startSymbol = OrderedNonTerminal[0];
        multimap<string, vector<string>> newGrammar;
        multimap<string, vector<string>> tmpGrammar;
        bool start = false;
        for(auto itr = useful.begin(); itr != useful.end(); itr++) {
            if(itr -> first == startSymbol) {
                start = true;
                break;
            }
        }
        if(!start) {
            useful.clear();
        } else {
            //Start with starting symbol and all of its RHS nonterminals
            for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                if(itr -> first == OrderedNonTerminal[0]) {
                    newGrammar.insert({itr -> first, itr -> second});
                    //Any non terminals in the RHS of the starting symbol are reachable
                    for(const auto& value : itr -> second) {
                        //Non Terminal Found
                        if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), value) != OrderedNonTerminal.end()) {
                            //All of its rules are reachable
                            for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                                if(itr -> first == value) {
                                    newGrammar.insert({itr -> first, itr -> second});
                                }
                            }
                        }
                    }
                }
            }
            //Iterate through the current grammar
            for(auto itr = newGrammar.begin(); itr != newGrammar.end(); itr++) {
                //Iterate through the RHS of each rule and look for non terminals
                for(const auto& value : itr -> second) {
                    if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), value) != OrderedNonTerminal.end()) {
                        //All of its rules are reachable
                        for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                            if(itr -> first == value) {
                                tmpGrammar.insert({itr -> first, itr -> second});
                            }
                        }
                    }
                }
            }
            //Any rules in the tmpGrammar not in the newGrammar should be added
            for(const auto& pair : tmpGrammar) {
                const string& symbol = pair.first;
                const vector<string>& rule = pair.second;
                auto range = newGrammar.equal_range(symbol);
                bool keyExists = (range.first != range.second);
                //If symbol is not found, insert the whole rule
                if(!keyExists) {
                    newGrammar.insert({symbol, rule});
                //If the symbol is found, check if the rule is different
                } else {
                    bool differentValue = true;
                    for(auto itr = range.first; itr != range.second; itr++) {
                        if(itr -> second == rule) {
                            differentValue = false;
                            break;
                        }
                    }
                    //If the rule is different, insert it
                    if(differentValue) {
                        newGrammar.insert({symbol, rule});
                    }
                }
            }
        } 
    //Print newGrammar in the relative order that the rules appear in the original grammar
        for(int i = 0; i < LHS.size(); i++) {
            string symbol = LHS[i];
            vector<string> rule = RHS[i];
            for(auto itr = newGrammar.begin(); itr != newGrammar.end(); itr++) {
                if(itr -> first == symbol && itr -> second == rule) {
                    cout << itr -> first << " -> ";
                    for(const auto& value : itr -> second) {
                        cout << value << " ";
                    }
                    cout << endl;
                }
            }
        }
}

// Task 3
void CalculateFirstSets()
{
    //Ordered Terminal Symbols
        vector<string> OrderedTerminal;
        //Iterate over the RHS vectors
        for(const auto &currentVector : RHS) {
            //Iterate over the elements in each RHS vector
            for(const auto &element : currentVector) {
                //Check each vector and add any terminal symbols not already included (excluding #)
                if(find(LHS.begin(), LHS.end(), element) == LHS.end() && find(OrderedTerminal.begin(), OrderedTerminal.end(), element) == OrderedTerminal.end()) {
                    OrderedTerminal.push_back(element);
                }
            }
        }
    //Ordered Non-Terminal Symbols
        vector<string> OrderedNonTerminal;
        //Iterate over the LHS vector
        for(int i = 0; i < LHS.size(); i++) {
            //Add each element to the ordered non terminal vector if not already included
            if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), LHS[i]) == OrderedNonTerminal.end()) {
                OrderedNonTerminal.push_back(LHS[i]);
            }
            //Check each corresponding rule for non terminal symbols
            if(i < RHS.size()) {
                for(const auto &element : RHS[i]) {
                    //Any non terminal symbols not already in the ordered non terminal vector are added
                    if(find(LHS.begin(), LHS.end(), element) != LHS.end() && find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), element) == OrderedNonTerminal.end()) {
                        OrderedNonTerminal.push_back(element);
                    }
                }
            }
        }
    //Map of original grammar
    multimap<string, vector<string>> grammar;
    for(int i = 0; i < LHS.size(); ++i) {
        grammar.insert({LHS[i], RHS[i]});
    }
    //Map of Symbols and their first set
    map<string, vector<string>> firstSet;

    bool change = true;
    while(true) {
        change = false;
        //Parse through the grammar
        for(auto itr = grammar.begin(); itr != grammar.end(); itr++) {
            //Temporary vector to hold the current rule
            vector<string> tmpRule;
            for(const auto& symbol : itr -> second) {
                tmpRule.push_back(symbol);
            }
            //Temporary vector to hold current first set
            vector<string> tmpSet = firstSet[itr -> first];

            //If first symbol is a terminal and not already in the set, add that symbol
            if(find(OrderedTerminal.begin(), OrderedTerminal.end(), tmpRule[0]) != OrderedTerminal.end()) {
                if(find(tmpSet.begin(), tmpSet.end(), tmpRule[0]) == tmpSet.end()) {
                    tmpSet.push_back(tmpRule[0]);
                    firstSet[itr -> first] = tmpSet;
                    change = true;
                }
            //If first symbol is a non terminal, iterate through the rule
            } else {
                //Iterate through the rule
                for(int i = 0; i < tmpRule.size(); i++) {
                    //If a terminal is found and not already in the set, add that symbol, break regadless
                    if(find(OrderedTerminal.begin(), OrderedTerminal.end(), tmpRule[i]) != OrderedTerminal.end()) {
                        if(find(tmpSet.begin(), tmpSet.end(), tmpRule[i]) == tmpSet.end()) {
                            tmpSet.push_back(tmpRule[i]);
                            firstSet[itr -> first] = tmpSet;
                            change = true;
                        }
                        break;
                    } else {
                        //Check if the first set of the non terminal has the # symbol
                        bool empty = false;
                        for(const auto& element : firstSet[tmpRule[i]]) {
                            if(element == "#") {
                                empty = true;
                            }
                        }
                        //If it doesnt, add the first set and break
                        if(!empty) {
                            for(const auto& element : firstSet[tmpRule[i]]) {
                                //Only push new terminals to the tmpSet
                                if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                    tmpSet.push_back(element);
                                    change = true;
                                }
                            }
                            firstSet[itr -> first] = tmpSet; 
                            break;
                        //If it does, add the first set and do not break
                        } else {
                            //Check if this is an ending non terminal, if it is add all symbols
                            if(i == tmpRule.size() - 1) {
                                for(const auto& element : firstSet[tmpRule[i]]) {
                                    //Only push new terminals to the tmpSet
                                    if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                        tmpSet.push_back(element);
                                        change = true;
                                    }
                                }
                                firstSet[itr -> first] = tmpSet; 
                            //If it is not an ending terminal, add all symbols except #
                            } else {
                                for(const auto& element : firstSet[tmpRule[i]]) {
                                    //Only push new terminals to the tmpSet
                                    if(element != "#" && find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                        tmpSet.push_back(element);
                                        change = true;
                                    }
                                }
                                firstSet[itr -> first] = tmpSet;  
                            }
                        }
                    }
                }
            }
        }
        //Exit while loop if no changes are made
        if(!change) {
            break;
        }
    }
    //Print First Sets in relative order
    for(int i = 0; i < OrderedNonTerminal.size(); i++) {
        string symbol = OrderedNonTerminal[i];
        for(auto itr = firstSet.begin(); itr != firstSet.end(); itr++) {
            if(itr -> first == symbol) {
            cout << "FIRST(" << itr -> first << ") = { ";
            //Maintain Size
            int size = itr -> second.size();
            int counter = 0;
            //If # is in the first set, print it first
            for(const auto& element : itr -> second) {
                if(element == "#") {
                    cout << "#, ";
                    counter++;
                }
            }
            //Print remaining symbols in relative order
            for(int j = 0; j < OrderedTerminal.size(); j++) {
                for(const auto& element : itr -> second) {
                    if(element == OrderedTerminal[j] && element != "#") {
                        counter++;
                        if(counter < size) {
                            cout << element << ", ";
                        } else {
                            cout << element << " ";
                        }
                    }
                }
            }
            cout << "}" << endl;
            }
        }
    }
}

// Task 4
void CalculateFollowSets()
{
    //Ordered Terminal Symbols
        vector<string> OrderedTerminal;
        //Iterate over the RHS vectors
        for(const auto &currentVector : RHS) {
            //Iterate over the elements in each RHS vector
            for(const auto &element : currentVector) {
                //Check each vector and add any terminal symbols not already included (excluding #)
                if(find(LHS.begin(), LHS.end(), element) == LHS.end() && find(OrderedTerminal.begin(), OrderedTerminal.end(), element) == OrderedTerminal.end()) {
                    OrderedTerminal.push_back(element);
                }
            }
        }
    //Ordered Non-Terminal Symbols
        vector<string> OrderedNonTerminal;
        //Iterate over the LHS vector
        for(int i = 0; i < LHS.size(); i++) {
            //Add each element to the ordered non terminal vector if not already included
            if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), LHS[i]) == OrderedNonTerminal.end()) {
                OrderedNonTerminal.push_back(LHS[i]);
            }
            //Check each corresponding rule for non terminal symbols
            if(i < RHS.size()) {
                for(const auto &element : RHS[i]) {
                    //Any non terminal symbols not already in the ordered non terminal vector are added
                    if(find(LHS.begin(), LHS.end(), element) != LHS.end() && find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), element) == OrderedNonTerminal.end()) {
                        OrderedNonTerminal.push_back(element);
                    }
                }
            }
        }
//Compute First Sets to be used in Follow Sets--------------
    //Map of original grammar
    multimap<string, vector<string>> grammar;
    for(int i = 0; i < LHS.size(); ++i) {
        grammar.insert({LHS[i], RHS[i]});
    }
    //Map of Symbols and their first set
    map<string, vector<string>> firstSet;
    bool change = true;
    while(true) {
        change = false;
        //Parse through the grammar
        for(auto itr = grammar.begin(); itr != grammar.end(); itr++) {
            //Temporary vector to hold the current rule
            vector<string> tmpRule;
            for(const auto& symbol : itr -> second) {
                tmpRule.push_back(symbol);
            }
            //Temporary vector to hold current first set
            vector<string> tmpSet = firstSet[itr -> first];
            //If first symbol is a terminal and not already in the set, add that symbol
            if(find(OrderedTerminal.begin(), OrderedTerminal.end(), tmpRule[0]) != OrderedTerminal.end()) {
                if(find(tmpSet.begin(), tmpSet.end(), tmpRule[0]) == tmpSet.end()) {
                    tmpSet.push_back(tmpRule[0]);
                    firstSet[itr -> first] = tmpSet;
                    change = true;
                }
            //If first symbol is a non terminal, iterate through the rule
            } else {
                //Iterate through the rule
                for(int i = 0; i < tmpRule.size(); i++) {
                    //If a terminal is found and not already in the set, add that symbol, break regadless
                    if(find(OrderedTerminal.begin(), OrderedTerminal.end(), tmpRule[i]) != OrderedTerminal.end()) {
                        if(find(tmpSet.begin(), tmpSet.end(), tmpRule[i]) == tmpSet.end()) {
                            tmpSet.push_back(tmpRule[i]);
                            firstSet[itr -> first] = tmpSet;
                            change = true;
                        }
                        break;
                    } else {
                        //Check if the first set of the non terminal has the # symbol
                        bool empty = false;
                        for(const auto& element : firstSet[tmpRule[i]]) {
                            if(element == "#") {
                                empty = true;
                            }
                        }
                        //If it doesnt, add the first set and break
                        if(!empty) {
                            for(const auto& element : firstSet[tmpRule[i]]) {
                                //Only push new terminals to the tmpSet
                                if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                    tmpSet.push_back(element);
                                    change = true;
                                }
                            }
                            firstSet[itr -> first] = tmpSet; 
                            break;
                        //If it does, add the first set and do not break
                        } else {
                            //Check if this is an ending non terminal, if it is add all symbols
                            if(i == tmpRule.size() - 1) {
                                for(const auto& element : firstSet[tmpRule[i]]) {
                                    //Only push new terminals to the tmpSet
                                    if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                        tmpSet.push_back(element);
                                        change = true;
                                    }
                                }
                                firstSet[itr -> first] = tmpSet; 
                            //If it is not an ending terminal, add all symbols except #
                            } else {
                                for(const auto& element : firstSet[tmpRule[i]]) {
                                    //Only push new terminals to the tmpSet
                                    if(element != "#" && find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                        tmpSet.push_back(element);
                                        change = true;
                                    }
                                }
                                firstSet[itr -> first] = tmpSet;  
                            }
                        }
                    }
                }
            }
        }
        //Exit while loop if no changes are made
        if(!change) {
            break;
        }
    }
//Compute Follow Sets------------------------------------
    map<string, vector<string>> followSet;
    //Start By Pushing $ to the starting symbol follow set, and initialize all remaining follow sets
    followSet.insert({OrderedNonTerminal[0], {"$"}});
    for(int i = 0; i < OrderedNonTerminal.size(); i++) {
        followSet.insert({OrderedNonTerminal[i], {}});
    }
    bool update = true;
    while(true) {
        update = false;
        //For each LHS Symbol
        for(int i = 0; i < OrderedNonTerminal.size(); i++) {
            //Parse the grammar
            for(auto itr = grammar.begin(); itr != grammar.end(); itr++) {
                //Temporary vector to hold the current rule
                vector<string> tmpRule;
                for(const auto& symbol : itr -> second) {
                    tmpRule.push_back(symbol);
                }
                //Temporary vector to hold the current follow set
                vector<string> tmpSet = followSet[OrderedNonTerminal[i]];
                //Iterate through the current rule
                bool encounteredEmpty = false;
                for(int j = 0; j < tmpRule.size(); j++) {
                    int size = tmpRule.size();
                    int nextIndex = j + 1;
                    //LHS symbol is found in the rule
                    if(OrderedNonTerminal[i] == tmpRule[j] || encounteredEmpty) {
                        //If not the rightmost symbol, check the next symbol type
                        if(nextIndex < size) {
                            //if it is a terminal and not already in the set, add that symbol and stop parsing the rule
                            if(find(OrderedTerminal.begin(), OrderedTerminal.end(), tmpRule[nextIndex]) != OrderedTerminal.end()) {
                                if(find(tmpSet.begin(), tmpSet.end(), tmpRule[nextIndex]) == tmpSet.end()) {
                                    tmpSet.push_back(tmpRule[nextIndex]);
                                    followSet[OrderedNonTerminal[i]] = tmpSet;
                                    update = true;
                                }
                                //Check if the LHS Symbol appears again in the rule
                                bool repeat = false;
                                for(int tmp = nextIndex; tmp < tmpRule.size(); tmp++) {
                                    if(OrderedNonTerminal[i] == tmpRule[tmp]) {
                                        repeat = true;
                                    }
                                }
                                if(!repeat) {
                                    break;
                                }
                            //if it is a non terminal, add the first set of that non terminal
                            } else {
                                //Check if the first set of the non terminal has the # symbol
                                bool empty = false;
                                if(find(firstSet[tmpRule[nextIndex]].begin(), firstSet[tmpRule[nextIndex]].end(), "#") != firstSet[tmpRule[nextIndex]].end()) {
                                    empty = true;
                                }
                                //If it doesnt, add the first set and break
                                if(!empty) {
                                    for(const auto& element : firstSet[tmpRule[nextIndex]]) {
                                        //Only push new terminals to the tmpSet
                                        if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                            tmpSet.push_back(element);
                                            update = true;
                                        }
                                    }
                                    followSet[OrderedNonTerminal[i]] = tmpSet;
                                    //Check if the LHS Symbol appears again in the rule
                                    bool repeat = false;
                                    for(int tmp = nextIndex; tmp < tmpRule.size(); tmp++) {
                                        if(OrderedNonTerminal[i] == tmpRule[tmp]) {
                                            repeat = true;
                                        }
                                    }
                                    if(!repeat) {
                                        break;
                                    }
                                //If it does, add the first set except # and mark the rule for continued parsing
                                } else {
                                    encounteredEmpty = true;
                                    for(const auto& element : firstSet[tmpRule[nextIndex]]) {
                                        //Only push new terminals to the tmpSet
                                        if(element != "#" && find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                            tmpSet.push_back(element);
                                            update = true;
                                        }
                                    }
                                    followSet[OrderedNonTerminal[i]] = tmpSet;
                                }
                            }
                        //If rightmost symbol, add follow set of current LHS symbol
                        } else {
                            for(const auto& element : followSet[itr -> first]) {
                                if(find(tmpSet.begin(), tmpSet.end(), element) == tmpSet.end()) {
                                    tmpSet.push_back(element);
                                    update = true;
                                }    
                            }
                            followSet[OrderedNonTerminal[i]] = tmpSet;
                        }
                    }
                }
            }
        }
        //Exit while loop if no changes are made
        if(!update) {
            break;
        }
    }
    //Print Follow Sets in relative order
    for(int i = 0; i < OrderedNonTerminal.size(); i++) {
        string symbol = OrderedNonTerminal[i];
        for(auto itr = followSet.begin(); itr != followSet.end(); itr++) {
            if(itr -> first == symbol) {
            cout << "FOLLOW(" << itr -> first << ") = { ";
            //Maintain Size
            int size = itr -> second.size();
            int counter = 0;
            //If $ is in the first set, print it first
            for(const auto& element : itr -> second) {
                if(element == "$" && size == 1) {
                    cout << "$ ";
                    counter++;
                } else if (element == "$" && size != 1) {
                    cout << "$, ";
                    counter++;
                }
            }
            //Print remaining symbols in relative order
            for(int j = 0; j < OrderedTerminal.size(); j++) {
                for(const auto& element : itr -> second) {
                    if(element == OrderedTerminal[j] && element != "$") {
                        counter++;
                        if(counter < size) {
                            cout << element << ", ";
                        } else {
                            cout << element << " ";
                        }
                    }
                }
            }
            cout << "}" << endl;
            }
        }
    }
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    //Ordered Terminal Symbols
        vector<string> OrderedTerminal;
        //Iterate over the RHS vectors
        for(const auto &currentVector : RHS) {
            //Iterate over the elements in each RHS vector
            for(const auto &element : currentVector) {
                //Check each vector and add any terminal symbols not already included (excluding #)
                if(find(LHS.begin(), LHS.end(), element) == LHS.end() && find(OrderedTerminal.begin(), OrderedTerminal.end(), element) == OrderedTerminal.end()) {
                    OrderedTerminal.push_back(element);
                }
            }
        }
    //Ordered Non-Terminal Symbols
        vector<string> OrderedNonTerminal;
        //Iterate over the LHS vector
        for(int i = 0; i < LHS.size(); i++) {
            //Add each element to the ordered non terminal vector if not already included
            if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), LHS[i]) == OrderedNonTerminal.end()) {
                OrderedNonTerminal.push_back(LHS[i]);
            }
            //Check each corresponding rule for non terminal symbols
            if(i < RHS.size()) {
                for(const auto &element : RHS[i]) {
                    //Any non terminal symbols not already in the ordered non terminal vector are added
                    if(find(LHS.begin(), LHS.end(), element) != LHS.end() && find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), element) == OrderedNonTerminal.end()) {
                        OrderedNonTerminal.push_back(element);
                    }
                }
            }
        }
    //Map of original grammar <LHS : RHS> & initialization
        multimap<string, vector<string>> grammar;
        for(int i = 0; i < LHS.size(); ++i) {
            grammar.insert({LHS[i], RHS[i]});
        }
    //Map of useful rules <LHS : RHS>
        multimap<string, vector<string>> useful;
    //Rules with generating Symbols
        bool change = true;
        while(true) {
            change = false;
            //Iterate over every pair in the grammar map
            for(const auto& pair : grammar) {
                bool genRule = true;
                bool found = false;
                //Check if the rule is already in the useful map
                for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                    if(itr -> first == pair.first && itr -> second == pair.second) {
                        found = true;
                        break;
                    }
                }
                //If rule is found, continue to next rule
                if(found) {
                    continue;
                }
                //Iterate over the symbols in each rule
                for(const auto& value : pair.second) {
                    //Search the current symbol in the rule for a match in generating symbols
                    if(find(OrderedTerminal.begin(), OrderedTerminal.end(), value) == OrderedTerminal.end()) {
                        genRule = false;
                        break;
                    }
                }
                //If rule found to be generating add it to the useful map and push the LHS to generating vector
                if(genRule == true) {
                    useful.insert({pair.first, pair.second});
                    OrderedTerminal.push_back(pair.first);
                    change = true;
                }
            }
            //Exit while loop if no changes made
            if(!change) {
                break;
            }
        }
    //Reachable Symbols
        //If starting symbol not in grammar, the grammar is empty, otherwise parse grammar for reachable symbols
        string startSymbol = OrderedNonTerminal[0];
        multimap<string, vector<string>> newGrammar;
        multimap<string, vector<string>> tmpGrammar;
        bool start = false;
        for(auto itr = useful.begin(); itr != useful.end(); itr++) {
            if(itr -> first == startSymbol) {
                start = true;
                break;
            }
        }
        if(!start) {
            useful.clear();
        } else {
            //Start with starting symbol and all of its RHS nonterminals
            for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                if(itr -> first == OrderedNonTerminal[0]) {
                    newGrammar.insert({itr -> first, itr -> second});
                    //Any non terminals in the RHS of the starting symbol are reachable
                    for(const auto& value : itr -> second) {
                        //Non Terminal Found
                        if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), value) != OrderedNonTerminal.end()) {
                            //All of its rules are reachable
                            for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                                if(itr -> first == value) {
                                    newGrammar.insert({itr -> first, itr -> second});
                                }
                            }
                        }
                    }
                }
            }
            //Iterate through the current grammar
            for(auto itr = newGrammar.begin(); itr != newGrammar.end(); itr++) {
                //Iterate through the RHS of each rule and look for non terminals
                for(const auto& value : itr -> second) {
                    if(find(OrderedNonTerminal.begin(), OrderedNonTerminal.end(), value) != OrderedNonTerminal.end()) {
                        //All of its rules are reachable
                        for(auto itr = useful.begin(); itr != useful.end(); itr++) {
                            if(itr -> first == value) {
                                tmpGrammar.insert({itr -> first, itr -> second});
                            }
                        }
                    }
                }
            }
            //Any rules in the tmpGrammar not in the newGrammar should be added
            for(const auto& pair : tmpGrammar) {
                const string& symbol = pair.first;
                const vector<string>& rule = pair.second;
                auto range = newGrammar.equal_range(symbol);
                bool keyExists = (range.first != range.second);
                //If symbol is not found, insert the whole rule
                if(!keyExists) {
                    newGrammar.insert({symbol, rule});
                //If the symbol is found, check if the rule is different
                } else {
                    bool differentValue = true;
                    for(auto itr = range.first; itr != range.second; itr++) {
                        if(itr -> second == rule) {
                            differentValue = false;
                            break;
                        }
                    }
                    //If the rule is different, insert it
                    if(differentValue) {
                        newGrammar.insert({symbol, rule});
                    }
                }
            }
        }
    //Check if the original grammar is equivalent to the finalGrammar
    if(grammar == useful) {
        //If it is, output -- YES
        cout << "YES";
    } else {
        //If it is not, output -- NO
        cout << "NO";
    }
}
  
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

