# @UnrealClaude Script
# @Description: Set SM_Cylinder mesh on existing TestPendulum actor

import unreal

# Find the spawned actor
actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
all_actors = actor_subsys.get_all_level_actors()

pendulum = None
for a in all_actors:
    if a.get_actor_label() == "OAPendulumSwing" or "TestPendulum" in a.get_name():
        pendulum = a
        unreal.log(f"Found: {a.get_name()} / {a.get_actor_label()} / {a.get_class().get_name()}")
        break

if pendulum:
    # Get all components
    comps = pendulum.get_components_by_class(unreal.StaticMeshComponent)
    unreal.log(f"StaticMeshComponent count: {len(comps)}")
    
    mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Cylinder")
    unreal.log(f"Mesh: {mesh.get_name() if mesh else 'None'}")
    
    for comp in comps:
        current = comp.static_mesh
        unreal.log(f"  Comp: {comp.get_name()}, Current mesh: {current.get_name() if current else 'None'}")
        comp.set_static_mesh(mesh)
        unreal.log(f"  -> Set SM_Cylinder on {comp.get_name()}")
    
    # Now also get the class for creating the BP
    actor_class = pendulum.get_class()
    unreal.log(f"Actor class: {actor_class.get_name()}, Path: {actor_class.get_path_name()}")
    
    # Try to create BP from this class
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", actor_class)
    
    if not unreal.EditorAssetLibrary.does_asset_exist("/Game/Blueprints/Actor/BP_OAPendulumSwing"):
        bp = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)
        if bp:
            unreal.log(f"BP created: {bp.get_path_name()}")
            # Save
            unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
            unreal.log("BP saved")
        else:
            unreal.log_error("BP creation failed")
    else:
        unreal.log("BP already exists")
else:
    unreal.log_error("Pendulum actor not found")
