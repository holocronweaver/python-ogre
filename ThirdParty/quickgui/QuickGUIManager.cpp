#include "QuickGUIManager.h"
#include "QuickGUIEffect.h"
#include "QuickGUIConfigScriptParser.h"

namespace QuickGUI
{
	GUIManager::GUIManager() :		
		mViewport(0),
		mActiveSheet(0),
		mWidgetContainingMouse(0),
		mActiveWidget(0),
		mClickTimeout(75),
		mQueueID(Ogre::RENDER_QUEUE_OVERLAY),
		mMouseCursor(0),
		mSceneManager(0),
		mDraggingWidget(false),
		mDebugString("")
	{
		mWidgetNames.clear();

		mMouseButtonDown[0] = NULL;
		mMouseButtonDown[1] = NULL;
		mMouseButtonDown[2] = NULL;
		mMouseButtonDown[3] = NULL;
		mMouseButtonDown[4] = NULL;
		mMouseButtonDown[5] = NULL;
		mMouseButtonDown[6] = NULL;
		mMouseButtonDown[7] = NULL;

		new ConfigScriptLoader();

		// by default, we support codepoints 9, and 32-166.
		mSupportedCodePoints.push_back(9);
		for(Ogre::UTFString::code_point i = 32; i < 167; ++i)
			mSupportedCodePoints.push_back(i);


		mTimer = new Ogre::Timer();
	}

	GUIManager::~GUIManager()
	{
		delete mTimer;

		removeFromRenderQueue();
		clearAll();

		delete mMouseCursor;
		mMouseCursor = NULL;

		// delete imagesets
		std::map<Ogre::String,SkinSet*>::iterator it;
		for( it = mSkinSets.begin(); it != mSkinSets.end(); ++it )
			delete (it->second);
		mSkinSets.clear();

		delete ConfigScriptLoader::getSingletonPtr();
	}

	void GUIManager::init(Ogre::Viewport* vp, const Ogre::String &skinName) 
	{
		mViewport = vp;
		// load default skin into an SkinSet Texture - must be done before we start rendering.
		loadSkin(skinName);

		mMouseCursor = new MouseCursor(Size(13,17),"qgui.cursor.png",this);
		mMouseCursor->setPosition(getViewportWidth()/2.0,getViewportHeight()/2.0);

		mDefaultSheet = createSheet();
		// Initialize all widget tracking pointers.
		mActiveWidget = mWidgetContainingMouse = mActiveSheet = mDefaultSheet;

		_createDefaultTextures();
	}

	void GUIManager::_createDefaultTextures()
	{
		Ogre::TextureManager* tm = Ogre::TextureManager::getSingletonPtr();
		// QuickGUI.White - Plain White
		if( !tm->resourceExists("QuickGUI.White") )
		{
			Ogre::TexturePtr tp = 
				tm->createManual("QuickGUI.White",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,1,1,1,0,Ogre::PF_A8B8G8R8);

			Ogre::HardwarePixelBufferSharedPtr buf = tp->getBuffer();
			buf->lock(Ogre::HardwareBuffer::HBL_NORMAL);
			const Ogre::PixelBox& pb = buf->getCurrentLock();
			Ogre::uint8* pixelData = static_cast<Ogre::uint8*>(pb.data);

			*pixelData++ = 255;	// Blue
			*pixelData++ = 255; // Green
			*pixelData++ = 255; // Red
			*pixelData++ = 255; // Alpha

			buf->unlock();
		}

		// QuickGUI.TextSelection - Used for text selections.  Has half alpha value.
		if( !tm->resourceExists("QuickGUI.TextSelection") )
		{
			Ogre::TexturePtr tp = 
				tm->createManual("QuickGUI.TextSelection",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,1,1,1,0,Ogre::PF_A8B8G8R8);

			Ogre::HardwarePixelBufferSharedPtr buf = tp->getBuffer();
			buf->lock(Ogre::HardwareBuffer::HBL_NORMAL);
			const Ogre::PixelBox& pb = buf->getCurrentLock();
			Ogre::uint8* pixelData = static_cast<Ogre::uint8*>(pb.data);

			*pixelData++ = 255;	// Blue
			*pixelData++ = 255; // Green
			*pixelData++ = 255; // Red
			*pixelData++ = 200; // Alpha

			buf->unlock();
		}
	}

	void GUIManager::_menuOpened(Widget* w)
	{
		if(w == NULL)
			return;

		// check if widget already in list.
		if(mOpenMenus.find(w) != mOpenMenus.end())
			return;

		mOpenMenus.insert(w);
	}

	void GUIManager::_menuClosed(Widget* w)
	{
		if(w == NULL)
			return;

		mOpenMenus.erase(mOpenMenus.find(w));
	}

	void GUIManager::clearAll()
	{
		mWidgetNames.clear();

		mMouseButtonDown[0] = NULL;
		mMouseButtonDown[1] = NULL;
		mMouseButtonDown[2] = NULL;
		mMouseButtonDown[3] = NULL;
		mMouseButtonDown[4] = NULL;
		mMouseButtonDown[5] = NULL;
		mMouseButtonDown[6] = NULL;
		mMouseButtonDown[7] = NULL;
		mWidgetContainingMouse = NULL;
		mActiveWidget = NULL;

		mTimeListeners.clear();
		mOpenMenus.clear();

		std::list<Effect*>::iterator itEffect = mActiveEffects.begin();
		while (itEffect != mActiveEffects.end())        
		{
			delete *itEffect;
			++itEffect;            
		} 

		std::list<Sheet*>::iterator it;
		for( it = mSheets.begin(); it != mSheets.end(); ++it )
			delete (*it);
		mSheets.clear();

		// create default sheet
		mDefaultSheet = createSheet();
		mActiveSheet = mDefaultSheet;

		mActiveWidget = mActiveSheet;
		mWidgetContainingMouse = mActiveSheet;
	}

	Sheet* GUIManager::createSheet()
	{
		Ogre::String name = generateName(Widget::TYPE_SHEET);
		notifyNameUsed(name);

		Sheet* newSheet = new Sheet(name,"",this);
		mSheets.push_back(newSheet);

		return newSheet;
	}

	void GUIManager::destroySheet(const Ogre::String& name)
	{
		// Cannot destroy active sheet!
		if( (name == "") || (mActiveSheet->getInstanceName() == name) ) 
			return;

		std::list<Sheet*>::iterator it;
		for( it = mSheets.begin(); it != mSheets.end(); ++it )
		{
			if( (*it)->getInstanceName() == name )
			{
				Sheet* s = (*it);
				it = mSheets.erase(it);
				delete s;
				break;
			}
		}
	}

	void GUIManager::destroySheet(Sheet* sheet)
	{
		destroySheet(sheet->getInstanceName());
	}

	void GUIManager::destroyWidget(Widget* w)
	{
		if( w == NULL )
			return;

		if(w->getWidgetType() == Widget::TYPE_SHEET)
		{
			destroySheet(dynamic_cast<Sheet*>(w));
			return;
		}

		mFreeList.push_back(w);
	}

	void GUIManager::destroyWidget(const Ogre::String& widgetName)
	{
		Widget* w = mActiveSheet->getChildWidget(widgetName);
		if( w != NULL )
			destroyWidget(w);
	}

	Sheet* GUIManager::getActiveSheet()
	{
		return mActiveSheet;
	}

	Widget* GUIManager::getActiveWidget()
	{
		return mActiveWidget;
	}

	Ogre::String GUIManager::getDebugString()
	{
		return mDebugString;
	}

	Sheet* GUIManager::getDefaultSheet()
	{
		return mDefaultSheet;
	}

	SkinSet* GUIManager::getSkinImageSet(const Ogre::String& name)
	{
		if(!skinLoaded(name)) return NULL;
		else return mSkinSets[name];
	}

	MouseCursor* GUIManager::getMouseCursor()
	{
		return mMouseCursor;
	}

	Widget* GUIManager::getMouseOverWidget()
	{
		return mWidgetContainingMouse;
	}

	Ogre::Viewport* GUIManager::getViewport()
	{
		return mViewport;
	}

	Ogre::Real GUIManager::getViewportWidth()
	{
		return static_cast<Ogre::Real>(mViewport->getActualWidth());
	}

	Ogre::Real GUIManager::getViewportHeight()
	{
		return static_cast<Ogre::Real>(mViewport->getActualHeight());
	}

	Sheet* GUIManager::getSheet(const Ogre::String& name)
	{
		if( name == "" ) return NULL;

		std::list<Sheet*>::iterator it;
		for( it = mSheets.begin(); it != mSheets.end(); ++it )
		{
			if( (*it)->getInstanceName() == name ) 
				return (*it);
		}

		return NULL;
	}

	bool GUIManager::embeddedInSkinImageset(const Ogre::String& skinName, const Ogre::String& textureName)
	{
		if(!skinLoaded(skinName)) return false;
		else return mSkinSets[skinName]->containsImage(textureName);
	}
	void GUIManager::addEffect (Effect* e)
	{
		mActiveEffects.push_back(e);
	}
	Ogre::String GUIManager::generateName(Widget::Type t)
	{
		Ogre::String s;
		switch(t)
		{
			case Widget::TYPE_BORDER:				s = "Border";			break;
			case Widget::TYPE_BUTTON:				s = "Button";			break;
			case Widget::TYPE_COMBOBOX:				s = "ComboBox";			break;
			case Widget::TYPE_CONSOLE:				s = "Console";			break;
			case Widget::TYPE_IMAGE:				s = "Image";			break;
			case Widget::TYPE_LABEL:				s = "Label";			break;
			case Widget::TYPE_LIST:					s = "List";				break;
			case Widget::TYPE_MENULABEL:			s = "MenuLabel";		break;
			case Widget::TYPE_LABELAREA:		s = "LabelArea";	break;
			case Widget::TYPE_TEXTAREA:		s = "TextArea";	break;
			case Widget::TYPE_NSTATEBUTTON:			s = "NStateButton";		break;
			case Widget::TYPE_PANEL:				s = "Panel";			break;
			case Widget::TYPE_PROGRESSBAR:			s = "ProgressBar";		break;
			case Widget::TYPE_SCROLL_PANE:			s = "ScrollPane";		break;
			case Widget::TYPE_SCROLLBAR_HORIZONTAL: s = "HScrollBar";		break;
			case Widget::TYPE_SCROLLBAR_VERTICAL:	s = "VScrollBar";		break;
			case Widget::TYPE_SHEET:				s = "Sheet";			break;
			case Widget::TYPE_TEXTBOX:				s = "TextBox";			break;
			case Widget::TYPE_TITLEBAR:				s = "TitleBar";			break;
			case Widget::TYPE_TRACKBAR_HORIZONTAL:	s = "HTrackBar";		break;
			case Widget::TYPE_TRACKBAR_VERTICAL:	s = "VTrackBar";		break;
			case Widget::TYPE_WINDOW:				s = "Window";			break;
			default:								s = "Widget";			break;
		}

		int counter = 1;
		while( !isNameUnique(s + Ogre::StringConverter::toString(counter)) )
			++counter;

		return (s + Ogre::StringConverter::toString(counter));
	}

	bool GUIManager::injectChar(Ogre::UTFString::unicode_char c)
	{
		if( std::find(mSupportedCodePoints.begin(),mSupportedCodePoints.end(),c) == mSupportedCodePoints.end() )
			return false;

		KeyEventArgs args(mActiveWidget);
		args.codepoint = c;

		return mActiveWidget->fireEvent(Widget::EVENT_CHARACTER_KEY,args);
	}

	bool GUIManager::injectKeyDown(const KeyCode& kc)
	{
		KeyEventArgs args(mActiveWidget);
		args.scancode = kc;

		return mActiveWidget->fireEvent(Widget::EVENT_KEY_DOWN,args);
	}

	bool GUIManager::injectKeyUp(const KeyCode& kc)
	{
		KeyEventArgs args(mActiveWidget);
		args.scancode = kc;

		return mActiveWidget->fireEvent(Widget::EVENT_KEY_UP,args);
	}

	bool GUIManager::injectMouseButtonDown(const MouseButtonID& button)
	{
		if( !mMouseCursor->isVisible() ) 
			return false;

		bool eventHandled = false;

		MouseEventArgs args(mActiveWidget);
		args.position = mMouseCursor->getPosition();
		args.button = button;

		// Feature, allowing widgets to be clicked, without transfering focus.  Widget will receive
		// Mouse Button Down Event.
		if(!mWidgetContainingMouse->getGainFocusOnClick())
		{
			return mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_BUTTON_DOWN,args);
		}

		// mActiveWidget is the last widget the user clicked on, ie TextBox, ComboBox, etc.
		if( mActiveWidget != mWidgetContainingMouse )
		{
			if(mActiveWidget->fireEvent(Widget::EVENT_LOSE_FOCUS,args))
				eventHandled = true;

			// Update active widget reference.
			mActiveWidget = mWidgetContainingMouse;
		}
		
		args.widget = mActiveWidget;
		args.position = mMouseCursor->getPosition();
		args.button = button;

		if(mActiveWidget->fireEvent(Widget::EVENT_MOUSE_BUTTON_DOWN,args))
			eventHandled = true;
		if(mActiveWidget->fireEvent(Widget::EVENT_GAIN_FOCUS,args))
			eventHandled = true;
		mActiveWidget->setGrabbed(true);
			
		// If the user clicked on a widget that is a part of a window, make sure the window is brought to front.
		Window* w = mActiveWidget->getParentWindow();
		if( w != NULL ) 
			w->bringToFront();

		// Record that the mouse button went down on this widget (non-window)
		mMouseButtonDown[args.button] = mActiveWidget;
		
		mMouseButtonTimings[button] = mTimer->getMilliseconds();

		return eventHandled;
	}

	bool GUIManager::injectMouseButtonUp(const MouseButtonID& button)
	{
		if( !mMouseCursor->isVisible() ) 
			return false;

		bool eventHandled = false;

		MouseEventArgs args(mActiveWidget);
		args.position = mMouseCursor->getPosition();
		args.button = button;

		// If dragging a widget, fire EVENT_DROPPED event
		if(mDraggingWidget)
		{
			mDraggingWidget = false;
			mActiveWidget->setGrabbed(false);
			eventHandled = mActiveWidget->fireEvent(Widget::EVENT_DROPPED,args);
			return eventHandled;
		}

		// Feature, allowing widgets to be clicked, without transfering focus.  Widget will receive
		// Mouse Button Up and Click Events, if appropriate.
		if(!mWidgetContainingMouse->getGainFocusOnClick())
		{
			if( mMouseButtonDown[args.button] == mWidgetContainingMouse )
			{
				eventHandled = mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_BUTTON_UP,args);
				// check if time elapsed it within click time.
				if( (mTimer->getMilliseconds() - mMouseButtonTimings[button]) < mClickTimeout ) 
					eventHandled = mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_CLICK,args);
			}
			return eventHandled;
		}

		// If the MouseButton was not pressed on this widget, do not register the button being released on the widget
		if( mWidgetContainingMouse != mActiveWidget  )
		{
			if(mActiveWidget->fireEvent(Widget::EVENT_LOSE_FOCUS,args))
				eventHandled = true;

			return eventHandled;
		}

		// after this point, we know that the user had mouse button down on this widget, and is now doing mouse button up

		args.widget = mActiveWidget;
		args.position = mMouseCursor->getPosition();
		args.button = button;

		if(mActiveWidget->fireEvent(Widget::EVENT_MOUSE_BUTTON_UP,args))
			eventHandled = true;
		mActiveWidget->setGrabbed(false);

		// check if time elapsed it within click time.
		if( (mTimer->getMilliseconds() - mMouseButtonTimings[button]) < mClickTimeout ) 
			if(mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_CLICK,args))
				eventHandled = true;

		return eventHandled;
	}

	bool GUIManager::injectMouseLeaves(void)
	{
		if( !mMouseCursor->isVisible() ) 
			return false;

		if(mMouseCursor->getHideWhenOffScreen()) 
			mMouseCursor->_hide();

		return true;
	}

	bool GUIManager::injectMouseMove(const int& xPixelOffset, const int& yPixelOffset)
	{
		MouseEventArgs args(0);
		args.position = mMouseCursor->getPosition();
		args.moveDelta.x = xPixelOffset;
		args.moveDelta.y = yPixelOffset;

		if( mMouseCursor->mouseOnTopBorder() && yPixelOffset < 0 ) 
			args.moveDelta.y = 0;
		if( mMouseCursor->mouseOnBotBorder() && yPixelOffset > 0 ) 
			args.moveDelta.y = 0;
		if( mMouseCursor->mouseOnLeftBorder() && xPixelOffset < 0 ) 
			args.moveDelta.x = 0;
		if( mMouseCursor->mouseOnRightBorder() && xPixelOffset > 0 ) 
			args.moveDelta.x = 0;

		// Update Mouse Cursor Position
		mMouseCursor->offsetPosition(xPixelOffset,yPixelOffset);

		if(!mMouseCursor->isVisible()) 
			return false;

		// See if we should be dragging a widget.
		if( (mActiveWidget->getGrabbed()) && (mActiveWidget->draggingEnabled()) )
		{
			mDraggingWidget = true;

			// Dragging, which uses move function, works with pixel values (uninfluenced by parent dimensions!)
			mActiveWidget->drag(xPixelOffset,yPixelOffset);

			return true;
		}

		// Now get the widget the cursor is over.
		
		// Check Open Menus first, since they are not within their parent bounds.
		Widget* hitWidget = NULL;
		for(std::set<Widget*>::iterator it = mOpenMenus.begin(); it != mOpenMenus.end(); ++it)
		{
			if( (hitWidget = (*it)->getTargetWidget(args.position)) != NULL )
				break;
		}

		// If no menu's are open, or cursor not over a menu, query the Sheet for the widget the cursor is over.
		if(hitWidget == NULL)
			hitWidget = mActiveSheet->getTargetWidget(args.position);
		
		// NOTE: Widget is only detected if it is enabled.
		args.widget = hitWidget;

		bool eventHandled = false;

		if( mWidgetContainingMouse != hitWidget )
		{
			if(mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_LEAVE,args))
				eventHandled = true;

			mWidgetContainingMouse = hitWidget;
			if(mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_ENTER,args))
				eventHandled = true;
		}
		else // widget containing mouse has not changed.
			eventHandled = mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_MOVE,args);

		return eventHandled;
	}

	bool GUIManager::injectMousePosition(const int& xPixelPosition, const int& yPixelPosition)
	{
		Point pos = mMouseCursor->getPosition();

		return injectMouseMove(xPixelPosition - pos.x,yPixelPosition - pos.y);
	}

	bool GUIManager::injectMouseWheelChange(float delta)
	{
		MouseEventArgs args(mWidgetContainingMouse);
		args.wheelChange = delta;

		return mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_WHEEL,args);
	}

	void GUIManager::injectTime(Ogre::Real time)
	{
		{
			std::vector<Widget*>::iterator it;
			for( it = mTimeListeners.begin(); it != mTimeListeners.end(); ++it )
				(*it)->timeElapsed(time);
		}

		// Effects.
		{
			/*
			std::vector<Widget*>::iterator it;   
			while (itWindow != mWidgets.end())        
			{
				(*itWindow)->setUnderEffect(false);
				++itWindow;
			} 
			*/
		}
		{
			std::list<Effect*>::iterator itEffect = mActiveEffects.begin();       
			while (itEffect != mActiveEffects.end())        
			{
				if ((*itEffect)->update(time))
				{
					delete *itEffect;
					itEffect = mActiveEffects.erase(itEffect);
				}
				else
				{
					++itEffect;
				}
			} 
		}
	}

	bool GUIManager::isNameUnique(const Ogre::String& name)
	{
		if(name == "")
			return false;

		return (mWidgetNames.find(name) == mWidgetNames.end());
	}

	void GUIManager::loadSkin(const Ogre::String& skinName)
	{
		// check if imageset is already created for this skin
		if( mSkinSets.find(skinName) != mSkinSets.end() ) return;

		mSkinSets[skinName] = new SkinSet(skinName);
	}

	void GUIManager::notifyNameFree(const Ogre::String& name)
	{
		std::set<Ogre::String>::iterator it = mWidgetNames.find(name);
		if( it == mWidgetNames.end() )
			return;

		mWidgetNames.erase(it);
	}

	void GUIManager::notifyNameUsed(const Ogre::String& name)
	{
		if(!isNameUnique(name))
			return;

		mWidgetNames.insert(name);
	}

	void GUIManager::registerTimeListener(Widget* w)
	{
		if(w == NULL)
			return;

		// check if widget already in list.
		std::vector<Widget*>::iterator it;
		for( it = mTimeListeners.begin(); it != mTimeListeners.end(); ++it )
		{
			if(w == (*it))
				return;
		}

		mTimeListeners.push_back(w);
	}

	void GUIManager::renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue)
	{
		if(mQueueID == id)
		{
			mActiveSheet->render();
			mMouseCursor->render();
		}
	}

	void GUIManager::renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue)
	{
		if(mFreeList.empty())
			return;

		bool activeWidgetDestroyed = false;
		if( std::find(mFreeList.begin(),mFreeList.end(),mActiveWidget) != mFreeList.end() )
		{
			activeWidgetDestroyed = true;
			mActiveWidget = mActiveSheet;
			mWidgetContainingMouse = mActiveSheet;
		}

		std::vector<Widget*>::iterator it;
		for( it = mFreeList.begin(); it != mFreeList.end(); ++it )
			delete (*it);
		mFreeList.clear();

		if(activeWidgetDestroyed)
			injectMouseMove(0,0);
	}

	void GUIManager::removeFromRenderQueue()
	{
		if(mSceneManager != NULL)
			mSceneManager->removeRenderQueueListener(this);
	}

	void GUIManager::setActiveSheet(Sheet* s)
	{
		if( (s == NULL) || (s == mActiveSheet) ) 
			return;

		mActiveSheet = s;

		// Make sure active widget loses focus.
		mActiveWidget->fireEvent(Widget::EVENT_LOSE_FOCUS,EventArgs());
		
		// Make sure mouse over widget has mouse leave event.
		MouseEventArgs args(mWidgetContainingMouse);
		args.position = mMouseCursor->getPosition();

		mWidgetContainingMouse->fireEvent(Widget::EVENT_MOUSE_LEAVE,args);

		// Update the active widget
		mActiveWidget = mActiveSheet;
		mWidgetContainingMouse = mActiveSheet;
		injectMouseMove(0,0);
	}

	void GUIManager::setActiveWidget(Widget* w)
	{
		if ( w == NULL )
			return;

		if (!w->enabled())
			return;

		if( w != mActiveWidget ) 
		{
			WidgetEventArgs args(mActiveWidget);
			mActiveWidget->fireEvent(Widget::EVENT_LOSE_FOCUS,args);
		}

		mActiveWidget = w;

		WidgetEventArgs args(mActiveWidget);
		mActiveWidget->fireEvent(Widget::EVENT_GAIN_FOCUS,args);
	}

	void GUIManager::setDebugString(const Ogre::String s)
	{
		mDebugString = s;
	}

	void GUIManager::setRenderQueueID(Ogre::uint8 id)
	{
		mQueueID = id;
	}

	void GUIManager::setSceneManager(Ogre::SceneManager* sm)
	{
		// remove listener from previous scene manager
		if(mSceneManager != NULL)
			mSceneManager->removeRenderQueueListener(this);
		// update
		mSceneManager = sm;
		// add listener to new scene manager
		if(mSceneManager != NULL)
			mSceneManager->addRenderQueueListener(this);
	}

	void GUIManager::setSupportedCodePoints(const std::vector<Ogre::UTFString::code_point>& list)
	{
		mSupportedCodePoints.clear();
		mSupportedCodePoints.assign(list.begin(),list.end());
	}

	void GUIManager::setViewport(Ogre::Viewport* vp)
	{
		mViewport = vp;
	}

	bool GUIManager::skinLoaded(const Ogre::String& skinName)
	{
		return (mSkinSets.find(skinName) != mSkinSets.end());
	}

	bool GUIManager::textureExists(const Ogre::String& textureName)
	{
		if(textureName == "")
			return false;

		if(Utility::textureExistsOnDisk(textureName))
			return true;

		if(Ogre::TextureManager::getSingletonPtr()->resourceExists(textureName)) 
			return true;

		std::map<Ogre::String,SkinSet*>::iterator it;
		for( it = mSkinSets.begin(); it != mSkinSets.end(); ++it )
			if( it->second->containsImage(textureName) )
				return true;

		return false;
	}

	void GUIManager::unregisterTimeListener(Widget* w)
	{
		if(w == NULL)
			return;

		std::vector<Widget*>::iterator it;
		for( it = mTimeListeners.begin(); it != mTimeListeners.end(); ++it )
		{
			if(w == (*it))
			{
				mTimeListeners.erase(it);
				return;
			}
		}
	}
}