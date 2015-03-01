/**
 * \file
 *
 * \brief Tests for the Backend class
 *
 * \copyright BSD License (see doc/COPYING or http://www.libelektra.org)
 *
 */


#include <backend.hpp>
#include <keysetio.hpp>

#include <gtest/gtest.h>

TEST(SameMountpoint, setMountpoint)
{
	using namespace kdb;
	using namespace kdb::tools;
	KeySet ks;

	Backend b1;
	b1.setMountpoint(Key("user/hello", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user//hello", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user////hello", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user////hello//", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user////hello////.", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user////hello////.//", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");

	b1.setMountpoint(Key("user////hello////.//x/..", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");
}

TEST(SameMountpoint, wrongMountpoints)
{
	using namespace kdb;
	using namespace kdb::tools;
	KeySet ks;

	Backend b1;
	ASSERT_THROW(b1.setMountpoint(Key(static_cast<ckdb::Key*>(0)), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key(".", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("invalid", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	/*
	ASSERT_THROW(b1.setMountpoint(Key("/..", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("/../..", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("user/../..", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("system/../..", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	ASSERT_THROW(b1.setMountpoint(Key("dir/../..", KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b1.getMountpoint(), "");
	*/
}

TEST(SameMountpoint, notSame)
{
	using namespace kdb;
	using namespace kdb::tools;
	KeySet ks;

	Backend b1;
	b1.setMountpoint(Key("user/hello", KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), "user/hello");
	b1.serialize(ks);

	Backend b2;
	b2.setMountpoint(Key("user/else", KEY_END), ks);
	EXPECT_EQ(b2.getMountpoint(), "user/else");
	b2.serialize(ks);

	Backend b3;
	b2.setMountpoint(Key("/somewhere", KEY_END), ks);
	EXPECT_EQ(b2.getMountpoint(), "/somewhere");
	b2.serialize(ks);
}

void checkSame(std::string name1, std::string name2)
{
	using namespace kdb;
	using namespace kdb::tools;
	KeySet ks;

	Backend b1;
	EXPECT_EQ(b1.getMountpoint(), "");
	b1.setMountpoint(Key(name1.c_str(), KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), name1);
	b1.serialize(ks);

	Backend b2;
	EXPECT_EQ(b2.getMountpoint(), "");
	ASSERT_THROW(b2.setMountpoint(Key(name2.c_str(), KEY_END), ks), kdb::tools::MountpointAlreadyInUseException);
	EXPECT_EQ(b2.getMountpoint(), "");
}

void checkAllow(std::string name1, std::string name2)
{
	using namespace kdb;
	using namespace kdb::tools;
	KeySet ks;

	Backend b1;
	EXPECT_EQ(b1.getMountpoint(), "");
	b1.setMountpoint(Key(name1.c_str(), KEY_END), ks);
	EXPECT_EQ(b1.getMountpoint(), name1);
	b1.serialize(ks);

	Backend b2;
	EXPECT_EQ(b2.getMountpoint(), "");
	ASSERT_NO_THROW(b2.setMountpoint(Key(name2.c_str(), KEY_END), ks));
	EXPECT_EQ(b2.getMountpoint(), name2);
}


TEST(SameMountpoint, exactlySameHelloSpec) { checkSame("spec/hello", "spec/hello"); }
TEST(SameMountpoint, exactlySameHelloProc) { checkSame("proc/hello", "proc/hello"); }
TEST(SameMountpoint, exactlySameHelloDir) { checkSame("dir/hello", "dir/hello"); }
TEST(SameMountpoint, exactlySameHelloUser) { checkSame("user/hello", "user/hello"); }
TEST(SameMountpoint, exactlySameHelloSystem) { checkSame("system/hello", "system/hello"); }

TEST(SameMountpoint, cascadingFirstSameHelloSpec) { checkAllow("/hello", "spec/hello"); }
TEST(SameMountpoint, cascadingFirstSameHelloProc) { checkSame("/hello", "proc/hello"); }
TEST(SameMountpoint, cascadingFirstSameHelloDir) { checkSame("/hello", "proc/hello"); }
TEST(SameMountpoint, cascadingFirstSameHelloUser) { checkSame("/hello", "user/hello"); }
TEST(SameMountpoint, cascadingFirstSameHelloSystem) { checkSame("/hello", "system/hello"); }

TEST(SameMountpoint, cascadingSameHelloSpec) { checkAllow("spec/hello", "/hello"); }
TEST(SameMountpoint, cascadingSameHelloProc) { checkSame("proc/hello", "/hello"); }
TEST(SameMountpoint, cascadingSameHelloDir) { checkSame("dir/hello", "/hello"); }
TEST(SameMountpoint, cascadingSameHelloUser) { checkSame("user/hello", "/hello"); }
TEST(SameMountpoint, cascadingSameHelloSystem) { checkSame("system/hello", "/hello"); }

TEST(SameMountpoint, exactlySameElektraSystem) { checkSame("system/hello", "system/elektra"); }
TEST(SameMountpoint, cascadingSameElektra) { checkSame("system/hello", "/elektra"); }

TEST(SameMountpoint, exactlySameSpec) { checkSame("spec/an/more/involved/deeper/mountpoint", "spec/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, exactlySameProc) { checkSame("proc/an/more/involved/deeper/mountpoint", "proc/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, exactlySameDir) { checkSame("dir/an/more/involved/deeper/mountpoint", "dir/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, exactlySameUser) { checkSame("user/an/more/involved/deeper/mountpoint", "user/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, exactlySameSystem) { checkSame("system/an/more/involved/deeper/mountpoint", "system/an/more/involved/deeper/mountpoint"); }

TEST(SameMountpoint, cascadingFirstSameSpec) { checkAllow("/an/more/involved/deeper/mountpoint", "spec/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingFirstSameProc) { checkSame("/an/more/involved/deeper/mountpoint", "proc/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingFirstSameDir) { checkSame("/an/more/involved/deeper/mountpoint", "dir/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingFirstSameUser) { checkSame("/an/more/involved/deeper/mountpoint", "user/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingFirstSameSystem) { checkSame("/an/more/involved/deeper/mountpoint", "system/an/more/involved/deeper/mountpoint"); }

TEST(SameMountpoint, cascadingSameSpec) { checkAllow("spec/an/more/involved/deeper/mountpoint", "/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingSameProc) { checkSame("proc/an/more/involved/deeper/mountpoint", "/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingSameDir) { checkSame("dir/an/more/involved/deeper/mountpoint", "/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingSameUser) { checkSame("user/an/more/involved/deeper/mountpoint", "/an/more/involved/deeper/mountpoint"); }
TEST(SameMountpoint, cascadingSameSystem) { checkSame("system/an/more/involved/deeper/mountpoint", "/an/more/involved/deeper/mountpoint"); }

TEST(SameMountpoint, sameRoot) { checkSame("/", "/"); }
TEST(SameMountpoint, sameRootSpec) { checkSame("/", "spec"); }
TEST(SameMountpoint, sameRootProc) { checkSame("/", "proc"); }
TEST(SameMountpoint, sameRootDir) { checkSame("/", "dir"); }
TEST(SameMountpoint, sameRootUser) { checkSame("/", "user"); }
TEST(SameMountpoint, sameRootSystem) { checkSame("/", "system"); }

TEST(SameMountpoint, sameRootSlashSpec) { checkSame("/", "spec/"); }
TEST(SameMountpoint, sameRootSlashProc) { checkSame("/", "proc/"); }
TEST(SameMountpoint, sameRootSlashDir) { checkSame("/", "dir/"); }
TEST(SameMountpoint, sameRootSlashUser) { checkSame("/", "user/"); }
TEST(SameMountpoint, sameRootSlashSystem) { checkSame("/", "system/"); }
