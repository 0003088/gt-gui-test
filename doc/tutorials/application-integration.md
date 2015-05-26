# Introduction

Applications should use Elektra to read (and store) configurations for a
overall better integrated system.
A light integration would be to write parsers for the configuration files
of your application. This yields following advantages:

- applications and administrators can easily change individual values
- import/export of configuration


For a full integration, however, the application needs to be patched
to directly access Elektra's key database.
When the application is fully integrated in the Elektra's ecosystem
additional benefits arise:

- Benefits that shared libraries have, e.g.
 - All applications profit from fixes, optimization and new features
 - Less memory consumption, because the libraries executable
     instructions are only loaded once
 - Faster development time, because many non-trivial problems (e.g.
     OS-dependent resolving of configuration file names with atomic
     updates) are already solved and tested properly
- The administrator can choose:
 - the configuration file syntax (e.g. XML or JSON)
 - notification and logging on configuration changes
 - and all other features [the plugins provide](/src/plugins)
- Other applications can use your configuration as override or as
    fallback

The process to make applications aware of other's configuration is
called "to elektrify". This tutorial is both for new and existing
applications.


## Get Started

As first step in a C-applicaiton you need to create an in-memory `Key`. Such a `Key` is
Elektra's atomic unit and consists of:

- a unique name
- a value
- meta data

`Key`s are either associated with entries in configuration files or used
as arguments in the API to transport some information.

Thus a key is only in-memory and does not need any of the other Elektra's objects
we always can create one:

	Key *parentKey = keyNew("/sw/org/myapp/#0/current",
		KEY_CACADING_NAME,
		KEY_END);

- The first argument of `keyNew` always is the name of the key.
 - `sw` is for software
 - `org` is a URL/organisation name to avoid name clashes with other
     application names
 - `myapp` is the name of the most specific component that has its own
     configuration
 - `#0` is the version number of the configuration
 - `current` is the profile to be used. This is needed by administrators
     if they want to start up multiple applications with different
     configurations.
- `KEY_CACADING_NAME` is needed to accept a name starting with `/`. Such a
    name cannot physically exist in configuration files, but they are
    the most important keys to actually work with configuration within
    applications as we will see in this tutorial.
- `KEY_END` is needed because C needs a proper termination of variable
    length arguments.

[See API doc of Key for more information.](http://doc.libelektra.org/api/current/html/group__key.html)


Now we have the `Key` we will use to pass as argument.
First we open our key database (KDB). This is done by:

	KDB *repo = kdbOpen(parentKey);

A `Key` is seldom alone, but they are often found in groups, e.g. in
configuration files. To represent many keys (a set of keys) Elektra
has the data structure `KeySet`. Because the `Key`'s name is unique we
can easily lookup keys in a `KeySet` without ambiguity. Additionally, we
can can iterate over all `Key`s in a `KeySet` without a hassle.
To create an empty `KeySet` we use:

	KeySet *conf = ksNew(200,
		KS_END);

- 200 is an approximation for how many `Key`s we think we will have in
    the `KeySet` conf
- After the first argument we can list hardcoded keys.
- The last argument needs to be `KS_END`.

Now we have everything ready to fetch the latest configuration:

	kdbGet(repo, conf, parentKey);


## Lookup

To lookup a key, we simply use, e.g.:

	Key *k = ksLookupByName(conf,
		"/sw/org/myapp/#0/current/section/subsection/key",
		0);

Thus we are interested in the value we use:

	char *val = keyString(k);

Finally, we need to convert the configuration value to the datatype we
need.

Obviously, to do this manually has severe drawbacks:

- names are hardcoded: might have typos or might be inconsistent
- tedious handling if key or value might be absent
- converting to needed data type is error prone

So (larger) applications should not directly use the `KeySet`, but
instead use code generation that provides a type-safe front-end.

For more information about that, continue reading
[here](https://github.com/ElektraInitiative/libelektra/tree/master/src/tools/gen)
