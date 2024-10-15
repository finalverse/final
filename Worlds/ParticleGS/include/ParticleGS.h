#ifndef __World_ParticleGS_H__
#define __World_ParticleGS_H__


#include "ProceduralManualObject.h"
#include "OgreRenderToVertexBuffer.h"
#include "RandomTools.h"
#include "WorldPlugin.h"
#include "SdkWorld.h"

// #define LOG_GENERATED_BUFFER
namespace OgreBites {
using namespace Ogre;

class _OgreWorldClassExport World_ParticleGS : public SdkWorld
{
 public:
    World_ParticleGS();

 protected:

    void createProceduralParticleSystem();
    void testCapabilities(const RenderSystemCapabilities* caps) override;
    void setupContent(void) override;
    void cleanupContent() override;
#ifdef LOG_GENERATED_BUFFER
    bool frameEnded(const FrameEvent& evt);
#endif
    ProceduralManualObject* mParticleSystem;
};
}
#endif
