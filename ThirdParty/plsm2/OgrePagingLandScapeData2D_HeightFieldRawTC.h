/***************************************************************************
	OgrePagingLandScapeData2D_HeightFieldRawTC.h  -  description
  -------------------
  begin                : Mon Oct 13 2003
  copyright            : (C) 2002-2006 by Jose A Milan & Tuan Kuranes
  email                : spoke@supercable.es & tuan.kuranes@free.fr
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Lesser General Public License as        *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
***************************************************************************/

#ifndef PAGINGLandScapeDATA2D_HEIGHTFIELDRawTC_H
#define PAGINGLandScapeDATA2D_HEIGHTFIELDRawTC_H

#include "OgrePagingLandScapePrerequisites.h"

namespace Ogre
{
    /**
    * A specialized class for loading 2D Data from a compressed HeightField file.
    */
    class PagingLandScapeData2D_HeightFieldRawTC: public PagingLandScapeData2D
    {
        public:
	        PagingLandScapeData2D_HeightFieldRawTC(PagingLandScapeData2DManager *dataMgr);
            virtual String getName() const{return String("HeightFieldRawTC");}

	        virtual ~PagingLandScapeData2D_HeightFieldRawTC(void);

            virtual const Vector3 getNormalAt(const Real mX, const Real mZ);
            virtual const ColourValue getBase(const Real mX, const Real mZ);
            virtual const ColourValue getCoverage(const Real mX, const Real mZ);
            virtual PagingLandScapeData2D* newPage();;
            const Real getMaxAbsoluteHeight(void) const; 


        protected:
            virtual void _save(void);
	        virtual bool _load(const unsigned int x, const unsigned int z);
            virtual void _load(void);
	        virtual void _unload(void);

        private:
            inline Real _decodeRawTC(const Real encoded) const;
            inline ushort _encodeRawTC(const Real decoded) const;    

	        Real input_max, input_min;
	        Image* mImage;
            unsigned int mBpp;// should be 4 bytes (mImage is RGBA)

    };
}

#endif
