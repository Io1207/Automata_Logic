#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

class CDCLSolver {
public:
    CDCLSolver(int numVars) : numVars(numVars), decisionLevel(0) {
        assignment.resize(numVars + 1, 0); 
        decisionLevels.resize(numVars + 1, -1); 
        reasonClauses.resize(numVars + 1, -1);  
        clauses.clear(); 
        watchList.resize(2 * numVars + 2);
    }

    void addClause(const std::vector<int>& clause) {
        clauses.push_back(clause);
        watchList[literalToIndex(clause[0])].push_back(clauses.size() - 1);
        if (clause.size() > 1) {
            watchList[literalToIndex(clause[1])].push_back(clauses.size() - 1);
        }
    }

    bool solve() {
        while (true) {
            while (!unitPropagation()) {
                if (!backtrack()) {
                    return false; // UNSAT
                }
            }

            if (isSolved()) {
                return true; // SAT
            }

            decide(); // Make a decision
        }
    }

    void printSolution() const {
        std::cout << "SATISFIABLE\n";
        for (int i = 1; i <= numVars; ++i) {
            std::cout << (assignment[i] == 1 ? i : -i) << " ";
        }
        std::cout << std::endl;
    }

private:
    int numVars;
    int decisionLevel;
    std::vector<int> assignment; // 0 = unassigned, 1 = true, -1 = false
    std::vector<int> decisionLevels; 
    std::vector<int> reasonClauses; 
    std::vector<std::vector<int>> clauses; 
    std::vector<std::vector<int>> watchList; // Watch list for each literal
    std::stack<int> trail; // for backtracking

    // Converts a literal (e.g., x or -x) to an index in the watch list
    int literalToIndex(int lit) const {
        return lit > 0 ? 2 * lit : -2 * lit + 1;
    }

    int indexToLiteral(int index) const {
        return index % 2 == 0 ? index / 2 : -(index / 2);
    }

    bool unitPropagation() {
        while (!trail.empty()) {
            int var = trail.top();
            trail.pop();
            int litIndex = literalToIndex(var);

            for (auto it = watchList[litIndex].begin(); it != watchList[litIndex].end();) {
                int clauseIndex = *it;
                auto& clause = clauses[clauseIndex];

                bool satisfied = false;
                int falseCount = 0;
                int unassignedLit = 0;

                for (int lit : clause) {
                    int var = abs(lit);
                    if (assignment[var] == 0) {
                        unassignedLit = lit;
                    } else if (assignment[var] == (lit > 0 ? 1 : -1)) {
                        satisfied = true;
                        break;
                    } else {
                        falseCount++;
                    }
                }

                if (satisfied) {
                    ++it; // Clause already satisfied
                } else if (falseCount == (int)clause.size() - 1 && unassignedLit != 0) {
                    assignLiteral(unassignedLit);
                    ++it;
                } else if (falseCount == (int)clause.size()) {
                    // Conflict all literals are false
                    return false;
                } else {
                    ++it;
                }
            }
        }
        return true; // No conflicts
    }

    void decide() {
        for (int i = 1; i <= numVars; ++i) {
            if (assignment[i] == 0) {
                decisionLevel++;
                assignLiteral(i); // Assign true to the first unassigned variable
                return;
            }
        }
    }

    void assignLiteral(int lit) {
        int var = abs(lit);
        assignment[var] = (lit > 0 ? 1 : -1);
        decisionLevels[var] = decisionLevel;
        trail.push(lit);
    }

    bool backtrack() {
        if (decisionLevel == 0) {
            return false; // Nothing to backtrack, UNSAT
        }

        decisionLevel--;

        while (!trail.empty()) {
            int var = abs(trail.top());
            if (decisionLevels[var] <= decisionLevel) {
                break;
            }
            assignment[var] = 0; // Unassign variables at deeper levels
            trail.pop();
        }

        return true; // Backtrack succeeded
    }

    bool isSolved() const {
        for (int i = 1; i <= numVars; ++i) {
            if (assignment[i] == 0) {
                return false; 
            }
        }
        return true;
    }
};

int main() {
    CDCLSolver solver(3);

    solver.addClause({1, -2});
    solver.addClause({-1, 2});
    solver.addClause({-2, 3});
    solver.addClause({-3});

    if (solver.solve()) {
        solver.printSolution();
    } else {
        std::cout << "UNSATISFIABLE\n";
    }

    return 0;
}
