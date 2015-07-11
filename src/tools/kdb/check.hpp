#ifndef CHECK_HPP
#define CHECK_HPP

#include <command.hpp>

#include <kdb.hpp>

class CheckCommand : public Command
{
public:
	CheckCommand();
	~CheckCommand();

	virtual std::string getShortOptions()
	{
		return "vcf";
	}

	virtual std::string getSynopsis()
	{
		return "[<name>]";
	}

	virtual std::string getShortHelpText()
	{
		return "Do some basic checks on a plugin.";
	}

	virtual std::string getLongHelpText()
	{
		return  "If no arguments are given checks on key database\n"
			"are done instead. Use -f to also do a write test\n"
			"(might change configuration files!)\n"
			"\n"
			"For kdb checking a bit pattern will be returned:\n"
			" 0 .. everything ok (no output)\n"
			" bit 1 .. warning on open\n"
			" bit 2 .. error on open\n"
			" bit 3 .. warning on get\n"
			" bit 4 .. error on get\n"
			" bit 5 .. warning on set (only checked when -f is used)\n"
			" bit 6 .. error on set (only checked when -f is used)\n"
			" bit 7 .. warning on close\n"
			" bit 8 .. error on close\n"
			"\n"
			"\n"
			"\n"
			"\n"
			"If a plugin name is given, checks will only be done with given plugin.\n"
			"Use -c to pass options to the plugin.\n"
			"\n"
			"Return values on plugin checking:\n"
			" 0 .. everything ok (no output)\n"
			" 1 .. no such plugin found or plugin could not be opened\n"
			" 2 .. plugin did not pass checks\n"
			" 3 .. plugin has warnings\n"
			"\n"
			"Please report any output caused by official plugins to http://www.libelektra.org\n"
			"\n";
	}

	virtual int execute (Cmdline const& cmdline);
};

#endif
