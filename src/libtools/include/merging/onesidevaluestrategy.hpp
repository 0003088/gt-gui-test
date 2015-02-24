/*
 * onesidevaluestrategy.hpp
 *
 *  Created on: 12 Aug 2014
 *      Author: felixl
 */

#ifndef ONESIDEVALUESTRATEGY_HPP_
#define ONESIDEVALUESTRATEGY_HPP_

#include <merging/mergeconflictstrategy.hpp>

namespace kdb
{

namespace tools
{

namespace merging
{
// This strategy is a subset of the OneSideStrategy. It also uses
// the key of the winning side in case of a conflict. However, different
// than the OneSideStrategy it only resolves conflicts where no new keys are
// introduced or old ones deleted.
class OneSideValueStrategy : public MergeConflictStrategy
{

public:
	ConflictResolutionSide winningSide;

	OneSideValueStrategy(ConflictResolutionSide _winningSide) : winningSide (_winningSide)
	{
	}

	virtual void resolveConflict(const MergeTask& task, Key& conflictKey, MergeResult& result);
};

}
}
}

#endif /* ONESIDEVALUESTRATEGY_HPP_ */
