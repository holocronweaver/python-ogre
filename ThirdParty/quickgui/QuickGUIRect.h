#ifndef QUICKGUIRECT_H
#define QUICKGUIRECT_H

#include "OgrePrerequisites.h"

#include "QuickGUIExportDLL.h"
#include "QuickGUIPoint.h"
#include "QuickGUISize.h"

namespace QuickGUI
{
	class _QuickGUIExport Rect
	{
	public:
		Rect();
		Rect(Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height);
		Rect(Point p, Size s);

		inline Rect( const Rect& r )
            : x( r.x ), y( r.y ), width( r.width ), height( r.height )
        {
        }

		inline Rect& operator = ( const Rect& r )
        {
            x = r.x;
            y = r.y;
            width = r.width;
            height = r.height;

            return *this;
        }

		inline bool operator != ( const Rect& r ) const
        {
            return ( x != r.x ||
                y != r.y ||
                width != r.width ||
                height != r.height );
        }

		bool inside(const Rect& r);

		Ogre::Real x;
		Ogre::Real y;
		Ogre::Real width;
		Ogre::Real height;

		static const Rect ZERO;
	};
}

#endif
