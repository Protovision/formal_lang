/****************************************************************************
 * Author: Mark Swoope
 * Date: June 16, 2017
 *
 * C++11 implementation of FIRST and FOLLOW functions for a grammar.
 * Original explanation of these algorithms were studied from:
 * https://www.cs.virginia.edu/~weimer/2008-415/reading/FirstFollowLL.pdf
 *
 * Do not plagiarize.
 *****************************************************************************/

#include "Grammar.h"
#include <functional>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <sstream>

/* Finds the set of all terminals that can begin the derivation of a sequence of
 * symbols
 */
Grammar::symbol_set Grammar::first(const symbol_sequence& seq) const
{
	if (seq.empty()) return {};

	/* FIRST function defined for a single symbol */
	std::function<symbol_set(const symbol&)> first_single =
	[&, this](const symbol& sym) -> symbol_set
	{
		symbol_set first_set;

		/* If symbol is a terminal or empty string, return it as a set */
		if (sym == symbol{} or this->has_terminal(sym)) {
			return { sym };
		}
		
		/* For each rule */
		for (const rule& r : this->rules) {
			symbol_sequence::const_iterator it;
		
			/* Skip rules whose head is not the symbol */ 
			if (r.first != sym) continue;

			/* For each symbol in the body of this rule */
			for (it = r.second.begin(); it != r.second.end(); ++it) {
				symbol_set sub_first_set;
				bool sub_first_set_had_empty_string;

				/* Skip if body symbol is the same as the original symbol */
				if (*it == sym) continue;

				/* Add the FIRST set of the body symbol (excluding empty
				 * strings) to the result
				 */
				sub_first_set = symbol_set{first_single(*it)};
				sub_first_set_had_empty_string = (
					sub_first_set.erase(symbol{}) == 1
				);
				first_set.insert(sub_first_set.begin(), sub_first_set.end());

				/* Stop when the FIRST set of the body symbol did not
				 * contain empty strings
				 */
				if (not sub_first_set_had_empty_string) break;
			}
			/* If all FIRST sets of the body symbols had empty strings, add 
			 * empty string to the result
			 */
			if (it == r.second.end()) first_set.insert(symbol{});
		}
		return first_set;
	};

	/* Finding the FIRST set of a sequence of symbols is similar to the above
	 * algorithm
	 */
	symbol_set first_set;
	symbol_sequence::const_iterator it;
	for (it = seq.begin(); it != seq.end(); ++it) {
		symbol_set sub_first_set{first_single(*it)};
		bool had_empty_string = (sub_first_set.erase(symbol{}) == 1);
		first_set.insert(sub_first_set.begin(), sub_first_set.end());
		if (not had_empty_string) break;
	}
	if (it == seq.end()) first_set.insert(symbol{});
	return first_set;
}

/* Finds all terminals that can follow the derivation of a non-terminal */
Grammar::symbol_set Grammar::follow(const symbol& nt) const
{
	symbol_set follow_set;

	/* If the non-terminal is the start symbol, add empty string */
	if (nt == this->start) follow_set.insert(symbol{});

	/* For each production */
	for (const rule& r : this->rules) {
		symbol_sequence::const_iterator found;
		symbol_sequence after;
		symbol_set first_of_after;
		bool first_of_after_had_empty_string;

		/* Skip any production that contains the non-terminal in it's head */
		if (r.first == nt) continue;

		/* Skip any production that does not contain the non-terminal in it's
		 * body
		 */
		found = std::find(r.second.begin(), r.second.end(), nt);
		if (found == r.second.end()) continue;

		/* Add FIRST set of symbols that follow the non-terminal in this
		 * production to the result
		 */
		after = symbol_sequence(
			++symbol_sequence::const_iterator{found},
			r.second.end()
		);
		first_of_after = symbol_set{this->first(after)};
		first_of_after_had_empty_string = (
			first_of_after.erase(symbol{}) == 1
		);
		follow_set.insert(first_of_after.begin(), first_of_after.end());

		/* If there were no symbols following our non-terminal or the FIRST
		 * set contained an empty string, add the FOLLOW set of this 
		 * production's LHS
		 */
		if (after.empty() or first_of_after_had_empty_string) {
			symbol_set sub_follow_set{this->follow(r.first)};
			follow_set.insert(sub_follow_set.begin(), sub_follow_set.end());
		}
	}
	return follow_set;
}

/* The following are I/O overloads for printing and scanning Grammar components */

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const Grammar::symbol> >& t)
{
	if (
		std::get<1>(t).get() == Grammar::symbol{} or
		std::get<0>(t).get().has_terminal(std::get<1>(t))
	) {
		return (os << put_quoted(std::string{std::get<1>(t)}));
	}
	return (os << std::string{std::get<1>(t)});
}

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const Grammar::symbol_sequence> >& t)
{
	Grammar::symbol_sequence::size_type seen = 0;
	Grammar::symbol_sequence::const_iterator it;
	for (
		it = std::get<1>(t).get().begin();
		seen != std::get<1>(t).get().size()-1;
		++seen, ++it
	) {
		os << Grammar::put(std::get<0>(t), *it) << ' ';
	}
	os << Grammar::put(std::get<0>(t), *it);
	return os;
}

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const Grammar::symbol_set> >& t)
{
	Grammar::symbol_set::size_type seen = 0;
	Grammar::symbol_set::const_iterator it;
	for (
		it = std::get<1>(t).get().begin();
		seen != std::get<1>(t).get().size()-1;
		++seen, ++it
	) {
		os << Grammar::put(std::get<0>(t), *it) << ' ';
	}
	os << Grammar::put(std::get<0>(t), *it);
	return os;
}

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const Grammar::rule> >& t)
{
	return (
		os <<
		Grammar::put(std::get<0>(t), std::get<1>(t).get().first) <<
		" = " <<
		Grammar::put(std::get<0>(t), std::get<1>(t).get().second)
	);
}

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const Grammar::rule_set> >& t)
{
	Grammar::rule_set::size_type seen = 0;
	Grammar::rule_set::const_iterator it;
	for (
		it = std::get<1>(t).get().begin();
		seen != std::get<1>(t).get().size()-1;
		++seen, ++it
	) {
		os << Grammar::put(std::get<0>(t), *it) << std::endl;
	}
	os << Grammar::put(std::get<0>(t), *it);
	return os;
}

std::ostream& operator<<(
	std::ostream& os, const std::tuple<
	std::reference_wrapper<const Grammar> >&t)
{
	os <<
		Grammar::put(
			std::get<0>(t), std::get<0>(t).get().non_terminals
		)
	<< std::endl << std::endl <<
		Grammar::put(
			std::get<0>(t), std::get<0>(t).get().terminals
		)
	<< std::endl << std::endl <<
		Grammar::put(
			std::get<0>(t), std::get<0>(t).get().rules
		)
	<< std::endl << std::endl <<
		Grammar::put(
			std::get<0>(t), std::get<0>(t).get().start
		);
	return os;
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar::symbol> >& t)
{
	return (is >> get_quoted(std::get<0>(t).get()));
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar::symbol_set> >& t)
{
	Grammar::symbol sym;
	std::string line;

	std::get<0>(t).get().clear();
	if (not (is >> std::ws)) return is;
	if (not std::getline(is, line)) return is;
	std::stringstream ss{line};
	while (ss >> Grammar::get(sym)) {
		std::get<0>(t).get().insert(sym);
	}
	return is;
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar::symbol_sequence> >& t)
{
	Grammar::symbol sym;
	std::string line;

	std::get<0>(t).get().clear();
	if (not (is >> std::ws)) return is;
	if (not std::getline(is, line)) return is;
	std::stringstream ss{line};
	while (ss >> Grammar::get(sym)) {
		std::get<0>(t).get().push_back(sym);
	}
	return is;
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar::rule> >& t)
{
	Grammar::symbol equals;
	if (not (is >> Grammar::get(std::get<0>(t).get().first)))
		return is;
	if (not (is >> Grammar::get(equals)))
		return is;
	if (equals != "=") {
		is.setstate(std::ios_base::failbit);
		return is;
	}
	return (is >> Grammar::get(std::get<0>(t).get().second));
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar::rule_set> >& t)
{
	std::string line;
	Grammar::rule r;

	std::get<0>(t).get().clear();
	if (not (is >> std::ws)) return is;
	while (getline(is, line)) {
		if (line == "") break;
		std::stringstream ss{line};
		if (not (ss >> Grammar::get(r))) break;
		std::get<0>(t).get().insert(r);
	}
	return is;
}

std::istream& operator>>(
	std::istream& is, const std::tuple<
	std::reference_wrapper<Grammar> >& t)
{
	if (not (is >> Grammar::get(std::get<0>(t).get().non_terminals)))
		return is;
	if (not (is >> Grammar::get(std::get<0>(t).get().terminals)))
		return is;
	if (not (is >> Grammar::get(std::get<0>(t).get().rules)))
		return is;
	return (is >> Grammar::get(std::get<0>(t).get().start));
}


