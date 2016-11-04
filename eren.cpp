#include<iomanip>      // std::setprecision
#include<algorithm>
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>
#include<vector>
#include<streambuf>
#include<fstream>
#include<sstream>

using namespace std;
using namespace boost;


void getCMO(vector< vector<string> >&, unsigned int&, string&, vector<double>&, ofstream&);
void findBlock(vector< vector<string> >&, string&, unsigned int&, unsigned int&);
void writeData(vector<double>&, vector<double>&, vector<double>&, vector<double>&,
               vector<string>&, unsigned int&, vector<double>&, string&, ofstream&); 


int main(int argc, char* argv[]) {


  string kw ="CMO";
 
  cout << endl << " Bitte geben Sie das Schlüsselwort (z.B. CMO): "; 
  cin >> kw;

  vector<double> ndaysSum={0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
  //vector<double> ndaysSum={0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

  // Define output file name ----
  ofstream fileOut("output.csv");
  // Write header
  fileOut << "Rechnungen;Erstellung;;Validation I;;;Validation II;;;Validation III;;;Validation IV\n";
  fileOut << ";Datum;User;Datum;User;Delta;Datum;User;Delta;Datum;User;Delta;Datum;User;Delta\n";

  // Read data ------------------
  string fileName = argv[1];
  ifstream t(fileName);

  stringstream buffer;
  buffer << t.rdbuf();
  
  string dataIn0 = buffer.str();

  int ncolumns = 11;
  int nrows = count(dataIn0.begin(), dataIn0.end(), ';')/(ncolumns-1);

  //cout << " rows: " << nrows << "\t" << "columns: " << ncolumns << endl;


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

  cout << endl << " Successfully read data!" << endl;

  unsigned int nn0 = 0, nn = 0;
  for(auto vec : dataIn) {
    if(vec[4].find(kw) != string::npos) {
      nn += 1;
      //cout << vec[4] << endl;
    }
  }

  nn0 = nn;
 
  cout << "\n\n";
  cout << " Progress:  " << fixed << setprecision(2) << double(nn0-nn)/double(nn0)*100.0 << "%";

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
    //varID="CMO - 2016 - 101759 - 0";
    //varID="CMO - 2016 - 101702 - 0";
    getCMO(dataIn, nn, varID, ndaysSum, fileOut);

/*
  fileOut.close();
  cout << endl << endl;
  return 0;
*/  
    if(double(nn0-nn)/double(nn0)*100.0<10) {
      cout << "\b\b\b\b\b" <<  fixed << setprecision(2) << double(nn0-nn)/double(nn0)*100.0 << "%";
    } else {
      cout << "\b\b\b\b\b\b" <<  fixed << setprecision(2) << double(nn0-nn)/double(nn0)*100.0 << "%";
    }

  }

  fileOut.close();
  cout << endl << endl;

  return 0;
}



inline void getCMO(vector< vector<string> >& dataIn, unsigned int& nn, string& varID, vector<double>& ndaysSum, ofstream& fileOut) {

  unsigned int kk = 0, erstellen = 0, jj = 1, nn1 = 0, mm1 = 0;
  double year0, month0, day0, hour0, min0, sec0, time0;
  string user0;
  vector<double> year_k(100,0), month_k(100,0), day_k(100,0), time_k(100,0);
  vector<string> user_k(100,"NaN");
  string sep1 = "", sep2 = "", sep3 = "";
  escaped_list_separator<char> sep;


  for( int ii=0; ii<dataIn.size(); ii++) {
      
    findBlock(dataIn, varID, nn1, mm1);
    if(nn1==0){
      break;
    }
    nn--;
    //cout << nn1 << "\t" << mm1 << endl;
  
    // Get date ------------------------------------------------------------
    {
      sep2 = "/.";
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
      
      //if(dataIn[jj][3].find("erstellen") != string::npos) {
      if(dataIn[jj][3].find("Création") != string::npos) {
        //cout << dataIn[jj][3] << "\t" << jj << "\t" << varID << endl;
        erstellen = 1;
        time_k[0] = time0;
        year_k[0] = year0;
        month_k[0] = month0;
        day_k[0] = day0;
        user_k[0] = user0;
      }
  
      //if((dataIn[jj][6].find("Utilisateur de validation") != string::npos) && (dataIn[jj][7] != "")) {
      if((dataIn[jj][6].find("Utilisateur de validation") != string::npos) && (dataIn[jj][7].empty() != 1)) {
        kk++;
        if(kk>98){
          cout << "\n\n" << varID << "\t" << nn1 << "\t" << mm1 << endl;
        }
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

  if(erstellen == 1) {
    writeData(year_k, month_k, day_k, time_k, user_k, kk, ndaysSum, varID, fileOut);
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
         (dataIn[mm1][9] == "") && (dataIn[mm1][9] == "")) {
        break;
      }
      mm1++;
    }
  }

}

inline void writeData(vector<double>& year_k, vector<double>& month_k, vector<double>& day_k,
                      vector<double>& time_k, vector<string>& user_k, unsigned int& kk,
                      vector<double>& ndaysSum, string& varID, ofstream& fileOut) {
  

  if(kk==0) {
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);
  
    //double secNow = aTime->tm_sec;
    //double minNow = aTime->tm_min;
    //double hourNow = aTime->tm_hour;
    double dayNow = aTime->tm_mday;
    int   monthNow = aTime->tm_mon + 1;    // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    double yearNow = aTime->tm_year + 1900; // Year is # years since 1900 

    double dtNow = yearNow*365+ndaysSum[monthNow]+dayNow
                 -year_k[0]*365-ndaysSum[int(month_k[0])]-day_k[0];

    fileOut << varID << ";";
    fileOut << day_k[kk] << "." << month_k[kk] << "." << year_k[kk] << ";";
    fileOut << user_k[kk] << ";";
    fileOut << dayNow << "." << monthNow << "." << yearNow << ";----;";
    fileOut << dtNow << "\n";
  }

  if(kk==1) {
    double dt = year_k[1]*365+ndaysSum[int(month_k[1])]+day_k[1]
              -year_k[0]*365-ndaysSum[int(month_k[0])]-day_k[0];

   fileOut << varID << ";"; 
   fileOut << day_k[0]<< "." << month_k[0]<< "." << year_k[0] << ";" << user_k[0] << ";"; 
   fileOut << day_k[1]<< "." << month_k[1]<< "." << year_k[1] << ";" << user_k[1] << ";" << dt << "\n"; 
  }


  if(kk>1) {
    double timeMin=2016050413355900;
    int itMin = -1;
    vector<double> yearSor(kk+1,0), monthSor(kk+1,0), daySor(kk+1,0);
    vector<string> userSor(kk+1,"NaN");

    for(int ii = 1;ii <= kk; ii++) {
      timeMin=2016050413355900;
      itMin = -1;
      for(int jj=1; jj <= kk; jj++) {
        if(timeMin > time_k[jj]) {
           timeMin = time_k[jj];
           itMin = jj;
        }     
      }
     yearSor[ii]  = year_k[itMin];
     monthSor[ii] = month_k[itMin];
     daySor[ii]   = day_k[itMin];
     userSor[ii]  = user_k[itMin];

     time_k[itMin] = 2016050413355900;
    }

   fileOut << varID << ";";
   fileOut << day_k[0] << "." << month_k[0] << "." << year_k[0] << ";" << user_k[0];

   vector<double> dt(kk+1,0);
   for(int ii=1; ii<=kk; ii++) {
     if(ii==1) {
       dt[ii]= yearSor[1]*365 + ndaysSum[int(monthSor[1])] + daySor[1]
              -year_k[0]*365 - ndaysSum[int(month_k[0])] - day_k[0];
     } else {    
       dt[ii]=yearSor[ii]*365 + ndaysSum[int(monthSor[ii])] + daySor[ii]
             -yearSor[ii-1]*365 - ndaysSum[int(monthSor[ii-1])] - daySor[ii-1];
     }

     fileOut << ";" << daySor[ii] << "." << monthSor[ii] << "." << yearSor[ii] << ";"
             << userSor[ii] << ";" << dt[ii]; 
   }

   fileOut <<  "\n";

  }
}


