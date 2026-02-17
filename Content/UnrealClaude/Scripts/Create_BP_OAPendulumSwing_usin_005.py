# @UnrealClaude Script
# @Description: Create BP_OAPendulumSwing using KismetEditorUtilities

import unreal

# Method: Use subsystem to create BP from native class
# First, try to get class from generated types
all_classes = unreal.EditorUtilityLibrary.get_selection_set() if hasattr(unreal, 'EditorUtilityLibrary') else []

# Try using the class registry
registry = unreal.AssetRegistryHelpers.get_asset_registry()

# Search for the class by scanning loaded classes
# The native class should be available since it compiled
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory = unreal.BlueprintFactory()

# Try setting parent class via string path
# In UE5, native C++ classes are at /Script/ModuleName.ClassName
# The 'A' prefix is stripped for Actor classes in the type system
# So AOAPendulumSwing becomes OAPendulumSwing in the reflection system

# Let's check what's actually available
result = unreal.SystemLibrary.get_class_from_primary_asset_id(unreal.PrimaryAssetId())
unreal.log(f"Available: {dir(unreal)[:20]}")

# Try the subsystem approach
subsystem = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
unreal.log(f"Subsystem: {subsystem}")

# Let's try to find what unreal classes are available that contain 'Pendulum'
matching = [x for x in dir(unreal) if 'Pendulum' in x or 'pendulum' in x.lower()]
unreal.log(f"Pendulum classes: {matching}")

# Also check for OA prefix classes
oa_classes = [x for x in dir(unreal) if x.startswith('OA')]
unreal.log(f"OA classes: {oa_classes}")
