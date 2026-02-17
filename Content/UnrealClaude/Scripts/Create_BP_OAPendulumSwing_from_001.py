/** @UnrealClaude Script
 * @Description: Create BP_OAPendulumSwing from AOAPendulumSwing and assign SM_Cylinder mesh
 */
import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory = unreal.BlueprintFactory()
factory.set_editor_property("parent_class", unreal.OAPendulumSwing)

bp_asset = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)

if bp_asset:
    unreal.log("BP_OAPendulumSwing created successfully")
    
    # Get the default object to set the mesh
    gen_class = bp_asset.generated_class()
    cdo = unreal.get_default_object(gen_class)
    
    # Load SM_Cylinder mesh
    mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Cylinder")
    
    if cdo and mesh:
        # Access MeshComp on the CDO
        mesh_comp = cdo.get_editor_property("MeshComp")
        if mesh_comp:
            mesh_comp.set_static_mesh(mesh)
            unreal.log("SM_Cylinder mesh assigned to MeshComp")
        else:
            unreal.log_warning("MeshComp not found on CDO, trying via SCS")
    
    # Also try through SimpleConstructionScript (SCS) for component defaults
    scs = bp_asset.simple_construction_script()
    if scs:
        all_nodes = scs.get_all_nodes()
        unreal.log(f"SCS nodes count: {len(all_nodes)}")
        for node in all_nodes:
            comp_template = node.component_template
            unreal.log(f"  Node: {node.get_name()}, Component: {comp_template.get_name() if comp_template else 'None'}, Class: {comp_template.get_class().get_name() if comp_template else 'None'}")
            if comp_template and isinstance(comp_template, unreal.StaticMeshComponent):
                comp_template.set_static_mesh(mesh)
                unreal.log(f"Set SM_Cylinder on SCS component: {comp_template.get_name()}")
    
    # Save the asset
    unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
    unreal.log("BP_OAPendulumSwing saved")
else:
    unreal.log_error("Failed to create BP_OAPendulumSwing")
