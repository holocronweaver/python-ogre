#include "QuickGUIComponentWidget.h"
#include "QuickGUIBrush.h"
#include "QuickGUISerialWriter.h"
#include "QuickGUIWindow.h"
#include "QuickGUISheet.h"
#include "QuickGUIFactoryManager.h"

namespace QuickGUI
{
	ComponentWidgetDesc::ComponentWidgetDesc(const Ogre::String& id) :
		WidgetDesc(id)
	{
		resetToDefault();
	}

	void ComponentWidgetDesc::resetToDefault()
	{
		WidgetDesc::resetToDefault();

		componentwidget_clipComponentsToDimensions = true;
	}

	void ComponentWidgetDesc::serialize(SerialBase* b)
	{
		WidgetDesc::serialize(b);

		b->IO("ClipComponentsToDimensions",&componentwidget_clipComponentsToDimensions);
	}

	ComponentWidget::ComponentWidget(const Ogre::String& name) :
		Widget(name)
	{
	}

	ComponentWidget::~ComponentWidget()
	{
		Factory<Widget>* widgetFactory = FactoryManager::getSingleton().getWidgetFactory();
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
			widgetFactory->destroyInstance((*it).second);
	}

	void ComponentWidget::_initialize(WidgetDesc* d)
	{
		Widget::_initialize(d);

		ComponentWidgetDesc* cwd = dynamic_cast<ComponentWidgetDesc*>(d);
		setClipComponentsToDimensions(cwd->componentwidget_clipComponentsToDimensions);
	}

	void ComponentWidget::_setGUIManager(GUIManager* gm)
	{
		Widget::_setGUIManager(gm);

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
			(*it).second->_setGUIManager(gm);
	}

	void ComponentWidget::_setSheet(Sheet* sheet)
	{
		Widget::_setSheet(sheet);

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
			(*it).second->_setSheet(sheet);
	}

	void ComponentWidget::addComponent(const Ogre::String& alias, Widget* w)
	{
		if(mComponents.find(alias) != mComponents.end())
			throw Exception(Exception::ERR_WIDGET_ALREADY_ADDED,"The widget \"" + mWidgetDesc->widget_name + "\" already contains a Component using alias \"" + alias + "\"!","addComponent");

		w->mComponentOfAWidget = true;
		w->setPositionRelativeToParentClientDimensions(false);
		mComponents[alias] = w;
		w->setParent(this);
		w->_setGUIManager(mWidgetDesc->guiManager);
		w->_setSheet(mWidgetDesc->sheet);
	}

	void ComponentWidget::draw()
	{
		// check visibility
		if( !mWidgetDesc->widget_visible )
			return;

		Brush* brush = Brush::getSingletonPtr();

		// check and store clip region
		Rect prevClipRegion = brush->getClipRegion();
		if ( prevClipRegion.getIntersection(Rect(mTexturePosition,mWidgetDesc->widget_dimensions.size)) == Rect::ZERO )
			return;

		// set clip region to dimensions
		brush->setClipRegion(Rect(mTexturePosition,mWidgetDesc->widget_dimensions.size).getIntersection(prevClipRegion));

		// Set opacity before drawing operations
		brush->setOpacity(getAbsoluteOpacity());

		// draw self
		onDraw();

		if(!dynamic_cast<ComponentWidgetDesc*>(mWidgetDesc)->componentwidget_clipComponentsToDimensions)
			brush->setClipRegion(prevClipRegion);

		// draw components
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
			(*it).second->draw();

		// restore clip region
		brush->setClipRegion(prevClipRegion);
	}

	Widget* ComponentWidget::findWidget(const Ogre::String& name)
	{
		Widget* w;
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			w = (*it).second->findWidget(name);
			if(w != NULL)
				return w;
		}

		return Widget::findWidget(name);
	}

	Widget* ComponentWidget::findWidgetAtPoint(const Point& p, bool ignoreDisabled)
	{
		// If we are not widget_visible, return NULL
		if(!mWidgetDesc->widget_visible)
			return NULL;

		// If we ignore disabled and this widget is !widget_enabled, return NULL
		if(ignoreDisabled && !mWidgetDesc->widget_enabled)
			return NULL;

		// Check components before verifying point is within bounds. (Components can lie outside widget dimensions)
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			Widget* w = (*it).second->findWidgetAtPoint(p,ignoreDisabled);
			if(w != NULL)
				return w;
		}

		// See if point is within widget bounds
		if(!Rect(mTexturePosition,mWidgetDesc->widget_dimensions.size).isPointWithinBounds(p))
			return NULL;
				
		// Take transparency picking into account
		if(mWidgetDesc->widget_transparencyPicking && (mSkinType != NULL))
		{
			// If the background is transparent, return NULL
			if(mSkinType->getSkinElement(mSkinElementName)->getTextureName() == "")
				return NULL;

			// Get relative position
			Point relPos = p - mTexturePosition;
			// Get percentage of position relative to widget widget_dimensions
			relPos.x = relPos.x / mWidgetDesc->widget_dimensions.size.width;
			relPos.y = relPos.y / mWidgetDesc->widget_dimensions.size.height;
			// Get pixel position of texture
			Point pixelPos;
			SkinElement* se = mSkinType->getSkinElement(mSkinElementName);
			pixelPos.x = relPos.x * (se->getWidth() - 1);
			pixelPos.y = relPos.y * (se->getHeight() - 1);
			
			if(mSkinType->getSkinElement(mSkinElementName)->transparentPixel(pixelPos))
				return NULL;
		}

		return this;
	}

	bool ComponentWidget::getClipComponentsToDimensions()
	{
		return dynamic_cast<ComponentWidgetDesc*>(mWidgetDesc)->componentwidget_clipComponentsToDimensions;
	}

	bool ComponentWidget::isComponentWidget()
	{
		return true;
	}

	void ComponentWidget::setClipComponentsToDimensions(bool clip)
	{
		dynamic_cast<ComponentWidgetDesc*>(mWidgetDesc)->componentwidget_clipComponentsToDimensions = clip;

		redraw();
	}

	void ComponentWidget::setHeight(float pixelHeight)
	{
		float previousHeight = mWidgetDesc->widget_dimensions.size.height;

		// Almost the same as Widget::setHeight, only we took out the firing of the event, and fired the event at the
		// end of the function.
		{
			// Enforce max height
			if((mWidgetDesc->widget_maxSize.height > 0) && (pixelHeight > mWidgetDesc->widget_maxSize.height))
				pixelHeight = mWidgetDesc->widget_maxSize.height;
			// Enforce min height
			else if((mWidgetDesc->widget_minSize.height > 0) && (pixelHeight < mWidgetDesc->widget_minSize.height))
				pixelHeight = mWidgetDesc->widget_minSize.height;

			mWidgetDesc->widget_dimensions.size.height = pixelHeight;

			redraw();

			updateClientDimensions();
		}

		float difference = mWidgetDesc->widget_dimensions.size.height - previousHeight;

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			switch((*it).second->getVerticalAnchor())
			{
			case ANCHOR_VERTICAL_CENTER:
				{
					// Center vertically
					Point p = (*it).second->getPosition();
					p.y = (mWidgetDesc->widget_dimensions.size.height / 2.0) - ((*it).second->getHeight() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_VERTICAL_CENTER_DYNAMIC:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setHeight((*it).second->getHeight() + difference);
					// Center vertically
					Point p = (*it).second->getPosition();
					p.y = (mWidgetDesc->widget_dimensions.size.height / 2.0) - ((*it).second->getHeight() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_VERTICAL_TOP_BOTTOM:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setHeight((*it).second->getHeight() + difference);
				}
				break;
			case ANCHOR_VERTICAL_BOTTOM:
				{
					// re-align component widget according to bottom edge of this widgets new dimensions
					Point p = (*it).second->getPosition();
					p.y += difference;
					(*it).second->setPosition(p);
				}
				break;
			}
		}

		WidgetEventArgs args(this);
		fireWidgetEvent(WIDGET_EVENT_SIZE_CHANGED,args);
	}

	void ComponentWidget::setParent(Widget* parent)
	{
		Widget::setParent(parent);

		// Update components window reference via setParent
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			(*it).second->setParent(this);
		}
	}

	void ComponentWidget::setSize(Size size)
	{
		Size previousSize = mWidgetDesc->widget_dimensions.size;

		// Almost the same as Widget::setSize, only we took out the firing of the event, and fired the event at the
		// end of the function.
		{
			// Enforce max width
			if((mWidgetDesc->widget_maxSize.width > 0) && (size.width > mWidgetDesc->widget_maxSize.width))
				size.width = mWidgetDesc->widget_maxSize.width;
			// Enforce min width
			else if((mWidgetDesc->widget_minSize.width > 0) && (size.width < mWidgetDesc->widget_minSize.width))
				size.width = mWidgetDesc->widget_minSize.width;

			// Enforce min height
			if((mWidgetDesc->widget_maxSize.height > 0) && (size.height > mWidgetDesc->widget_maxSize.height))
				size.height = mWidgetDesc->widget_maxSize.height;
			// Enforce min width
			else if((mWidgetDesc->widget_minSize.height > 0) && (size.height < mWidgetDesc->widget_minSize.height))
				size.height = mWidgetDesc->widget_minSize.height;

			// Update size
			mWidgetDesc->widget_dimensions.size = size;

			updateClientDimensions();

			redraw();
		}

		// Get difference.  A larger size indicates a positive difference
		Size difference = mWidgetDesc->widget_dimensions.size - previousSize;

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			switch((*it).second->getHorizontalAnchor())
			{
			case ANCHOR_HORIZONTAL_CENTER:
				{
					// Center vertically
					Point p = (*it).second->getPosition();
					p.x = (mWidgetDesc->widget_dimensions.size.width / 2.0) - ((*it).second->getWidth() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_HORIZONTAL_CENTER_DYNAMIC:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setWidth((*it).second->getWidth() + difference.width);
					// Center vertically
					Point p = (*it).second->getPosition();
					p.x = (mWidgetDesc->widget_dimensions.size.width / 2.0) - ((*it).second->getWidth() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_HORIZONTAL_LEFT_RIGHT:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setWidth((*it).second->getWidth() + difference.width);
				}
				break;
			case ANCHOR_HORIZONTAL_RIGHT:
				{
					// re-align component widget according to right edge of this widgets new dimensions
					Point p = (*it).second->getPosition();
					p.x += difference.width;
					(*it).second->setPosition(p);
				}
				break;
			}

			switch((*it).second->getVerticalAnchor())
			{
			case ANCHOR_VERTICAL_CENTER:
				{
					// Center vertically
					Point p = (*it).second->getPosition();
					p.y = (mWidgetDesc->widget_dimensions.size.height / 2.0) - ((*it).second->getHeight() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_VERTICAL_CENTER_DYNAMIC:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setHeight((*it).second->getHeight() + difference.height);
					// Center vertically
					Point p = (*it).second->getPosition();
					p.y = (mWidgetDesc->widget_dimensions.size.height / 2.0) - ((*it).second->getHeight() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_VERTICAL_TOP_BOTTOM:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setHeight((*it).second->getHeight() + difference.height);
				}
				break;
			case ANCHOR_VERTICAL_BOTTOM:
				{
					// re-align component widget according to bottom edge of this widgets new dimensions
					Point p = (*it).second->getPosition();
					p.y += difference.height;
					(*it).second->setPosition(p);
				}
				break;
			}
		}

		WidgetEventArgs args(this);
		fireWidgetEvent(WIDGET_EVENT_SIZE_CHANGED,args);
	}

	void ComponentWidget::setSkinType(const Ogre::String type)
	{
		Widget::setSkinType(type);

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			if(mSkinType->hasComponentType((*it).first))
				(*it).second->setSkinType(mSkinType->getComponentType((*it).first)->typeName);
		}
	}

	void ComponentWidget::setWidth(float pixelWidth)
	{
		float previousWidth = mWidgetDesc->widget_dimensions.size.width;

		// Almost the same as Widget::setWidth, only we took out the firing of the event, and fired the event at the
		// end of the function.
		{
			// Enforce max width
			if((mWidgetDesc->widget_maxSize.width > 0) && (pixelWidth > mWidgetDesc->widget_maxSize.width))
				pixelWidth = mWidgetDesc->widget_maxSize.width;
			// Enforce min width
			else if((mWidgetDesc->widget_minSize.width > 0) && (pixelWidth < mWidgetDesc->widget_minSize.width))
				pixelWidth = mWidgetDesc->widget_minSize.width;

			mWidgetDesc->widget_dimensions.size.width = pixelWidth;

			redraw();

			updateClientDimensions();
		}

		float difference = mWidgetDesc->widget_dimensions.size.width - previousWidth;

		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			switch((*it).second->getHorizontalAnchor())
			{
			case ANCHOR_HORIZONTAL_CENTER:
				{
					// Center vertically
					Point p = (*it).second->getPosition();
					p.x = (mWidgetDesc->widget_dimensions.size.width / 2.0) - ((*it).second->getWidth() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_HORIZONTAL_CENTER_DYNAMIC:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setWidth((*it).second->getWidth() + difference);
					// Center vertically
					Point p = (*it).second->getPosition();
					p.x = (mWidgetDesc->widget_dimensions.size.width / 2.0) - ((*it).second->getWidth() / 2.0);
					(*it).second->setPosition(p);
				}
				break;
			case ANCHOR_HORIZONTAL_LEFT_RIGHT:
				{
					// grow or shrink the widget according to new dimensions
					(*it).second->setWidth((*it).second->getWidth() + difference);
				}
				break;
			case ANCHOR_HORIZONTAL_RIGHT:
				{
					// re-align component widget according to right edge of this widgets new dimensions
					Point p = (*it).second->getPosition();
					p.x += difference;
					(*it).second->setPosition(p);
				}
				break;
			}
		}

		WidgetEventArgs args(this);
		fireWidgetEvent(WIDGET_EVENT_SIZE_CHANGED,args);
	}

	void ComponentWidget::updateTexturePosition()
	{
		Widget::updateTexturePosition();

		// Update component screen dimensions, must be done after client and screen rect have been calculated
		for(std::map<Ogre::String,Widget*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			(*it).second->updateTexturePosition();
		}
	}
}
