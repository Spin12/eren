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

  cout << " Progress:  " << fixed << setprecision(2) << float(nn0-nn)/float(nn0)*100.0 << "%";

  string varID;
  unsigned int nPos;                  // Position of varID

  while(nn != 0) {
    for(auto vec : dataIn) {
      if(vec[4].find(kw) != string::npos) {
        varID = vec[4];
        break;
      }
      nPos++;
    }
  
  
    //varID="CMO - 2016 - 100696 - 0";
    getCMO(dataIn, nn, varID, ndaysSum);
  
    //cout << "\b\b\b\b\b\b" <<  setprecision(1) << float(nn0-nn)/float(nn0)*100 << "%";
    if(float(nn0-nn)/float(nn0)*100.0<10) {
      cout << "\b\b\b\b\b" <<  fixed << setprecision(2) << float(nn0-nn)/float(nn0)*100.0 << "%";
    } else {
      cout << "\b\b\b\b\b\b" <<  fixed << setprecision(2) << float(nn0-nn)/float(nn0)*100.0 << "%";
    }
    //cout << setprecision(4) << float(nn) << endl;
    //cout << varID << endl;
  }

  cout << endl << endl;
  return 0;
}



inline void getCMO(vector< vector<string> >& dataIn, unsigned int& nn, string& varID, vector<float>& ndaysSum) {

  unsigned int kk = 0, erstellen = 0,
               jj = 1, nn1 = 0, mm1 = 0;
  float year0, month0, day0, hour0, min0, sec0, time0;
  string user0;
  vector<float> year_k(20,0), month_k(20,0), day_k(20,0), time_k(20,0);
  vector<string> user_k(20,"NaN");
  string sep1 = "", sep2 = "", sep3 = "";
  escaped_list_separator<char> sep;


  for( int ii=0; ii<dataIn.size(); ii++) {
      
    findBlock(dataIn, varID, nn1, mm1);
    if(nn1==0){
      break;
    }
    nn--;
  
    // Get date ------------------------------------------------------------
    {
      sep2 = "/";
      sep={sep1,sep2,sep3};
      tokenizer<escaped_list_separator<char> > tokDate( dataIn[nn1][0], sep );

      day0 = stof(*(next(tokDate.begin(),0)));
      month0 = stof(*(next(tokDate.begin(),1)));
      year0 = stof(*(next(tokDate.begin(),2)));

    }
    
    // Get time ------------------------------------------------------------
    {
      sep2 =":";
      sep = {sep1,sep2,sep3};
      tokenizer<escaped_list_separator<char> > tokTime( dataIn[nn1][1], sep );

      hour0 = stof(*(next(tokTime.begin(),0)));
      min0 = stof(*(next(tokTime.begin(),1)));
      sec0 = stof(*(next(tokTime.begin(),2)));
    }
  
    // Get user ------------------------------------------------------------
    user0 = dataIn[nn1][2];
  
    // Calculate past time -------------------------------------------------
    time0 = year0*365*24*60*60 +
            ndaysSum[int(month0)]*24*60*60 +
            day0*24*60*60 +
            hour0*60*60 +
            min0*60 + 
            sec0;
  
    // Find "erstellen" and "Utilisateur de validation" -------------------
    for(int jj = nn1; jj <= mm1; jj++) {
      
      if(dataIn[jj][3].find("erstellen") != string::npos) {
        time_k[0] = time0;
        year_k[0] = year0;
        month_k[0] = month0;
        day_k[0] = day0;
        user_k[0] = user0;
      }
  
      if((dataIn[jj][6].find("Utilisateur de validation") != string::npos) && (dataIn[jj][7] != "")) {
        kk++;
        time_k[kk] = time0;
        year_k[kk] = year0;
        month_k[kk] = month0;
        day_k[kk] = day0;
        user_k[kk] = user0;
      }
  
      for(int ll = 0; ll < dataIn[0].size();ll++){
        dataIn[jj][ll] = ""; 
      }
  
    }
  
  }

}

inline void findBlock(vector< vector<string> >& dataIn, string& varID, unsigned int& nn1, unsigned int& mm1) {

  nn1 = 0;

  for(unsigned int ii = mm1; ii < dataIn.size(); ii++) {
    if(dataIn[ii][4] == varID) {
      nn1 = ii;
      break;
    }
  }

  if(nn1 != 0) {
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
  }

}

