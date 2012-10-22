#ifndef WEIGHTED_H
#define WEIGHTED_H


/*
 *  weighted.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "treecalculator.h"
#include "counttable.h"

/***********************************************************************/

class Weighted : public TreeCalculator  {
	
	public:
        Weighted( bool r) : includeRoot(r) {};
		~Weighted() {};
		
		EstOutput getValues(Tree*, string, string);
		EstOutput getValues(Tree*, int, string);
		
	private:
		struct linePair {
			int start;
			int num;
			linePair(int i, int j) : start(i), num(j) {}
		};
		vector<linePair> lines;

		EstOutput data;
		map<string, int>::iterator it;
		map<string, double> WScore; //a score for each group combination i.e. AB, AC, BC.
		int processors;
		string outputDir;
		map< vector<string>, set<int> > rootForGrouping;  //maps a grouping combo to the root for that combo
		bool includeRoot;
		
		EstOutput driver(Tree*, vector< vector<string> >, int, int, CountTable*); 
		EstOutput createProcesses(Tree*, vector< vector<string> >, CountTable*);
		double getLengthToRoot(Tree*, int, string, string);
};

/***********************************************************************/


#endif
