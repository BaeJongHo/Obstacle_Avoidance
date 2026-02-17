# @UnrealClaude Script
# @Description: Create BP_OAPendulumSwing using load_class and assign SM_Cylinder mesh

import unreal

# Try to find the class by loading it
parent_class = unreal.EditorAssetLibrary.load_blueprint_class("/Script/Obstacle_Avoidance.OAPendulumSwing")
if not parent_class:
    # Try loading via load_object
    parent_class = unreal.load_class(None, "/Script/Obstacle_Avoidance.OAPendulumSwing")

if not parent_class:
    unreal.log_warning("Could not load class directly, trying find_class")
    # List available classes to debug
    import sys
    # Try using the class path directly with the factory
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.BlueprintFactory()
    
    # Use load_object to get the UClass
    cls = unreal.load_object(None, "/Script/Obstacle_Avoidance.OAPendulumSwing")
    if cls:
        unreal.log(f"Found class via load_object: {cls}")
        factory.set_editor_property("parent_class", cls)
    else:
        unreal.log_error("Could not find OAPendulumSwing class at all")
        # Fallback: just use AActor as parent
        unreal.log("Falling back to AActor as parent class")
        factory.set_editor_property("parent_class", unreal.Actor)
    
    bp_asset = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)
else:
    unreal.log(f"Found parent class: {parent_class}")
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    bp_asset = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)

if bp_asset:
    unreal.log(f"BP_OAPendulumSwing created: {bp_asset.get_path_name()}")
    
    mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Cylinder")
    unreal.log(f"Mesh loaded: {mesh.get_name() if mesh else 'FAILED'}")
    
    scs = bp_asset.simple_construction_script()
    if scs:
        all_nodes = scs.get_all_nodes()
        unreal.log(f"SCS nodes: {len(all_nodes)}")
        for node in all_nodes:
            ct = node.component_template
            if ct:
                unreal.log(f"  {ct.get_name()} ({ct.get_class().get_name()})")
                if isinstance(ct, unreal.StaticMeshComponent):
                    ct.set_static_mesh(mesh)
                    unreal.log(f"  -> Assigned SM_Cylinder!")
    
    unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
    unreal.log("Saved BP_OAPendulumSwing")
else:
    unreal.log_error("Failed to create BP")
