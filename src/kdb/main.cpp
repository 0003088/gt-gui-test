#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <factory.hpp>
#include <command.hpp>

#include <key.hpp>
#include <kdb.hpp>

using namespace kdb;
using namespace std;

void displayHelp(std::string app, std::vector<std::string> commands)
{
	std::cout << "Usage: " << app << " <command> [args]"
		<< std::endl;
	std::cout << app
		<< " is a program to manage elektra's key database\n"
		<< "Run a command with -H to show a help text for\n"
		<< "this command."
		<< std::endl;
	std::cerr << "Known commands are:" << std::endl;
	for (
		std::vector<std::string>::iterator it =
		commands.begin();
		it != commands.end();
		it++)
	{
		std::cout << *it  << std::endl;
	}
}

void displayVersion()
{
	kdb::KDB kdb;
	kdb::KeySet versions;
	kdb::Key k("system/elektra/version", KEY_END);
	kdb.get (versions, k);
	kdb::Key kdb_version = versions.lookup("system/elektra/version/constants/KDB_VERSION");
	if (!kdb_version)
	{
		cerr << "Could not lookup KDB_VERSION key" << endl;
	}
	else
	{
		cout << "KDB_VERSION: " << kdb_version.getString() << endl;
	}
	kdb::Key so_version = versions.lookup("system/elektra/version/constants/SO_VERSION");
	if (!so_version)
	{
		cerr << "Could not lookup SO_VERSION key" << endl;
	}
	else
	{
		cout << "SO_VERSION: " << so_version.getString() << endl;
	}
}

int main(int argc, char**argv)
{
	Factory f;

	if (argc < 2 )
	{
		displayHelp(argv[0], f.getCommands());
		return 0;
	}

	string command = argv[1];

	if (command == "-H" || command == "--help")
	{
		displayHelp(argv[0], f.getCommands());
		return 0;
	}

	if (command == "-V" || command == "--version")
	{
		displayVersion();
		return 0;
	}

	try {
		std::auto_ptr<Command> cmd = f.get(command);
		Cmdline cl (argc, argv,
				cmd->getShortOptions(),
				cmd->getLongHelpText());

		if (cl.version)
		{
			displayVersion();
			return 0;
		}

		if (cl.help)
		{
			cerr << cl << endl;
			return 0;
		}

		if (cl.invalidOpt)
		{
			cerr << "Invalid options passed\n" << endl;
			cerr << cl << endl;
			return 1;
		}

		try
		{
			return cmd->execute (cl);
		}
		catch (std::invalid_argument const& ia)
		{
			cerr << "Invalid arguments passed: " << ia.what()
				<< endl << endl;
			cerr << cl << endl;
			return 2;
		}
	}
	catch (CommandException const& ce)
	{
		std::cerr << "The command "
			<< command
			<< " terminated unsuccessfully with the info: "
			<< ce.what()
			<< std::endl;
		return 3;
	}
	catch (UnknownCommand const& uc)
	{
		std::cerr << "The command "
			<< command
			<< " is not known"
			<< std::endl;
		displayHelp(argv[0], f.getCommands());
		return 4;
	}
	catch (kdb::Key& key)
	{
		std::cerr << command << " failed while accessing the key database"
			<< std::endl;
		printError(key);
		printWarnings(key);
		return 5;
	}
	catch (std::exception const& ce)
	{
		std::cerr << "The command "
			<< command
			<< " terminated unsuccessfully with the info: "
			<< ce.what()
			<< std::endl;
		return 6;
	}
	catch (...)
	{
		std::cerr << "Unkown error" << std::endl;
		displayHelp(argv[0], f.getCommands());
		return 7;
	}
}
