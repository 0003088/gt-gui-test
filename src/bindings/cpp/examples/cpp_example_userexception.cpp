/**
 * @file
 *
 * This example explains how to define user defined exception.
 *
 * That means that the user can implement their own exceptions,
 * e.g. potentially derived from their own base class with its
 * own what() information.
 * The user could even decide to use another mechanism
 * instead of what().
 *
 * The only requirements are: They must be named exactly like
 * the original classes and KDBException constructor must take a key
 * as argument (which has all information like error and warnings).
 *
 * It does not matter from which class the exceptions are derived
 * or which members they have (if they are binary compatible) as
 * long as they are used everywhere.
 *
 * Never use non-binary compatible user exceptions if you do not
 * use them everywhere where you include any of elektras header
 * files!
 */
#include <stdexcept>

class UserException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception";
	}
};

namespace kdb
{

class Exception : public UserException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Exception thrown by Elektra";
	}
};

class KeyException : public Exception
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Exception thrown by a Key";
	}
};

class KeyTypeMismatch: public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Binary or String key mismatch";
	}
};

class KeyInvalidName : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Invalid Keyname";
	}
};

class KeyMetaException : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Exception thrown by Key Meta Data related Operations";
	}
};

class KeyNoSuchMeta : public KeyMetaException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: No such meta data";
	}
};

class KeyBadMeta : public KeyMetaException
{
public:
	virtual const char* what() const throw()
	{
		return "User Exception: Could not convert bad meta data";
	}
};

}

#define USER_DEFINED_EXCEPTIONS
#include <key.hpp>

namespace kdb
{

class KDBException : public Exception
{
public:
	KDBException (Key key) :
		m_key (key)
	{}

	virtual ~KDBException() throw()
	{}

	virtual const char* what() const throw()
	{
		return "User Exception: KDB";
	}
private:
	Key m_key;
};

}


#include <kdb.hpp>

int main()
{
	kdb::Key k ("abc", KEY_END);
	kdb::KDB kdb;
	kdb::KeySet ks;
	kdb.get(ks, k);
}
