/*
This file is part of Caelum.
See http://www.ogre3d.org/wiki/index.php/Caelum 

Copyright (c) 2006-2007 Caelum team. See Contributors.txt for details.

Caelum is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Caelum is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Caelum. If not, see <http://www.gnu.org/licenses/>.
*/

#include "CaelumPrecompiled.h"
#include "UniversalClock.h"

namespace caelum {

UniversalClock::UniversalClock () {
	setSecondsPerDay (86400);
	setDaysPerYear (365);

	setTimeScale (1.0);
	setUpdateRate (0);	// Sets a new mTimeSinceLastUpdate value as well
}

void UniversalClock::setSecondsPerDay (const Ogre::Real secs) {
	mSecondsPerDay = secs;

	updateSecondsPerYear ();
}

Ogre::Real UniversalClock::getSecondsPerDay () const {
	return mSecondsPerDay;
}

void UniversalClock::setDaysPerYear (const Ogre::Real days) {
	mDaysPerYear = days;

	updateSecondsPerYear ();
}

Ogre::Real UniversalClock::getDaysPerYear () const {
	return mDaysPerYear;
}

void UniversalClock::setCurrentTime (const Ogre::Real sec) {
	mCurrentSecond = Ogre::Math::Abs (sec);

	mCurrentSecond -= mSecondsPerYear * Ogre::Math::Floor (mCurrentSecond / mSecondsPerYear);
}

void UniversalClock::setCurrentRelativeTime (const Ogre::Real time) {
	setCurrentTime (mSecondsPerYear * time);
}

Ogre::Real UniversalClock::getCurrentTime () const {
	return mCurrentSecond;
}

Ogre::Real UniversalClock::getCurrentRelativeTime () const {
	return getCurrentTime () / mSecondsPerYear;
}

Ogre::Real UniversalClock::getCurrentDayTime () const {
	return mCurrentSecond - (mSecondsPerDay * Ogre::Math::Floor (mCurrentSecond / mSecondsPerDay));
}

Ogre::Real UniversalClock::getCurrentRelativeDayTime () const {
	return getCurrentDayTime () / mSecondsPerDay;
}

void UniversalClock::setTimeScale (const Ogre::Real scale) {
	mTimeScale = scale;
}

Ogre::Real UniversalClock::getTimeScale () const {
	return mTimeScale;
}

void UniversalClock::setUpdateRate (const Ogre::Real rate) {
	mUpdateRate = rate;

	if (mUpdateRate < 0)
		mUpdateRate = 0;

	forceUpdate ();
}

Ogre::Real UniversalClock::getUpdateRate () const {
	return mUpdateRate;
}

bool UniversalClock::update (const Ogre::Real time) {
	setCurrentTime (mCurrentSecond + time * mTimeScale);

	mTimeSinceLastUpdate += time;

	if (mTimeSinceLastUpdate > mUpdateRate) {
		mTimeSinceLastUpdate -= mUpdateRate * Ogre::Math::Floor (mUpdateRate > 0 ? mTimeSinceLastUpdate / mUpdateRate : 0);

		return true;
	}
	else {
		return false;
	}
}

void UniversalClock::updateSecondsPerYear () {
	mSecondsPerYear = mSecondsPerDay * mDaysPerYear;

	forceUpdate ();
}

void UniversalClock::forceUpdate () {
	mTimeSinceLastUpdate = mUpdateRate + 1;
}

} // namespace caelum