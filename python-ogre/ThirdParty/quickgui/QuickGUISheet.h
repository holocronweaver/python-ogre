#ifndef QUICKGUISHEET_H
#define QUICKGUISHEET_H

#include "OgreException.h"

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
		/** Constructor
            @param
                name The name to be given to the widget (must be unique).
			@note
				If you want a transparent background, pass "" as the material.
        */
		Sheet(const Ogre::String& name, Type type, const Ogre::String& texture, GUIManager* gm);
		virtual ~Sheet();

		/**
		* Internal method that sets the pixel location and size of the widget.
		*/
		void _applyDimensions();

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
		Window* createWindow(const Rect& pixelDimensions, const Ogre::String& texture);
		/** Create a Window with material, borders, and TitleBar
			@param
				name Window name, must be unique.
            @param
                dimensions The relative x Position, y Position, width, and height of the window.
			@param
                material Ogre material to define the Window image.
        */
		Window* createWindow(const Ogre::String& name, const Rect& pixelDimensions, const Ogre::String& texture);
		/** Create a Window with material, borders, and TitleBar
			@param
				name Window name, must be unique.
            @param
                dimensions The relative x Position, y Position, width, and height of the window.
			@note
				Default Skin material for windows will be applied.
        */
		Window* createWindow(const Ogre::String& name, const Rect& pixelDimensions);
		/** Create a Window with material, borders, and TitleBar
            @param
                dimensions The relative x Position, y Position, width, and height of the window.
			@note
				Name for window will be autogenerated. (Convenience method)
			@note
				Default Skin material for windows will be applied.
        */
		Window* createWindow(const Rect& pixelDimensions);
		
		Ogre::String getDefaultFont();
		Ogre::ColourValue getDefaultTextColor();
		Ogre::String getDefaultSkin();
		Window* getWindow(const Ogre::String& name);

		void setDefaultFont(const Ogre::String& font);
		void setDefaultTextColor(const Ogre::ColourValue& color);
		/**
		* Sets the default skin, used for widgets with no specified material.
		*/
		void setDefaultSkin(const Ogre::String& skin);

	protected:
		int	mAutoNameWindowCounter;

		Ogre::String mDefaultSkin;
		Ogre::String mDefaultFont;
		Ogre::ColourValue mDefaultTextColor;

		/** Create a Window with material, borders, and TitleBar
            @param
                name The name to be given to the Window (must be unique).
            @param
                dimensions The relative x Position, y Position, width, and height of the window.
			@param
                material Ogre material to define the Window image.
			@param
				toggle visibility.
			@note
				Private function preventing users from setting the Widget Instance Name.  Names
				can be given to Windows using the "setReferenceName()" function.
        */
		Window* _createWindow(const Ogre::String& name, const Rect& pixelDimensions, const Ogre::String& texture);
	};
}

#endif