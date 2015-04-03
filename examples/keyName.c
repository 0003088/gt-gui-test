#include <assert.h>
#include <string.h>

#include <kdb.h>

int main()
{
//! [add name]
Key *k = keyNew("user/x/r", KEY_END);
keyAddName(k, "../y/a//././z");
assert(!strcmp(keyName(k), "user/x/a/z"));
keyDel(k);
//! [add name]
//! [namespace]
Key *n = keyNew("user/away", KEY_END);
keyAddName(n, "../../../new/name");
assert(!strcmp(keyName(n), "user/new/name"));
keyDel(n);
//! [namespace]
}
