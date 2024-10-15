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
#ifndef __WorldContext_H__
#define __WorldContext_H__

#include "OgreApplicationContext.h"

#include "World.h"
#include "OgreRenderWindow.h"

namespace OgreBites
{
    /*=============================================================================
    | Base class responsible for setting up a common context for worlds.
    | May be subclassed for specific world types (not specific worlds).
    | Allows one world to run at a time, while maintaining a world queue.
    =============================================================================*/
    class WorldContext : public ApplicationContext, public InputListener
    {
    public:
        Ogre::RenderWindow* mWindow;

        WorldContext(const Ogre::String& appName = OGRE_VERSION_NAME)
        : ApplicationContext(appName), mWindow(NULL)
        {
            mCurrentWorld = 0;
            mWorldPaused = false;
            mLastRun = false;
            mLastWorld = 0;
        }

        virtual World* getCurrentWorld()
        {
            return mCurrentWorld;
        }

        /*-----------------------------------------------------------------------------
        | Quits the current world and starts a new one.
        -----------------------------------------------------------------------------*/
        virtual void runWorld(World* s)
        {
            Ogre::Profiler* prof = Ogre::Profiler::getSingletonPtr();
            if (prof)
                prof->setEnabled(false);

            if (mCurrentWorld)
            {
                mCurrentWorld->_shutdown();    // quit current world
                mWorldPaused = false;          // don't pause the next world
            }

            mWindow->removeAllViewports();                  // wipe viewports
            mWindow->resetStatistics();

            if (s)
            {
                // retrieve world's required plugins and currently installed plugins
                for (const auto& p : s->getRequiredPlugins())
                {
                    bool found = false;
                    // try to find the required plugin in the current installed plugins
                    for (const auto *i : mRoot->getInstalledPlugins())
                    {
                        if (i->getName() == p)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)  // throw an exception if a plugin is not found
                    {
                        OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, "World requires plugin: " + p);
                    }
                }

                // test system capabilities against world requirements
                s->testCapabilities(mRoot->getRenderSystem()->getCapabilities());
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
                s->setShaderGenerator(mShaderGenerator);
#endif
                s->_setup(this);   // start new world
            }

            if (prof)
                prof->setEnabled(true);

            mCurrentWorld = s;
        }

        /*-----------------------------------------------------------------------------
        | This function encapsulates the entire lifetime of the context.
        -----------------------------------------------------------------------------*/
        virtual void go(World* initialWorld = 0)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mLastRun = true;  // assume this is our last run

            initApp();
            loadStartUpWorld();
#else
            while (!mLastRun)
            {
                mLastRun = true;  // assume this is our last run

                initApp();

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
                // restore the last world if there was one or, if not, start initial world
                if (!mFirstRun) recoverLastWorld();
                else if (initialWorld) runWorld(initialWorld);
#endif

                loadStartUpWorld();

                if (mRoot->getRenderSystem() != NULL)
                {
                    mRoot->startRendering();    // start the render loop
                }

                closeApp();

                mFirstRun = false;
            }
#endif
        }

        virtual void loadStartUpWorld() {}

        bool isCurrentWorldPaused()
        {
            return !mCurrentWorld || mWorldPaused;
        }

        virtual void pauseCurrentWorld()
        {
            if (!isCurrentWorldPaused())
            {
                mWorldPaused = true;
                mCurrentWorld->paused();
            }
        }

        virtual void unpauseCurrentWorld()
        {
            if (mCurrentWorld && mWorldPaused)
            {
                mWorldPaused = false;
                mCurrentWorld->unpaused();
            }
        }

        /*-----------------------------------------------------------------------------
        | Processes frame started events.
        -----------------------------------------------------------------------------*/
        bool frameStarted(const Ogre::FrameEvent& evt) override
        {
            pollEvents();

            // manually call world callback to ensure correct order
            return !isCurrentWorldPaused() ? mCurrentWorld->frameStarted(evt) : true;
        }

        /*-----------------------------------------------------------------------------
        | Processes rendering queued events.
        -----------------------------------------------------------------------------*/
        bool frameRenderingQueued(const Ogre::FrameEvent& evt) override
        {
            // manually call world callback to ensure correct order
            return !isCurrentWorldPaused() ? mCurrentWorld->frameRenderingQueued(evt) : true;
        }

        /*-----------------------------------------------------------------------------
        | Processes frame ended events.
        -----------------------------------------------------------------------------*/
        bool frameEnded(const Ogre::FrameEvent& evt) override
        {
            // manually call world callback to ensure correct order
            if (mCurrentWorld && !mWorldPaused && !mCurrentWorld->frameEnded(evt)) return false;
            // quit if window was closed
            if (mWindow->isClosed()) return false;
            // go into idle mode if current world has ended
            if (mCurrentWorld && mCurrentWorld->isDone()) runWorld(0);

            return true;
        }

        bool isFirstRun() { return mFirstRun; }
        void setFirstRun(bool flag) { mFirstRun = flag; }
        bool isLastRun() { return mLastRun; }
        void setLastRun(bool flag) { mLastRun = flag; }
    protected:
        /*-----------------------------------------------------------------------------
        | Reconfigures the context. Attempts to preserve the current world state.
        -----------------------------------------------------------------------------*/
        void reconfigure(const Ogre::String& renderer)
        {
            // save current world state
            mLastWorld = mCurrentWorld;
            if (mCurrentWorld) mCurrentWorld->saveState(mLastWorldState);

            mLastRun = false;             // we want to go again with the new settings
            mNextRenderer = renderer;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // Need to save the config on iOS to make sure that changes are kept on disk
            mRoot->saveConfig();
#endif
            mRoot->queueEndRendering(); // break from render loop
        }
        using ApplicationContextBase::reconfigure; // unused, silence warning

        /*-----------------------------------------------------------------------------
        | Recovers the last world after a reset. You can override in the case that
        | the last world is destroyed in the process of resetting, and you have to
        | recover it through another means.
        -----------------------------------------------------------------------------*/
        virtual void recoverLastWorld()
        {
            runWorld(mLastWorld);
            mLastWorld->restoreState(mLastWorldState);
            mLastWorld = 0;
            mLastWorldState.clear();
        }

        /*-----------------------------------------------------------------------------
        | Cleans up and shuts down the context.
        -----------------------------------------------------------------------------*/
        void shutdown() override
        {
            if (mCurrentWorld)
            {
                mCurrentWorld->_shutdown();
                mCurrentWorld = 0;
            }

            ApplicationContext::shutdown();
        }

        World* mCurrentWorld;         // The active world (0 if none is active)
        bool mWorldPaused;             // whether current world is paused
        bool mLastRun;                  // whether or not this is the final run
        World* mLastWorld;            // last world run before reconfiguration
        Ogre::NameValuePairList mLastWorldState;     // state of last world
    };
}

#endif
