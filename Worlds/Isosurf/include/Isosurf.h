#ifndef __World_Isosurf_H__
#define __World_Isosurf_H__

#include "SdkWorld.h"
#include "WorldPlugin.h"
#include "ProceduralTools.h"

namespace OgreBites {
using namespace Ogre;

class _OgreWorldClassExport World_Isosurf : public SdkWorld
{
    Entity* tetrahedra;
    MeshPtr mTetrahedraMesh;

 public:
    World_Isosurf();
    void testCapabilities(const RenderSystemCapabilities* caps) override;
    void setupContent(void) override;
    void cleanupContent() override;
    bool frameRenderingQueued(const FrameEvent& evt) override;
};
}

#endif
