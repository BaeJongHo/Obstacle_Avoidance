/** @UnrealClaude Script
 * @Description: Create BP_OAMovingPlatform from AOAMovingPlatform and assign SM_Plane mesh
 */
import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_asset_lib = unreal.EditorAssetLibrary

# Blueprint path
bp_path = '/Game/Blueprints/Actor'
bp_name = 'BP_OAMovingPlatform'
full_path = f'{bp_path}/{bp_name}'

# Check if it already exists
if editor_asset_lib.does_asset_exist(full_path):
    unreal.log_warning(f'{full_path} already exists, skipping creation')
else:
    # Find the C++ parent class
    parent_class = unreal.EditorAssetLibrary.find_class_by_name('OAMovingPlatform')
    if parent_class is None:
        # Try loading by path
        parent_class = unreal.load_object(name='/Script/Obstacle_Avoidance.OAMovingPlatform', outer=None)
    
    if parent_class is None:
        unreal.log_error('AOAMovingPlatform class not found! LiveCoding may not have compiled yet. Try restarting the editor.')
    else:
        # Create the Blueprint
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('parent_class', parent_class)
        
        bp_asset = asset_tools.create_asset(bp_name, bp_path, unreal.Blueprint, factory)
        
        if bp_asset is not None:
            unreal.log(f'Created Blueprint: {full_path}')
            
            # Get the CDO (Class Default Object) to set mesh
            gen_class = bp_asset.get_editor_property('generated_class')
            cdo = unreal.get_default_object(gen_class)
            
            # Load SM_Plane mesh
            mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Plane.SM_Plane")
            if mesh is not None:
                # Find the MeshComp on the CDO
                mesh_comp = cdo.get_editor_property('mesh_comp')
                if mesh_comp is not None:
                    mesh_comp.set_editor_property('static_mesh', mesh)
                    unreal.log('Assigned SM_Plane to MeshComp')
                else:
                    unreal.log_warning('MeshComp not found on CDO, trying root component')
                    root = cdo.get_editor_property('root_component')
                    if root is not None:
                        root.set_editor_property('static_mesh', mesh)
                        unreal.log('Assigned SM_Plane to root component')
            else:
                unreal.log_error('SM_Plane mesh not found!')
            
            # Save the asset
            editor_asset_lib.save_asset(full_path)
            unreal.log(f'Saved {full_path}')
        else:
            unreal.log_error('Failed to create Blueprint!')
