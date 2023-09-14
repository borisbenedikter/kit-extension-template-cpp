// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#define CARB_EXPORTS

#include <carb/PluginUtils.h>

#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include "snippetcommon/SnippetPrint.h"
#include "snippetcommon/SnippetPVD.h"
#include "snippetutils/SnippetUtils.h"

using namespace physx;

const struct carb::PluginImplDesc pluginImplDesc = {
	"boris.example.cpp.hello_world_physX.plugin",
	"An example C++ extension.", "NVIDIA",
	carb::PluginHotReload::eEnabled, "dev"};

CARB_PLUGIN_IMPL_DEPS(omni::kit::IApp)

// -------------------------------------------------------------------------- //
// ------------------------------- Callbacks -------------------------------- //
// -------------------------------------------------------------------------- //

// Usually, the allocator callback and error callback are specific to the application,
// but PhysX provides default implementations that make it easy to get started.

// PhysX performs all allocations via the PxAllocatorCallback interface.
// You must implement this interface in order to initialize PhysX:
static PxDefaultAllocator		gAllocator;

// PhysX logs all error messages through the PxErrorCallback interface.
// You must implement this interface in order to initialize PhysX:
static PxDefaultErrorCallback	gErrorCallback;

static PxFoundation*			gFoundation = NULL;
static PxPhysics*				gPhysics	= NULL;
static PxDefaultCpuDispatcher*	gDispatcher = NULL;
static PxScene*					gScene		= NULL;
static PxMaterial*				gMaterial	= NULL;
static PxPvd*					gPvd        = NULL;

static PxReal stackZ = 10.0f;

static PxRigidDynamic *createDynamic(const PxTransform &t,
									 const PxGeometry &geometry,
									 const PxVec3 &velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for(PxU32 i=0; i<size;i++)
	{
		for(PxU32 j=0;j<size-i;j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

void initPhysics(bool interactive)
{
	// Every PhysX module requires a PxFoundation instance to be available
	// Inputs are:
	// - SDK version we are using
	// - Allocator callback
	// - Error callback
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	// The recordMemoryAllocations parameter specifies whether to perform memory profiling.
	bool recordMemoryAllocations = true;

	// Create the PhysX Visual Debugger (PVD)
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	// Create the (top-level?) PxPhysics object
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), recordMemoryAllocations, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);

	for(PxU32 i=0;i<5;i++)
		createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);

	if(!interactive)
		createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}

void stepPhysics(bool /*interactive*/)
{
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}

void cleanupPhysics(bool /*interactive*/)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetHelloWorld done.\n");
}

void keyPress(unsigned char key, const PxTransform& camera)
{
	switch(toupper(key))
	{
	case 'B':	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);						break;
	case ' ':	createDynamic(camera, PxSphereGeometry(3.0f), camera.rotate(PxVec3(0,0,-1))*200);	break;
	}
}

int snippetMain(int, const char*const*)
{
#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}

namespace boris
{
namespace example
{
namespace cpp
{
namespace hello_world_physX
{

// When this extension is enabled, any class that derives from omni.ext.IExt
// will be instantiated and 'onStartup(extId)' called. When the extension is
// later disabled, a matching 'onShutdown()' call will be made on the object.
class ExampleCppHelloWorldExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        printf("ExampleCppHelloWorldExtension starting up (ext_id: %s).\n", extId);

        // Get the app interface from the Carbonite Framework.
        if (omni::kit::IApp* app = carb::getFramework()->acquireInterface<omni::kit::IApp>())
        {
            // Subscribe to update events.
            m_updateEventsSubscription =
                carb::events::createSubscriptionToPop(app->getUpdateEventStream(), [this](carb::events::IEvent*) {
                onUpdate();
            });
        }

        // PhysX snippet code main
        snippetMain(0, NULL);
    }

    void onShutdown() override
    {
        printf("ExampleCppHelloWorldExtension shutting down.\n");

        // Unsubscribe from update events.
        m_updateEventsSubscription = nullptr;
    }

    void onUpdate()
    {
        if (m_updateCounter % 1000 == 0)
        {
            printf("Hello from the boris.example.cpp.hello_world_physX extension! %d updates counted.\n", m_updateCounter);
        }
        m_updateCounter++;
    }

private:
    carb::events::ISubscriptionPtr m_updateEventsSubscription;
    int m_updateCounter = 0;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, boris::example::cpp::hello_world_physX::ExampleCppHelloWorldExtension)

void fillInterface(boris::example::cpp::hello_world_physX::ExampleCppHelloWorldExtension& iface)
{
}
