/* Author: Mark Swoope
 * Date: June 16, 2017
 * 
 * quoted substitute for C++11 compilers.
 *
 * quoted is a stream manipulator for safely transferring strings containing 
 * embedded whitespaces across iostreams.
 *
 * Usage:
 * 		istream >> get_quoted(str);
 *		ostream << put_quoted(str);
 */
#ifndef QUOTED_H
#define QUOTED_H
#include <tuple>
#include <string>
#include <vector>
#include <algorithm>
#include <istream>
#include <ostream>
#include <functional>

template <typename CharT, typename Traits, typename Allocator>
class _quoted_ostr {
public:
	typedef CharT char_type;
	typedef Traits traits_type;

	std::basic_string<char_type, traits_type, Allocator> str;
	char_type delim;
	char_type escape;
};

template <typename CharT>
_quoted_ostr< CharT, std::char_traits<CharT>, std::allocator<CharT> >
put_quoted(const CharT* s, CharT delim=CharT('"'), CharT escape=CharT('\\'))
{
	return {s, delim, escape};
}

template <typename CharT, typename Traits, typename Allocator>
_quoted_ostr< CharT, Traits, Allocator >
put_quoted(const std::basic_string<CharT, Traits, Allocator>& s,
CharT delim = CharT('"'), CharT escape = CharT('\\'))
{
	return {s, delim, escape};
}

template <typename CharT, typename Traits, typename Allocator>
std::basic_ostream<CharT, Traits>& operator<<(
std::basic_ostream<CharT, Traits>& os,
const _quoted_ostr<CharT, Traits, Allocator>& qos)
{
	typedef CharT char_type;
	typedef Traits traits_type;

	std::vector<char_type> seq;

	seq.push_back(qos.delim);
	for (const char_type& c : qos.str) {
		if (traits_type::eq(c, qos.delim) or
		traits_type::eq(c, qos.escape)) {
			seq.push_back(qos.escape);
		}
		seq.push_back(c);
	}
	seq.push_back(qos.delim);
	if (seq.size() < os.width()) {
		if (os.flags() & std::ios_base::left) {	
			std::fill_n(std::back_inserter(seq), os.width() - seq.size(),
			os.fill());
		} else {
			std::vector<char_type> temp{seq};
			seq.clear();
			std::fill_n(std::back_inserter(seq), os.width() - seq.size(),
			os.fill());
			std::copy(temp.begin(), temp.end(), std::back_inserter(seq));
		}
	}
	os.width(0);
	os.rdbuf()->sputn(seq.data(), seq.size());
	return os;	

}


template <typename CharT, typename Traits, typename Allocator>
class _quoted_istr {
public:
	typedef CharT char_type;
	typedef Traits traits_type;

	std::reference_wrapper<
		std::basic_string<char_type, traits_type, Allocator>
	> rstr;
	char_type delim;
	char_type escape;
};

template <typename CharT, typename Traits, typename Allocator>
_quoted_istr<CharT, Traits, Allocator>
get_quoted(std::basic_string<CharT, Traits, Allocator>& s,
CharT delim = CharT('"'), CharT escape = CharT('\\'))
{
	return {std::ref(s), delim, escape};
}


template <typename CharT, typename Traits, typename Allocator>
std::basic_istream<CharT, Traits>& operator>>(
std::basic_istream<CharT, Traits>& is,
const _quoted_istr<CharT, Traits, Allocator>& qis)
{
	typedef CharT char_type;
	typedef Traits traits_type;

	char_type c;
	std::ios_base::fmtflags oldflags;

	if (not (is >> c)) return is;
	if (not traits_type::eq(c, qis.delim)) {
		is.unget();
		return (is >> qis.rstr.get());
	}
	oldflags = is.flags();
	is >> std::noskipws;
	qis.rstr.get().clear();
	while (is >> c) {
		if (traits_type::eq(c, qis.escape)) {
			if (not (is >> c)) break;
		} else if (traits_type::eq(c, qis.delim)) break;
		qis.rstr.get().push_back(c);
	}
	is.flags(oldflags);
	return is;

}

#endif
