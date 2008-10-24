#include "QuickGUIToolBar.h"
#include "QuickGUIManager.h"
#include "QuickGUISkinDefinitionManager.h"

namespace QuickGUI
{
	const Ogre::String ToolBar::BACKGROUND = "background";

	void ToolBar::registerSkinDefinition()
	{
		SkinDefinition* d = new SkinDefinition("ToolBar");
		d->defineSkinElement(BACKGROUND);
		d->definitionComplete();

		SkinDefinitionManager::getSingleton().registerSkinDefinition("ToolBar",d);
	}

	ToolBarDesc::ToolBarDesc() :
		ContainerWidgetDesc()
	{
		supportScrollBars = false;
		itemLayout = TOOLBAR_ITEM_LAYOUT_NEGATIVE_TO_POSITIVE;
	}

	void ToolBarDesc::serialize(SerialBase* b)
	{
		ContainerWidgetDesc::serialize(b);

		b->IO("ItemLayout",&itemLayout);
	}

	ToolBar::ToolBar(const Ogre::String& name) :
		ContainerWidget(name),
		mCurrentState(TOOLBAR_STATE_DEFAULT),
		mMenuOpened(false)
	{
		mSkinElementName = BACKGROUND;
	}

	ToolBar::~ToolBar()
	{
	}

	void ToolBar::_initialize(WidgetDesc* d)
	{
		ContainerWidget::_initialize(d);

		mDesc = dynamic_cast<ToolBarDesc*>(mWidgetDesc);

		ToolBarDesc* tbd = dynamic_cast<ToolBarDesc*>(d);

		mDesc->itemLayout = tbd->itemLayout;

		if(mWidgetDesc->dimensions.size.width > mWidgetDesc->dimensions.size.height)
			mOrientation = TOOLBAR_ORIENTATION_HORIZONTAL;
		else
			mOrientation = TOOLBAR_ORIENTATION_VERTICAL;

		setSkinType(d->skinTypeName);
	}

	void ToolBar::addChild(Widget* w)
	{
		if(!w->isToolBarItem())
			throw Exception(Exception::ERR_INVALID_CHILD,"Cannot add a non-ToolBarItem to a ToolBar!","ToolBar::addChild");

		ContainerWidget::addChild(w);

		// Update link to parent ToolBar
		dynamic_cast<ToolBarItem*>(w)->notifyToolBarParent(this);

		if(w->getClass() == "Menu")
		{
			Menu* m = dynamic_cast<Menu*>(w);
			// For menu's that are directly connected to a ToolBar, we set their owner menu reference to themselves.
			m->notifyMenuParent(m);
			mMenus.push_back(m);
		}
	}

	void ToolBar::closeMenus()
	{
		for(std::vector<Menu*>::iterator it = mMenus.begin(); it != mMenus.end(); ++it)
			(*it)->closeMenu();

		mMenuOpened = false;
	}

	Menu* ToolBar::createMenu(MenuDesc& d)
	{
		d.toolBar = this;

		if(mOrientation == TOOLBAR_ORIENTATION_HORIZONTAL)
		{
			d.verticalAnchor = ANCHOR_VERTICAL_TOP_BOTTOM;
			d.dimensions.size = Size(d.textDesc.getTextWidth() + (d.padding[PADDING_LEFT] + d.padding[PADDING_RIGHT]),mDesc->dimensions.size.height);
		}
		else
		{
			d.horizontalAnchor = ANCHOR_HORIZONTAL_LEFT_RIGHT;
			d.dimensions.size = Size(mDesc->dimensions.size.width,d.textDesc.getTextHeight() + (d.padding[PADDING_TOP] + d.padding[PADDING_BOTTOM]));
		}

		Menu* newMenu = dynamic_cast<Menu*>(mWidgetDesc->guiManager->createWidget("Menu",d));

		newMenu->setPosition(getNextAvailableItemPosition(newMenu));

		// Its important we add the child after getNextAvailableItemPosition,
		// as it queries children to find the next position.
		addChild(newMenu);

		return newMenu;
	}

	ToolBarItem* ToolBar::createToolBarItem(const Ogre::String& className, ToolBarItemDesc& d)
	{
		if(className == "Menu")
			return createMenu(dynamic_cast<MenuDesc&>(d));

		d.toolBar = this;

		if(mOrientation == TOOLBAR_ORIENTATION_HORIZONTAL)
			d.verticalAnchor = ANCHOR_VERTICAL_TOP_BOTTOM;
		else
			d.horizontalAnchor = ANCHOR_HORIZONTAL_LEFT_RIGHT;

		ToolBarItem* newItem = dynamic_cast<ToolBarItem*>(mWidgetDesc->guiManager->createWidget(className,d));

		newItem->setPosition(getNextAvailableItemPosition(newItem));

		// Its important we add the child after getNextAvailableItemPosition,
		// as it queries children to find the next position.
		addChild(newItem);

		return newItem;
	}

	Widget* ToolBar::factory(const Ogre::String& widgetName)
	{
		Widget* newWidget = new ToolBar(widgetName);

		newWidget->_createDescObject("ToolBarDesc");

		return newWidget;
	}

	Ogre::String ToolBar::getClass()
	{
		return "ToolBar";
	}

	int ToolBar::getNumberOfItems()
	{
		return static_cast<int>(mChildren.size());
	}

	ToolBarOrientation ToolBar::getOrientation()
	{
		return mOrientation;
	}

	Point ToolBar::getNextAvailableItemPosition(ToolBarItem* newlyAddedItem)
	{
		if(mDesc->itemLayout == TOOLBAR_ITEM_LAYOUT_NEGATIVE_TO_POSITIVE)
		{
			if(mOrientation == TOOLBAR_ORIENTATION_HORIZONTAL)
			{
				float xPos = 0;
				if(!mChildren.empty())
					xPos = mChildren.back()->getPosition().x + mChildren.back()->getSize().width;
				return Point(xPos,0);
			}
			else
			{
				float yPos = 0;
				if(!mChildren.empty())
					yPos = mChildren.back()->getPosition().y + mChildren.back()->getSize().height;
				return Point(0,yPos);
			}
		}
		else
		{
			if(mOrientation == TOOLBAR_ORIENTATION_HORIZONTAL)
			{
				float xPos = mDesc->dimensions.size.width - newlyAddedItem->getSize().width;
				if(!mChildren.empty())
					xPos = mChildren.back()->getPosition().x - mChildren.back()->getSize().width;
				return Point(xPos,0);
			}
			else
			{
				float yPos = mDesc->dimensions.size.height - newlyAddedItem->getSize().height;
				if(!mChildren.empty())
					yPos = mChildren.back()->getPosition().y - mChildren.back()->getSize().height;
				return Point(0,yPos);
			}
		}
	}

	bool ToolBar::isMenuOpened()
	{
		return mMenuOpened;
	}

	void ToolBar::onDraw()
	{
		SkinType* st = mSkinType;
		if(!mWidgetDesc->enabled && mWidgetDesc->disabledSkinType != "")
			st = SkinTypeManager::getSingleton().getSkinType(getClass(),mWidgetDesc->disabledSkinType);

		Brush* brush = Brush::getSingletonPtr();

		brush->setFilterMode(mDesc->brushFilterMode);
			
		brush->drawSkinElement(Rect(mTexturePosition,mWidgetDesc->dimensions.size),st->getSkinElement(mSkinElementName));
	}

	void ToolBar::openMenu(Menu* m)
	{
		bool found = false;
		for(std::vector<Menu*>::iterator it = mMenus.begin(); it != mMenus.end(); ++it)
		{
			if((*it)->hasSubMenu(m))
			{
				(*it)->openMenu();	
				(*it)->openSubMenu(m);
				found = true;
				mMenuOpened = true;
			}
			else
				(*it)->closeMenu();
		}

		if(!found)
			throw Exception(Exception::ERR_INVALID_CHILD,"Given Menu is not a Child of this ToolBar!","ToolBar::openMenu");
	}
}