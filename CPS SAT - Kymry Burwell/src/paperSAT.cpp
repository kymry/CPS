#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include "math.h"
#include <cmath>

#define V +

using namespace std;

int n; // number of boxes
int xSize; // width of grid
int ySize; // height/length of grid
vector<int> xDim; // x-dimensions of all boxes
vector<int> yDim; // y-dimensions of all boxes
int n_vars; // total number of variables
int n_clauses; // total number of clauses
int zStart;
int xtlStart;
int MIN_LENGTH; // theoretical minimum grid height/length
int MAX_LENGTH; // maximum grid height/length

ofstream cnf;
ifstream sol;

typedef string literal;
typedef string  clause;

literal operator-(const literal& lit) {
  if (lit[0] == '-') return lit.substr(1);
  else               return "-" + lit;
}

// Create literal for box variable (i = y-coordinate, j = x-coordinate, k = box number)
literal x(int i, int j, int k) {
  assert(0 <= i and i < ySize);
  assert(0 <= j and j < xSize);
  assert(0 <= k and k < n);
  return to_string( (xSize*i + j + 1) + k*(xSize*ySize) ) + " ";
}

// Create literal for top-left box variable (i = y-coordinate, j = x-coordinate, k = box number)
literal xtl(int i, int j, int k) {
  assert(0 <= i and i < ySize);
  assert(0 <= j and j < xSize);
  assert(0 <= k and k < n);
  return to_string((xSize*i + j + 1) + k*(xSize*ySize) + xtlStart ) + " ";
}

// Adds a clause
void add_clause(const clause& c) {
  cnf << c << "0" << endl;
  ++n_clauses;
}

// Adds an at most one constraint
void add_amo(const vector<literal>& z) {
  int N = z.size();
  for (int i1 = 0; i1 < N; ++i1)
    for (int i2 = i1+1; i2 < N; ++i2)
      add_clause(-z[i1] V -z[i2]);
}


/*********** Main function to create CNF **********/
void createCNF(){

  // Constraint (1a): Each box has at least one top-left coordinate
  for (int k = 0; k < n; ++k){                // boxes
      clause c;
      for (int i = 0; i < ySize; ++i){        // y coordinates
        for (int j = 0; j < xSize; ++j)       // x coordinates
          c += xtl(i,j,k);
      }
      add_clause(c);
  }

  // Constraint (1b): Each box has at most one top-left coordinate
  for (int k = 0; k < n; ++k){               // boxes
      vector<literal> z;
      for (int i = 0; i < ySize; ++i){       // y coordinates
        for (int j = 0; j < xSize; ++j)      // x coordinates
          z.push_back(xtl(i,j,k));
      }
      add_amo(z);
  }

  // Constraint (2): Each grid point has at most one box
  for (int i = 0; i < ySize; ++i){          // y coordinates
    for (int j = 0; j < xSize; ++j){        // x coordinates
    vector<literal> z;
    for (int k = 0; k < n; ++k)             // boxes
      z.push_back(x(i,j,k));
    add_amo(z);
    }
  }

  // Constraints (3-7): Each box occupies the correct grid points
  for (int k = 0; k < n; ++k){                 // boxes

    for (int i = 0; i < ySize; ++i){           // y coordinates
      for (int j = 0; j < xSize; ++j){         // x coordinates

      // Constraint (3): Box may be placed vertically
      if (i+yDim[k] <= ySize && j+xDim[k] <= xSize){      // if it can be placed vertically
        for (int i1 = 0; i1 < yDim[k]; ++i1){             // over all y dimensions of box
          for (int j1 = 0; j1 < xDim[k]; ++j1){           // over all x dimensions of box
            clause c;                                     // create clause
            c += -xtl(i,j,k);                             // add negated top-left variable
            if (i+xDim[k] <= ySize && j+yDim[k] <= xSize) // if the box can also be placed horizontally
              c += (to_string(zStart+k) + " ");            // then add the rotation auxilary variable in
            c += x( i+i1, j+j1, k);                       // add the variable in
            add_clause(c);                                // add clause
          }
        }
      }

      // Constraint (4): Box may be placed horizontally
      if (i+xDim[k] <= ySize && j+yDim[k] <= xSize){
        for (int i1 = 0; i1 < xDim[k]; ++i1){
          for (int j1 = 0; j1 < yDim[k]; ++j1){
            clause c;
            c += -xtl(i,j,k);
            if (i+yDim[k] <= ySize && j+xDim[k] <= xSize)
              c += (-to_string(zStart+k) + " ");
            c += x( i+i1, j+j1, k);
            add_clause(c);
          }
        }
      }

      // Constraint (5): Box cannot be placed both horizontally and vertically
      for (int i1 = xDim[k]; i1 < yDim[k]; ++i1){
        if(i+i1 >= ySize || j+i1 >= xSize) continue;
        else {
          clause c;
          c += -x(i+i1,j,k);
          c += -x(i,j+i1,k);
          add_clause(c);
        }
      }

    // Constraint (6): Box adheres to top-left placement positioning
    if ( (i+yDim[k] <= ySize && j+xDim[k] <= xSize) || (i+xDim[k] <= ySize && j+yDim[k] <= xSize) ){
      for (int i1 = 0; i1 < ySize; ++i1){
        for (int j1 = 0; j1 < xSize; ++j1){
          if (i1 >= i && i1 < i + yDim[k] && j1 >= j && j1 < j + xDim[k])
            continue;
          else if (i1 >= i && i1 < i + xDim[k] && j1 >= j && j1 < j + yDim[k])
            continue;
          else {
            clause c;
            c +=  -xtl(i,j,k);
            c +=  -x(i1,j1,k);
            add_clause(c);
          }
        }
      }
    }

    // Constraint (7): Top-left coordinate of box cannot be in unfeasible position
    if ( !(i+yDim[k] <= ySize && j+xDim[k] <= xSize) && !(i+xDim[k] <= ySize && j+yDim[k] <= xSize) ){
        clause c;
        c += -xtl(i,j,k);
        add_clause(c);
    }

      }
    }
  }

/*
    // Constraint (8): Symmetry breaking - Boxes of same size must be placed in increasing grid positions
    for (int box1 = 0; box1 < n; ++box1){
      for (int box2 = box1+1; box2 < n; ++box2){
        if (xDim[box1] == xDim[box2] && yDim[box1] == yDim[box2]){
          for (int i1 = 0; i1 < ySize; ++i1){
            for (int j1 = 0; j1 < xSize; ++j1){
              for (int i2 = 0; i2 <= i1; ++i2){
                for (int j2 = 0; j2 < xSize && ( (xSize*i1+j1+1) >= (xSize*i2+j2+1) ); ++j2){
                  clause c;
                  c += -xtl(i1,j1,box1);
                  c += -xtl(i2,j2,box2);
                  add_clause(c);
                }
              }
            }
          }
          break;
        }
      }
    }
*/

  cnf << "p cnf " << n_vars << " " << n_clauses << endl;
}



/*********** Auxilary Functions ***********/

// Get solution from Lingeling output
void get_solution(vector<int>& q) {
  int lit;
  while (sol >> lit)
    if (lit <= xSize*ySize*n && lit > 0) {
      q.push_back(lit);
    }
}

// Print solution to standard output in form: (x-start,y-start) (x-end,y-end)
void write_solution(vector<int> &q){
  int boxCounter = 0, grid = xSize*ySize;
  for (int i = 0; i < q.size();i++){
    if (boxCounter >= n) break;
    for (int k = 0; k < xDim[boxCounter]*yDim[boxCounter]; k++){
      // 1x1 boxes only
      if (k == 0 && k == xDim[boxCounter]*yDim[boxCounter]-1){
        cout <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << " ";
        cout <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << endl;
      }
      // all other type of boxes
      else if (k == 0 || k == xDim[boxCounter]*yDim[boxCounter]-1){
        cout <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << " ";
        if (k != 0) {
          i += k;
          cout << endl;
        }
      }
    }
    boxCounter++;
  }
}

// Write solution to output file
void write_solution_file(vector<int> &q, const char* outputFileName, int currentLength){
  string outputFile = outputFileName;
  outputFile.erase(outputFile.end()-3, outputFile.end());
  outputFile.append("_SAT.out");
  ofstream output(outputFile);

  output << currentLength << endl;

  int boxCounter = 0, grid = xSize*ySize;
  for (int i = 0; i < q.size();i++){
    if (boxCounter >= n) break;
    for (int k = 0; k < xDim[boxCounter]*yDim[boxCounter]; k++){
      // 1x1 boxes only
      if (k == 0 && k == xDim[boxCounter]*yDim[boxCounter]-1){
        output <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << " ";
        output <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << endl;
      }
      // all other type of boxes
      else if (k == 0 || k == xDim[boxCounter]*yDim[boxCounter]-1){
        output <<  ((q[i+k]-(boxCounter*grid)-1) % xSize)  << " " <<   ((q[i+k]-(boxCounter*grid)-1) / xSize) << " ";
        if (k != 0) {
          i += k;
          output << endl;
        }
      }
    }
    boxCounter++;
  }
}

// Read data from file
static void configureData(const char* inputFile) {

  // Read file into program
  ifstream input(inputFile);
  vector<int> tempData;
  istream_iterator<int> iter(input), eof;
  while (iter != eof) tempData.push_back(*iter++);
  for (auto i = tempData.begin()+1; i < tempData.end(); i+=3) {n += *i;}

  // Write box dimensions
  for (auto i = tempData.begin()+1; i < tempData.end(); i+=3){
    for (int j = 0; j < *i; j++){
      xDim.push_back(*(i+1));
      yDim.push_back(*(i+2));
    }
  }

  // Calculate maximum paper roll height
  for (int i = 0; i < n; i++){ (xDim[i] > yDim[i]) ? (ySize += xDim[i]) : (ySize += yDim[i]);}
  xSize = tempData[0];

  // Determine the theoretical minimum length
  double area = 0;
  for (double i = 0; i < n; i++) area += xDim[i]*yDim[i];
  MIN_LENGTH = ceil(area/xSize);
  MAX_LENGTH = ySize;

  // Set parameters
  n_vars = n*xSize*ySize*2+n;
  zStart = n*xSize*ySize*2+1;
  xtlStart = n*xSize*ySize;
}

// Check if output file is empty
bool isEmpty(ifstream &file_to_check){
    return file_to_check.peek() == ifstream::traits_type::eof();
}



/*********** Main Function ***********/

int main(int argc, char* argv[]) {

  // Configure input data
  configureData(argv[1]);

    // Create initial CNF
    cnf.open("tmp.rev");
    createCNF();

    // Continually decrease grid height/length until optimal solution is found
    for (int i = MAX_LENGTH; i >= MIN_LENGTH; --i){

    // Run Lingeling
    // for linux (may need to be altered to meet specific system requirements)
    //system("tac tmp.rev | lingeling | grep -E -v \"^c\" | tail --lines=+2 | cut --delimiter=' ' --field=1 --complement > tmp.out");
    // for OSX (may need to be altered to meet specific system requirements)
    system("gtac tmp.rev | ./lingeling | grep -E -v '^c' | gtail --lines=+2 | gcut --delimiter=' ' --field=1 --complement > tmp.out");

    // Check if solution was found
    sol.open("tmp.out");
    if (isEmpty(sol)) {
      break;
    } else{

      // print current solution to file
      vector<int> q;
      get_solution(q);
      sol.close();
      write_solution_file(q,argv[1],i);

      /************ Add new clauses needed to decrease roll height/length ************/

      // Prep file for new clauses by deleteing last line
      ifstream in("tmp.rev");
      string lastline;
      bool looping = true;
      char ch;
      in.seekg(-2,ios_base::end);
      while(looping){
          in.get(ch);
          if (ch == '\n'){
              looping = false;
          } else {
              in.seekg(-2,ios_base::cur);
          }
      }
      getline(in,lastline);
      cnf.seekp(- (lastline.size()+1),ios_base::end);

      // insert first new clause
      for (int box = 0; box < 1; ++box){
        for (int j = 0; j < xSize; ++j){
          if (j == 0) {
            // pad line
            for (int i = 0; i < lastline.size(); ++i){
                cnf << " ";
            }
          }
          clause c = -x(i-1,j,box);
          add_clause(c);
        }
      }
      // create new clauses to reduce roll length
      for (int box = 1; box < n; ++box){
        for (int j = 0; j < xSize; ++j){
          clause c = -x(i-1,j,box);
          add_clause(c);
        }
      }
      cnf << "p cnf " << n_vars << " " << n_clauses << endl;
  }
  }
}
