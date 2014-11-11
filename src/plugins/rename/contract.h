/*
 * contract.h
 *
 *  Created on: 25 Jul 2014
 *      Author: felixl
 */

#ifndef CONTRACT_RENAME_H_
#define CONTRACT_RENAME_H_

// @formatter:off

ksNew (30,
			keyNew ("system/elektra/modules/rename",
				KEY_VALUE, "rename plugin waits for your orders", KEY_END),
			keyNew ("system/elektra/modules/rename/exports", KEY_END),
			keyNew ("system/elektra/modules/rename/exports/get",
				KEY_FUNC, elektraRenameGet,
				KEY_END),
			keyNew ("system/elektra/modules/rename/exports/set",
				KEY_FUNC, elektraRenameSet,
				KEY_END),
#include "readme_rename.c"
			keyNew ("system/elektra/modules/rename/infos",
				KEY_VALUE, "All information you want to know", KEY_END),
			keyNew ("system/elektra/modules/rename/infos/version",
				KEY_VALUE, PLUGINVERSION, KEY_END),
			KS_END);

// @formatter:on

#endif /* CONTRACT_RENAME_H_ */
