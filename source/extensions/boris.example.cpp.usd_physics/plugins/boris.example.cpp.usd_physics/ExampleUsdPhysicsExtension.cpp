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

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdPhysics/collisionAPI.h>
#include <pxr/usd/usdPhysics/massAPI.h>
#include <pxr/usd/usdPhysics/rigidBodyAPI.h>
#include <pxr/usd/usdPhysics/scene.h>
#include <pxr/usd/usdUtils/stageCache.h>

const struct carb::PluginImplDesc pluginImplDesc = { "boris.example.cpp.usd_physics.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace boris
{
namespace example
{
namespace cpp
{
namespace usd_physics
{

class ExampleUsdPhysicsExtension : public omni::ext::IExt
{
protected:
    void onStartup(const char* extId) override
    {
        // Get the 'active' USD stage from the USD stage cache.
        std::vector<PXR_NS::UsdStageRefPtr> allStages = PXR_NS::UsdUtilsStageCache::Get().GetAllStages();
        if (allStages.size() == 0)
        {
            CARB_LOG_WARN("No stage found, creating a new one in Memory.");
            // PXR_NS::UsdStageRefPtr activeStage = PXR_NS::UsdStage::CreateNew("World.usda");
            PXR_NS::UsdStageRefPtr activeStage = PXR_NS::UsdStage::CreateInMemory();
            allStages.push_back(activeStage);
        }

        if (allStages.size() != 1)
        {
            CARB_LOG_WARN("Cannot determine the 'active' USD stage (%zu stages present in the USD stage cache).", allStages.size());
            return;
        }

        // Get the meters per unit and up axis of the 'active' USD stage.
        // Meters per unit is the unit of measurement used for length in the stage.
        // For example, if the stage is in centimeters, meters per unit will be 0.01.
        // Instead, if the stage is in feet, meters per unit will be 0.3048.
        PXR_NS::UsdStageRefPtr activeStage = allStages[0];
        const double activeStageMetersPerUnit = PXR_NS::UsdGeomGetStageMetersPerUnit(activeStage);

        // The up axis of the stage must be Y.
        if (PXR_NS::UsdGeomGetStageUpAxis(activeStage) != PXR_NS::UsdGeomTokens->y)
        {
            // Handling this is possible, but it would complicate the example,
            // that is only designed to work in an empty stage with Y-axis up.
            CARB_LOG_WARN("The up axis of the 'active' USD stage is not Y.");
            return;
        }

        // Create and setup the USD physics scene.
        static const PXR_NS::SdfPath kPhysicsScenePath("/World/PhysicsScene");
        if (!activeStage->GetPrimAtPath(kPhysicsScenePath))
        {
            static constexpr PXR_NS::GfVec3f kGravityDirection = { 0.0f, -1.0f, 0.0f };
            static constexpr float kGravityMagnitude = 981.0f;

            PXR_NS::UsdPhysicsScene physicsScene = PXR_NS::UsdPhysicsScene::Define(activeStage, kPhysicsScenePath);
            physicsScene.CreateGravityDirectionAttr().Set(kGravityDirection);
            physicsScene.CreateGravityMagnitudeAttr().Set(kGravityMagnitude);
        }

        // Create and setup a static ground plane (box for now, should use UsdGeomPlane instead).
        static const PXR_NS::SdfPath kGroundPlanePath("/World/StaticGroundPlane");
        if (!activeStage->GetPrimAtPath(kGroundPlanePath))
        {
            const double kSize = 5.0 / activeStageMetersPerUnit;
            const PXR_NS::GfVec3f kColour = { 1.0f, 1.0f, 1.0f };       // White.
            const PXR_NS::GfVec3f kPosition = { 0.0f, -(float)kSize * 0.5f, 0.0f };

            // Create a cube.
            PXR_NS::UsdGeomCube geomPrim = PXR_NS::UsdGeomCube::Define(activeStage, kGroundPlanePath);

            // Set the size, position and colour of the cube.
            geomPrim.CreateSizeAttr().Set(kSize);
            geomPrim.AddTranslateOp(PXR_NS::UsdGeomXformOp::PrecisionFloat).Set(kPosition);
            geomPrim.CreateDisplayColorAttr().Set(PXR_NS::VtArray<PXR_NS::GfVec3f>({ kColour }));

            // Enable collision
            PXR_NS::UsdPhysicsCollisionAPI::Apply(geomPrim.GetPrim());
        }

        // Create and setup a rigid body box.
        const PXR_NS::SdfPath kRigidBodyPath("/World/RigidBodyBox");
        if (!activeStage->GetPrimAtPath(kRigidBodyPath))
        {
            const double kSize = 0.5 / activeStageMetersPerUnit;
            const PXR_NS::GfVec3f kColour = { 0.0f, 0.0f, 1.0f };    // Blue.
            const PXR_NS::GfVec3f kPosition = { 0.0f, (float)kSize * 5.0f, 0.0f };

            // Create a cube.
            PXR_NS::UsdGeomCube geomPrim = PXR_NS::UsdGeomCube::Define(activeStage, kRigidBodyPath);

            // Set the size, position and colour of the cube.
            geomPrim.CreateSizeAttr().Set(kSize);
            geomPrim.AddTranslateOp(PXR_NS::UsdGeomXformOp::PrecisionFloat).Set(kPosition);
            geomPrim.CreateDisplayColorAttr().Set(PXR_NS::VtArray<PXR_NS::GfVec3f>({ kColour }));

            // Enable collision, mass and rigid body.
            PXR_NS::UsdPhysicsCollisionAPI::Apply(geomPrim.GetPrim());
            PXR_NS::UsdPhysicsMassAPI::Apply(geomPrim.GetPrim());
            auto rigidBodyAPI = PXR_NS::UsdPhysicsRigidBodyAPI::Apply(geomPrim.GetPrim());

            // Set the linear and angular velocity of the rigid body.
            static constexpr PXR_NS::GfVec3f kVelocity = { 2.0f, 1.0f, 2.0f };
            static constexpr PXR_NS::GfVec3f kAngularVelocity = { 180.0f, 0.0f, 0.0f };
            rigidBodyAPI.CreateVelocityAttr().Set(kVelocity);
            rigidBodyAPI.CreateAngularVelocityAttr().Set(kAngularVelocity);
        }
    }

    void onShutdown() override
    {
    }
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, boris::example::cpp::usd_physics::ExampleUsdPhysicsExtension)

void fillInterface(boris::example::cpp::usd_physics::ExampleUsdPhysicsExtension& iface)
{
}
