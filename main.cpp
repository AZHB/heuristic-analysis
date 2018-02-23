#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

class BooleanFormula
{
public:
	BooleanFormula(int, int);
	
	int numVariables;
	int numClauses;
	std::vector<int> variables;
	
	std::vector<std::list<int> > clauses;
	int SatisfiedClauses(const std::vector<bool>& truthAssignment);
	bool Evaluate(const std::vector<bool>& truthAssignment);
	std::ostream& print(std::ostream&);	
	std::ostream& filePrint(std::ostream&);
};

BooleanFormula::BooleanFormula(int c, int v) : numVariables(v), numClauses(c)
{
	//Construct a vector of size 2v holding all possible variables and their negation
	variables = *(new std::vector<int>(2*numVariables));
	int i=1;
	for(std::vector<int>::iterator it=variables.begin();it!=variables.end();++it)
	{
		*it = i;
		i++;
	}
	
	//Construct a vector (of lists) of size c for holding all clauses
	clauses = *(new std::vector<std::list<int> >(numClauses));
	
	//Randomly generate each of the clauses
	std::vector<std::list<int> >::iterator clause_it = clauses.begin();
	for(;clause_it!=clauses.end();++clause_it)
	{
		//Construct a clause (list of ints) of size in interval [1,2*numVariables]
		int clause_size = rand() % (2*numVariables) + 1;
		*clause_it = *(new std::list<int>(clause_size));
		
		//Copy formula variables into new vector (we can remove variables from this vector with no worries now)
		std::vector<int> unusedVariables = *(new std::vector<int>(variables));
		
		//Randomly choose variables from unusedVariables until clause is full
		std::list<int>::iterator var_it = clause_it->begin();
		for(;var_it!=clause_it->end();++var_it)
		{
			int chosenVar = rand() % unusedVariables.size();
			*var_it = unusedVariables[chosenVar];
			unusedVariables.erase(unusedVariables.begin()+chosenVar);
		}
	}
}

int BooleanFormula::SatisfiedClauses(const std::vector<bool>& truthAssignment)
{
	int satisfiedClauses = 0;
	std::vector<std::list<int> >::iterator clause_it = clauses.begin();
	for(;clause_it!=clauses.end();++clause_it)
	{
		std::list<int>::iterator var_it = clause_it->begin();
		for(;var_it!=clause_it->end();++var_it)
			{
				if (*var_it<=numVariables)
				{
					if (truthAssignment[(*var_it)-1] == true)
					{
						satisfiedClauses++;
						break;
					}
				}
				else
				{
					if (truthAssignment[(*var_it)-numVariables-1] == false)
					{
						satisfiedClauses++;
						break;	
					}
				}
			}
	}	
	return satisfiedClauses;
}

std::ostream& BooleanFormula::print(std::ostream& o)
{
	//print variables
	o << "Variables: ";
	for(std::vector<int>::iterator var_it = variables.begin(); var_it!=variables.end(); var_it++)
	{
		o << *var_it << " ";	
	}
	o << std::endl;
	
	//Print formula
	std::vector<std::list<int> >::iterator clause_it = clauses.begin();
	std::vector<std::list<int> >::iterator clauseStop_it = --clauses.end();
	for(;clause_it!=clauses.end();++clause_it)
	{
		o << "(";
		std::list<int>::iterator var_it = clause_it->begin();
		std::list<int>::iterator varStop_it = --clause_it->end();
		for(; var_it!=clause_it->end();var_it++)
		{
			o << *var_it;
			if(var_it!=varStop_it)
				o << "-";
		}
		o << ")"; 
			
		if(clause_it!=clauseStop_it)
				o << "&";
	}
	o << std::endl;
}

//Writes file using latex syntax
std::ostream& BooleanFormula::filePrint(std::ostream& file)
{
	std::list<std::string> outputBuffer;
	
	std::vector<std::list<int> >::iterator clause_it = clauses.begin();
	std::vector<std::list<int> >::iterator clauseStop_it = --clauses.end();
	for(;clause_it!=clauses.end();++clause_it)
	{
		outputBuffer.push_back("( ");
		std::list<int>::iterator var_it = clause_it->begin();
		std::list<int>::iterator varStop_it = --clause_it->end();
		for(;var_it!=clause_it->end();++var_it)
		{
			int varNumber = 0;
			std::string variableString;
			if(*var_it <= numVariables)
			{
				varNumber = *var_it;
				variableString = "x_{";
			}
			else
			{
				varNumber = *var_it - numVariables;
				variableString = "\\overline{x}_{";
			}
			char numberCharArray[sizeof(int)*8+1];
			itoa(varNumber,numberCharArray,10);
			std::string numberString(numberCharArray);
			numberString.push_back('}');
			numberString.push_back(' ');
			outputBuffer.push_back(variableString.append(numberString)); //"x_" + varNumber
			if (var_it!=varStop_it)
				outputBuffer.push_back("\\lor ");
		}
		outputBuffer.push_back(") ");
		if (clause_it!=clauseStop_it)
			outputBuffer.push_back("\\land ");
	}
	
	int newlineCounter=0;
	file << "$$ ";
	while(!outputBuffer.empty())
	{
		if(newlineCounter == 40)
		{
			file << "$$\n$$ ";
			newlineCounter=0;
		}
		file << *outputBuffer.begin();
		outputBuffer.pop_front();
		newlineCounter++;
	}
	
}

//NOTE variables[n-1] is the nth variable
bool BooleanFormula::Evaluate(const std::vector<bool>& truthAssignment)
{
	std::vector<std::list<int> >::iterator clause_it = clauses.begin();
	for(;clause_it!=clauses.end();++clause_it)
	{
		bool bClauseSatisfied = false;
		std::list<int>::iterator var_it = clause_it->begin();
		for(;var_it!=clause_it->end();++var_it)
			{
				if (*var_it<=numVariables)
				{
					if (truthAssignment[(*var_it)-1] == true)
					{
						bClauseSatisfied = true;
						break;
					}
				}
				else
				{
					if (truthAssignment[(*var_it)-numVariables-1] == false)
					{
						bClauseSatisfied = true;
						break;	
					}
				}
			}
		if (bClauseSatisfied == false)
			return false;
	}
	return true;
}

bool OneFlipLocalSearch(BooleanFormula& bF, std::vector<bool> &out)
{
	//Generate initial truth assignment ('all true')
	std::vector<bool> truthAssignment = *(new std::vector<bool>(bF.numVariables));
	for (int i=0; i<bF.numVariables;i++)
	{
		truthAssignment[i]=true;		
	}
	
	int currentNumberSatisfied = bF.SatisfiedClauses(truthAssignment);
	//Main algorithm loop
	while(1)
	{
		//Check if truth assignment is optimal
		if (bF.Evaluate(truthAssignment))
		{
			out = truthAssignment;
			return true;		 
		}
		 
		std::vector<bool> bestAssignment;
		int bestNumberSatisfied = currentNumberSatisfied;
		for(int bitFlipCounter=0; bitFlipCounter<bF.numVariables; ++bitFlipCounter)
		{
			std::vector<bool> flippedAssignment = truthAssignment;
			flippedAssignment[bitFlipCounter] = !flippedAssignment[bitFlipCounter];
			
			//Check if flipped assignment is better
			int performance =bF.SatisfiedClauses(flippedAssignment); 
			if (performance > bestNumberSatisfied)
			{
				bestAssignment = flippedAssignment;
				bestNumberSatisfied = performance;
			}
		}		
		
		//If improvement has been made then set the best truth assignment to current for next loop
		if (bestNumberSatisfied > currentNumberSatisfied)
		{
			truthAssignment = bestAssignment;
			currentNumberSatisfied = bestNumberSatisfied;	
		}
		//If no improvement has been made then terminate with local maxima
		else
		{
			out = truthAssignment;
			return false;
		}
	}
}

bool TotalSearch(BooleanFormula& bF, std::vector<bool> &out)
{
	//Preprocessing
	//Find unit clauses (index and contents)
	std::vector<int> unitClausesIndex;
	std::vector<int> units;
	for(int i=0; i<bF.clauses.size(); ++i)
	{
		if(bF.clauses[i].size()==1)
		{
			unitClausesIndex.push_back(i);
			units.push_back(*(bF.clauses[i].begin()));	
		}
	}
	//Clear duplicates from units vector
	std::sort(units.begin(),units.end());
	std::unique(units.begin(),units.end());
	//Check there are no conflicting unit clauses (if so, formula is unsatisfiable)
	for(std::vector<int>::iterator anchor=units.begin();anchor!=units.end();++anchor)
	{
		std::vector<int>::iterator scanner=anchor;
		++scanner;
		for(; scanner!=units.end();++scanner)
		{
			if ((*scanner)+(*anchor) == (2*bF.numVariables)+1)
				return false;
		}
	}
	//Reduce the boolean formula to a smaller problem
		
	
	//Final search algorithm
	int n = pow(2,bF.numVariables);
	std::vector<bool> currentAssignment(bF.numVariables);
	for(int k=0;k<n;++k)
	{
		//Generate next boolean string
		for (int j=0;j<bF.numVariables;++j)
		{
			currentAssignment[j] = (bool)((int)(k/pow(2,j))%2);
		}
		
		//Check if generated string satisfies boolean formula
		if (bF.Evaluate(currentAssignment))
			return true;
	}
	return false;
}

int main(int argc, char** argv) {
	srand(time(0));
	
//  Writing running time of one flip local search to a file
	std::ofstream fileOut;
	fileOut.open("OneFlipRuntimeBig.txt", std::ios::trunc);
	int n = 1000;
	int maxC = 1000;
	int v = 10;
	std::vector<bool> empty;
	for (int c=0; c<maxC; ++c)
	{
		int result=0;
		int successful=0;
		float cumulativeRuntime = 0;
		for(int t=0; t<n; ++t)
		{
			BooleanFormula b(c+1,v);
			clock_t run;
			run = clock();
			OneFlipLocalSearch(b,empty);
			run = run - clock();
			cumulativeRuntime += ((float)run)/CLOCKS_PER_SEC;
		}
		float totalRuntime  =cumulativeRuntime/((float)n);
		fileOut << totalRuntime << " ";
		std::cout << c << " clauses completed. Total runtime: " << totalRuntime << " \n";
	}
	
//  Writing running time of total search to a file
//	std::ofstream fileOut;
//	fileOut.open("TotalSearchRuntimeNoProcess.txt", std::ios::trunc);
//	int n = 1000;
//	int maxC = 100;
//	int v = 10;
//	std::vector<bool> empty;
//	for (int c=0; c<maxC; ++c)
//	{
//		int result=0;
//		int successful=0;
//		float cumulativeRuntime = 0;
//		for(int t=0; t<n; ++t)
//		{
//			BooleanFormula b(c+1,v);
//			clock_t run;
//			run = clock();
//			TotalSearch(b,empty);
//			run = run - clock();
//			cumulativeRuntime += ((float)run)/CLOCKS_PER_SEC;
//		}
//		float totalRuntime  =cumulativeRuntime/((float)n);
//		fileOut << totalRuntime << " ";
//		std::cout << c << " clauses completed. Total runtime: " << totalRuntime << " \n";
//	}
	
// Writing a clause to a text file in latex notation
//	int c = 50;
//	int v = 10;
//	std::vector<bool> empty;
//	BooleanFormula b(c,v);
//	std::ofstream clauseOutput;
//	clauseOutput.open("clause.txt", std::ios::trunc);
//	b.filePrint(clauseOutput);
//	std::ofstream answerOutput;
//	answerOutput.open("answer.txt", std::ios::trunc);
//	answerOutput << OneFlipLocalSearch(b,empty) << "\n";
//	for(std::vector<bool>::iterator it=empty.begin();it!=empty.end();++it)
//	{
//		answerOutput << *it << " ";
//	}

//Writing success rate of 1-flip vs. total search to a file
//	std::ofstream fileOut;
//	fileOut.open("successRateRevised.txt", std::ios::trunc);
//	int n = 1000;
//	int maxC = 100;
//	int v = 10;
//	std::vector<bool> empty;
//	for (int c=0; c<maxC; ++c)
//	{
//		int result=0;
//		int successful=0;
//		for(int t=0; t<n; ++t)
//		{
//			BooleanFormula b(c,v);
//			if (TotalSearch(b,empty))
//			{
//				++successful;
//				if (OneFlipLocalSearch(b,empty))
//					++result;
//			}
//		}
//		fileOut << (float)(result)/(float)(successful) << " ";
//		std::cout << c << " clauses completed.\n";
//	}

//Success rate testing of 1-flip vs. total search	
//	int n = 1000;
//	int maxC = 100;
//	int maxV = 10;
//	std::vector<bool> empty;
//	for (int c=0; c<maxC; ++c)
//	{
//		for(int v=0; v<maxV; ++v)
//		{
//			int result=0;
//			int successful=0;
//			for(int t=0; t<n; ++t)
//			{
//				BooleanFormula b(c+1,v+1);
//				if (TotalSearch(b,empty))
//				{
//					++successful;
//					if (OneFlipLocalSearch(b,empty))
//						++result;
//				}
//			}
//			//Print results for c,v
//			std:: cout << "Clauses: " << c+1 << "\t Variables: " << v+1 << "\t Success rate: " << result << "/" << successful << std::endl;
//		}
//	}

	return 0;
}
