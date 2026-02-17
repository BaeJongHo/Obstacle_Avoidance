# @UnrealClaude Script
# @Description: Create BP_OAMovingPlatform Blueprint from AOAMovingPlatform class
import unreal

parent_class = unreal.EditorAssetLibrary.load_asset("/Script/Obstacle_Avoidance.OAMovingPlatform")
if parent_class is None:
    # Try loading via find_class
    parent_class = unreal.find_class("OAMovingPlatform")

if parent_class is not None:
    unreal.log("Found parent class: {}".format(parent_class))
else:
    unreal.log_warning("Could not find OAMovingPlatform class")

# Create the Blueprint using KismetSystemLibrary or factory
factory = unreal.BlueprintFactory()
factory.set_editor_property("parent_class", parent_class)

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
bp = asset_tools.create_asset("BP_OAMovingPlatform", "/Game/Blueprints/Obstacle", unreal.Blueprint, factory)

if bp is not None:
    unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Obstacle/BP_OAMovingPlatform")
    unreal.log("Successfully created BP_OAMovingPlatform")
else:
    unreal.log_error("Failed to create BP_OAMovingPlatform")
