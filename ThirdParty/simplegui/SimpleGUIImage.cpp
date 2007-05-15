#include "SimpleGUIImage.h"

namespace SimpleGUI
{
	Image::Image(const Ogre::String& name, const Ogre::Vector4& dimensions, const Ogre::String& material, bool show, Ogre::OverlayContainer* overlayContainer, Widget* ParentWidget) :
		Widget(name,dimensions,overlayContainer,ParentWidget),
		mMaterial(material)
	{
		mWidgetType = Widget::SGUI_IMAGE;
		mMaterialPtr.setNull();

		mPanelOverlayElement = createPanelOverlayElement(mInstanceName+"_Background",mPixelDimensions,mMaterial);
		mOverlayContainer->addChild(mPanelOverlayElement);
		mPanelOverlayElement->show();

		if(show) this->show();
		else hide();
	}

	Image::~Image()
	{
		Ogre::OverlayManager* om = Ogre::OverlayManager::getSingletonPtr();

		mOverlayContainer->removeChild(mPanelOverlayElement->getName());
		om->destroyOverlayElement(mPanelOverlayElement);

		// If a material was created (for rendertexture use), destroy and unload it
		if(!mMaterialPtr.isNull())
		{
			Ogre::String name = mMaterialPtr->getName();
			mMaterialPtr.setNull();
			Ogre::MaterialManager::getSingleton().remove(name);
		}
	}

	void Image::_applyDimensions()
	{
		mPanelOverlayElement->setPosition(mPixelDimensions.x,mPixelDimensions.y);
		mPanelOverlayElement->setDimensions(mPixelDimensions.z,mPixelDimensions.w);
	}

	void Image::_notifyDimensionsChanged()
	{
		_updateDimensions(mRelativeDimensions);
		_applyDimensions();
	}

	void Image::hide()
	{
		mPanelOverlayElement->hide();
		mVisible = false;
	}

	void Image::setMaterial(Ogre::RenderTexture* texture)
	{
		setMaterial(texture->getName(),true);
	}

	void Image::setMaterial(const Ogre::String& name, bool texture)
	{
		if(!texture)
		{
			mMaterial = name;
			mPanelOverlayElement->setMaterialName(mMaterial);
		}
		else
		{
			// If a previous material exists, destroy and unload it
			if(!mMaterialPtr.isNull())
			{
				mPanelOverlayElement->setMaterialName(mMaterial);
				Ogre::String name = mMaterialPtr->getName();
				mMaterialPtr.setNull();
				Ogre::MaterialManager::getSingleton().remove(name);
			}

			mMaterialPtr = Ogre::MaterialManager::getSingleton().create(mInstanceName+"_RenderTextureMaterial",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::TextureUnitState* t = mMaterialPtr->getTechnique(0)->getPass(0)->createTextureUnitState(name);
			mPanelOverlayElement->setMaterialName(mMaterialPtr->getName());
		}
	}

	void Image::setTiling(const Ogre::Real& horizontalRepetitions, const Ogre::Real& verticalRepetitions, Ogre::ushort layer)
	{
		mPanelOverlayElement->setTiling(horizontalRepetitions,verticalRepetitions,layer);
	}

	void Image::setUV(const Ogre::Real& u1, const Ogre::Real& v1, const Ogre::Real& u2, const Ogre::Real& v2)
	{
		mPanelOverlayElement->setUV(u1,v1,u2,v2);
	}

	void Image::show()
	{
		mPanelOverlayElement->show();
		mVisible = true;
	}
}