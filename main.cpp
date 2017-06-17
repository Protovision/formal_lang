/* Reads a grammar and displays all FIRST and FOLLOWS sets for that grammar */
#include "Grammar.h"
#include <iostream>
#include <string>
using namespace std;

int main()
{
	Grammar g;
	Grammar::symbol_set symbols;

	/* Read the grammar */
	cin >> Grammar::get(g);

	/* Create set that is the union of the grammar's terminals and
	 * non-terminals */
	symbols.insert(g.non_terminals.begin(), g.non_terminals.end());
	symbols.insert(g.terminals.begin(), g.terminals.end());

	/* Print the grammar */
	cout << Grammar::put(g) << endl << endl;

	/* Print all FIRST sets of the grammar */
	for (const Grammar::symbol& s : symbols) {
		cout << "FIRST(" << Grammar::put(g, s) << "): " <<
			Grammar::put(g, g.first({s})) << endl;
	}

	/* Print all FOLLOW sets of the grammar */
	for (const Grammar::symbol& s : symbols) {
		cout << "FOLLOW(" << Grammar::put(g, s) << "): " <<
			Grammar::put(g, g.follow(s)) << endl;
	}
	return 0;
}
