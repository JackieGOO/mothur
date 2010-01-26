/*
 *  listseqscommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "listseqscommand.h"
#include "sequence.hpp"
#include "listvector.hpp"

//**********************************************************************************************************************

ListSeqsCommand::ListSeqsCommand(string option){
	try {
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"fasta","name", "group", "alignreport","list","outputdir","inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (map<string,string>::iterator it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";		}
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("alignreport");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["alignreport"] = inputDir + it->second;		}
				}
				
				it = parameters.find("fasta");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["fasta"] = inputDir + it->second;		}
				}
				
				it = parameters.find("list");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["list"] = inputDir + it->second;		}
				}
				
				it = parameters.find("name");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["name"] = inputDir + it->second;		}
				}
				
				it = parameters.find("group");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["group"] = inputDir + it->second;		}
				}
			}

			//check for required parameters
			fastafile = validParameter.validFile(parameters, "fasta", true);
			if (fastafile == "not open") { abort = true; }
			else if (fastafile == "not found") {  fastafile = "";  }	
			
			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") {  namefile = "";  }	
			
			groupfile = validParameter.validFile(parameters, "group", true);
			if (groupfile == "not open") { abort = true; }
			else if (groupfile == "not found") {  groupfile = "";  }	
			
			alignfile = validParameter.validFile(parameters, "alignreport", true);
			if (alignfile == "not open") { abort = true; }
			else if (alignfile == "not found") {  alignfile = "";  }
			
			listfile = validParameter.validFile(parameters, "list", true);
			if (listfile == "not open") { abort = true; }
			else if (listfile == "not found") {  listfile = "";  }

			
			if ((fastafile == "") && (namefile == "") && (listfile == "") && (groupfile == "") && (alignfile == ""))  { mothurOut("You must provide a file."); mothurOutEndLine(); abort = true; }
			
			int okay = 1;
			if (outputDir != "") { okay++; }
			
			if (parameters.size() > okay) { mothurOut("You may only enter one file."); mothurOutEndLine(); abort = true;  }
		}

	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "ListSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

void ListSeqsCommand::help(){
	try {
		mothurOut("The list.seqs command reads a fasta, name, group, list or alignreport file and outputs a .accnos file containing sequence names.\n");
		mothurOut("The list.seqs command parameters are fasta, name, group and alignreport.  You must provide one of these parameters.\n");
		mothurOut("The list.seqs command should be in the following format: list.seqs(fasta=yourFasta).\n");
		mothurOut("Example list.seqs(fasta=amazon.fasta).\n");
		mothurOut("Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n\n");
	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "help");
		exit(1);
	}
}

//**********************************************************************************************************************

int ListSeqsCommand::execute(){
	try {
		
		if (abort == true) { return 0; }
		
		//read functions fill names vector
		if (fastafile != "")		{	inputFileName = fastafile;	readFasta();	}
		else if (namefile != "")	{	inputFileName = namefile;	readName();		}
		else if (groupfile != "")	{	inputFileName = groupfile;	readGroup();	}
		else if (alignfile != "")	{	inputFileName = alignfile;	readAlign();	}
		else if (listfile != "")	{	inputFileName = listfile;	readList();		}
		
		//sort in alphabetical order
		sort(names.begin(), names.end());
		
		if (outputDir == "") {  outputDir += hasPath(inputFileName);  }
		
		string outputFileName = outputDir + getRootName(getSimpleName(inputFileName)) + "accnos";

		ofstream out;
		openOutputFile(outputFileName, out);
		
		//output to .accnos file
		for (int i = 0; i < names.size(); i++) {
			out << names[i] << endl;
		}
		out.close();
		
		return 0;		
	}

	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************
void ListSeqsCommand::readFasta(){
	try {
		
		ifstream in;
		openInputFile(fastafile, in);
		string name;
		
		while(!in.eof()){
			Sequence currSeq(in);
			name = currSeq.getName();
			
			if (name != "") {  names.push_back(name);  }
			
			gobble(in);
		}
		in.close();		

	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "readFasta");
		exit(1);
	}
}
//**********************************************************************************************************************
void ListSeqsCommand::readList(){
	try {
		ifstream in;
		openInputFile(listfile, in);
		
		if(!in.eof()){
			//read in list vector
			ListVector list(in);
			
			//for each bin
			for (int i = 0; i < list.getNumBins(); i++) {
				string binnames = list.get(i);
				
				while (binnames.find_first_of(',') != -1) { 
					string name = binnames.substr(0,binnames.find_first_of(','));
					binnames = binnames.substr(binnames.find_first_of(',')+1, binnames.length());
					names.push_back(name);
				}
			
				names.push_back(binnames);
			}
		}
		in.close();	
		
	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "readList");
		exit(1);
	}
}

//**********************************************************************************************************************
void ListSeqsCommand::readName(){
	try {
		
		ifstream in;
		openInputFile(namefile, in);
		string name, firstCol, secondCol;
		
		while(!in.eof()){

			in >> firstCol;				
			in >> secondCol;			
			
			//parse second column saving each name
			while (secondCol.find_first_of(',') != -1) { 
				name = secondCol.substr(0,secondCol.find_first_of(','));
				secondCol = secondCol.substr(secondCol.find_first_of(',')+1, secondCol.length());
				names.push_back(name);
			}
			
			//get name after last ,
			names.push_back(secondCol);
			
			gobble(in);
		}
		in.close();
		
	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "readName");
		exit(1);
	}
}

//**********************************************************************************************************************
void ListSeqsCommand::readGroup(){
	try {
	
		ifstream in;
		openInputFile(groupfile, in);
		string name, group;
		
		while(!in.eof()){

			in >> name;				//read from first column
			in >> group;			//read from second column
			
			names.push_back(name);
					
			gobble(in);
		}
		in.close();

	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "readGroup");
		exit(1);
	}
}

//**********************************************************************************************************************
//alignreport file has a column header line then all other lines contain 16 columns.  we just want the first column since that contains the name
void ListSeqsCommand::readAlign(){
	try {
	
		ifstream in;
		openInputFile(alignfile, in);
		string name, junk;
		
		//read column headers
		for (int i = 0; i < 16; i++) {  
			if (!in.eof())	{	in >> junk;		}
			else			{	break;			}
		}
		
		
		while(!in.eof()){

			in >> name;				//read from first column
			
			//read rest
			for (int i = 0; i < 15; i++) {  
				if (!in.eof())	{	in >> junk;		}
				else			{	break;			}
			}
			
			names.push_back(name);
					
			gobble(in);
		}
		in.close();

		
	}
	catch(exception& e) {
		errorOut(e, "ListSeqsCommand", "readAlign");
		exit(1);
	}
}
//**********************************************************************************************************************
