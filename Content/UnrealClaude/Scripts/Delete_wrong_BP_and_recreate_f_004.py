# @UnrealClaude Script
# @Description: Delete wrong BP and recreate from correct C++ class with mesh

import unreal

# Delete the incorrectly created BP
if unreal.EditorAssetLibrary.does_asset_exist("/Game/Blueprints/Actor/BP_OAPendulumSwing"):
    unreal.EditorAssetLibrary.delete_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
    unreal.log("Deleted incorrect BP_OAPendulumSwing")

# Find the OAPendulumSwing class by iterating registered classes
import unreal

# Use find_object to locate the UClass
cls = unreal.find_object(None, "/Script/Obstacle_Avoidance.OAPendulumSwing")
unreal.log(f"find_object result: {cls}")

if not cls:
    # Try with A prefix stripped - the module name
    cls = unreal.find_object(None, "/Script/Obstacle_Avoidance.AOAPendulumSwing")
    unreal.log(f"find_object with A prefix: {cls}")
