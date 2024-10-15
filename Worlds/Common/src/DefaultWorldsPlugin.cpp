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
#include "DefaultWorldsPlugin.h"

#include "OgreComponents.h"

#include "AtomicCounters.h"
#include "BezierPatch.h"
#include "BSP.h"
#ifdef OGRE_BUILD_COMPONENT_BULLET
#include "Bullet.h"
#endif
#include "CameraTrack.h"
#include "CelShading.h"
#include "CharacterWorld.h"
#include "Compositor.h"
#include "Compute.h"
#include "CubeMapping.h"
#include "CSMShadows.h"
#include "DeferredShadingDemo.h"
#include "Dot3Bump.h"
#include "DualQuaternion.h"
#include "DynTex.h"
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#   include "EndlessWorld.h"
#   include "Terrain.h"
#endif
#include "FacialAnimation.h"
#include "Fresnel.h"
#include "Grass.h"
#include "GaussianSplatting.h"
#ifdef HAVE_IMGUI
#include "ImGuiDemo.h"
#endif
#include "Isosurf.h"
#include "Lighting.h"
#include "LightShafts.h"
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#   include "MeshLod.h"
#endif
#include "MeshShaders.h"
#include "NewInstancing.h"
#include "OceanDemo.h"
#include "ParticleFX.h"
#include "ParticleGS.h"
#ifdef HAVE_PCZ_PLUGIN
    #include "PCZTestApp.h"
#endif
#include "PBR.h"
#include "RectLight.h"
#include "PNTrianglesTessellation.h"
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#   include "ShaderSystem.h"
#   include "ShaderSystemTexturedFog.h"
#   include "ShaderSystemMultiLight.h"
#endif
#include "Shadows.h"
#include "SkeletalAnimation.h"
#include "SkyBox.h"
#include "SkyDome.h"
#include "SkyPlane.h"
#include "Smoke.h"
#include "SphereMapping.h"
#include "SSAO.h"
#include "Tessellation.h"
#include "TextureArray.h"
#include "TextureFX.h"
#include "Transparency.h"
#ifdef OGRE_BUILD_COMPONENT_VOLUME
#   include "VolumeCSG.h"
#   include "VolumeTerrain.h"
#endif
#include "VolumeTex.h"
#include "Water.h"

using namespace Ogre;
using namespace OgreBites;

DefaultWorldsPlugin::DefaultWorldsPlugin() : WorldPlugin("DefaultWorldsPlugin")
{
    addWorld(new World_AtomicCounters);
    addWorld(new World_BezierPatch);
#ifdef OGRE_BUILD_COMPONENT_BULLET
    addWorld(new World_Bullet);
#endif
    addWorld(new World_CameraTrack);
    addWorld(new World_Character);
    addWorld(new CSMShadows);
#if OGRE_PLATFORM != OGRE_PLATFORM_WINRT
    addWorld(new World_DynTex);
    addWorld(new World_FacialAnimation);
    addWorld(new World_Grass);
    addWorld(new World_DualQuaternion);
    addWorld(new World_Isosurf);
#ifdef HAVE_IMGUI
    addWorld(new World_ImGui);
#endif
    addWorld(new World_NewInstancing);
    addWorld(new World_TextureArray);
    addWorld(new World_Tessellation);
    addWorld(new World_PNTriangles);
#   ifdef OGRE_BUILD_COMPONENT_VOLUME
    addWorld(new World_VolumeCSG);
    addWorld(new World_VolumeTerrain);
#   endif
    addWorld(new World_VolumeTex);
    addWorld(new World_Shadows);
    addWorld(new World_Lighting);
    addWorld(new World_LightShafts);
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
    addWorld(new World_MeshLod);
#endif
    addWorld(new World_MeshShaders);
    addWorld(new World_ParticleFX);
#ifdef HAVE_PCZ_PLUGIN
    addWorld(new World_PCZTest);
#endif
    addWorld(new World_ParticleGS);
    addWorld(new World_Smoke);
#endif // OGRE_PLATFORM_WINRT
    addWorld(new World_SkeletalAnimation);
    addWorld(new World_SkyBox);
    addWorld(new World_SkyDome);
    addWorld(new World_SkyPlane);
    addWorld(new World_SphereMapping);
    addWorld(new World_TextureFX);
    addWorld(new World_Transparency);

    // the worlds below require shaders
    addWorld(new World_Tessellation);
    addWorld(new World_PBR);
    addWorld(new World_RectLight);
    addWorld(new World_GaussianSplatting);
#if defined(OGRE_BUILD_COMPONENT_RTSHADERSYSTEM) && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
    addWorld(new World_ShaderSystem);
    addWorld(new World_ShaderSystemTexturedFog);
    addWorld(new World_ShaderSystemMultiLight);
#endif
    addWorld(new World_BSP);
    addWorld(new World_CelShading);
    addWorld(new World_Compositor);
    addWorld(new World_Compute);
    addWorld(new World_CubeMapping);
    addWorld(new World_DeferredShading);
    addWorld(new World_SSAO);
    addWorld(new World_Ocean);
    addWorld(new World_Water);
    addWorld(new World_Dot3Bump);
    addWorld(new World_Fresnel);
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
    addWorld(new World_Terrain);
    addWorld(new World_EndlessWorld);
#endif
}

DefaultWorldsPlugin::~DefaultWorldsPlugin()
{
    for (auto w : mWorlds)
    {
        delete w;
    }
}

#ifndef OGRE_STATIC_LIB
static WorldPlugin* sp;

extern "C" void _OgreWorldExport dllStartPlugin(void);
extern "C" void _OgreWorldExport dllStopPlugin(void);

extern "C" _OgreWorldExport void dllStartPlugin()
{
    sp = new DefaultWorldsPlugin();
    Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreWorldExport void dllStopPlugin()
{
    Root::getSingleton().uninstallPlugin(sp);
    delete sp;
}
#endif
