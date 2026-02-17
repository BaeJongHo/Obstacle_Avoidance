# @UnrealClaude Script
# @Description: Create BP_OAPendulumSwing and assign SM_Cylinder mesh

import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory = unreal.BlueprintFactory()
factory.set_editor_property("parent_class", unreal.OAPendulumSwing)

bp_asset = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)

if bp_asset:
    unreal.log("BP_OAPendulumSwing created successfully")
    
    # Load SM_Cylinder mesh
    mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Cylinder")
    
    if not mesh:
        unreal.log_error("Failed to load SM_Cylinder mesh")
    else:
        unreal.log(f"Mesh loaded: {mesh.get_name()}")
    
    # Try through SimpleConstructionScript (SCS) for component defaults
    scs = bp_asset.simple_construction_script()
    if scs:
        all_nodes = scs.get_all_nodes()
        unreal.log(f"SCS nodes count: {len(all_nodes)}")
        for node in all_nodes:
            comp_template = node.component_template
            comp_name = comp_template.get_name() if comp_template else "None"
            comp_class = comp_template.get_class().get_name() if comp_template else "None"
            unreal.log(f"  Node: {node.get_name()}, Component: {comp_name}, Class: {comp_class}")
            if comp_template and isinstance(comp_template, unreal.StaticMeshComponent):
                comp_template.set_static_mesh(mesh)
                unreal.log(f"Set SM_Cylinder on SCS component: {comp_name}")
    
    # Compile the blueprint
    unreal.KismetSystemLibrary.flush_persistent_debug_lines(None)
    
    # Save the asset
    unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
    unreal.log("BP_OAPendulumSwing saved successfully")
else:
    unreal.log_error("Failed to create BP_OAPendulumSwing")
