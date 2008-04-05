#include "QuickGUIPrecompiledHeaders.h"

#include "QuickGUIManager.h"
#include "QuickGUIPanel.h"
#include "QuickGUISheet.h"
#include "QuickGUITextArea.h"

#ifdef WIN32
// Turn off warnings generated by using 'this' in base member initializer list.
#   pragma warning (disable : 4355)
#endif

namespace QuickGUI
{
	Panel::Panel(const std::string& name, GUIManager* gm) :
		Widget(name,gm),
		QuadContainer(this),
		RadioButtonGroup(this),
		mScrollPane(0),
		mRightScrollBar(0),
		mBottomScrollBar(0)
	{
		mWidgetType = TYPE_PANEL;
		mSkinComponent = ".panel";
		mSize = Size(100,100);

		addEventHandler(EVENT_CHILD_ADDED,&Panel::onChildAdded,this);
		addEventHandler(EVENT_CHILD_REMOVED,&Panel::onChildRemoved,this);

		// Set up the Scroll Pane to allow scrolling within the Panel.
		mScrollPane = dynamic_cast<ScrollPane*>(_createComponent(mInstanceName+".ScrollPane",TYPE_SCROLL_PANE));
		mScrollPane->setSize(mSize);

		mScrollPane->removeChild(mScrollPane->mRightBar);
		// store reference to the scroll bar
		mRightScrollBar = mScrollPane->mRightBar;
		mComponents.push_back(mRightScrollBar);
		mRightScrollBar->setParent(this);
		mRightScrollBar->setPosition(mSize.width - 20,0);

		mScrollPane->removeChild(mScrollPane->mBottomBar);
		// store reference to the scroll bar
		mBottomScrollBar = mScrollPane->mBottomBar;
		mComponents.push_back(mBottomScrollBar);
		mBottomScrollBar->setParent(this);
		mBottomScrollBar->setPosition(0,mSize.height - 20);
	}

	Panel::~Panel()
	{
		mScrollPane = NULL;
		Widget::removeAndDestroyAllChildWidgets();
		setQuadContainer(NULL);
	}

	Widget*	Panel::_createComponent(const std::string& name, Type t)
	{
		Widget* w;
		switch(t)
		{
			case TYPE_BORDER:				w = new Border(name,mGUIManager);				break;
//		    case TYPE_TREE:					w = new Tree(name,mGUIManager);					break;
			case TYPE_BUTTON:				w = new Button(name,mGUIManager);				break;
			case TYPE_CHECKBOX:				w = new CheckBox(name,mGUIManager);				break;
			case TYPE_COMBOBOX:				w = new ComboBox(name,mGUIManager);				break;
			case TYPE_CONSOLE:				w = new Console(name,mGUIManager);				break;
			case TYPE_IMAGE:				w = new Image(name,mGUIManager);				break;
			case TYPE_LABEL:				w = new Label(name,mGUIManager);				break;
			case TYPE_LABELAREA:			w = new LabelArea(name,mGUIManager);			break;
			case TYPE_LIST:					w = new List(name,mGUIManager);					break;
			case TYPE_MENULABEL:			w = new MenuLabel(name,mGUIManager);			break;
			case TYPE_NSTATEBUTTON:			w = new NStateButton(name,mGUIManager);			break;
			case TYPE_PANEL:				w = new Panel(name,mGUIManager);				break;
			case TYPE_PROGRESSBAR:			w = new ProgressBar(name,mGUIManager);			break;
			case TYPE_SCROLL_PANE:			w = new ScrollPane(name,mGUIManager);			break;
			case TYPE_SCROLLBAR_HORIZONTAL: w = new HorizontalScrollBar(name,mGUIManager);	break;
			case TYPE_SCROLLBAR_VERTICAL:	w = new VerticalScrollBar(name,mGUIManager);	break;
			case TYPE_TEXTAREA:				w = new TextArea(name,mGUIManager);				break;
			case TYPE_TEXTBOX:				w = new TextBox(name,mGUIManager);				break;
			case TYPE_TITLEBAR:				w = new TitleBar(name,mGUIManager);				break;
			case TYPE_TRACKBAR_HORIZONTAL:	w = new HorizontalTrackBar(name,mGUIManager);	break;
			case TYPE_TRACKBAR_VERTICAL:	w = new VerticalTrackBar(name,mGUIManager);		break;
			case TYPE_WINDOW:				w = new Window(name,mGUIManager);				break;
			default:						w = new Widget(name,mGUIManager);				break;
		}

		mComponents.push_back(w);

		w->setParent(this);

		w->setSize(w->getSize());
		w->setFont(mFontName,true);
		w->setPosition(0,0);
		// Some Composition widgets will create components before inheritting skin name.
		if(mSkinName != "")
			w->setSkin(mSkinName,true);

		if(!mVisible && w->getHideWithParent())
			w->hide();

		return w;
	}

	void Panel::addChild(Widget* w)
	{
		if(w->getParentWidget() != NULL)
			return;

		if(!isNameUnique(w->getInstanceName()))
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + w->getInstanceName() + "\" already exists in " + getInstanceName(),"Panel::addChild");

		mChildWidgets.push_back(w);

		w->setParent(this);

		// Convert Widget's position to be relative to new parent.
		if(mScrollPane != NULL)
			w->setPosition(w->getScreenPosition() - mScrollPane->getScreenPosition());
		else
			w->setPosition(w->getScreenPosition() - getScreenPosition());

		WidgetEventArgs args(w);
		fireEvent(EVENT_CHILD_ADDED,args);
	}

/*
   Tree* Panel::createTree()
   {
      return createTree(mGUIManager->generateName(TYPE_TREE));
   }

   Tree* Panel::createTree(const std::string& name)
   {
      if(isNameUnique(name))
      {
         return dynamic_cast<Tree*>(_createChild(name,TYPE_TREE));
      }
      else
      {
         std::string name = generateName(TYPE_TREE);
         return dynamic_cast<Tree*>(_createChild(name,TYPE_TREE));
      }
	}
*/
	Button* Panel::createButton()
	{
		return createButton(generateName(TYPE_BUTTON));
	}

	Button* Panel::createButton(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<Button*>(_createChild(name,TYPE_BUTTON));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createButton");
		}
	}

	CheckBox* Panel::createCheckBox()
	{
		return createCheckBox(generateName(TYPE_CHECKBOX));
	}

	CheckBox* Panel::createCheckBox(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<CheckBox*>(_createChild(name,TYPE_CHECKBOX));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createCheckBox");
		}
	}

	ComboBox* Panel::createComboBox()
	{
		return createComboBox(generateName(TYPE_COMBOBOX));
	}

	ComboBox* Panel::createComboBox(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<ComboBox*>(_createChild(name,TYPE_COMBOBOX));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createComboBox");
		}
	}

	Console* Panel::createConsole()
	{
		return createConsole(generateName(TYPE_CONSOLE));
	}

	Console* Panel::createConsole(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<Console*>(_createChild(name,TYPE_CONSOLE));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createConsole");
		}
	}

	HorizontalScrollBar* Panel::createHorizontalScrollBar()
	{
		return createHorizontalScrollBar(generateName(TYPE_SCROLLBAR_HORIZONTAL));
	}

	HorizontalScrollBar* Panel::createHorizontalScrollBar(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<HorizontalScrollBar*>(_createChild(name,TYPE_SCROLLBAR_HORIZONTAL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createHorizontalScrollBar");
		}
	}

	HorizontalTrackBar* Panel::createHorizontalTrackBar()
	{
		return createHorizontalTrackBar(generateName(TYPE_TRACKBAR_HORIZONTAL));
	}

	HorizontalTrackBar* Panel::createHorizontalTrackBar(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<HorizontalTrackBar*>(_createChild(name,TYPE_TRACKBAR_HORIZONTAL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createHorizontalTrackBar");
		}
	}

	Image* Panel::createImage()
	{
		return createImage(generateName(TYPE_IMAGE));
	}

	Image* Panel::createImage(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<Image*>(_createChild(name,TYPE_IMAGE));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createImage");
		}
	}

	Label* Panel::createLabel()
	{
		return createLabel(generateName(TYPE_LABEL));
	}

	Label* Panel::createLabel(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<Label*>(_createChild(name,TYPE_LABEL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createLabel");
		}
	}

	List* Panel::createList()
	{
		return createList(generateName(TYPE_LIST));
	}

	List* Panel::createList(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<List*>(_createChild(name,TYPE_LIST));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createList");
		}
	}

	LabelArea* Panel::createLabelArea()
	{
		return createLabelArea(generateName(TYPE_LABELAREA));
	}

	LabelArea* Panel::createLabelArea(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<LabelArea*>(_createChild(name,TYPE_LABELAREA));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createLabelArea");
		}
	}

	NStateButton* Panel::createNStateButton()
	{
		return createNStateButton(generateName(TYPE_NSTATEBUTTON));
	}

	NStateButton* Panel::createNStateButton(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<NStateButton*>(_createChild(name,TYPE_NSTATEBUTTON));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createNStateButton");
		}
	}

	Panel* Panel::createPanel()
	{
		return createPanel(generateName(TYPE_PANEL));
	}

	Panel* Panel::createPanel(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<Panel*>(_createChild(name,TYPE_PANEL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createPanel");
		}
	}

	ProgressBar* Panel::createProgressBar()
	{
		return createProgressBar(generateName(TYPE_PROGRESSBAR));
	}

	ProgressBar* Panel::createProgressBar(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<ProgressBar*>(_createChild(name,TYPE_PROGRESSBAR));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createProgressBar");
		}
	}

	TextBox* Panel::createTextBox()
	{
		return createTextBox(generateName(TYPE_TEXTBOX));
	}

	TextBox* Panel::createTextBox(const std::string& name)
	{
		TextBox* tb;
		if(isNameUnique(name))
		{
			tb = dynamic_cast<TextBox*>(_createChild(name,TYPE_TEXTBOX));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createTextBox");
		}

		tb->setUseBorders(true);
		tb->setSkin(mSkinName,true);

		return tb;
	}

	VerticalScrollBar* Panel::createVerticalScrollBar()
	{
		return createVerticalScrollBar(generateName(TYPE_SCROLLBAR_VERTICAL));
	}

	VerticalScrollBar* Panel::createVerticalScrollBar(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<VerticalScrollBar*>(_createChild(name,TYPE_SCROLLBAR_VERTICAL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createVerticalScrollBar");
		}
	}

	VerticalTrackBar* Panel::createVerticalTrackBar()
	{
		return createVerticalTrackBar(generateName(TYPE_TRACKBAR_VERTICAL));
	}

	VerticalTrackBar* Panel::createVerticalTrackBar(const std::string& name)
	{
		if(isNameUnique(name))
		{
			return dynamic_cast<VerticalTrackBar*>(_createChild(name,TYPE_TRACKBAR_VERTICAL));
		}
		else
		{
			throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,"A widget with name \"" + name + "\" already exists in " + getInstanceName(),"Panel::createVerticalTrackBar");
		}
	}

	void Panel::enableScrollPane()
	{
		mScrollPane->enable();
	}

	void Panel::disableScrollPane()
	{
		mScrollPane->disable();
	}

	QuadContainer* Panel::getQuadContainer()
	{
		return this;
	}

	ScrollPane* Panel::getScrollPane()
	{
		return mScrollPane;
	}

	Widget* Panel::getTargetWidget(const Point& pixelPosition)
	{
		Widget* w = NULL;

		if( !mVisible || !mEnabled )
			return w;

		for(WidgetArray::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			if((w = (*it)->getTargetWidget(pixelPosition)) != NULL)
				return w;
		}

		// If position is not inside this widget, it can't be inside a child widget. (except menus, which are handled differently)
		if(!isPointWithinBounds(pixelPosition))
			return NULL;

		// Iterate through Child Widgets and record the highest offset menu and child layer widget.
		Widget* highestMenuLayerWidget = NULL;
		int highestMenuOffset = -1;

		Widget* highestChildLayerWidget = NULL;
		int highestChildOffset = -1;

		WidgetArray::iterator it;
		for( it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it )
		{
			if( (*it)->getQuadLayer() == Quad::LAYER_CHILD )
			{
				Widget* temp = (*it)->getTargetWidget(pixelPosition);
				if( (temp != NULL) && (temp->getOffset() > highestChildOffset) )
				{
					highestChildOffset = temp->getOffset();
					highestChildLayerWidget = temp;
				}
			}
			else // LAYER_MENU
			{
				Widget* temp = (*it)->getTargetWidget(pixelPosition);
				if( (temp != NULL) && (temp->getOffset() > highestMenuOffset) )
				{
					highestMenuOffset = temp->getOffset();
					highestMenuLayerWidget = temp;
				}
			}
		}

		if(highestMenuLayerWidget != NULL)
			return highestMenuLayerWidget;

		// Iterate through Panels, from highest offset to lowest.
		QuadContainerList* panelList = QuadContainer::getPanelList();
		QuadContainerList::reverse_iterator rit;
		for( rit = panelList->rbegin(); rit != panelList->rend(); ++rit )
		{
			if( (w = (*rit)->getOwner()->getTargetWidget(pixelPosition)) != NULL )
				return w;
		}

		if(highestChildLayerWidget != NULL)
			return highestChildLayerWidget;

		// If we made it here, we are inside this Widget's bounds, but not over any non-transparent child widget areas.

		if( !overTransparentPixel(pixelPosition) )
			return this;
		else // We're over a transparent pixel
			return NULL;
	}

	const Widget* Panel::getTargetWidget(const Point& pixelPosition) const
	{
		Widget* w = NULL;

		if( !mVisible || !mEnabled )
			return w;

		for(WidgetArray::const_iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			if((w = (*it)->getTargetWidget(pixelPosition)) != NULL)
				return w;
		}

		// Iterate through Child Widgets and record the highest offset menu and child layer widget.
		Widget* highestMenuLayerWidget = NULL;
		int highestMenuOffset = -1;

		Widget* highestChildLayerWidget = NULL;
		int highestChildOffset = -1;

		WidgetArray::const_iterator it;
		for( it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it )
		{
			if( (*it)->getQuadLayer() == Quad::LAYER_CHILD )
			{
				Widget* temp = (*it)->getTargetWidget(pixelPosition);
				if( (temp != NULL) && (temp->getOffset() > highestChildOffset) )
				{
					highestChildOffset = temp->getOffset();
					highestChildLayerWidget = temp;
				}
			}
			else // LAYER_MENU
			{
				Widget* temp = (*it)->getTargetWidget(pixelPosition);
				if( (temp != NULL) && (temp->getOffset() > highestMenuOffset) )
				{
					highestMenuOffset = temp->getOffset();
					highestMenuLayerWidget = temp;
				}
			}
		}

		if(highestMenuLayerWidget != NULL)
			return highestMenuLayerWidget;

		// Iterate through Panels, from highest offset to lowest.
		const QuadContainerList* panelList = QuadContainer::getPanelList();
		QuadContainerList::const_reverse_iterator rit;
		for( rit = panelList->rbegin(); rit != panelList->rend(); ++rit )
		{
			if( (w = (*rit)->getOwner()->getTargetWidget(pixelPosition)) != NULL )
				return w;
		}

		if(highestChildLayerWidget != NULL)
			return highestChildLayerWidget;

		// If we made it here, we are inside this Widget's bounds, but not over any non-transparent child widget areas.

		if( !overTransparentPixel(pixelPosition) )
			return this;
		else // We're over a transparent pixel
			return NULL;
	}

	void Panel::onChildAdded(const EventArgs& args)
	{
		mScrollPane->onChildAddedToParent(args);
	}

	void Panel::onChildRemoved(const EventArgs& args)
	{
		// Scroll Pane is deleted on destructor
		if(mScrollPane != NULL)
			mScrollPane->onChildRemovedFromParent(args);
	}

	void Panel::onSizeChanged(const EventArgs& args)
	{
		Widget::onSizeChanged(args);

		mScrollPane->onParentSizeChanged(args);
	}

	void Panel::setQuadContainer(QuadContainer* c)
	{
		if((mQuadContainer != NULL) && (c != mQuadContainer))
			mQuadContainer->removeChildPanelContainer(this);

		mQuadContainer = c;

		if(mQuadContainer != NULL)
			mQuadContainer->addChildPanelContainer(this);
	}

	void Panel::show()
	{
		Widget::show();

		if(mScrollPane != NULL)
			mScrollPane->_syncBarWithParentDimensions();
	}
}
