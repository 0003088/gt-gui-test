#include <test.hpp>

#include <kdb.hpp>
#include <cmdline.hpp>
#include <print.hpp>

#include <vector>
#include <iostream>
#include <algorithm>

using namespace kdb;
using namespace std;

TestCommand::TestCommand():
	root(),
	nrTest(0),
	nrError(0),
	// XXX add here the name if you add a test
	// (spaces at begin/end needed for check)
	testNames(" basic string umlauts binary naming meta ")
{}

void TestCommand::doBasicTest()
{
	{
		KDB kdb;
		Key t = root.dup();
		t.addBaseName ("basic");
		t.setString ("BasicString");
		KeySet basic;
		basic.append(t);

		KeySet test;
		kdb.get (test, root);
		kdb.set (basic, root);
	}

	{
		KDB kdb;
		Key t = root.dup();
		t.addBaseName ("basic");
		t.setString ("BasicString");

		KeySet test;
		kdb.get (test, root);

		nrTest ++;
		if (!test.lookup(t))
		{
			nrError ++;
			cerr << "Basic test failed" << endl;
		}
	}
}

void TestCommand::doStringTest()
{
	vector<string> teststrings;
	teststrings.push_back("");
	teststrings.push_back("value");
	teststrings.push_back("value with spaces");
	teststrings.push_back(" a very long value with many spaces and basically very very long, but only text ... ");
	for (int i=1; i<256; ++i) teststrings.back() += " very very long, but only text ... ";


	for (size_t i = 0; i< teststrings.size(); ++i)
	{
		{
			KDB kdb;
			Key t = root.dup();
			t.addBaseName ("string");
			t.setString (teststrings[i]);

			KeySet basic;
			basic.append(t);

			KeySet test;
			kdb.get (test, root);
			kdb.set (basic, root);
		}

		{
			KDB kdb;

			KeySet test;
			kdb.get (test, root);

			Key t = root.dup();
			t.addBaseName ("string");

			Key res = test.lookup(t);

			nrTest ++;
			if (!res)
			{
				nrError ++;
				cerr << "String test failed (key not found)" << t.getName() << endl;
				continue;
			}

			nrTest ++;
			if (res.getString() != teststrings[i])
			{
				nrError ++;
				cerr << "String test failed (value is not equal)" << endl;
				cerr << "We got: \"" << res.getString() << "\"" << endl;
				cerr << "We wanted: \"" <<  teststrings[i] << "\"" << endl;
			}
		}
	}
}

void TestCommand::doUmlautsTest()
{
	vector<string> teststrings;
	teststrings.push_back("ascii umlauts !\"§$%&/()=?`\\}][{");
	teststrings.push_back("utf8 umlauts ¸¬½¼³²¹ł€¶øæßðđł˝«»¢“”nµ─·");
	teststrings.push_back("all chars:");
	for (int i=1; i<256; ++i) teststrings.back().push_back(i);
	teststrings.push_back("€");
	for (int i=1; i<256; ++i)
	{
		string s;
		s.push_back (i);
		teststrings.push_back(s);
	}


	for (size_t i = 0; i< teststrings.size(); ++i)
	{
		{
			KDB kdb;
			Key t = root.dup();
			t.addBaseName ("string");
			t.setString (teststrings[i]);

			KeySet basic;
			basic.append(t);

			KeySet test;
			kdb.get (test, root);
			kdb.set (basic, root);
		}

		{
			KDB kdb;

			KeySet test;
			kdb.get (test, root);

			Key t = root.dup();
			t.addBaseName ("string");

			Key res = test.lookup(t);

			nrTest ++;
			if (!res)
			{
				nrError ++;
				cerr << "String test failed (key not found)" << t.getName() << endl;
				continue;
			}

			nrTest ++;
			if (res.getString() != teststrings[i])
			{
				nrError ++;
				cerr << "String test failed (value is not equal)" << endl;
				cerr << "We got: \"" << res.getString() << "\"" << endl;
				cerr << "We wanted: \"" <<  teststrings[i] << "\"" << endl;
			}
		}
	}
}

void TestCommand::doBinaryTest()
{
	vector<string> teststrings;
	teststrings.push_back("binary value");
	teststrings.push_back("binary value with null");
	teststrings.back().push_back('\0');

	teststrings.push_back("binary value with null");
	teststrings.back().push_back('\0');
	teststrings.back() += "in the middle";

	teststrings.push_back(" a very long value with many spaces and basically very very long, but only binary text ... ");
	for (int i=0; i<256; ++i)
	{
		teststrings.back().push_back('\0');
		teststrings.back() += " very very long, but only binary text ... ";
	}

	teststrings.push_back("all chars:");
	for (int i=0; i<256; ++i) teststrings.back().push_back(i);
	teststrings.push_back("€");
	for (int i=0; i<256; ++i)
	{
		string s;
		s.push_back (i);
		teststrings.push_back(s);
	}


	for (size_t i = 0; i< teststrings.size(); ++i)
	{
		{
			KDB kdb;
			Key t = root.dup();
			t.addBaseName ("binary");
			t.setBinary(teststrings[i].c_str(), teststrings[i].length());

			KeySet basic;
			basic.append(t);

			KeySet test;
			kdb.get (test, root);
			kdb.set (basic, root);
		}

		{
			KDB kdb;

			KeySet test;
			kdb.get (test, root);

			Key t = root.dup();
			t.addBaseName ("binary");

			Key res = test.lookup(t);
			nrTest ++;
			if (!res)
			{
				nrError ++;
				cerr << "Binary test failed (key not found)" << t.getName() << endl;
				continue;
			}

			nrTest ++;
			if (res.getBinarySize() != teststrings[i].length())
			{
				nrError ++;
				cerr << "Binary test failed (length is not equal)" << endl;
				cerr << "We got: \"" << res.getBinary() << "\"" << endl;
				cerr << "We wanted: \"" << teststrings[i] << "\"" << endl;
			}

			nrTest ++;
			if (res.getBinary() != teststrings[i])
			{
				nrError ++;
				cerr << "Binary test failed (value is not equal)" << endl;
				cerr << "We got: \"" << res.getBinary() << "\"" << endl;
				cerr << "We wanted: \"" << teststrings[i] << "\"" << endl;
			}
		}
	}
}

void TestCommand::doNamingTest()
{
	vector<string> teststrings;
	teststrings.push_back("keyname");
	teststrings.push_back("deep/below/keyname");
	teststrings.push_back("keyname with spaces");
	teststrings.push_back("deep/belowkeyname with spaces");
	teststrings.push_back(" a very long value with many spaces and basically very very long, but only text ");
	for (int i=1; i<256; ++i) teststrings.back() += "/ very very long, but only text ... ";
	teststrings.push_back("ascii umlauts !\"§$%&/()=?`\\}][{");
	teststrings.push_back("utf8 umlauts ¸¬½¼³²¹ł€¶øæßðđł˝«»¢“”nµ─·");
	teststrings.push_back("all chars:");
	for (int i=1; i<256; ++i) teststrings.back().push_back(i);
	teststrings.push_back("€");
	for (int i=1; i<256; ++i)
	{
		if (i == '.') continue;
		string s;
		s.push_back (i);
		teststrings.push_back(s);
	}


	for (size_t i = 0; i< teststrings.size(); ++i)
	{
		{
			KDB kdb;
			Key t = root.dup();
			t.addBaseName (teststrings[i]);

			KeySet basic;
			basic.append(t);

			KeySet test;
			kdb.get (test, root);
			kdb.set (basic, root);
		}

		{
			KDB kdb;

			KeySet test;
			kdb.get (test, root);

			test.rewind();
			Key res = test.next();

			nrTest ++;
			if (!res)
			{
				nrError ++;
				cerr << "Naming test failed (no key in keyset)" << endl;
				continue;
			}

			nrTest ++;
			Key cmp = root.dup();
			cmp.addBaseName(teststrings[i]);
			if (res != cmp)
			{
				nrError ++;
				cerr << "Naming test failed (name is not equal)" << endl;
				cerr << "We got: \"" << res.getName() << "\"" << endl;
				cerr << "We wanted: \"" <<  cmp.getName() << "\"" << endl;
			}
		}
	}
}

void TestCommand::doMetaTest()
{
	vector<string> teststrings;
	teststrings.push_back("");
	teststrings.push_back("value");
	teststrings.push_back("value with spaces");
	teststrings.push_back(" a very long value with many spaces and basically very very long, but only text ... ");
	for (int i=1; i<256; ++i) teststrings.back() += " very very long, but only text ... ";
	teststrings.push_back("ascii umlauts !\"§$%&/()=?`\\}][{");
	teststrings.push_back("utf8 umlauts ¸¬½¼³²¹ł€¶øæßðđł˝«»¢“”nµ─·");
	teststrings.push_back("all chars:");
	for (int i=1; i<256; ++i) teststrings.back().push_back(i);
	teststrings.push_back("€");
	for (int i=1; i<256; ++i)
	{
		string s;
		s.push_back (i);
		teststrings.push_back(s);
	}

	vector<string> testnames;
	testnames.push_back("keyname");
	testnames.push_back("deep/below/keyname");
	testnames.push_back("keyname with spaces");
	testnames.push_back("deep/belowkeyname with spaces");
	testnames.push_back(" a very long value with many spaces and basically very very long, but only text ");
	for (int i=1; i<256; ++i) testnames.back() += "/ very very long, but only text ... ";
	testnames.push_back("ascii umlauts !\"§$%&/()=?`\\}][{");
	testnames.push_back("utf8 umlauts ¸¬½¼³²¹ł€¶øæßðđł˝«»¢“”nµ─·");
	testnames.push_back("all chars:");
	for (int i=1; i<256; ++i) testnames.back().push_back(i);
	testnames.push_back("€");
	for (int i=1; i<256; ++i)
	{
		if (i == 46) continue; // ignore .
		string s;
		s.push_back (i);
		testnames.push_back(s);
	}


	for (size_t j = 0; j< testnames.size(); ++j)
	for (size_t i = 0; i< teststrings.size(); ++i)
	{
		{
			KDB kdb;
			Key t = root.dup();
			t.addBaseName (testnames[j]);
			t.setMeta<string> ("key", teststrings[i]);

			KeySet basic;
			basic.append(t);

			KeySet test;
			kdb.get (test, root);
			kdb.set (basic, root);
		}

		{
			KDB kdb;

			KeySet test;
			kdb.get (test, root);

			Key t = root.dup();
			t.addBaseName (testnames[j]);
			Key res = test.lookup(t);

			nrTest ++;
			if (!res)
			{
				nrError ++;
				cerr << "Meta test failed (key not found)" << t.getName() << endl;
				continue;
			}

			std::string meta = res.getMeta<std::string>("key");

			nrTest ++;
			if (meta != teststrings[i])
			{
				nrError ++;
				cerr << "Meta test failed (name is not equal)" << endl;
				cerr << "We got: \"" << meta << "\"" << endl;
				cerr << "We wanted: \"" <<  teststrings[i] << "\"" << endl;
			}
		}
	}
}

void TestCommand::doTests(std::vector<std::string> const& arguments)
{
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "basic") !=
		arguments.end())
	{
		cout << "Doing basic tests" << std::endl;
		doBasicTest();
	}
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "string") !=
		arguments.end())
	{
		cout << "Doing string tests" << std::endl;
		doStringTest();
	}
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "umlauts") !=
		arguments.end())
	{
		cout << "Doing umlauts tests" << std::endl;
		doUmlautsTest();
	}
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "binary") !=
		arguments.end())
	{
		cout << "Doing binary tests" << std::endl;
		doBinaryTest();
	}
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "naming") !=
		arguments.end())
	{
		cout << "Doing naming tests" << std::endl;
		doNamingTest();
	}
	if (arguments.size() == 1 ||
		find(arguments.begin(), arguments.end(), "meta") !=
		arguments.end())
	{
		cout << "Doing meta tests" << std::endl;
		doMetaTest();
	}
	// XXX add here a new test execution (as above)
}

int TestCommand::execute(Cmdline const& cl)
{
	if (cl.arguments.size() < 1)
	{
		throw invalid_argument ("need at least one argument");
	}

	// do a basic check on every argument
	for (size_t i=1; i<cl.arguments.size(); ++i)
	{
		string name = " ";
		name += cl.arguments[i];
		name += " ";
		if (testNames.find(name) == std::string::npos)
		{
			throw invalid_argument ("test name " +
					cl.arguments[i] +
					" does not exist in:" +
					testNames);
		}
	}

	root.setName(cl.arguments[0]);
	if (root.getNameSize() <= 1)
	{
		throw invalid_argument ("Not a valid root name");
	}

	KDB kdb;
	KeySet original;
	kdb.get(original, root);
	original.rewind();

	doTests(cl.arguments);

	cerr << "We got " << nrError << " errors in " << nrTest << " testcases." << endl;

	cout << "Test suite is now finished." << endl;
	cout << "Now restoring the original keyset." << endl;
	kdb.set(original, root);

	return nrError;
}

TestCommand::~TestCommand()
{}
