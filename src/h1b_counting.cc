#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// constants
const string STATUS = "CASE_STATUS";
const string STATUS_ALT = "STATUS";
const string OCCUPATION = "SOC_NAME";
const string OCCUPATION_ALT = "LCA_CASE_SOC_NAME";
const string STATE = "WORKSITE_STATE";
const string STATE_ALT = "LCA_CASE_WORKLOC1_STATE";
const string CERTIFIED = "CERTIFIED";
const string DELIM = ";";
const string DOUBLE_QUOTE = "\"";
const string OCCUPATION_HEADER = "TOP_OCCUPATIONS;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n";
const string STATE_HEADER = "TOP_STATES;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n";
const unsigned MAX_SUMMARY_LINES = 10;
const unsigned NUM_COL_INTEREST = 2;


// Parse header info and return a hash table that has column index by name
const unordered_map<string, unsigned> parseHeaderInfo(const string& s)
{
  unordered_map<string, unsigned> ht;
  size_t start = 0;
  size_t end = s.find(DELIM, start);
  unsigned columnIndex = 0; // keeps track of the column index

  while(end != string::npos) {
    ht[s.substr(start, end-start)] = columnIndex;
    
    start = end + DELIM.length();
    end = s.find(DELIM, start);
    columnIndex++;
  }
  // retrieve the last column
  ht[s.substr(start, end-start)] = columnIndex;
  
  return ht;
}


// Remove double-quotes around a string, if any
const string trimQuotes(string str)
{
  // trim leading double-quote
  str.erase(str.find_last_not_of(DOUBLE_QUOTE) + 1);

  // trim trailing double-quote
  str.erase(0, str.find_first_not_of(DOUBLE_QUOTE));

  return str;
}


// Check if the key exists in the hash table
const string checkKey(const unordered_map<string, unsigned>& ht, const string& key1, const string& key2)
{
  return (ht.find(key1) != ht.end()) ? key1: key2;
}


// Count the number of certified applicants
void count(map<string, unsigned>& m1, map<string, unsigned>& m2,
	   unordered_map<string, unsigned>& nCertified,
	   const string& s, unordered_map<string, unsigned> ht)
{
  size_t start = 0;
  size_t end = s.find(DELIM, start);
  unsigned columnIndex = 0;
  bool isCertified = false;

  // Check if the case is certified:
  // Since CASE_STATUS column may be positioned anywhere
  // relative to the SOC_NAME and WORKSITE_STATE columns,
  // do this first, then do a separate while-loop later
  const string keyStatus(checkKey(ht, STATUS, STATUS_ALT));
  while(end != string::npos) {
    if(columnIndex == ht[keyStatus]) {
      isCertified = (s.substr(start, end-start) == CERTIFIED);
      break;
    }
    
    start = end + DELIM.length();
    end = s.find(DELIM, start);
    columnIndex++;
  }
  // Check the last column if needed
  // (in case the CASE_STATUS is the last column)
  if(!isCertified)
    if(columnIndex == ht[keyStatus])
      isCertified = (s.substr(start, end-start) == CERTIFIED);

  // If certified, parse values from 2 columns, SOC_NAME and WORKSITE_STATE
  // and increment map object
  unsigned nCounted = 0;
  if(isCertified) {
    start = 0;
    end = s.find(DELIM, start);
    columnIndex = 0;
    const string keyOccupation(checkKey(ht, OCCUPATION, OCCUPATION_ALT));
    const string keyState(checkKey(ht, STATE, STATE_ALT));
    while(end != string::npos) {
      if(columnIndex == ht[keyOccupation]) {
	m1[trimQuotes(s.substr(start, end-start))]++;
	nCertified[OCCUPATION]++;
	++nCounted;
      } else if(columnIndex == ht[keyState]) {
	m2[s.substr(start, end-start)]++;
	nCertified[STATE]++;
	++nCounted;
      }
      // Check if both columns of interests have been checked
      if(nCounted == NUM_COL_INTEREST) break;
      
      start = end + DELIM.length();
      end = s.find(DELIM, start);
      columnIndex++;
    }
    if(nCounted < NUM_COL_INTEREST) {
      // Check the last column (in case the column of interest is there)
      if(columnIndex == ht[keyOccupation]) {
	m1[trimQuotes(s.substr(start, end-start))]++;
	nCertified[OCCUPATION]++;
      } else if (columnIndex == ht[keyState]) {
	m2[s.substr(start, end-start)]++;
	nCertified[STATE]++;
      }
    }
  }
}


// Sort the summary first by the highest number of applications, then by name. Write the first 10 to the output file
void sortOutput(ofstream& file, const map<string, unsigned>& m, const unsigned total)
{
  // find the maximum val
  unsigned val = 0;
  for(auto it:m)
    if(it.second > val)
      val = it.second;

  // sort and write lines up to the maximum allowed
  unsigned nLine = 0;
  auto it = m.begin();
  while(val > 0) {
    if(it != m.end()) {
      if(it->second == val) {
	// output will be capped at 10 with proper formatting
	if(nLine < MAX_SUMMARY_LINES) {
	  file << it->first.c_str() << DELIM << it->second << DELIM
	       << fixed << setprecision(1)
	       << round(static_cast<float>(it->second) / total * 1000.0) / 10.0
	       << "%\n";
	  ++nLine;
	} else {
	  break;
	}
      }
      ++it;
    } else {
      it = m.begin();
      --val;
    }
  }
}


// Output file
void output(const string nameFile, const string& header, const map<string, unsigned>& m, const unsigned total)
{
  ofstream outputFile(nameFile);
  if(outputFile.is_open()) {
    outputFile << header;
    sortOutput(outputFile, m, total);
    outputFile.close();
  } else {
    cerr << "Error: " << nameFile << " cannot be written\n";
  }
}


// Main program: read and analyze input file, and output 2 files
int main(int argc, char **argv)
{
  // open the input file
  ifstream inputFile(argv[1]);
  if(inputFile.is_open()) {
    string line;
    getline(inputFile, line);
    const unordered_map<string, unsigned> headerInfo(parseHeaderInfo(line));
    
    // Read each row and count by either occupation or state
    map<string, unsigned> occupation;
    map<string, unsigned> state;
    unordered_map<string, unsigned> nCertified;
    while(getline(inputFile, line))
      count(occupation, state, nCertified, line, headerInfo);
    
    inputFile.close();

    // output "top_10_occupations" file
    output(argv[2], OCCUPATION_HEADER, occupation, nCertified[OCCUPATION]);

    // output "top_10_states" file
    output(argv[3], STATE_HEADER, state, nCertified[STATE]);
  } else {
    cerr << "Error: " << argv[1] << " cannot be opened\n";
  }
  
  return 0;
}
