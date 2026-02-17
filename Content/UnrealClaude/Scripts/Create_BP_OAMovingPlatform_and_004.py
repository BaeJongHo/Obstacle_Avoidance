# @UnrealClaude Script
# @Description: Create BP_OAMovingPlatform and assign SM_Plane mesh
import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_asset_lib = unreal.EditorAssetLibrary

bp_path = '/Game/Blueprints/Actor'
bp_name = 'BP_OAMovingPlatform'
full_path = f'{bp_path}/{bp_name}'

# Check if already exists
if editor_asset_lib.does_asset_exist(full_path):
    unreal.log(f'{full_path} already exists, loading it')
    bp_asset = unreal.load_asset(full_path)
else:
    # Find parent class
    parent_class = unreal.find_object(None, '/Script/Obstacle_Avoidance.OAMovingPlatform')
    if parent_class is None:
        unreal.log_error('AOAMovingPlatform not found!')
    else:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('parent_class', parent_class)
        bp_asset = asset_tools.create_asset(bp_name, bp_path, unreal.Blueprint, factory)
        if bp_asset:
            unreal.log(f'Created Blueprint: {full_path}')
        else:
            unreal.log_error('Failed to create Blueprint!')

# Assign SM_Plane mesh via SCS (SimpleConstructionScript)
if bp_asset:
    # Get the SCS nodes to find MeshComp
    scs = bp_asset.get_editor_property('simple_construction_script')
    if scs:
        nodes = scs.get_all_nodes()
        unreal.log(f'SCS nodes count: {len(nodes)}')
        for node in nodes:
            comp_template = node.get_editor_property('component_template')
            if comp_template:
                comp_name = comp_template.get_name()
                comp_class = comp_template.get_class().get_name()
                unreal.log(f'  Node: {comp_name} ({comp_class})')
                if comp_class == 'StaticMeshComponent':
                    mesh = unreal.load_asset('/Game/LevelPrototyping/Meshes/SM_Plane')
                    if mesh:
                        comp_template.set_editor_property('static_mesh', mesh)
                        unreal.log(f'Assigned SM_Plane to {comp_name}')
                    else:
                        unreal.log_error('SM_Plane not found!')

    # Save
    editor_asset_lib.save_asset(full_path)
    unreal.log(f'Saved {full_path}')
