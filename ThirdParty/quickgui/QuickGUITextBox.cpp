#include "QuickGUITextBox.h"
#include "QuickGUIWindow.h"
#include "QuickGUIManager.h"
#include "QuickGUIRoot.h"
#include "QuickGUISkinDefinitionManager.h"

namespace QuickGUI
{
	const Ogre::String TextBox::BACKGROUND = "background";
	const Ogre::String TextBox::TEXTOVERLAY = "textoverlay";

	void TextBox::registerSkinDefinition()
	{
		SkinDefinition* d = OGRE_NEW_T(SkinDefinition,Ogre::MEMCATEGORY_GENERAL)("TextBox");
		d->defineSkinElement(BACKGROUND);
		d->defineSkinElement(TEXTOVERLAY);
		d->definitionComplete();

		SkinDefinitionManager::getSingleton().registerSkinDefinition("TextBox",d);
	}

	TextBoxDesc::TextBoxDesc(const Ogre::String& id) :
		WidgetDesc(id)
	{
		resetToDefault();
	}

	void TextBoxDesc::resetToDefault()
	{
		WidgetDesc::resetToDefault();

		textbox_cursorBlinkTime = 0.5;
		textbox_defaultColor = Ogre::ColourValue::White;
		textbox_defaultFontName = Root::getSingleton().getDefaultFontName();
		textbox_horizontalTextAlignment = TEXT_ALIGNMENT_HORIZONTAL_LEFT;
		textbox_keyDownTime = 0.6;
		textbox_keyRepeatTime = 0.04;
		textbox_maxCharacters = 255;
		// 42 happens to be the code point for * on regular english true type fonts
		textbox_maskSymbol = 42;
		textbox_maskText = false;
		textbox_textCursorDefaultSkinTypeName = "default";
		textbox_verticalTextAlignment = TEXT_ALIGNMENT_VERTICAL_CENTER;

		textDesc.resetToDefault();
	}

	void TextBoxDesc::serialize(SerialBase* b)
	{
		WidgetDesc::serialize(b);

		b->IO("CursorBlinkTime",&textbox_cursorBlinkTime);
		b->IO("DefaultColor",&textbox_defaultColor);
		b->IO("DefaultFontName",&textbox_defaultFontName);
		b->IO("TBHorizontalTextAlignment",&textbox_horizontalTextAlignment);
		b->IO("KeyDownTime",&textbox_keyDownTime);
		b->IO("KeyRepeatTime",&textbox_keyRepeatTime);
		b->IO("MaskSymbol",static_cast<unsigned short*>(&textbox_maskSymbol));
		b->IO("MaskText",&textbox_maskText);
		b->IO("MaxCharacters",&textbox_maxCharacters);
		b->IO("TextCursorDefaultSkinTypeName",&textbox_textCursorDefaultSkinTypeName);
		b->IO("TextPosition",&textbox_textPosition);
		b->IO("VerticalTextAlignment",&textbox_verticalTextAlignment);

		textDesc.serialize(b);
	}

	TextBox::TextBox(const Ogre::String& name) :
		Widget(name),
		mText(NULL),
		mDesc(NULL),
		mTextCursor(NULL),
		mTextInputValidatorSlot(NULL),
		mCursorIndex(0),
		mBlinkTimer(NULL)
	{
		mSkinElementName = BACKGROUND;

		addWidgetEventHandler(WIDGET_EVENT_CHARACTER_KEY,&TextBox::onCharEntered,this);
		addWidgetEventHandler(WIDGET_EVENT_KEY_DOWN,&TextBox::onKeyDown,this);
		addWidgetEventHandler(WIDGET_EVENT_KEY_UP,&TextBox::onKeyUp,this);
		addWidgetEventHandler(WIDGET_EVENT_KEYBOARD_INPUT_GAIN,&TextBox::onKeyboardInputGain,this);
		addWidgetEventHandler(WIDGET_EVENT_KEYBOARD_INPUT_LOSE,&TextBox::onKeyboardInputLose,this);
		addWidgetEventHandler(WIDGET_EVENT_MOUSE_BUTTON_DOWN,&TextBox::onMouseButtonDown,this);
		addWidgetEventHandler(WIDGET_EVENT_MOUSE_CLICK_TRIPLE,&TextBox::onTripleClick,this);
		addWidgetEventHandler(WIDGET_EVENT_VISIBLE_CHANGED,&TextBox::onVisibleChanged,this);

		mTextCursor = OGRE_NEW_T(TextCursor,Ogre::MEMCATEGORY_GENERAL)(this);
	}

	TextBox::~TextBox()
	{
		TimerManager::getSingleton().destroyTimer(mBlinkTimer);
		TimerManager::getSingleton().destroyTimer(mKeyRepeatTimer);
		TimerManager::getSingleton().destroyTimer(mKeyDownTimer);

		OGRE_DELETE_T(mTextCursor,TextCursor,Ogre::MEMCATEGORY_GENERAL);

		OGRE_DELETE_T(mText,Text,Ogre::MEMCATEGORY_GENERAL);

		OGRE_DELETE_T(mTextInputValidatorSlot,TextInputValidatorSlot,Ogre::MEMCATEGORY_GENERAL);
	}

	void TextBox::_initialize(WidgetDesc* d)
	{
		TextBoxDesc* td = dynamic_cast<TextBoxDesc*>(d);

		// Create Timers before calling Widget::_initialize.  Some functions like setEnabled affect timers.
		TimerDesc timerDesc;
		timerDesc.repeat = true;
		timerDesc.timePeriod = td->textbox_cursorBlinkTime;
		mBlinkTimer = TimerManager::getSingleton().createTimer(timerDesc);
		mBlinkTimer->setCallback(&TextBox::blinkTimerCallback,this);

		timerDesc.timePeriod = td->textbox_keyRepeatTime;
		mKeyRepeatTimer = TimerManager::getSingleton().createTimer(timerDesc);
		mKeyRepeatTimer->setCallback(&TextBox::keyRepeatTimerCallback,this);

		timerDesc.repeat = false;
		timerDesc.timePeriod = td->textbox_keyDownTime;
		mKeyDownTimer = TimerManager::getSingleton().createTimer(timerDesc);
		mKeyDownTimer->setCallback(&TextBox::keyDownTimerCallback,this);

		Widget::_initialize(d);

		mDesc = dynamic_cast<TextBoxDesc*>(mWidgetDesc);

		mTextCursor->setSkinType(td->textbox_textCursorDefaultSkinTypeName);

		mDesc->widget_consumeKeyboardEvents = true;

		mFunctionKeyDownLast = false;

		// Make a copy of the Text Desc.  The Text object will
		// modify it directly, which is used for serialization.
		mDesc->textDesc = td->textDesc;

		setDefaultFont(td->textbox_defaultFontName);
		setDefaultColor(td->textbox_defaultColor);
		mDesc->textbox_maxCharacters = td->textbox_maxCharacters;

		// Set a really high width, we want everything on 1 line.
		mDesc->textDesc.allottedWidth = mDesc->textbox_maxCharacters * Text::getGlyphWidth(mDesc->textbox_defaultFontName,'0');
			
		mText = OGRE_NEW_T(Text,Ogre::MEMCATEGORY_GENERAL)(mDesc->textDesc);

		mDesc->textbox_cursorBlinkTime = td->textbox_cursorBlinkTime;
		mDesc->textbox_keyDownTime = td->textbox_keyDownTime;
		mDesc->textbox_keyRepeatTime = td->textbox_keyRepeatTime;

		setMaskText(td->textbox_maskText,td->textbox_maskSymbol);
		setMaxCharacters(td->textbox_maxCharacters);
		setSkinType(d->widget_skinTypeName);
	}

	Ogre::String TextBox::getClass()
	{
		return "TextBox";
	}

	void TextBox::addCharacter(Ogre::UTFString::code_point cp)
	{
		if(mText->getLength() >= static_cast<int>(mDesc->textbox_maxCharacters))
			return;

		if(mTextInputValidatorSlot != NULL)
			if(!mTextInputValidatorSlot->isInputValid(cp,mCursorIndex,mText->getText()))
				return;

		mText->addCharacter(OGRE_NEW_T(Character,Ogre::MEMCATEGORY_GENERAL)(cp,mCurrentFont,mDesc->textbox_defaultColor),mCursorIndex);
		setCursorIndex(mCursorIndex+1);
	}

	void TextBox::addText(Ogre::UTFString s, Ogre::FontPtr fp, const Ogre::ColourValue& cv)
	{
		mText->addText(s,fp,cv);

		redraw();
	}

	void TextBox::addText(Ogre::UTFString s, const Ogre::String& fontName, const Ogre::ColourValue& cv)
	{
		addText(s,Text::getFont(fontName),cv);
	}

	void TextBox::addText(Ogre::UTFString s)
	{
		addText(s,mDesc->textbox_defaultFontName,mDesc->textbox_defaultColor);
	}

	void TextBox::addText(std::vector<TextSegment> segments)
	{
		mText->addText(segments);

		redraw();
	}

	void TextBox::blinkTimerCallback()
	{
		mTextCursor->setVisible(!mTextCursor->getVisible());
	}

	void TextBox::clearTextInputValidator()
	{
		delete mTextInputValidatorSlot;
		mTextInputValidatorSlot = NULL;
	}

	void TextBox::clearText()
	{
		mText->clearText();
		
		setCursorIndex(0);

		mTextCursor->setVisible(false);

		redraw();
	}

	Ogre::ColourValue TextBox::getDefaultColor()
	{
		return mDesc->textbox_defaultColor;
	}

	Ogre::String TextBox::getDefaultFont()
	{
		return mDesc->textbox_defaultFontName;
	}

	Ogre::UTFString::code_point TextBox::getMaskSymbol()
	{
		return mDesc->textbox_maskSymbol;
	}

	bool TextBox::getMaskText()
	{
		return mDesc->textbox_maskText;
	}

	int TextBox::getMaxCharacters()
	{
		return mDesc->textbox_maxCharacters;
	}

	Rect TextBox::getScreenRect()
	{
		return mTextBoxClipRegion;
	}

	Ogre::UTFString TextBox::getText()
	{
		return mText->getText();
	}

	std::vector<TextSegment> TextBox::getTextSegments()
	{
		return mText->getTextSegments();
	}

	Ogre::String TextBox::getTextCursorSkinType()
	{
		return mDesc->textbox_textCursorDefaultSkinTypeName;
	}

	void TextBox::keyDownTimerCallback()
	{
		mKeyRepeatTimer->start();
	}

	void TextBox::keyRepeatTimerCallback()
	{
		if(mFunctionKeyDownLast)
			onKeyDown(mLastKnownInput);
		else
			onCharEntered(mLastKnownInput);
	}

	void TextBox::onCharEntered(const EventArgs& args)
	{
		const KeyEventArgs kea = dynamic_cast<const KeyEventArgs&>(args);
		mLastKnownInput.codepoint = kea.codepoint;
		mLastKnownInput.keyMask = kea.keyMask;
		mLastKnownInput.keyModifiers = kea.keyModifiers;

		addCharacter(mLastKnownInput.codepoint);
	}

	void TextBox::onDraw()
	{
		Brush* brush = Brush::getSingletonPtr();

		brush->setFilterMode(mDesc->widget_brushFilterMode);

		SkinType* st = mSkinType;
		if(!mWidgetDesc->widget_enabled && mWidgetDesc->widget_disabledSkinType != "")
			st = SkinTypeManager::getSingleton().getSkinType(getClass(),mWidgetDesc->widget_disabledSkinType);

		// Draw Background

		brush->drawSkinElement(Rect(mTexturePosition,mWidgetDesc->widget_dimensions.size),st->getSkinElement(mSkinElementName));

		Ogre::ColourValue prevColor = brush->getColour();
		Rect prevClipRegion = brush->getClipRegion();

		Rect clipRegion = Rect(mTexturePosition,mClientDimensions.size);
		clipRegion.translate(mClientDimensions.position);

		mTextBoxClipRegion = prevClipRegion.getIntersection(clipRegion);
		brush->setClipRegion(mTextBoxClipRegion);

		// Center Text Vertically

		float textHeight = mText->getTextHeight();
		float yPos = 0;

		switch(mDesc->textbox_verticalTextAlignment)
		{
		case TEXT_ALIGNMENT_VERTICAL_BOTTOM:
			yPos = mDesc->widget_dimensions.size.height - st->getSkinElement(mSkinElementName)->getBorderThickness(BORDER_BOTTOM) - textHeight;
			break;
		case TEXT_ALIGNMENT_VERTICAL_CENTER:
			yPos = (mDesc->widget_dimensions.size.height / 2.0) - (textHeight / 2.0);
			break;
		case TEXT_ALIGNMENT_VERTICAL_TOP:
			yPos = st->getSkinElement(mSkinElementName)->getBorderThickness(BORDER_TOP);
			break;
		}

		Point textbox_textPosition = mTexturePosition;
		textbox_textPosition.translate(Point(mClientDimensions.position.x,yPos));
		textbox_textPosition.translate(mDesc->textbox_textPosition);
		mText->draw(textbox_textPosition);

		// Draw Text Overlay SkinElement

		brush->drawSkinElement(mTextBoxClipRegion,st->getSkinElement(TEXTOVERLAY));

		// Restore clipping

		brush->setClipRegion(prevClipRegion);

		brush->setColor(prevColor);
	}

	void TextBox::onKeyDown(const EventArgs& args)
	{
		const KeyEventArgs kea = dynamic_cast<const KeyEventArgs&>(args);
		mLastKnownInput.keyMask = kea.keyMask;
		mLastKnownInput.keyModifiers = kea.keyModifiers;
		mLastKnownInput.scancode = kea.scancode;

		mFunctionKeyDownLast = true;
		mKeyDownTimer->start();

		switch(kea.scancode)
		{
		case KC_LEFT:
			if(kea.keyModifiers & CTRL)
				setCursorIndex(mText->getIndexOfPreviousWord(mCursorIndex));
			else
				setCursorIndex(mCursorIndex - 1);
			break;
		case KC_RIGHT:
			if(kea.keyModifiers & CTRL)
				setCursorIndex(mText->getIndexOfNextWord(mCursorIndex));
			else
				setCursorIndex(mCursorIndex + 1);
			break;
		case KC_DELETE:
			removeCharacter(mCursorIndex);
			break;
		case KC_BACK:
			removeCharacter(mCursorIndex - 1);
			break;
		case KC_END:
			setCursorIndex(mText->getLength());
			break;
		case KC_HOME:
			setCursorIndex(0);
			break;
		default:
			mFunctionKeyDownLast = false;
			break;
		}
	}

	void TextBox::onKeyUp(const EventArgs& args)
	{
		mKeyDownTimer->stop();
		mKeyRepeatTimer->stop();
	}

	void TextBox::onKeyboardInputGain(const EventArgs& args)
	{
		mTextCursor->setVisible(true);
		mBlinkTimer->start();
	}

	void TextBox::onKeyboardInputLose(const EventArgs& args)
	{
		mBlinkTimer->stop();
		mTextCursor->setVisible(false);
	}

	void TextBox::onMouseButtonDown(const EventArgs& args)
	{
		const MouseEventArgs mea = dynamic_cast<const MouseEventArgs&>(args);
		if(mea.button == MB_Left)
		{
			// Convert position to coordinates relative to TextBox position
			Point relativePosition;
			relativePosition.x = mea.position.x - mTexturePosition.x;
			relativePosition.y = mea.position.y - mTexturePosition.y;

			// Convert relative TextBox position to coordinates relative to Text position
			Point relativeTextPosition;
			relativeTextPosition.x = relativePosition.x - mDesc->textbox_textPosition.x;
			relativeTextPosition.y = relativePosition.y - mDesc->textbox_textPosition.y;

			setCursorIndex(mText->getCursorIndexAtPosition(relativeTextPosition));
		}
	}

	void TextBox::onTripleClick(const EventArgs& args)
	{
		const MouseEventArgs mea = dynamic_cast<const MouseEventArgs&>(args);
		if(mea.button == MB_Left)
		{
			mText->highlight();
			redraw();
		}
	}

	void TextBox::onVisibleChanged(const EventArgs& args)
	{
		if(!mWidgetDesc->widget_visible)
		{
			mBlinkTimer->stop();
			mTextCursor->setVisible(false);
		}
	}

	void TextBox::onWindowDrawn(const EventArgs& args)
	{
		mTextCursor->onDraw();
	}

	void TextBox::removeCharacter(int index)
	{
		if(index < 0)
			return;

		if(index >= mText->getLength())
			return;

		mText->removeCharacter(index);

		// Update index
		setCursorIndex(index);
	}

	void TextBox::setColor(const Ogre::ColourValue& cv)
	{
		mText->setColor(cv);

		redraw();
	}

	void TextBox::setColor(const Ogre::ColourValue& cv, unsigned int index)
	{
		mText->setColor(cv,index);

		redraw();
	}

	void TextBox::setColor(const Ogre::ColourValue& cv, unsigned int startIndex, unsigned int endIndex)
	{
		mText->setColor(cv,startIndex,endIndex);

		redraw();
	}

	void TextBox::setColor(const Ogre::ColourValue& cv, Ogre::UTFString::code_point c, bool allOccurrences)
	{
		mText->setColor(cv,c,allOccurrences);

		redraw();
	}

	void TextBox::setColor(const Ogre::ColourValue& cv, Ogre::UTFString s, bool allOccurrences)
	{
		mText->setColor(cv,s,allOccurrences);

		redraw();
	}

	void TextBox::setCursorIndex(int index)
	{
		if(static_cast<int>(index) > mText->getLength())
			index = mText->getLength();
		else if(index < 0)
			index = 0;

		// Reset cursor blinking every time we move the cursor
		mBlinkTimer->reset();
		mTextCursor->setVisible(true);

		// Update CursorIndex
		mCursorIndex = index;

		// If text fits within TextBox, align text
		if(mText->getTextWidth() < mClientDimensions.size.width)
		{
			switch(mDesc->textbox_horizontalTextAlignment)
			{
			case TEXT_ALIGNMENT_HORIZONTAL_CENTER:
				mDesc->textbox_textPosition.x = (mClientDimensions.size.width - mText->getTextWidth()) / 2.0;
				break;
			case TEXT_ALIGNMENT_HORIZONTAL_LEFT:
				mDesc->textbox_textPosition.x = 0;
				break;
			case TEXT_ALIGNMENT_HORIZONTAL_RIGHT:
				mDesc->textbox_textPosition.x = mClientDimensions.size.width - mText->getTextWidth();
				break;
			}
		}
		// Else text is larger than TextBox bounds.  Ignore Text alignment property.
		else
		{
			// calculate the position of the desired index in relation to the textbox dimensions

			// X Position of cursor index relative to start of text
			Point relativeCursorPosition = mText->getPositionAtCharacterIndex(mCursorIndex);
			// X Position of cursor index relative to TextBox's top left client corner
			float indexPosition = (relativeCursorPosition.x + mDesc->textbox_textPosition.x);

			if(indexPosition < 0)
			{
				mDesc->textbox_textPosition.x -= indexPosition;
			}
			else if(indexPosition > mClientDimensions.size.width)
			{
				mDesc->textbox_textPosition.x -= (indexPosition - mClientDimensions.size.width);
			}
		}

		mCursorPosition.x = mDesc->textbox_textPosition.x + mText->getPositionAtCharacterIndex(mCursorIndex).x;

		// Position Cursor
		Point p = getScreenPosition();
		p.translate(mClientDimensions.position);
		p.translate(mCursorPosition);
		mTextCursor->setPosition(p);
		redraw();
	}

	void TextBox::setDefaultColor(const Ogre::ColourValue& cv)
	{
		mDesc->textbox_defaultColor = cv;
	}

	void TextBox::setDefaultFont(const Ogre::String& fontName)
	{
		mDesc->textbox_defaultFontName = fontName;
		mCurrentFont = Text::getFont(fontName);
	}

	void TextBox::setEnabled(bool enabled)
	{
		Widget::setEnabled(enabled);

		if(!mWidgetDesc->widget_enabled)
		{
			if(mBlinkTimer != NULL)
				mBlinkTimer->stop();
			if(mTextCursor != NULL)
				mTextCursor->setVisible(false);

			if((mWidgetDesc->sheet != NULL) && (mWidgetDesc->sheet->getKeyboardListener() == this))
				mWidgetDesc->sheet->setKeyboardListener(NULL);
		}
	}

	void TextBox::setFont(const Ogre::String& fontName)
	{
		mText->setFont(fontName);

		redraw();
	}

	void TextBox::setFont(const Ogre::String& fontName, unsigned int index)
	{
		mText->setFont(fontName,index);

		redraw();
	}

	void TextBox::setFont(const Ogre::String& fontName, unsigned int startIndex, unsigned int endIndex)
	{
		mText->setFont(fontName,startIndex,endIndex);

		redraw();
	}

	void TextBox::setFont(const Ogre::String& fontName, Ogre::UTFString::code_point c, bool allOccurrences)
	{
		mText->setFont(fontName,c,allOccurrences);

		redraw();
	}

	void TextBox::setFont(const Ogre::String& fontName, Ogre::UTFString s, bool allOccurrences)
	{
		mText->setFont(fontName,s,allOccurrences);

		redraw();
	}

	void TextBox::setMaskText(bool mask, Ogre::UTFString::code_point textbox_maskSymbol)
	{
		mDesc->textbox_maskText = mask;
		mDesc->textbox_maskSymbol = textbox_maskSymbol;

		mText->setMaskText(mask,textbox_maskSymbol);
	}

	void TextBox::setMaxCharacters(unsigned int max)
	{		
		if(static_cast<int>(max) < mDesc->textDesc.getTextLength())
		{
			throw Exception(Exception::ERR_TEXT,"Cannot set max Characters when text exceeds max! (Data loss)","TextBox::setMaxCharacters");
		}

		mDesc->textbox_maxCharacters = max;
	}

	void TextBox::setParent(Widget* parent)
	{
		Widget::setParent(parent);

		if(mWindow != NULL)
		{
			if(parent !=  NULL)
				mWindow->addWindowEventHandler(WINDOW_EVENT_DRAWN,&TextBox::onWindowDrawn,this);
			else
				mWindow->removeWindowEventHandler(WINDOW_EVENT_DRAWN,this);
		}
	}

	void TextBox::setText(Ogre::UTFString s)
	{
		setText(s,Text::getFont(mDesc->textbox_defaultFontName),mDesc->textbox_defaultColor);
	}

	void TextBox::setText(Ogre::UTFString s, Ogre::FontPtr fp, const Ogre::ColourValue& cv)
	{
		mText->setText(s,fp,cv);

		if(mTextCursor->getVisible())
			setCursorIndex(mCursorIndex);

		redraw();
	}

	void TextBox::setText(std::vector<TextSegment> segments)
	{
		mText->setText(segments);

		redraw();
	}

	void TextBox::setHorizontalTextAlignment(HorizontalTextAlignment a)
	{
		mDesc->textbox_horizontalTextAlignment = a;

		setCursorIndex(mCursorIndex);
	}

	void TextBox::setVerticalTextAlignment(VerticalTextAlignment a)
	{
		mDesc->textbox_verticalTextAlignment = a;

		redraw();
	}

	void TextBox::setTextCursorSkinType(const Ogre::String type)
	{
		mDesc->textbox_textCursorDefaultSkinTypeName = type;

		mTextCursor->setSkinType(type);
	}

	void TextBox::updateClientDimensions()
	{
		Widget::updateClientDimensions();

		if(mTextCursor != NULL)
			mTextCursor->setHeight(mClientDimensions.size.height);
	}

	void TextBox::updateTexturePosition()
	{
		Widget::updateTexturePosition();

		if(mDesc != NULL)
		{
			Point p = getScreenPosition();
			p.translate(mClientDimensions.position);
			p.translate(mCursorPosition);
			mTextCursor->setPosition(p);
		}
	}
}
