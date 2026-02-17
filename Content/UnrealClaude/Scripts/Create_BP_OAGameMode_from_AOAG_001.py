/** @UnrealClaude Script
 * @Description: Create BP_OAGameMode from AOAGameMode C++ class
 */
import unreal

# Find the AOAGameMode class
game_mode_class = unreal.EditorAssetLibrary.load_asset('/Script/Obstacle_Avoidance.OAGameMode')
if game_mode_class is None:
    # Try loading by class directly
    game_mode_class = unreal.load_class(None, '/Script/Obstacle_Avoidance.OAGameMode')

if game_mode_class is not None:
    print(f"Found AOAGameMode class: {game_mode_class}")
else:
    print("AOAGameMode class not found - checking available classes...")
    # List all game mode classes
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    # Try to find it via reflection
    obj = unreal.find_object(None, '/Script/Obstacle_Avoidance.OAGameMode')
    if obj:
        print(f"Found via find_object: {obj}")
    else:
        print("NOT FOUND via find_object either")
        # List classes in the module
        default_obj = unreal.find_object(None, '/Script/Obstacle_Avoidance.Default__OAGameMode')
        if default_obj:
            print(f"Found CDO: {default_obj}")
        else:
            print("CDO also not found - class may not be compiled")
