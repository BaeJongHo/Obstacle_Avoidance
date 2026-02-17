# @UnrealClaude Script
# @Description: Set SM_Obstacle_Light_Post mesh and 180 rotation on BP_OAPendulumSwing

import unreal

bp_path = "/Game/Blueprints/Actor/BP_OAPendulumSwing"
mesh_path = "/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Light_Post"

# Load BP and mesh
bp = unreal.load_asset(bp_path)
mesh = unreal.load_asset(mesh_path)

if bp is None:
    unreal.log_error("BP not found: " + bp_path)
elif mesh is None:
    unreal.log_error("Mesh not found: " + mesh_path)
else:
    # Get the CDO (Class Default Object)
    cdo = unreal.get_default_object(bp.generated_class())
    
    # Find StaticMeshComponent named MeshComp
    components = cdo.get_components_by_class(unreal.StaticMeshComponent)
    
    if len(components) == 0:
        unreal.log_warning("No StaticMeshComponent found, trying to access by property")
        # Try direct property access
        mesh_comp = cdo.get_editor_property("MeshComp")
        if mesh_comp:
            mesh_comp.set_editor_property("static_mesh", mesh)
            mesh_comp.set_editor_property("relative_rotation", unreal.Rotator(pitch=180.0, yaw=0.0, roll=0.0))
            unreal.log("Set mesh and rotation via property access")
        else:
            unreal.log_error("Could not find MeshComp property")
    else:
        for comp in components:
            comp_name = comp.get_name()
            unreal.log("Found component: " + comp_name)
            if "Mesh" in comp_name:
                comp.set_editor_property("static_mesh", mesh)
                comp.set_editor_property("relative_rotation", unreal.Rotator(pitch=180.0, yaw=0.0, roll=0.0))
                unreal.log("Set mesh to SM_Obstacle_Light_Post and rotation to 180 on " + comp_name)
    
    # Save the BP
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log("BP saved successfully")
