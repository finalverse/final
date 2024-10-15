/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2014 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef __WorldPlugin_H__
#define __WorldPlugin_H__

#include "OgrePlugin.h"
#include "World.h"

// Export macro to export the world's main dll functions.
#if defined( OGRE_STATIC_LIB )
#  define _OgreWorldExport
#else
#  if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && !defined(__MINGW32__)
#    define _OgreWorldExport __declspec(dllexport)
#  elif defined ( OGRE_GCC_VISIBILITY )
#   define _OgreWorldExport  __attribute__ ((visibility("default")))
#  else
#   define _OgreWorldExport
#  endif
#endif

#define _OgreWorldClassExport

namespace OgreBites
{
    /*=============================================================================
    | Utility class used to hold a set of worlds in an OGRE plugin.
    =============================================================================*/
    class _OgreWorldExport WorldPlugin : public Ogre::Plugin
    {
    public:

        WorldPlugin(const Ogre::String& name)
        : mName(name)
        {
        }

        const Ogre::String& getName() const override
        {
            return mName;
        }
        
        void install() override {}
        void uninstall() override {}
        void initialise() override {}
        void shutdown() override {}

        /*-----------------------------------------------------------------------------
        | Adds a world to the queue.
        -----------------------------------------------------------------------------*/
        void addWorld(World* s)
        {
            mWorlds.insert(s);
        }

        /*-----------------------------------------------------------------------------
        | Retrieves the queue of worlds.
        -----------------------------------------------------------------------------*/
        const WorldSet& getWorlds()
        {
            return mWorlds;
        }

    protected:

        Ogre::String mName;
        WorldSet mWorlds;
    };
}

#endif
