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


void getCMO(vector< vector<string> >&, unsigned int&, string&, vector<float>&);
void findBlock(vector< vector<string> >&, string&, unsigned int&, unsigned int&);


int main(int argc, char* argv[]) {

  string kw ="CMO";
  vector<float> ndaysSum={0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

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


  unsigned int nn0 = 0, nn = 0;
  for(auto vec : dataIn) {
    if(vec[4].find(kw) != string::npos) {
      nn += 1;
      //cout << vec[4] << endl;
    }
  }

  nn0 = nn;

  cout << " Progress: " << setprecision(2) << float(nn0-nn)/float(nn0) << "%" << endl;

  string varID;
  unsigned int nPos = 0;                  // Position of varID
  for(auto vec : dataIn) {
    if(vec[4].find(kw) != string::npos) {
      varID = vec[4];
      break;
    }
    nPos++;
  }


  getCMO(dataIn, nn, varID, ndaysSum);

  return 0;
}



void getCMO(vector< vector<string> >& dataIn, unsigned int& nn, string& varID, vector<float>& ndaySum) {

  unsigned int kk = 0, erstellen = 0,
               jj = 1, nn1 = 0, mm1 = 0;
  //for( int ii=1;ii<=nn;ii++) {
      
  findBlock(dataIn, varID, nn1, mm1);
  float year0, month0, day0, hour0, min0, sec0, tim0;
  string user0;

  // Get date ------------------------------------------------------------
  string sep1 = "", sep2 = "/", sep3 = "";
  escaped_list_separator<char> sep(sep1,sep2,sep3);
  tokenizer<escaped_list_separator<char> > tokDate( dataIn[nn1][0], sep );
  day0 = stof(*(next(tokDate.begin(),0)));
  month0 = stof(*(next(tokDate.begin(),1)));
  year0 = stof(*(next(tokDate.begin(),2)));
  
  // Get time ------------------------------------------------------------
  sep2 =":";
  sep = {sep1,sep2,sep3};
  tokenizer<escaped_list_separator<char> > tokTime( dataIn[nn1][1], sep );
  hour0 = stof(*(next(tokTime.begin(),0)));
  min0 = stof(*(next(tokTime.begin(),1)));
  sec0 = stof(*(next(tokTime.begin(),2)));

  // Get time ------------------------------------------------------------
  user0 = dataIn[nn1][2];


  cout << day0 <<  "\t" << month0 <<  "\t" << setprecision(4) << year0 << endl;
  cout << sec0 <<  "\t" << min0 <<  "\t" << hour0 << endl;
  cout << user0 << endl;
  
  

}

void findBlock(vector< vector<string> >& dataIn, string& varID, unsigned int& nn1, unsigned int& mm1) {

  nn1 = 0;
  while(true) {
    if(dataIn[nn1][4] == varID) {
      break;
    }
    nn1++;
  }

  mm1 = nn1+1;
  while(true) {
    if((dataIn[mm1][0] == "") && (dataIn[mm1][1] == "") && (dataIn[mm1][2] == "") && 
       (dataIn[mm1][3] == "") && (dataIn[mm1][4] == "") && (dataIn[mm1][5] == "") && 
       (dataIn[mm1][6] == "") && (dataIn[mm1][7] == "") && (dataIn[mm1][8] == "") && 
       (dataIn[mm1][2] == "") && (dataIn[mm1][10] == "")) {
      break;
    }
    mm1++;
  }

  cout << "nn1 = " << nn1 << ",\t" << "mm1 = " << mm1 << endl;

}

























