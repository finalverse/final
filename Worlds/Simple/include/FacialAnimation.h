/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#ifndef __FacialAnimation_H__
#define __FacialAnimation_H__

#include "SdkWorld.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreWorldClassExport World_FacialAnimation : public SdkWorld
{
 public:

 World_FacialAnimation():
    mSpeakAnimState(0), mManualAnimState(0), mManualKeyFrame(0), mPlayAnimation(0)
    {
        mInfo["Title"] = "Facial Animation";
        mInfo["Description"] = "A demonstration of the facial animation feature, using pose animation.";
        mInfo["Thumbnail"] = "thumb_facial.png";
        mInfo["Category"] = "Animation";
        mInfo["Help"] = "Use the checkbox to enable/disable manual animation. "
            "When manual animation is enabled, use the sliders to adjust each pose's influence.";
    }

 protected:

    void setupContent(void) override
    {
        // setup some basic lighting for our scene
        mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
        mSceneMgr->getRootSceneNode()
            ->createChildSceneNode(Vector3(40, 60, 50))
            ->attachObject(mSceneMgr->createLight());
        mSceneMgr->getRootSceneNode()
            ->createChildSceneNode(Vector3(-120, -80, -50))
            ->attachObject(mSceneMgr->createLight());

        // pre-load the mesh so that we can tweak it with a manual animation
        mHeadMesh = MeshManager::getSingleton().load("facial.mesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        // create a manual animation, create a pose track for it, and create a keyframe in that track
        mManualKeyFrame = mHeadMesh->createAnimation("Manual", 0)->createVertexTrack(4, VAT_POSE)->createVertexPoseKeyFrame(0);

        // create pose references for the first 15 poses
        for (unsigned int i = 0; i < 15; i++) mManualKeyFrame->addPoseReference(i, 0);

        // create a head entity from the mesh and attach it to a node with a vertical offset to center it
        Entity* head = mSceneMgr->createEntity("Head", "facial.mesh");

        mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, -30, 0))->attachObject(head);

        // get the animation states
        mSpeakAnimState = head->getAnimationState("Speak");
        mManualAnimState = head->getAnimationState("Manual");

        // make the camera orbit around the head, and show the cursor
        mCameraMan->setStyle(CS_ORBIT);
        mCameraMan->setYawPitchDist(Radian(0), Radian(0), 130);
        mTrayMgr->showCursor();

        // by default, the speaking animation is enabled
        auto& controllerMgr = ControllerManager::getSingleton();
        mPlayAnimation = controllerMgr.createFrameTimePassthroughController(
            AnimationStateControllerValue::create(mSpeakAnimState, true));

        setupControls();
    }

    void setupControls()
    {
        // make logo and frame stats a little more compact to make room for controls
        mTrayMgr->showLogo(TL_BOTTOMLEFT);
        mTrayMgr->toggleAdvancedFrameStats();

        // create group labels for the different sliders
        mExpressions.push_back(mTrayMgr->createLabel(TL_NONE, "ExpressionLabel", "Expressions"));
        mMouthShapes.push_back(mTrayMgr->createLabel(TL_NONE, "MouthShapeLabel", "Mouth Shapes"));

        // create sliders to adjust pose influence
        for (unsigned int i = 0; i < mManualKeyFrame->getPoseReferences().size(); i++)
        {
            String sliderName = "Pose" + StringConverter::toString(i);
            String poseName = mHeadMesh->getPoseList()[i]->getName();

            if (poseName.find("Expression") != std::string::npos)
                mExpressions.push_back(mTrayMgr->createLongSlider(TL_NONE, sliderName, poseName.substr(11), 200, 80, 44, 0, 1, 11));
            else mMouthShapes.push_back(mTrayMgr->createLongSlider(TL_NONE, sliderName, poseName.substr(0, 1), 160, 80, 44, 0, 1, 11));
        }

        // checkbox to switch between automatic animation and manual animation.
        mTrayMgr->createCheckBox(TL_TOP, "Manual", "Manual Animation")->setChecked(!mPlayAnimation);
    }

    void cleanupContent() override
    {
        mExpressions.clear();
        mMouthShapes.clear();
        mSpeakAnimState = 0;
        mManualAnimState = 0;
        mManualKeyFrame = 0;
        mHeadMesh->unload();
    }

    void checkBoxToggled(OgreBites::CheckBox * box) override
    {
        bool play = !box->isChecked();
        mPlayAnimation->setEnabled(play);

        // toggle animation states
        mSpeakAnimState->setEnabled(play);
        mManualAnimState->setEnabled(!play);

        // toggle expression controls
        for (unsigned int i = 0; i < mExpressions.size(); i++)
        {
            mTrayMgr->moveWidgetToTray(mExpressions[i], play ? TL_NONE : TL_TOPLEFT);
            if (play) mExpressions[i]->hide();
            else mExpressions[i]->show();
        }

        // toggle mouth shape controls
        for (unsigned int i = 0; i < mMouthShapes.size(); i++)
        {
            mTrayMgr->moveWidgetToTray(mMouthShapes[i], play ? TL_NONE : TL_TOPRIGHT);
            if (play) mMouthShapes[i]->hide();
            else mMouthShapes[i]->show();
        }
    }

    void sliderMoved(OgreBites::Slider * slider) override
    {
        // update the pose reference controlled by this slider
        mManualKeyFrame->updatePoseReference(StringConverter::parseInt(slider->getName().substr(4)), slider->getValue());
        // dirty animation state since we're fudging this manually
        mManualAnimState->getParent()->_notifyDirty();
    }

    MeshPtr mHeadMesh;
    AnimationState* mSpeakAnimState;
    AnimationState* mManualAnimState;
    VertexPoseKeyFrame* mManualKeyFrame;
    ControllerFloat* mPlayAnimation;
    WidgetList mExpressions;
    WidgetList mMouthShapes;
};

#endif
