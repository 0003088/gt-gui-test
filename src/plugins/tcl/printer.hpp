#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <keyset>

#include <vector>
#include <string>

namespace elektra
{

struct Printer
{
	int nr_keys;
	int nr_meta;

	kdb::KeySet &current;

	std::string keyname;
	std::string metaname;

	Printer (kdb::KeySet &ks);

	void add_key(std::vector<char> const& c);
	void add_val(std::vector<char> const& c);

	void add_metakey(std::vector<char> const& c);
	void add_metaval(std::vector<char> const& c);
};


} // end namespace elektra

#endif
