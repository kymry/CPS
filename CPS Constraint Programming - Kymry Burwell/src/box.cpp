#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include "math.h"

using namespace Gecode;
using namespace std;

class box : public Space {
protected:
  int width; // Width of paper roll
  int height; // Height of paper roll (sum of max(x,y) of each box)
  IntVarArray xtl; // top left x-coordinate of all boxes
  IntVarArray ytl; // top left y-coordinate of all boxes
  IntVarArray xbr; // bottom right x-coordinate of all boxes
  IntVarArray ybr; // bottom right y-coordinate of all boxes
  IntVarArray xw; // Width of the box - Allows the box to be placed horizontally or vertically
  IntVarArray yh; // Height of the box - Allows the box to be placed horizontally or vertically
  vector<int> tempx; // Temporarily holds the x dimensions of the boxes
  vector<int> tempy; // Temporarily holds the y dimensions of the boxes

public:
  int numVars; // Number of boxes
  box(int n, int w, int h, vector<int> xDimTemp, vector<int> yDimTemp) :
          width(w), height(h), xtl(*this, n, 0, w), ytl(*this, n, 0, h),
          xbr(*this, n, 0, w), ybr(*this, n, 0, h), xw(*this,n ,0 ,w), yh(*this, n, 0, h), tempx(xDimTemp), tempy(yDimTemp), numVars(n) {

  /*************** Primary Constraints ****************/
  // (1) Boxes can be placed horizontally or vertically and dimensions of boxes are correct
  for (int i = 0; i < numVars; i++){
      IntArgs a(2, tempx[i],tempy[i]);
      IntSet d(a);
      dom(*this, xw[i], d); // xw is the set of widths that are accepted for a specific box - the domain always has size 2
      dom(*this, yh[i], d); // yh is the set of heights that are accepted for a specific box - the domain always has size 2
      rel(*this, (xw[i] == tempx[i]) >> (yh[i] == tempy[i])); // Sets a relations on the width and height to ensure the box is correct dimensions when it is flipped
      rel(*this, (xw[i] == tempy[i]) >> (yh[i] == tempx[i]));  // Sets a relations on the width and height to ensure the box is correct dimensions when it is flipped
  }

  // (2) Boxes cannot overlap
  nooverlap(*this, xtl, xw, xbr, ytl, yh, ybr, IPL_BND);

  // (3) Box sizes are correct
  for (int i = 0; i < numVars; i++){
    rel(*this, xtl[i] + xw[i] == xbr[i]); // Bottom-right x coordinate is equal to top-right plus width
    rel(*this, ytl[i] + yh[i] == ybr[i]); // Bottom-right y coordinate is equal to top-right plus height
  }

  // (4) Dimensions of the roll are not exceeded
  for (int i = 0; i < numVars; i++){
    rel(*this, ybr[i] <= height);
    rel(*this, xtl[i] <= width-xw[i]);
  }

/*************** Symmetry Breaking ****************/
// All boxes of the same size are interchangable
for (int i = 0; i < numVars; i++){
    IntVarArgs temp(2); temp[0]=xtl[i]; temp[1]=ytl[i];
    for (int j = i+1; j <numVars; j++){
        if ( (tempx[i]+tempy[i]) == (tempx[j]+tempy[j]) ){
            rel(*this, (temp[0]+temp[1]) < (xtl[j]+(100*ytl[j])));
            temp[0]=xtl[j]; temp[1]=ytl[j];
        }
    }
}

/*************** Branching ****************/
  Rnd r(1U); // Random variable used in value selection
  branch(*this, xtl, tiebreak(INT_VAR_DEGREE_MAX(),INT_VAR_SIZE_MIN()),INT_VAL_RND(r)); // Variable: Tiebreak with max degree and min domain. Value: rand
  branch(*this, ytl, tiebreak(INT_VAR_DEGREE_MAX(),INT_VAR_SIZE_MIN()),INT_VAL_MIN()); // Variable: Tiebreak with max degree and min domain. Value: min
  branch(*this, xbr, tiebreak(INT_VAR_DEGREE_MAX(),INT_VAR_SIZE_MIN()),INT_VAL_RND(r)); // Variable: Tiebreak with max degree and min domain. Value: rand
  branch(*this, ybr, tiebreak(INT_VAR_DEGREE_MAX(),INT_VAR_SIZE_MIN()),INT_VAL_MIN()); // Variable: Tiebreak with max degree and min domain. Value: min
}

/*************** Constrain Function ****************/
  // Uses the branch and bound search algorithm and only returns improved results
  virtual void constrain(const Space& _b) {
    const box& b = static_cast<const box&>(_b);
    IntVarArgs ybr_old = b.ybr;
    IntVarArgs ybr_new = ybr;
    int max = 0;
    for (int i = 0; i < numVars; i++) {
      if (ybr_old[i].val() > max) max = ybr_old[i].val();
    }
    for (int i = 0; i < numVars; i++){
      rel(*this, ybr_new[i], IRT_LE, max); // All new solution bottom-right y-coordinates must be strictly less than previous solution
    }
  }

box(box& s) : Space(s) {
  xtl.update(*this, s.xtl);
  ytl.update(*this, s.ytl);
  xbr.update(*this, s.xbr);
  ybr.update(*this, s.ybr);
  numVars = s.numVars;
}

virtual Space* copy(void) {
  return new box(*this);
}

// Prints out the solution
void print(void) const {
  cout << "xtl: ";
  for (int i = 0; i < numVars; i++) cout << (xtl[i].val()) << " ";
  cout  << endl << "ytl: ";
  for (int i = 0; i < numVars; i++) cout << (ytl[i].val()) << " ";
  cout << endl << "xbr: ";
  for (int i = 0; i < numVars; i++) cout << ((xbr[i].val())-1) << " ";
  cout << endl  << "ybr: ";
  for (int i = 0; i < numVars; i++) cout << ((ybr[i].val())-1) << " ";
  cout << endl << endl;
}

// Returns best solution thus far
vector<int> getSol() {
  // Calculate the total length
  int length = 0;
  for (int i = 0; i < numVars; i++) if (ybr[i].val() > length) length = ybr[i].val();
  // Add solution to vector
  vector<int> solution;
  solution.push_back(length);
  for (int i = 0; i < numVars; i++){
    solution.push_back(xtl[i].val());
    solution.push_back(ytl[i].val());
    solution.push_back(xbr[i].val());
    solution.push_back(ybr[i].val());
  }
  return solution;
}

};

int main(int argc, char* argv[]) {
  int w = 0, n = 0; /// w is width, n is number of boxes//
  vector<int> xDimension; // holds x dimenions of boxes
  vector<int> yDimension; // holds y dimensions of boxes

  /* For manual input only
  string inputFile = "/Google Drive/UPC/Spring 2018/CPS/Constraint Programming/First Assignment/instances/bwp_11_13_1.in";
  string outputFile = "/Google Drive/UPC/Spring 2018/CPS/Constraint Programming/First Assignment/instances/bwp_11_13_1_CP.out";
  */
  
  // Input and output file configuration
  string inputFile = argv[1];
  string outputFile = inputFile;
  outputFile.erase(outputFile.end()-3, outputFile.end());
  outputFile.append("_CP.out");

  // Read file into program
  ifstream input(inputFile);
  vector<int> tempData;
  istream_iterator<int> iter(input), eof;
  while (iter != eof) tempData.push_back(*iter++);
  w = tempData[0];
  for (auto i = tempData.begin()+1; i < tempData.end(); i+=3){
    n += *i;
    for (int j = 0; j < *i; j++){
      xDimension.push_back(*(i+1));
      yDimension.push_back(*(i+2));
    }
  }

  // Determine the max height of the roll (Not strictly necessary, but a good addition to restrict height)
  int maxHeight = 0;
  for (int i = 0; i < n; i++){
    (xDimension[i] > yDimension[i]) ? (maxHeight += xDimension[i]) : (maxHeight += yDimension[i]);
  }

  // Determine the theoretical minimum length
  double area = 0;
  for (double i = 0; i < n; i++) area += xDimension[i]*yDimension[i];
  int minLength = ceil(area/w);

  // Use Branch and Bound to search solution space, only returning improved solutions
  box* m = new box(n, w,maxHeight,xDimension,yDimension);
  BAB<box> e(m);
  delete m;
  vector<int> solution;
  while (box* s = e.next()) {
    solution = s->getSol();
    s->print();
    if (solution[0] <= minLength){
      delete s;
      break;
    }
    delete s;
  }
  // Output results to file after best solution is found
  ofstream output(outputFile);
  output << (solution[0]) << '\n';
  for (int i = 1 ; i < solution.size(); i+=4){
    output << solution[i] << " " << solution[i+1] << "  " << (solution[i+2]-1)
           << " " << (solution[i+3]-1) << '\n';
  }
  return 0;
}
