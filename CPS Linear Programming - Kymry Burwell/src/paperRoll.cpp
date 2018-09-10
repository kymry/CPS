#include <ilcplex/ilocplex.h>
#include <vector>
#include "math.h"
#include <cmath>

ILOSTLBEGIN

/************ Configure Input Data and Variables ************/
// Input Data Declaration
IloInt w; // width of grid
IloInt h; // height of grid
IloInt grid; // grid size
IloInt numB; // number of boxes
IloInt M = 10000; // used for big M trick
IloNumArray xDim; // x-dimensions of boxes
IloNumArray yDim; // y-dimensions of boxes
IloInt minLength; // theoretical minimum length of roll

// Decision Variable Declaration
IloNumVarArray b; // markers for box positions
IloNumVarArray tl; // markers for top-left coordinates of boxes
IloNumVarArray boolOr; // used as OR statement in constraints 4 and 5
IloNumVar length; // length of paper roll

string inputFile;

// Function to read data from file
static void configureData(const char* filename, const IloEnv env) {

  // Input and output file configuration
  inputFile = filename;
  string outputFile = inputFile;
  outputFile.erase(outputFile.end()-3, outputFile.end());
  outputFile.append("_LP.out");

  // Read file into program
  ifstream input(inputFile);
  vector<int> tempData;
  istream_iterator<int> iter(input), eof;
  while (iter != eof) tempData.push_back(*iter++);
  w = tempData[0];
  for (auto i = tempData.begin()+1; i < tempData.end(); i+=3) {numB += *i;}
  xDim = IloNumArray(env,numB);
  yDim = IloNumArray(env,numB);
  IloInt counter = 0;
  // Box Dimensions
  for (auto i = tempData.begin()+1; i < tempData.end(); i+=3){
    for (int j = 0; j < *i; j++){
      xDim[counter] = (*(i+1));
      yDim[counter] = (*(i+2));
      counter++;
    }
  }

  // Calculate paper roll height
  for (int i = 0; i < numB; i++){ (xDim[i] > yDim[i]) ? (h += xDim[i]) : (h += yDim[i]);}
  grid = w*h;

  // Determine the theoretical minimum length
  double area = 0;
  for (double i = 0; i < numB; i++) area += xDim[i]*yDim[i];
  minLength = ceil(area/w);
}

// Utility functions to retrieve variables from grid
IloNumVar getB(IloInt x, IloInt y, IloInt box) {return b[x + (y*w) + grid*(box-1)];}
IloNumVar getTL(IloInt x, IloInt y, IloInt box) {return tl[x + (y*w) + grid*(box-1)];}
int getGrid(IloInt x, IloInt y) {return (x + (y*w));}


int main (int argc, char* argv[]) {

/************ Setup Environment ************/
  IloEnv             env;
  IloModel     model(env);
  configureData(argv[1],env);

  b = IloNumVarArray(env, (grid*numB), 0, 1, ILOBOOL); // indicate grid points box ocupies
  tl = IloNumVarArray(env, (grid*numB), 0, 1, ILOBOOL); // indicate starting coordinate of box
  boolOr = IloNumVarArray(env, numB, 0, 1, ILOBOOL); // auxiliary variable for box rotation
  length = IloNumVar(env,1,700,ILOINT);


/************ Post Constraints ************/
  // (1) Each box has one starting location (i.e. one top-left coordinate per box)
  for (IloInt i = 1; i <= numB; i++){
    IloExpr expr(env);
    for (IloInt yc = 0; yc < h; yc++) {
      for (IloInt xc = 0; xc < w; xc++){ expr += getTL(xc,yc,i);}
    }
    model.add(expr == 1);
    expr.end();
  }

  // (2) Each box has correct number of occupied grid points (ex. 2x2 box occuppies exactly 4 grid points)
  for (IloInt i = 1; i <= numB; i++){
    IloExpr expr(env);
    for (IloInt yc = 0; yc < h; yc++) {
      for (IloInt xc = 0; xc < w; xc++){
        expr += getB(xc,yc,i);
      }
    }
    model.add(expr == (xDim[i-1]*yDim[i-1]));
    expr.end();
  }


 // (3) Boxes don't overlap
 for (IloInt i = 0; i < grid; i++){
   IloExpr expr(env);
   IloExpr expr1(env);
   for (IloInt j = 0; j < numB; j++) {
     expr += b[j*grid + i];
     expr1 += tl[j*grid + i];

   }
   model.add(expr <= 1);
   model.add(expr1 <= 1);
   expr.end();
   expr1.end();
 }

 // (4 and 5) Boxes can rotate AND must adhere to proper dimensions
 for (IloInt box = 1; box <= numB; box++){
   for (IloInt yc = 0; yc < h; yc++){
     for (IloInt xc = 0; xc < w; xc++){
       // Boxes horizontal
       IloExpr expr(env);
       for (IloInt i = 0; (i < yDim[box-1]) && (i+yc < h); i++){
         for (IloInt j = 0; (j < xDim[box-1]) && (j+xc < w); j++){
           expr += getB(xc+j,yc+i,box);
         }
       }
       model.add(expr + (boolOr[box-1]*M) >= getTL(xc,yc,box) * xDim[box-1] * yDim[box-1]);
       expr.end();

      // Boxes Vertical
      IloExpr expr1(env);
      for (IloInt i = 0; (i < yDim[box-1]) && (i+xc < w); i++){
        for (IloInt j = 0; (j < xDim[box-1]) && (j+yc < h); j++){
          expr1 += getB(xc+i,yc+j,box);
        }
      }
      model.add(expr1 + ((1-boolOr[box-1])*M) >= getTL(xc,yc,box) * xDim[box-1] * yDim[box-1]);
      expr1.end();
     }
   }
 }

 // (6) Symmetry Breaking: Boxes of same dimensions must be placed in increasingly higher grid positions
 for (IloInt i = 1; i <= numB; i++){
   IloInt tempX = xDim[i-1], tempY = yDim[i-1];
   for (IloInt j = i+1; j <= numB; j++){
     if ((xDim[j-1] == tempX) && (yDim[j-1] == tempY)){
       IloExpr expr(env);
       IloExpr expr1(env);
       for (IloInt yc = 0; yc < h; yc++) {
         for (IloInt xc = 0; xc < w; xc++){
             expr += (getTL(xc,yc,i)*(yc*w+xc));
             expr1 += (getTL(xc,yc,j)*(yc*w+xc));
          }
        }
       model.add(expr <= expr1);
       expr.end();
       expr1.end();
       break;
   }
 }
}

// (7) Length constraint is adhered to
for (IloInt i = 1; i <= numB; i++){
  for (IloInt yc = 0; yc < h; yc++) {
    for (IloInt xc = 0; xc < w; xc++){
      model.add(length >= (yc+1)*getB(xc,yc,i));
    }
  }
}

// (8) Length is atleast the theoretical minimum
model.add(length >= minLength);


/************ Objective Function ************/
  model.add(IloMinimize(env, length));
  IloCplex cplex(model);
  cplex.solve();

/************ Print Solution ************/
  // Print solution in form: (x-start,y-start) (x-end,y-end)
  vector<int> solution;
  solution.push_back(cplex.getObjValue());
  int startCoordinate = 1000, endCoordinate = 1000, counter = 0;
  for (IloInt i = 0; i < grid*numB; i++){
    if (cplex.getValue(b[i] != 0)){
      if (startCoordinate == 1000) startCoordinate = counter;
      endCoordinate = counter;
    }
    counter++;
    if (counter == grid){
      solution.push_back(startCoordinate % w);
      solution.push_back(floor(startCoordinate/w));
      solution.push_back(endCoordinate % w);
      solution.push_back(floor(endCoordinate/w));
      cout << (startCoordinate % w) << " " << floor(startCoordinate/w) << " ";
      cout << (endCoordinate % w) << " " << floor(endCoordinate/w) << endl;
      counter = 0, startCoordinate = 1000;
    }
  }

  // Output solution to file
  string outputFile = inputFile;
  outputFile.erase(outputFile.end()-3, outputFile.end());
  outputFile.append("_LP.out");
  ofstream output(outputFile);
  output << (solution[0]) << '\n';
  for (int i = 1 ; i < solution.size(); i+=4){
    output << solution[i] << " " << solution[i+1] << "  " << (solution[i+2]) << " " << (solution[i+3]) << '\n';
  }

  // Output objective value
  cout << "Minimum length: " << cplex.getObjValue() << endl;
  env.end();
  return 0;
}
