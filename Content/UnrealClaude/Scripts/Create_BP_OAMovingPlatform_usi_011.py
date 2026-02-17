# @UnrealClaude Script
# @Description: Create BP_OAMovingPlatform using load_class
import unreal

# Load the native C++ class
parent_class = unreal.load_class(None, "/Script/Obstacle_Avoidance.OAMovingPlatform")
if parent_class is None:
    unreal.log_error("Could not load OAMovingPlatform class")
else:
    unreal.log("Found class: {}".format(parent_class))
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    bp = asset_tools.create_asset("BP_OAMovingPlatform", "/Game/Blueprints/Obstacle", unreal.Blueprint, factory)
    
    if bp is not None:
        unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Obstacle/BP_OAMovingPlatform")
        unreal.log("Successfully created BP_OAMovingPlatform at /Game/Blueprints/Obstacle/")
    else:
        unreal.log_error("Failed to create Blueprint")
