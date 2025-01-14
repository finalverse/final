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

#ifndef __Creator_iOS_H__
#define __Creator_iOS_H__

#include "OgrePlatform.h"

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
#error This header is for use with iOS only
#endif

#import <UIKit/UIKit.h> 
#import <QuartzCore/QuartzCore.h>

// Defaulting to 2 means that we run at 30 frames per second. For 60 frames, use a value of 1.
// 30 FPS is usually sufficient and results in lower power consumption.
#define DISPLAYLINK_FRAME_INTERVAL      2

#ifdef __OBJC__

namespace OgreBites
{
    class Creator;
}

@interface CreatorGestureView : UIView
{
    OgreBites::Creator *mCreator;
}
@property (assign) OgreBites::Creator *mCreator;

@end

#import "Creator.h"

@interface AppDelegate : NSObject <UIApplicationDelegate>
{
    OgreBites::Creator sb;

    CADisplayLink *mDisplayLink;
    NSDate* mDate;
    NSTimeInterval mLastFrameTime;
}

- (void)go;
- (void)renderOneFrame:(id)sender;

@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

@implementation AppDelegate

@dynamic mLastFrameTime;

- (NSTimeInterval)mLastFrameTime
{
    return mLastFrameTime;
}

- (void)setLastFrameTime:(NSTimeInterval)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        mLastFrameTime = frameInterval;
    }
}

- (void)go {

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    try {
        sb.go();
    } catch( Ogre::Exception& e ) {
        std::cerr << "An exception has occurred: " <<
        e.getFullDescription().c_str() << std::endl;
    }

    [pool release];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    mLastFrameTime = DISPLAYLINK_FRAME_INTERVAL;
    mDisplayLink = nil;

    [self go];

    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    sb.shutdown();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Reset event times and reallocate the date and displaylink objects
    Ogre::Root::getSingleton().clearEventTimes();
    mDate = [[NSDate alloc] init];
    mLastFrameTime = DISPLAYLINK_FRAME_INTERVAL; // Reset the timer

    mDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderOneFrame:)];
    [mDisplayLink setFrameInterval:mLastFrameTime];
    [mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    Ogre::Root::getSingleton().saveConfig();

    [mDate release];
    mDate = nil;
    
    [mDisplayLink invalidate];
    mDisplayLink = nil;
}

- (void)renderOneFrame:(id)sender
{
    [sb.mGestureView becomeFirstResponder];

    // NSTimeInterval is a simple typedef for double
    NSTimeInterval currentFrameTime = -[mDate timeIntervalSinceNow];
    NSTimeInterval differenceInSeconds = currentFrameTime - mLastFrameTime;
    mLastFrameTime = currentFrameTime;

    dispatch_async(dispatch_get_main_queue(), ^(void)
    {
        Ogre::Root::getSingleton().renderOneFrame((Ogre::Real)differenceInSeconds);
    });
}

@end

@implementation CreatorGestureView

@synthesize mCreator;

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (void)dealloc {
    [super dealloc];
}

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mCreator && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mCreator->motionBegan();

    if ([super respondsToSelector:@selector(motionBegan:withEvent:)]) {
        [super motionBegan:motion withEvent:event];
    }
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mCreator && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mCreator->motionEnded();

    if ([super respondsToSelector:@selector(motionEnded:withEvent:)]) {
        [super motionEnded:motion withEvent:event];
    }
}

- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mCreator && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mCreator->motionCancelled();

    if ([super respondsToSelector:@selector(motionCancelled:withEvent:)]) {
        [super motionCancelled:motion withEvent:event];
    }
}
@end

#endif

#endif
