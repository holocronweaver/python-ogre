/*---------------------------------------------------------------------------*\
** This source file is part of OgreAL                                        **
** an OpenAL plugin for the Ogre Rendering Engine.                           **
**                                                                           **
** Copyright 2006 Casey Borders                                              **
**                                                                           **
** OgreAL is free software; you can redistribute it and/or modify it under   **
** the terms of the GNU Lesser General Public License as published by the    **
** Free Software Foundation; either version 2, or (at your option) any later **
** version.                                                                  **
**                                                                           **
** The developer really likes screenshots and while he recognises that the   **
** fact that this is an AUDIO plugin means that the fruits of his labor will **
** never been seen in these images he would like to kindly ask that you send **
** screenshots of your application using his library to                      **
** screenshots@mooproductions.org                                            **
**                                                                           **
** Please bear in mind that the sending of these screenshots means that you  **
** are agreeing to allow the developer to display them in the media of his   **
** choice.  They will, however, be fully credited to the person sending the  **
** email or, if you wish them to be credited differently, please state that  **
** in the body of the email.                                                 **
**                                                                           **
** OgreAL is distributed in the hope that it will be useful, but WITHOUT     **
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     **
** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for **
** more details.                                                             **
**                                                                           **
** You should have received a copy of the GNU General Public License along   **
** with OgreAL; see the file LICENSE.  If not, write to the                  **
** Free Software Foundation, Inc.,                                           **
** 59 Temple Place - Suite 330,                                              **
** Boston, MA 02111-1307, USA.                                               **
\*---------------------------------------------------------------------------*/

#include "OgreALListener.h"

template<> OgreAL::Listener* Ogre::Singleton<OgreAL::Listener>::ms_Singleton = 0;

namespace OgreAL {
	Listener::Listener() :
		mGain(1.0),
		mPosition(Ogre::Vector3::ZERO),
		mDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
		mVelocity(Ogre::Vector3::ZERO),
		mUp(Ogre::Vector3::UNIT_Y),
		mDerivedPosition(Ogre::Vector3::ZERO),
		mDerivedDirection(Ogre::Vector3::NEGATIVE_UNIT_Z)
	{
		mParentNode = NULL;
		mLocalTransformDirty = false;
		initListener();
	}

	Listener::Listener(const Ogre::String& name) :
		Ogre::MovableObject(name),
		mGain(1.0),
		mPosition(Ogre::Vector3::ZERO),
		mDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
		mVelocity(Ogre::Vector3::ZERO),
		mUp(Ogre::Vector3::UNIT_Y),
		mDerivedPosition(Ogre::Vector3::ZERO),
		mDerivedDirection(Ogre::Vector3::NEGATIVE_UNIT_Z)
	{
		mParentNode = NULL;
		initListener();
	}
	
	Listener::~Listener()
	{}

	Listener* Listener::getSingletonPtr(void)
	{
		return ms_Singleton;
	}

	Listener& Listener::getSingleton(void)
	{  
		assert( ms_Singleton );  return ( *ms_Singleton );  
	}

	void Listener::setGain(Ogre::Real gain)
	{
		mGain = gain;
		alListenerf(AL_GAIN, mGain);
	}
	
	void Listener::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;
		mLocalTransformDirty = true;
	}

	void Listener::setPosition(const Ogre::Vector3& vec)
	{
		mPosition = vec;
		mLocalTransformDirty = true;
	}

	const Ogre::Vector3& Listener::getPosition() const
	{
		return mPosition;
	}

	void Listener::setDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mDirection.x = x;
		mDirection.y = y;
		mDirection.z = z;
		mLocalTransformDirty = true;
	}

	void Listener::setDirection(const Ogre::Vector3& vec)
	{
		mDirection = vec;
		mLocalTransformDirty = true;
	}

	const Ogre::Vector3& Listener::getDirection() const
	{
		return mDirection;
	}

	void Listener::setVelocity(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mVelocity.x = x;
		mVelocity.y = y;
		mVelocity.z = z;
		alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	}

	void Listener::setVelocity(const Ogre::Vector3& vec)
	{
		mVelocity = vec;
		alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	}

	const Ogre::Vector3& Listener::getVelocity() const
	{
		return mVelocity;
	}

	const Ogre::Vector3& Listener::getDerivedPosition(void) const
	{
		update();
		return mDerivedPosition;
	}

	const Ogre::Vector3& Listener::getDerivedDirection(void) const
	{
		update();
		return mDerivedPosition;
	}

	void Listener::initListener()
	{
		mOrientation[0]= mDirection.x; // Forward.x
		mOrientation[1]= mDirection.y; // Forward.y
		mOrientation[2]= mDirection.z; // Forward.z

		mOrientation[3]= mUp.x; // Up.x
		mOrientation[4]= mUp.y; // Up.y
		mOrientation[5]= mUp.z; // Up.z

		alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
		alListenerfv(AL_ORIENTATION, mOrientation);
		alListenerf (AL_GAIN, 1.0f);
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	}

	void Listener::update() const
	{
		if (mParentNode)
        {
            if (!(mParentNode->_getDerivedOrientation() == mLastParentOrientation &&
                mParentNode->_getDerivedPosition() == mLastParentPosition)
                || mLocalTransformDirty)
            {
                // Ok, we're out of date with SceneNode we're attached to
                mLastParentOrientation = mParentNode->_getDerivedOrientation();
                mLastParentPosition = mParentNode->_getDerivedPosition();
                mDerivedDirection = mLastParentOrientation * mDirection;
                mDerivedPosition = (mLastParentOrientation * mPosition) + mLastParentPosition;
            }
        }
        else
        {
            mDerivedPosition = mPosition;
            mDerivedDirection = mDirection;
        }

        mLocalTransformDirty = false;
	}

	void Listener::updateListener()
	{
		update();
		if(mParentNode)
		{
			mPosition = mLastParentPosition;
			mDirection = mLastParentOrientation.zAxis();
			mUp = mLastParentOrientation.yAxis();
		}
		alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
		mOrientation[0]= -mDirection.x; // Forward.x
		mOrientation[1]= -mDirection.y; // Forward.y
		mOrientation[2]= -mDirection.z; // Forward.z

		mOrientation[3]= mUp.x; // Up.x
		mOrientation[4]= mUp.y; // Up.y
		mOrientation[5]= mUp.z; // Up.z
		alListenerfv(AL_ORIENTATION, mOrientation); 
	}

	const Ogre::String& Listener::getMovableType() const
	{
		return ListenerFactory::FACTORY_TYPE_NAME;
	}

	const Ogre::AxisAlignedBox& Listener::getBoundingBox() const
	{
		// Null, Sounds are not visible
		static Ogre::AxisAlignedBox box;
		return box;
	}

	void Listener::_updateRenderQueue(Ogre::RenderQueue* queue)
	{
		// Do Nothing
	}

	void Listener::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		mParentNode = parent;
	}

	Ogre::String ListenerFactory::FACTORY_TYPE_NAME = "OgreAL_Listener";

	const Ogre::String& ListenerFactory::getType(void) const
	{
		return FACTORY_TYPE_NAME;
	}

	Ogre::MovableObject* ListenerFactory::createInstanceImpl(const Ogre::String& name, 
				const Ogre::NameValuePairList* params)
	{
		Listener *listener = Listener::getSingletonPtr();
		if(listener)
			return listener;
		else
			return new Listener(name);
	}

	void ListenerFactory::destroyInstance(Ogre::MovableObject* obj)
	{
		delete obj;
	}
}