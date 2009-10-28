#ifndef QUICKGUISKINTYPEMANAGER_H
#define QUICKGUISKINTYPEMANAGER_H

#include "QuickGUIException.h"
#include "QuickGUIExportDLL.h"
#include "QuickGUISkinType.h"

#include "OgreSingleton.h"

#include <map>

namespace QuickGUI
{
	class _QuickGUIExport SkinTypeManager :
		public Ogre::Singleton<SkinTypeManager>,
		public Ogre::GeneralAllocatedObject
	{
	public:
		friend class SkinDefinitionManager;
	public:

		static SkinTypeManager& getSingleton(void);
		static SkinTypeManager* getSingletonPtr(void);

		void addSkinType(const Ogre::String& className, const Ogre::String& typeName, SkinType* t);

		SkinType* getSkinType(const Ogre::String& className, const Ogre::String& typeName);

		bool hasSkinType(const Ogre::String& className, const Ogre::String& typeName);

		/**
		* This function checks the ScriptReader to see if new SkinType definitions have been parsed.
		* If the SkinType library needs to be updated, it will first be cleared and then rebuilt
		* according to the ScriptDefinitions held by the ScriptReader.
		* NOTE: this function should be called whenever more SkinTypes have been parsed, to make sure the
		*       library is up to date. (ie, if you load resource groups at different times and some contain
		*       SkinType definitions, this function should be called.  When in doubt, call this function.
		*/
		void loadTypes();

		void saveTypesToFile(const Ogre::String& fileName);

	protected:
		SkinTypeManager();
		virtual ~SkinTypeManager();

		std::map<Ogre::String, std::map<Ogre::String,SkinType*> > mSkinTypes;

	private:
	};
}

#endif
