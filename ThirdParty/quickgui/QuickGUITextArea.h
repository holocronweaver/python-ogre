#ifndef QUICKGUITEXTAREA_H
#define QUICKGUITEXTAREA_H

#include "QuickGUIPadding.h"
#include "QuickGUIText.h"
#include "QuickGUITextCursor.h"
#include "QuickGUITimerManager.h"
#include "QuickGUIWidget.h"

namespace QuickGUI
{
	class _QuickGUIExport TextAreaDesc :
		public WidgetDesc
	{
	public:
		TextAreaDesc();

		/// Amount of time until a cursor changes from visible to not visible, or vice versa.
		float cursorBlinkTime;
		Ogre::ColourValue defaultColor;
		Ogre::String defaultFontName;
		/// Amount of time a key must be held down before it starts repeating.
		float keyDownTime;
		/// Amount of time a key must be held down to repeat its input.
		float keyRepeatTime;
		unsigned int maxCharacters;
		float padding[PADDING_COUNT];
		TextAlignment textAlignment;

		/// Describes the Text used in this TextBox
		TextDesc textDesc;

		/**
		* Returns the class of Desc object this is.
		*/
		virtual Ogre::String getClass() { return "TextAreaDesc"; }
		/**
		* Returns the class of Widget this desc object is meant for.
		*/
		virtual Ogre::String getWidgetClass() { return "TextArea"; }

		// Factory method
		static WidgetDesc* factory() { return new TextAreaDesc(); }

		/**
		* Outlines how the desc class is written to XML and read from XML.
		*/
		virtual void serialize(SerialBase* b);
	};

	class _QuickGUIExport TextArea :
		public Widget
	{
	public:
		// Skin Constants
		static const Ogre::String BACKGROUND;
		// Define Skin Structure
		static void registerSkinDefinition();
	public:
		// Factory method
		static Widget* factory(const Ogre::String& widgetName);
	public:

		/**
		* Internal function, do not use.
		*/
		virtual void _initialize(WidgetDesc* d);

		/**
		* Adds a character to the end of the current text, and
		* positions the text cursor at the end of the text. (-1)
		*/
		void addCharacter(Ogre::UTFString::code_point cp);
		/**
		* Inserts a character into the text at the index given, and
		* positions the text cursor after that character.
		*/
		void addCharacter(Ogre::UTFString::code_point cp, int index);

		/**
		* Returns the class name of this Widget.
		*/
		virtual Ogre::String getClass();
		/**
		* Gets the text in UTFString form.
		*/
		Ogre::UTFString getText();

		void onWindowDrawn(const EventArgs& args);

		/**
		* Removes a character from the text at the index given, and
		* positions the text cursor before that character.
		* NOTE: If the cursor index is -1 it will stay -1.
		*/
		void removeCharacter(int index);

		/**
		* Sets the index of the text cursor. A cursor index represents the position
		* to the left of the character with the same index. -1 represents the right most
		* position.
		*/
		void setCursorIndex(int index);
		/**
		* Sets the color used when adding characters to the TextBox.
		*/
		void setDefaultColor(const Ogre::ColourValue& cv);
		/**
		* Sets the font used when adding character to the TextBox.
		*/
		void setDefaultFont(const Ogre::String& fontName);
		/**
		* Sets the maximum number of characters that this TextBox will support.
		*/
		void setMaxCharacters(unsigned int max);
		/**
		* Sets the distance between a Label border and the text.
		*/
		void setPadding(Padding p, float distance);
		/**
		* Internal function to set a widget's parent, updating its window reference and position.
		*/
		virtual void setParent(Widget* parent);

		/**
		* Recalculate Screen and client dimensions and force a redrawing of the widget.
		*/
		virtual void updateTexturePosition();

	protected:
		TextArea(const Ogre::String& name);
		virtual ~TextArea();

		Text* mText;

		TextCursor* mTextCursor;
		int mCursorIndex;
		Point mCursorPosition;

		/// Record is last key down was a function button or character input.
		bool mFunctionKeyDownLast;
		/// Store the last key that went down
		KeyEventArgs mLastKnownInput;
		/// Timer that repeats keys
		Timer* mKeyRepeatTimer;
		/**
		* Callback that repetitively inputs the last held down key.
		*/
		void keyRepeatTimerCallback();
		/// Timer that starts repeat timer
		Timer* mKeyDownTimer;
		/**
		* Callback that starts the key repeat timer.
		*/
		void keyDownTimerCallback();

		/// Storing reference to font for quick use.
		Ogre::FontPtr mCurrentFont;

		/// Timer that toggles cursor on and off.
		Timer* mBlinkTimer;
		/**
		* Toggles blinker on and off.
		*/
		void blinkTimerCallback();

		// Pointer pointing to mWidgetDesc object, but casted for quick use.
		TextAreaDesc* mDesc;

		/**
		* Outlines how the widget is drawn to the current render target
		*/
		virtual void onDraw();

		void onCharEntered(const EventArgs& args);
		void onKeyDown(const EventArgs& args);
		void onKeyUp(const EventArgs& args);
		void onKeyboardInputGain(const EventArgs& args);
		void onKeyboardInputLose(const EventArgs& args);
		void onTripleClick(const EventArgs& args);

	private:
	};
}

#endif
