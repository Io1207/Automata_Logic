#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

typedef int Literal;
typedef vector<Literal> Clause;

struct DPLL 
{
    vector<Clause> clauses;
    unordered_map<int, int> assignment;  // variable -> 1 for true, -1 for false, 0 for unassigned

    DPLL(const vector<Clause>& cnf) : clauses(cnf) {}

    bool solve() {
        return dpll();
    }

private:
    // Recursive DPLL function
    bool dpll() {
        // If all clauses are satisfied SAT
        if (allClausesSatisfied()) return true;

        // If any clause is unsatisfied UNSAT
        if (anyClauseUnsatisfied()) return false;

        // Selecting unassigned literal variable
        int var = pickUnassignedVariable();

        // Try assigning the variable to true
        assignment[var] = 1;
        if (dpll()) return true;

        // Backtrack if unsuccessful
        assignment[var] = -1;
        if (dpll()) return true;

        // Backtrack
        assignment[var] = 0;
        return false;
    }

    // Check if all clauses are satisfied 
    bool allClausesSatisfied() 
    {
        for (const Clause& clause : clauses) 
        {
            bool clauseSatisfied = false;
            for (Literal lit : clause) 
            {
                if ((lit > 0 && assignment[abs(lit)] == 1) || (lit < 0 && assignment[abs(lit)] == -1)) 
                {
                    clauseSatisfied = true;
                    break;
                }
            }
            if (!clauseSatisfied) return false;
        }
        return true;
    }

    // Check if any clause is unsatisfied
    bool anyClauseUnsatisfied() 
    {
        for (const Clause& clause : clauses) 
        {
            bool clauseUnsatisfied = true;
            for (Literal lit : clause) 
            {
                if ((lit > 0 && assignment[abs(lit)] != -1) || (lit < 0 && assignment[abs(lit)] != 1)) 
                {
                    clauseUnsatisfied = false;
                    break;
                }
            }
            if (clauseUnsatisfied) return true;
        }
        return false;
    }

    // Pick an unassigned variable (the first unassigned here, ideally I should have chosen something appearing in the most number of clauses and unassigned)
    int pickUnassignedVariable() 
    {
        for (const Clause& clause : clauses) 
        {
            for (Literal lit : clause) 
            {
                if (assignment[abs(lit)] == 0) 
                {
                    return abs(lit);
                }
            }
        }
        return 0;  // All variables are assigned
    }
};

int main() 
{
    vector<Clause> cnf = {{1, 3},{-1, -1},{-3, -3}}; 

    DPLL solver(cnf);
    if (solver.solve()) 
    {
        cout << "SAT" << endl;
    } 
    else 
    {
        cout << "UNSAT" << endl;
    }

    return 0;
}
