using namespace std;

#include "hmm.hpp"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>



Model::Model(int stateCount) 
  : N(stateCount) 
{
  AllocateMemory();
}

Model::Model(char *modelFile) 
{
  std::ifstream ifs(modelFile);
  if (ifs.fail()) {
    std::cerr << "Failed to open the model file:"<< modelFile << endl;
    exit(1);
  }
  ifs >> N; // the first is the number of states
  AllocateMemory();

  char str[1000];
  int entryCount;

  // load initial state prob
  ifs >> str >> entryCount;
  if (strcmp(str, "InitPr")) {
    std::cerr << "Error: InitPr expected in model file\n";
  }
  int i;
  for (i=0; i<N; i++) {
    I[i]=0;
  }
  int s;
  double pr;
  for (i=0; i<entryCount; i++) {
    ifs >> s >> pr;
    I[s]=pr;
  }

  // load output prob
  ifs >> str >> entryCount;
  if (strcmp(str, "OutputPr")) {
    std::cerr << "Error: OutputPr expected in model file\n";
  }
  int j;
  for (i=0; i<N; i++) {
    for (j=0; j<SYMNUM; j++) {
      B[j][i] = 0;
    }
  }
  char sym;
  for (i=0; i<entryCount; i++) {
    ifs >> s >> sym >> pr;
    B[sym-baseChar][s]=pr;
  }


  // load state transition prob
  ifs >> str >> entryCount;
  if (strcmp(str, "TransPr")) {
    std::cerr << "Error: TransPr expected in model file\n";
  }
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      A[i][j] = 0;
    }
  }
  int s1;
  for (i=0; i<entryCount; i++) {
    ifs >> s >> s1 >> pr;
    A[s][s1]=pr;
  }
  
}


void Model::AllocateMemory()
{
  I = new double[N];
  A = new double *[N];
  B = new double *[94];
  int i;
  for (i=0; i< N; i++) {
    A[i] = new double[N];
  }
  for (i=0; i< SYMNUM; i++) {
    B[i] = new double[N];
  }

}



Model::~Model() 
{ 
  int i;
  for (i=0; i< N; i++) {
    delete [] A[i];
  }
  for (i=0; i< SYMNUM; i++) {
    delete [] B[i];
  }
  delete [] A;
  delete [] B;
  delete [] I;
}



// Decoding function: You need to complete two assignment statements

void Model::Decode(char *seqFile)
{
  // a node in the path
  struct PathNode {
    int s; // the state 
    PathNode *prev; // the previous state in transition
  };

  // information related to a state needed for Viterbi iteration
  struct TrellisNode {
    PathNode *path; // the most likely path that has led to this state
    double pr; // the probability of the "path"
    double newPr; // a buffer to store the probability for the new extended path
  };
  
  PathNode **curBest = new PathNode *[N]; 
  // A buffer for constructing the best path to each state

  TrellisNode *trellis = new TrellisNode[N]; // Current trellis information

  int i;
  ifstream seq(seqFile);
  if (seq.fail()) {
    std::cerr << "can't open sequence file : "<< seqFile << endl;
    exit(1);
  }

  char c;  // store the symbol
  seq >> c;
  int cIndex = c- baseChar;
  // use cIndex to access matrix B, e.g., B[cIndex][i] gives you
  // the probability of generating symbol c from state i
  for (i=0; i<N; i++) {
    //

    // ===== Uncomment and complete the following assignment statement to compute the 
    // probability of starting at state i, i.e., p(i) * p(c|s_i)
    // working on the logorithm domain to avoid overflow.
    
    trellis[i].pr = I[i] * B[cIndex][i];


    trellis[i].path = new PathNode();  // construct a path node for this state
    trellis[i].path->s = i;  // record the state
    trellis[i].path->prev = NULL; // prev=NULL means that it's the very first node
  }


  int j;
  while (seq >> c) {
    int cIndex = c- baseChar;
    // use cIndex to access matrix B, e.g., B[cIndex][i] gives you
    // the probability of generating symbol c from state i


    // =========================================================
    // The code in this loop is to grow 
    // the best path for each state according to the Viterbi algorithm
    // Suppose the sequence already processed is <c1, c2, ..., ck>.
    // For the next character c, we will update each of
    // the trellis node i.e., trellis[i], so that it will hold 
    // the state transition path ending at state i that most likley 
    // "generates" the
    // sequence <c1, ..., ck, c>. Remember trellis[i].path was supposed 
    // to hold the most likely path for <c1, ..., ck> that ends at state i
    //  and trellis[i].pr was supposed to hold the log of probability of data given
    // the path.

    for (i=0; i<N; i++) { // We now consider a path ending at each of the N states
      int bestFromState; // temporary variable to store the best "from-state", i.e.,
      // the best previous path's ending state.
      double bestPr;  // temporary variable to help select the best path
      for (j=0; j<N; j++) { // consider all previous states
	double thisPr;

	// the variable thisPr would hold the log of probability of 
	// <c1,...,ck, c> given that we follow the path stored in
	// trellis[j].path for generating <c1,...,ck> and we go from
	// state j to state i to generate c.

	thisPr = trellis[j].pr * A[trellis[j].path->s][i] * B[cIndex][i]; 

	if (j==0 || thisPr > bestPr) { // keep track of the best "from state".
	  bestFromState = j;
	  bestPr = thisPr;
	}
      }


      // Now bestFromState can tell us "previous path" works the best with our current
      // state i, and bestPr tells us the probability of the data given this best path. 
      trellis[i].newPr = bestPr;
      // Note that the newPr field is needed for temporarily storing the 
      // probability for the new best path you constructed. (Other states may
      // still need the probability for the old path (stored in the field pr)
      // in order to compute the new probabilities for their new best paths.
      curBest[i] = new PathNode();
      curBest[i]->prev = trellis[bestFromState].path;
      curBest[i]->s = i;
      // Note that the auxilary array curBest is necessary for temporarily storing
      // the constructed best path, so that it won't mess up with the old best
      // path which may be needed for constructing new paths for other states.
    }
    // now update pr
    for (i=0; i<N; i++) {
      trellis[i].pr = trellis[i].newPr;  // update trellis[i].pr so that it
      // now has p(<c1,....,ck,c>|path_new); before it had p(<c1,...,ck>|path_old)
      trellis[i].path = curBest[i]; // record the new, extended path

      // The following code, if uncommented, would print out the path stored in trellis[i].path
      // This may be useful for debugging...
      /cerr << "stop at "<< i << " :";
         PathNode *p = trellis[i].path;
         while (p) {
         cerr << " "<< p->s;
         p = p->prev;
         }
         cerr << " =>"<< trellis[i].pr << " "<< endl;
      
    }
    
  }


  // Now, we only need to take the best trellis, i.e., the one with the highest
  // probability (trellis[i].pr), and print out the path according to trellis[i].path.
  // ========= the following code in this function is complete ============
  ifstream seq2(seqFile); 
  if (seq2.fail()) { 
    std::cerr << "can't open sequence file : "<< seqFile << endl; 
    exit(1); 
  }

  vector<int> bestPath;
  bestPath.clear();
  int bestTrellis;
  double bestTrellisPr = 0;
  for (int i = 0; i<N; i++ ){
    if (i==0 || (trellis[i].pr > bestTrellisPr)) {
      bestTrellis =i;
      bestTrellisPr = trellis[i].pr;
    }
  }
  PathNode *p = trellis[bestTrellis].path;
  while (p) {
    bestPath.push_back(p->s);
    p = p->prev;
  }
  vector<int>::reverse_iterator it;
  it=bestPath.rbegin(); 
  while (seq2 >> c)  {
    if (it!=bestPath.rend()) {
      cout << c << " "<< *it << endl;
      it++;
    } else {
      std::cerr << "mismatched path and sequence\n";
    }
  }
  
  delete [] trellis;
}




