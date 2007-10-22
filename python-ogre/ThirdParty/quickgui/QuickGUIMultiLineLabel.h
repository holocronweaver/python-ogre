#ifndef QUICKGUIMULTILINELABEL_H
#define QUICKGUIMULTILINELABEL_H

#include "QuickGUIList.h"
#include "QuickGUILabel.h"

#include <vector>

namespace QuickGUI
{
	class _QuickGUIExport MultiLineLabel :
		public Label
	{
	public:
		MultiLineLabel(const Ogre::String& instanceName, Size pixelSize, Ogre::String textureName, GUIManager* gm);
		~MultiLineLabel();

		/**
		* Aligns the child Label widget horizontally and vertically
		*/
		virtual void alignText();
		void allowScrolling(bool allow);

		virtual void clearText();

		/**
		* Disable Widget, making it unresponsive to events.
		*/
		virtual void disable();
		/**
		* Enable Widget, allowing it to accept and handle events.
		*/
		virtual void enable();

		virtual Ogre::UTFString getText();

		bool scrollingAllowed();
		/**
		* Sets the color of the text when the widget is disabled.
		*/
		virtual void setDisabledTextColor(const Ogre::ColourValue& c);
		virtual void setFont(const Ogre::String& fontScriptName, bool recursive = false);
		/**
		* Sets text horizontal alignment.
		*/
		virtual void setHorizontalAlignment(HorizontalAlignment ha);
		virtual void setText(const Ogre::UTFString& text);
		virtual void setTextColor(Ogre::ColourValue color);
		/**
		* Sets text vertical alignment.
		*/
		virtual void setVerticalAlignment(VerticalAlignment va);

	protected:
		Ogre::UTFString mCaption;
		List* mTextList;
		// if ScrollBar is visible, width used to calculating a line of text is modified.
		Ogre::Real mOffset;

		bool mScrollingAllowed;

		// Properties that are used across all TextBoxes
		Ogre::String mTextBoxTextureName;
		Ogre::String mTextBoxFont;

		// Finds the end of the last word that can fit in one text box.
		int _getLine(int startIndex);
	};
}

#endif