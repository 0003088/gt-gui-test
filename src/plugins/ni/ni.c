/***************************************************************************
                     ni.c  -  Skeleton of a plugin
                             -------------------
    begin                : Fri May 21 2010
    copyright            : (C) 2010 by Markus Raab
    email                : elektra@markus-raab.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This is the skeleton of the methods you'll have to implement in order *
 *   to provide a valid plugin.                                            *
 *   Simple fill the empty functions with your code and you are            *
 *   ready to go.                                                          *
 *                                                                         *
 ***************************************************************************/


#include "ni.h"

#ifndef HAVE_KDBCONFIG
# include "kdbconfig.h"
#endif

#include <kdberrors.h>

#include <string.h>
#include <errno.h>

int elektraNiGet(Plugin *handle ELEKTRA_UNUSED, KeySet *returned, Key *parentKey)
{
	/* get all keys */

	if (!strcmp (keyName(parentKey), "system/elektra/modules/ni"))
	{
		KeySet *moduleConfig = ksNew (30,
			keyNew ("system/elektra/modules/ni",
				KEY_VALUE, "ni plugin waits for your orders", KEY_END),
			keyNew ("system/elektra/modules/ni/exports", KEY_END),
			keyNew ("system/elektra/modules/ni/exports/get",
				KEY_FUNC, elektraNiGet, KEY_END),
			keyNew ("system/elektra/modules/ni/exports/set",
				KEY_FUNC, elektraNiSet, KEY_END),
#include "readme_ni.c"
			keyNew ("system/elektra/modules/ni/infos/version",
				KEY_VALUE, PLUGINVERSION, KEY_END),
			KS_END);
		ksAppend (returned, moduleConfig);
		ksDel (moduleConfig);
		return 1;
	}

	Ni_node root = Ni_New();
	int errnosave = errno;
	int error = Ni_ReadFile(root, keyString(parentKey), 0);
	if (error == 0)
	{
		Ni_Free(root);
		ELEKTRA_SET_ERROR_GET(parentKey);
		errno = errnosave;
		return -1;
	}

	Ni_node current = NULL;
	while ((current = Ni_GetNextChild(root, current)) != NULL)
	{
		Key *k = keyNew(keyName(parentKey), KEY_END);
		keyAddName (k, Ni_GetName(current, NULL));
		keySetString (k, Ni_GetValue (current, NULL));
		Ni_node mcur = NULL;
		while ((mcur = Ni_GetNextChild(current, mcur)) != NULL)
		{
			keySetMeta (k, Ni_GetName(mcur, NULL), Ni_GetValue (mcur, NULL));
			// printf("get meta %s %s from %s\n", Ni_GetName(mcur, NULL), Ni_GetValue (mcur, NULL), keyName(k));
		}
		ksAppendKey (returned, k);
	}

	Ni_Free(root);

	return 1; /* success */
}

static void keyMetaToNi(Ni_node add, Key *cur)
{
	Ni_SetValue (add, keyString(cur), keyGetValueSize(cur)-1);

	const Key *m;
	keyRewindMeta(cur);
	while ((m = keyNextMeta(cur)) != 0)
	{
		// printf("set meta %s %s from %s\n", keyName(m), keyString(m), keyName(cur));
		Ni_node madd = Ni_GetChild(add, keyName(m), keyGetNameSize(m)-1, 1, 0);
		Ni_SetValue (madd, keyString(m), keyGetValueSize(m)-1);
	}
}

int elektraNiSet(Plugin *handle ELEKTRA_UNUSED, KeySet *returned, Key *parentKey)
{
	/* set all keys */

	Ni_node root = Ni_New();

	Key *cur;
	ksRewind (returned);

	if (keyCmp(ksHead(returned), parentKey) == 0)
	{
		// printf ("found parentkey");
		Ni_node add = Ni_GetChild(root, NULL, 0, 1, 0);
		keyMetaToNi(add, ksHead(returned));
		ksNext(returned); // do not process parent in loop again
	}

	const size_t parentSize = keyGetNameSize(parentKey);
	while ((cur = ksNext(returned)) != 0)
	{
		const size_t curSize = keyGetNameSize(cur);
		Ni_node add = Ni_GetChild(root, keyName(cur)+parentSize, curSize-parentSize-1, 1, 0);
		keyMetaToNi(add, cur);
	}

	int errnosave = errno;
	int error = Ni_WriteFile (root,  keyString(parentKey), 0);
	Ni_Free(root);

	if (error == 0)
	{
		ELEKTRA_SET_ERROR_SET(parentKey);
		errno = errnosave;
		return -1;
	}

	return 1; /* success */
}

Plugin *ELEKTRA_PLUGIN_EXPORT(ni)
{
	return elektraPluginExport("ni",
		ELEKTRA_PLUGIN_GET,	&elektraNiGet,
		ELEKTRA_PLUGIN_SET,	&elektraNiSet,
		ELEKTRA_PLUGIN_END);
}

