#ifndef QUICKGUISHEET_H
#define QUICKGUISHEET_H

#include "OgreException.h"

#include "QuickGUIPrerequisites.h"
#include "QuickGUIPanel.h"
#include "QuickGUIWindow.h"

namespace QuickGUI
{
	/** Represents a transparent area for holding widgets.
		@remarks
		The Sheet class is derived from a Window, although it
		differs because it is the size of the screen.  
		In addition, the Sheet class can create Windows.
	*/
	class _QuickGUIExport Sheet :
		public Panel
	{
	public:
		friend class GUIManager;
	public:
		/** Constructor
            @param
                name The name to be given to the widget (must be unique).
			@note
				If you want a transparent background, pass "" as the material.
        */
		Sheet(const Ogre::String& name, const Ogre::String &skinName, GUIManager* gm);
		virtual ~Sheet();

		/** Create a Window with material, borders, and TitleBar
            @param
                dimensions The relative x Position, y Position, width, and height of the window.
			@param
                material Ogre material to define the Window image.
			@param
				toggle visibility.
			@note
				Name for window will be autogenerated. (Convenience method)
        */
		Window* createWindow();
		Window* createWindow(const Ogre::String& name);
		
		virtual Widget* getTargetWidget(const Point& pixelPosition);
		Window* getWindow(const Ogre::String& name);

		/**
		* Applies the texture to the Quad if exists in some form, and updates the Image used for
		* transparency picking.
		*/
		virtual void setTexture(const Ogre::String& textureName);

	protected:
		virtual void setHeight(Ogre::Real pixelHeight);

		virtual void setQuadContainer(QuadContainer* container);

		/**
		* Manually set size of widget.
		*/
		virtual void setSize(const Ogre::Real& pixelWidth, const Ogre::Real& pixelHeight);
		virtual void setSize(const Size& pixelSize);
		virtual void setWidth(Ogre::Real pixelWidth);
	};
}

#endif