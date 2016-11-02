#include <iomanip>      // std::setprecision
#include <algorithm>
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>
#include<vector>
#include<streambuf>
#include<fstream>
#include<sstream>

using namespace std;
using namespace boost;


int main(int argc, char* argv[]) {

  string kw ="CMO";

  // Read data ------------------
  string fileName = argv[1];
  ifstream t(fileName);

  stringstream buffer;
  buffer << t.rdbuf();
  
  string dataIn0 = buffer.str();

  int ncolumns = 11;
  int nrows = count(dataIn0.begin(), dataIn0.end(), ';')/(ncolumns-1);

  cout << " rows: " << nrows << "\t" << "columns: " << ncolumns << endl;


  vector< vector<string> > dataIn ( nrows+1, vector<string> (ncolumns) );

  string sep1 = "", sep2 = ";\n", sep3 = "\"";
  escaped_list_separator<char> sep(sep1,sep2,sep3);

  tokenizer<escaped_list_separator<char> > tok( dataIn0, sep );
  
  int ii = 0, jj = 0;
  for (const auto &it : tok) {
    dataIn[ii][jj] = it;
    if(jj==ncolumns-1)
      ii++, jj = 0;
    else 
      jj++;
  }

  // end Read data --------------


  unsigned int c0 = 0, c = 0;
  for(auto vec : dataIn) {
    if(vec[4].find(kw) != string::npos) {
      c += 1;
      //cout << vec[4] << endl;
    }
  }

  c0 = c;

  cout << " Progress: " << setprecision(2) << float(c0-c)/float(c0) << "%" << endl;

  string varID;
  unsigned int nPos = 0;                  // Position of varID
  for(auto vec : dataIn) {
    if(vec[4].find(kw) != string::npos) {
      varID = vec[4];
      break;
    }
    nPos++;
  }


  return 0;
}
