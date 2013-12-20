#ifndef TEST_H
#define TEST_H

#include <vector>
#include <string>

#include <command.hpp>
#include <kdb.hpp>

class TestCommand : public Command
{
	kdb::Key root;
	int nrTest;
	int nrError;

public:
	TestCommand();
	~TestCommand();

	void doTests(std::vector<std::string> const& arguments);

	// add here the name if you add a test
	// (spaces at begin/end needed for check)
	std::string test_names = " basic string umlauts binary naming meta ";

	void doBasicTest();
	void doStringTest();
	void doUmlautsTest();
	void doBinaryTest();
	void doNamingTest();
	void doMetaTest();

	virtual std::string getShortOptions()
	{
		return "";
	}

	virtual std::string getSynopsis()
	{
		return "<root-key> [<test-name> ...]";
	}

	virtual std::string getShortHelpText()
	{
		return "Run key database test suite.";
	}

	virtual std::string getLongHelpText()
	{
		return
			"This command runs an internal test suite.\n"
			"The tests will set and get many keys below\n"
			"the given rootkey.\n"
			"\n"
			"The main purpose of these tests is to check\n"
			"if a backend is capable of storing and retrieving\n"
			"all kinds of configuration keys and values.\n"
			"\n"
			"If no test name is given, every available test\n"
			" is executed.\n"
			"\n"
			"Following tests are available:" + test_names
			;
	}

	virtual int execute (Cmdline const& cmdline);
};

#endif
