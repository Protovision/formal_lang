/* Author: Mark Swoope
 * Date: June 17, 2017
 *
 * Context-free grammar class with FIRST and FOLLOW operations 
 * and components that are transferrable across I/O streams
 */
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "quoted.h"
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <functional>

/* Represents a context-free grammar.
 * Contains the components of a formal grammar with the restriction that each 
 * rule must contain only a single non-terminal as it's head.
 *
 * Grammar symbols are allowed to contain whitespace except for '\n'.
 *
 * Grammar symbols containing whitespace must be surrounded by double quotes 
 * to be safely extracted from an istream.
 */
class Grammar {
public:
	typedef std::string symbol;
	typedef std::set<symbol> symbol_set;
	typedef std::vector<symbol> symbol_sequence;
	typedef std::pair<symbol, symbol_sequence> rule;
	typedef std::set<rule> rule_set;

	/* Grammar components */
	symbol_set non_terminals;
	symbol_set terminals;
	rule_set rules;
	symbol start;

	/* Finds the set of terminals that can begin the derivation of a symbol
	 * sequence
	 */
	symbol_set first(const symbol_sequence& sequence) const;

	/* Finds the set of terminals that can appear after a non-terminal has
	 * been expanded.
	 */
	symbol_set follow(const symbol& non_terminal) const;

	/* Checks if a symbol is a terminal of this grammar */
	inline bool has_terminal(const symbol& s) const
	{
		return this->terminals.find(s) != this->terminals.end();
	}

	/* Checks if a symbol is a non-terminal of this grammar */
	inline bool has_non_terminal(const symbol& s) const
	{
		return this->non_terminals.find(s) != this->non_terminals.end();
	}

	/* Static functions for inserting a grammar or grammar component into
	 * an ostream
	 */
	inline static std::tuple< std::reference_wrapper<const Grammar> >
	put(const Grammar& g)
	{
		return std::make_tuple(std::cref(g));
	}

	inline static std::tuple< std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const rule_set> > put(const Grammar& g,
	const rule_set& r)
	{
		return std::make_tuple(std::cref(g), std::cref(r));
	}

	inline static std::tuple< std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const rule> > put(const Grammar& g, const rule& r)
	{
		return std::make_tuple(std::cref(g), std::cref(r));
	}

	inline static std::tuple< std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const symbol_set> > put(const Grammar& g,
	const symbol_set& s)
	{
		return std::make_tuple(std::cref(g), std::cref(s));
	}

	inline static std::tuple< std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const symbol_sequence> > put(const Grammar& g,
	const symbol_sequence& s)
	{
		return std::make_tuple(std::cref(g), std::cref(s));
	}

	static std::tuple< std::reference_wrapper<const Grammar>,
	std::reference_wrapper<const symbol> > put(const Grammar& g,
	const symbol& s)
	{
		return std::make_tuple(std::cref(g), std::cref(s));
	}

	/* Static functions for extracting a grammar or grammar component from 
	 * an istream
	 */
	inline static std::tuple< std::reference_wrapper<Grammar> >
	get(Grammar& g)
	{
		return std::make_tuple(std::ref(g));
	}
	
	inline static std::tuple< std::reference_wrapper<Grammar::symbol> >
	get(Grammar::symbol& s)
	{
	 	return std::make_tuple(std::ref(s));
	}

	inline static std::tuple< std::reference_wrapper<
	Grammar::symbol_sequence> > get(Grammar::symbol_sequence& s)
	{
		return std::make_tuple(std::ref(s));
	}

	inline static std::tuple< std::reference_wrapper<Grammar::symbol_set> >
	get(Grammar::symbol_set& s)
	{
		return std::make_tuple(std::ref(s));
	}

	inline static std::tuple< std::reference_wrapper<Grammar::rule> >
	get(Grammar::rule& r)
	{
		return std::make_tuple(std::ref(r));
	}

	inline static std::tuple< std::reference_wrapper<Grammar::rule_set> >
	get(Grammar::rule_set& r)
	{
		return std::make_tuple(std::ref(r));
	}
};

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar> >& t);

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar>, 
std::reference_wrapper<const Grammar::rule_set> >& t);

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar>,
std::reference_wrapper<const Grammar::rule> >& t);

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar>,
std::reference_wrapper<const Grammar::symbol_set> >& t);

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar>,
std::reference_wrapper<const Grammar::symbol_sequence> >& t);

std::ostream& operator<<(std::ostream& os, const std::tuple<
std::reference_wrapper<const Grammar>,
std::reference_wrapper<const Grammar::symbol> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar::symbol> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar::symbol_set> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar::symbol_sequence> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar::rule> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar::rule_set> >& t);

std::istream& operator>>(std::istream& is, const std::tuple<
std::reference_wrapper<Grammar> >& t);

#endif
