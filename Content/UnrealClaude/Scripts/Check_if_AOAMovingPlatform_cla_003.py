# @UnrealClaude Script
# @Description: Check if AOAMovingPlatform class is available
import unreal

# Try to find the class
cls = unreal.find_object(None, '/Script/Obstacle_Avoidance.OAMovingPlatform')
if cls is not None:
    unreal.log(f'Found AOAMovingPlatform class: {cls}')
else:
    unreal.log_warning('AOAMovingPlatform class NOT found. Need editor restart or full rebuild.')
    # List all classes in the module to see what's available
    all_classes = unreal.EditorAssetLibrary.list_assets('/Script/Obstacle_Avoidance/', recursive=False)
    unreal.log(f'Available classes: {all_classes}')
