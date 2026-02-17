# @UnrealClaude Script
# @Description: Assign SM_Plane mesh to BP_OAMovingPlatform via CDO
import unreal

bp_path = '/Game/Blueprints/Actor/BP_OAMovingPlatform'
bp_asset = unreal.load_asset(bp_path)

if bp_asset is None:
    unreal.log_error('BP_OAMovingPlatform not found!')
else:
    unreal.log(f'BP loaded: {bp_asset.get_name()}')
    
    # Get generated class and CDO
    gen_class = bp_asset.get_editor_property('generated_class')
    cdo = unreal.get_default_object(gen_class)
    unreal.log(f'CDO: {cdo.get_name()}')
    
    # List all properties to find mesh component
    for prop in dir(cdo):
        if 'mesh' in prop.lower() or 'comp' in prop.lower():
            unreal.log(f'  Property: {prop}')
    
    # Try to access the mesh component
    try:
        mesh_comp = cdo.get_editor_property('mesh_comp')
        unreal.log(f'Found mesh_comp: {mesh_comp}')
    except:
        unreal.log_warning('mesh_comp not accessible directly, trying components')
    
    # Try getting all components
    comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
    unreal.log(f'StaticMeshComponent count: {len(comps)}')
    for c in comps:
        unreal.log(f'  Comp: {c.get_name()} class={c.get_class().get_name()}')
        mesh = unreal.load_asset('/Game/LevelPrototyping/Meshes/SM_Plane')
        if mesh:
            c.set_editor_property('static_mesh', mesh)
            unreal.log(f'  Assigned SM_Plane to {c.get_name()}')
    
    # Use subsystem to modify and save
    subsys = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    
    # Save
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log('Saved BP_OAMovingPlatform')
