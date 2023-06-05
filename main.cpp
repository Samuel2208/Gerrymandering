//
//  Project 1 - Gerrymandering
/// @author Victor Samuel Escudero
/// @date January 20, 2023
/// @brief Gerrymandering App for CS-251. This app is able to load data and display it to the screen. 
///        It loads voting data for each state and each district in every state. 
///        We can then search for an specific state and get the statistics of the overall match between political parties.
///        We can also get a plot of the percentage of votes belonging to a political party in each district.

#include <iostream>
#include <vector>
#include <sstream>//to use << , >> operators
#include <string>//to use string functions
#include <fstream>//for files
#include <math.h> //math functions
#include <algorithm>// for algorithms

#include "ourvector.h"

using namespace std;

//struct containing district info extracted from district.txt
struct Districts{

    int districtNum = 0;
    int democraticVotes = 0;
    int republicanVotes = 0;

    int totalVotes = 0;
    int overHalf = 0;
    int wastedDemVotes = 0;
    int wastedRepVotes = 0;
};

struct State{

    string stateName;
    ourvector<Districts> districtVector;
    int eligibleVoters = 0;
    double efficiencyGap = 0.0;

    int totalVotesCast = 0;
    int totalWastedDemVotes = 0;
    int totalWastedRepVotes = 0;

};


//Function prototypes
//functions used in may other functions:
string parseString(string &);
void calculateGerrymandering(Districts &, int &, int &, double &);
void calculateStateInfo(ourvector<State>& , int & );

//search function
bool searchState(ourvector<State>&, const string&,int &);

//load function and their respective extra functions
void extractEligibleVoters(string &, ourvector<State> &, int &, string );
void loadEligibleVotersTypeFile(ifstream &, ourvector<State> &, State&);
void initializeDistrictObject(int &, Districts &, const int &, bool &, ourvector<Districts> &, int &);
void addDistrictInfo(string &, ourvector< Districts> &, int &);
void loadDistrictTypeFyle(ifstream &, ourvector<State> &, State&);
void load(string &,  ourvector<State> &, string &, string &);

//stats function
void stats(ourvector<State>& , int &);

//plot function and their repective extra fuctions
void displayPlot(Districts &);
void plot(ourvector<State>& , int &);


int main() {

    //vector containing all info from each state
    ourvector<State> statesVector;

    string loadedData = "No";
    string state = "N/A";
    string filesToRead;
    string comand;
    bool exit = false;
    int stateIndex;
    string stateName;
    string temp;
    string file1_loaded = "No";
    string file2_loaded = "No";

    cout << "\nWelcome to the Gerrymandering App!" << endl;

    do
    {
        cout << "Data loaded? " << loadedData << endl;
        cout << "State: " << state << endl << endl;
        cout << "Enter command: ";
        cin >> comand;
        cout << endl;
        cout << "-----------------------------" << endl << endl;

        //load data
        if(comand == "load")
        {
            getline(cin, filesToRead);

            if(loadedData == "Yes")
            {
                cout << "\nAlready read data in, exit and start over." << endl << endl;
                continue;
            }
       
            load(filesToRead, statesVector, file1_loaded, file2_loaded);

            if(file1_loaded == "Yes" && file2_loaded == "Yes")
            {
                loadedData = "Yes";
            }

        }

        //search for state, if located it calculate data for to use for next commands
        //if state not found tell the user to try again
        if(comand == "search" && (file1_loaded == "Yes" || file2_loaded == "Yes"))
        {
            getline(cin >> ws, stateName);
            state = stateName;

            if(searchState(statesVector, stateName, stateIndex))
            {
                state = statesVector.at(stateIndex).stateName;
                calculateStateInfo(statesVector, stateIndex);
            }else{
                cout << "State does not exist, search again." << endl << endl;
                state = "N/A";
            }

        }else if(comand == "search" && (file1_loaded == "No" || file2_loaded == "No"))
        {
            cout << "No data loaded, please load data first." << endl << endl;
            getline(cin >>ws, temp);
            continue;
        }

        //display all statistic data if we have a state locate
        //otherwise if we dont have a state searched ask user to search for a state
        if(comand == "stats" && state != "N/A" && loadedData == "Yes")
        {
            stats(statesVector, stateIndex);

        }else if(comand == "stats" && state == "N/A" && (loadedData == "No" || loadedData == "Yes"))
        {
            cout << "No state indicated, please search for state first." << endl << endl;
        }else if(comand == "stats" && loadedData == "No")
        {
            cout << "No data loaded, please load data first." << endl << endl;
        }

        //plot statistical data for every distric in the searched state
        //otherwise ask user to search for a state if we don't have one
        if(comand == "plot" && state != "N/A" && loadedData == "Yes")
        {
            plot(statesVector, stateIndex);

        }else if(comand == "plot" && state == "N/A" && (loadedData == "No" || loadedData == "Yes"))
        {
            cout << "No state indicated, please search for state first." << endl << endl;
        }else if(comand == "plot" && loadedData == "No")
        {
            cout << "No data loaded, please load data first." << endl << endl;
        }

        //exit application
        if(comand == "exit")
        {
            exit  = true;
            statesVector.clear();
        }

    }while(!exit);


    return 0;

}
///end of main---------------------------------------------------------------------------------------------------

/// @brief using district object calculate percentage of votes in an specific district and display plot
/// @param district district object containing all info for that district
void displayPlot(Districts &district)
{
    int demPercent = 0;
    int repPercent = 0;

    //If the total votes of that district is not zero calculate percentage of political party votes
    //and display percentage of votes using D's and R's respectably
    if(district.totalVotes != 0)
    {
        demPercent = floor((district.democraticVotes / (double)district.totalVotes) * 100);
        repPercent = 100 - demPercent;
    }

    //display plot
    if(demPercent != 0 && repPercent != 0)
    {
        //Display D's depending on the percentage of democratic votes in an specific district
        cout << "District: " << district.districtNum << endl;
        int i;
        for(i = 0; i< demPercent; i++)
        {
            cout << "D";
        }
        //Display the rest of the percent as R's 
        for(int j = i; j< 100; j++)
        {
            cout << "R";
        }
    }else if(demPercent == 0 && repPercent != 0)//All Republican votes
    {
        repPercent = 100;
        cout << "District: " << district.districtNum  << endl;
        for(int j = 0; j< repPercent; j++)
        {
            cout << "R";
        }
    }else if(demPercent != 0 && repPercent == 0)//All Democrat votes
    {
        demPercent = 100;
        cout << "District: " << district.districtNum  << endl;
        for(int j = 0; j< demPercent; j++)
        {
            cout << "D";
        }
    }else//Don't display anything if total votes is zero 
    {
        cout << "District: " << district.districtNum;
    }
    
    cout << endl;
    cout << endl;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief using a vector of structs of type State, locate an specific state using index parameter and
///         display a plot of political party percentage of votes in each district
/// @param vec vector of structs of type State containing all info
/// @param index int representing the index of an specific state
void plot(ourvector<State>& vec, int &index)
{
    //pass every sinlge district in a state and display plot
    for(int i = 0; i< vec.at(index).districtVector.size(); i++)
    {
        displayPlot(vec.at(index).districtVector.at(i));
    }
}
//---------------------------------------------------------------------------------------------------------------
/// @brief using districts information for a state it calculates and updates all statistical information 
///         to use and saves it to a district object. It also calculates and updates some of the parameters
///         such as total democratic wasted votes, total republican wasted votes, and the efficiency gap
/// @param district district object containg partial info. It will be fully initallized after function
/// @param totalDemWasted int for a count of the total wasted democrat votes, it will get updated in the function
/// @param totalRepWasted int for a count of the total wasted republican votes, it will get updated in the function
/// @param efficiencyGap double for the efficiency gap, it will get updated in the function
void calculateGerrymandering(Districts &district, int &totalDemWasted, 
int &totalRepWasted, double &efficiencyGap)
{
    int totalDemVotes = 0;
    int totalRepVotes = 0;

    //find total votes in a district and the overhalf
    district.totalVotes = district.democraticVotes + district.republicanVotes;
    district.overHalf = (district.totalVotes / 2) + 1;

    //depending on who won in the district, perform respective calculation to find wasted votes for each party
    if(district.democraticVotes > district.republicanVotes)
    {
        district.wastedDemVotes = abs(district.overHalf - district.democraticVotes);
        district.wastedRepVotes = district.republicanVotes;

    }else if(district.democraticVotes < district.republicanVotes)
    {
        district.wastedRepVotes = abs(district.overHalf - district.republicanVotes);
        district.wastedDemVotes = district.democraticVotes;
    }

    //calculate total votes for each category in all districts
    totalDemVotes += district.democraticVotes;
    totalRepVotes += district.republicanVotes;

    totalDemWasted += district.wastedDemVotes;
    totalRepWasted += district.wastedRepVotes;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief using districts information for a state it calculates all data including
///         efficiency gap, wasted votes, totals, etc...
/// @param statesVector vector of structs of type State containing all info
/// @param stateIndex an int that will be updated with the location/index of the state in the vector
void calculateStateInfo(ourvector<State>& statesVector, int & stateIndex)
{
    int totalDemVotes = 0;
    int totalRepVotes = 0;

    int totalVotesCast;
    int totalDemWasted;
    int totalRepWasted;

    string gerrymandered = "No";
    string state = statesVector.at(stateIndex).stateName;

    int demVotes;
    int repVotes;
    //calculate all statistic data for every district in that state
    for(int i = 0; i< statesVector.at(stateIndex).districtVector.size(); i++)
    {
        demVotes = statesVector.at(stateIndex).districtVector.at(i).democraticVotes;
        repVotes = statesVector.at(stateIndex).districtVector.at(i).republicanVotes;

        //if total votes for a district is not zero then proceed 
        //  to calculate if that state is gerrymandering
        if((demVotes + repVotes) != 0)
        {
            calculateGerrymandering(
            statesVector.at(stateIndex).districtVector.at(i), 
            statesVector.at(stateIndex).totalWastedDemVotes, 
            statesVector.at(stateIndex).totalWastedRepVotes, 
            statesVector.at(stateIndex).efficiencyGap);
        }
        
        //calculate total votes in a state for both parties
        totalDemVotes += demVotes;
        totalRepVotes += repVotes;
    }

    totalDemWasted = statesVector.at(stateIndex).totalWastedDemVotes;
    totalRepWasted = statesVector.at(stateIndex).totalWastedRepVotes;

    totalVotesCast = totalDemVotes + totalRepVotes;

    statesVector.at(stateIndex).totalVotesCast = totalVotesCast;

    //Only calculate efficiency gap if we have more than 3 districts
    if(statesVector.at(stateIndex).districtVector.size() >= 3)
    {
        statesVector.at(stateIndex).efficiencyGap = ((abs(totalDemWasted - totalRepWasted)/(double)totalVotesCast ) * 100); 
    }

}
//---------------------------------------------------------------------------------------------------------------
/// @brief using the vector of structs of type State and loctation it will display 
///         statistical data about that specific state
/// @param vec vector of structs of type State containing all info
/// @param index location of that particular state
void stats(ourvector<State>& vec, int & index)
{
    string gerrymandered = "No";
    string GerrymanderedAgainst;

    int totalDemWasted = vec.at(index).totalWastedDemVotes;
    int totalRepWasted = vec.at(index).totalWastedRepVotes;

    //identify which political party gerrymandering calculation is against
    if(totalDemWasted > totalRepWasted)
    {
        GerrymanderedAgainst = "Democrats";
    }else{
        GerrymanderedAgainst = "Republicans";
    }

    //if efficiency gap for that state is over 7% then display gerrymandering stats
    //otherwise display just the regular stats
    if(vec.at(index).efficiencyGap > 7.0)
    {
        gerrymandered = "Yes";

        cout << "Gerrymandered: " << gerrymandered << endl;
        cout << "Gerrymandered against: " << GerrymanderedAgainst << endl;
        cout << "Efficiency Factor: " << vec.at(index).efficiencyGap << "%" << endl;
        cout << "Wasted Democratic votes: " << totalDemWasted << endl;
        cout << "Wasted Republican votes: " << totalRepWasted << endl;
        cout << "Eligible voters: " << vec.at(index).eligibleVoters << endl << endl;

    }else
    {
        cout << "Gerrymandered: " << gerrymandered << endl;
        cout << "Wasted Democratic votes: " << totalDemWasted << endl;
        cout << "Wasted Republican votes: " << totalRepWasted << endl;
        cout << "Eligible voters: " << vec.at(index).eligibleVoters << endl << endl;

    }
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Using the extracted string initilize district object memeber respectably 
//          and once filled push it to the vector of structs of type districts
/// @param count an integer that counts which position we are to assign respectebly the value
/// @param districtObject district object that will be initialized in  the function
/// @param value the value that we are going to store in the district object
/// @param fullyInitialized boolean value that signifies when the object has completely been initilallized
/// @param stateDistrictsInfo vector of structs of type Districts for storing data
/// @param totalDistricts count of total districts in that state
void initializeDistrictObject(int &count, Districts &districtObject, const int &value, bool &fullyInitialized, 
ourvector<Districts> &stateDistrictsInfo, int & totalDistricts)
{
    //assign values to struc district object respectebly
    if(count == 0)
    {
        districtObject.districtNum = value;
        totalDistricts++;
    }
    else if(count == 1)
    {
        districtObject.democraticVotes = value;
    }
    else if(count == 2)
    {
        count = 0;

        fullyInitialized = true;

        districtObject.republicanVotes = value;
        stateDistrictsInfo.push_back(districtObject);
    }

    if(count != 2 && fullyInitialized == false)
    {
        count++;
    }
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Using the extracted line from a file it extracts the first piece of data 
///         that is separated by comma and return it
/// @param inputLine extracted string line from any file
string parseString(string &inputLine)
{
    string word;
    size_t location;
    int start = 0;
    size_t size;

 
    location = inputLine.find(",");
    size = inputLine.size();

    //if find function reaches end and finds no comma but 
    //  string still is not empty extract last piece of data.
    //  If there is something to extract return it otherwise return an empty string
    if(location==string::npos && !inputLine.empty())
    {
        word = inputLine.substr(start);//,inputLine.size() - 1)

        inputLine = "";
    } else if(location==string::npos){

        string emptyWord;
        return emptyWord;

    }else{
        word = inputLine.substr(start, location);
        inputLine = inputLine.substr(location + 1);
    }

    return word;
  
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Using the extracted line from a district file the function initiliazes Districts type object
///         and stores it into a vector of struct of type Districts
/// @param inputLine a string containing the line extracted from a districts file
/// @param districtVector the vector where we are going to store the district objects
/// @param totalDistricts an integer that counts the total districts for that state
void addDistrictInfo(string &inputLine, ourvector< Districts> &districtVector, int &totalDistricts)
{ 
    stringstream sstream;
    Districts districtObject;

    bool fullyInitialized = false;
    int count = 0;
    int districtNum = 0;

    int value; 
    string temp;

    temp = parseString(inputLine);
    
    //loops until there is no more info to extract from inputLine
    while(!(temp.empty()))
    {
        //if there is an instance where the 
        //  district num is stored as AL change it to 1
        if(temp == "AL")
        {
            int district = 1;

            districtNum++;
            districtObject.districtNum = district;
            count++;
            temp = parseString(inputLine);
            continue;
        }

        //extract numbers from the stringstream
        if(stringstream(temp) >> value)//
        {
            //intialize district objects and assign values to their respective position
            initializeDistrictObject(count, districtObject, value, fullyInitialized, districtVector,districtNum);
        }
        
        //ignore commas 
        if (sstream.peek() == ',')
        {
            sstream.ignore();
        }

        temp = parseString(inputLine);
        fullyInitialized = false;
    }

    totalDistricts = districtNum;

}
//---------------------------------------------------------------------------------------------------------------
/// @brief using states vector it searches for an specific state updating the location if found
///         and returns true or false if we found were able to find the state
/// @param statesVector vector of structs of type State containing all info
/// @param stateName string containg the desired state to find
/// @param Index an int that will be updated with the location/index of the state in the vector
bool searchState(ourvector<State> &statesVector, const string &stateName, int &Index)
{
    bool found = false;
    string stateInVector;
    string stateNameCopy = stateName;

    //change state name to lowercase 
    transform(stateNameCopy.begin(), stateNameCopy.end(), stateNameCopy.begin(), ::tolower);

    //search for desired state through state vector
    for(int i = 0; i<statesVector.size(); i++)
    {
        stateInVector = statesVector.at(i).stateName;
        transform(stateInVector.begin(), stateInVector.end(), stateInVector.begin(), ::tolower);

        if(stateNameCopy == stateInVector)
        {
            Index = i;
            found = true;
            return found;
        }
    }

    return found;

}
//---------------------------------------------------------------------------------------------------------------
/// @brief using ifstream, state vector , state object, and firstline extracts and saves all info 
///         into a state object and pushes that object once initialized into a vector of structs 
/// @param inFile ifstream containing file data
/// @param statesVector the vector of struct to store all state info
/// @param stateObject state object to intialize it and push it to our vector
void loadDistrictTypeFyle(ifstream &inFile, ourvector<State> &statesVector, State &stateObject)
{
    int totalDistricts = 0;
    string stateName;
    string inputLine;


    //Do the same process for every single line in the file:
    while (getline(inFile,inputLine))
    {
        //add state name to state object
        stateName = parseString(inputLine);
        stateObject.stateName = stateName;

        //Add district info to state object and push it to state vector 
        addDistrictInfo(inputLine,stateObject.districtVector,totalDistricts);
        statesVector.push_back(stateObject);

        //clear stateobject vector for reuse
        stateObject.districtVector.clear();

        cout << "..." << stateName << "..." << totalDistricts<< " districts total" << endl;

        totalDistricts = 0;
    }
    
}
//---------------------------------------------------------------------------------------------------------------
/// @brief using ifstream, state vector , and state object. It extracts the eligible voters
///         per state and saves the info into a state object and displays data
/// @param data data as string, it will get converted to int and get stored 
/// @param statesVector the vector of struct to store all state info
/// @param stateFoundAt an int containing the location/index of a state in the vector
/// @param stateName name of the state to display to screen
void extractEligibleVoters(string &data, ourvector<State> &statesVector, int &stateFoundAt, string stateName)
{
    int eligiblityVoters;

    if(stringstream(data) >> eligiblityVoters)
    {    
        statesVector.at(stateFoundAt).eligibleVoters = eligiblityVoters;
        cout << "..." << stateName << "..." << eligiblityVoters<< " eligible voters" << endl;
    }

}
//---------------------------------------------------------------------------------------------------------------
/// @brief using the ifstream inFile, state vector, and state object. It extracts the 
///         eligible voters for a state. Saving and updating state object 
/// @param inFile ifstream containing file data
/// @param statesVector the vector of struct to access state objects and update eligibility voters info
/// @param stateObject state object to access 
void loadEligibleVotersTypeFile(ifstream &inFile, ourvector<State> &statesVector, State &stateObject)
{
    string data;
    int stateFoundAt;
    string stateName;
    stringstream sstream;
    string inputLine;

    //Do the same process for every single line in the file:
    while (getline(inFile,inputLine))
    {
        stateName = parseString(inputLine);
        stateObject.stateName = stateName;

        if(searchState(statesVector, stateName, stateFoundAt))
        {
            data = parseString(inputLine);
            sstream << data;

            extractEligibleVoters(data, statesVector, stateFoundAt, stateName);
            
        }
    }
}
//---------------------------------------------------------------------------------------------------------------
/// @brief using two provided files containing information separated by commas
///         opens and reads all the information and stores them into a vector of struct type State
/// @param filesString a string with the both of the filenames
/// @param statesVector the vector of struct to store all state info
/// @param file_1_loaded a string that returns yes if data from file 1 was loaded successfully
/// @param file_2_loaded a string that returns yes if data from file 2 was loaded successfully
void load(string &filesString,  ourvector<State> &statesVector, string &file_1_loaded, string &file_2_loaded)
{
    stringstream fileStream;
    State stateObject;
    string districtFile;
    string eligibleVotersFile;

    fileStream << filesString;

    //get both files from user input
    fileStream >> districtFile;
    fileStream >> eligibleVotersFile;

    //START EXTRACTING DISTRICTS FILE INFO
    ifstream inFirstFile(districtFile);//open district file

    if(inFirstFile.fail())
    {
        cout << "Invalid first file, try again." << endl << endl;
        return;
    }
    
    //read and load all data
    cout << "Reading: " << districtFile << endl;
    loadDistrictTypeFyle(inFirstFile,statesVector,stateObject);
    cout << endl;

    file_1_loaded = "Yes";
    inFirstFile.close();
 
    //START EXTRACTING ELIGIBLE VOTERS FILE INFO
    ifstream inSecondFile(eligibleVotersFile);

    if(inSecondFile.fail())
    {
        cout << "Invalid second file, try again." << endl << endl;
        return;
    }
    
    cout << "Reading: " << eligibleVotersFile << endl;
    loadEligibleVotersTypeFile(inSecondFile, statesVector, stateObject);
    cout << endl;

    file_2_loaded = "Yes";
    inSecondFile.close();
}
//---------------------------------------------------------------------------------------------------------------