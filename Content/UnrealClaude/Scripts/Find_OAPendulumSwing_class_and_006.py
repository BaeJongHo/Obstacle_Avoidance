# @UnrealClaude Script
# @Description: Find OAPendulumSwing via class iterator and create BP with mesh

import unreal

# Method: spawn a temp actor to get its class, then use that to create BP
# First, let's try spawning via the editor
editor_sys = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_sys.get_editor_world()

# Try to spawn an instance to get the class reference
actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# Use spawn_actor_from_class with a string class path
# The class path in UE5 for native classes: /Script/ModuleName.ClassName
spawn_result = actor_subsys.spawn_actor_from_class(
    unreal.EditorAssetLibrary.load_asset("/Script/Obstacle_Avoidance.OAPendulumSwing"),
    unreal.Vector(0, 0, 500)
)
unreal.log(f"Spawn result: {spawn_result}")

if spawn_result:
    # Get the class from the spawned actor
    actor_class = spawn_result.get_class()
    unreal.log(f"Actor class: {actor_class.get_name()}, path: {actor_class.get_path_name()}")
    
    # Now create BP from this class
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", actor_class)
    
    bp_asset = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", None, factory)
    
    if bp_asset:
        unreal.log(f"BP created: {bp_asset.get_path_name()}")
        
        # Now the BP inherits from OAPendulumSwing which has SM_Cylinder in constructor
        # The SCS should have the mesh component with the default mesh
        scs = bp_asset.simple_construction_script()
        if scs:
            for node in scs.get_all_nodes():
                ct = node.component_template
                if ct:
                    unreal.log(f"  Component: {ct.get_name()} ({ct.get_class().get_name()})")
                    if isinstance(ct, unreal.StaticMeshComponent):
                        sm = ct.get_editor_property("static_mesh")
                        unreal.log(f"    Current mesh: {sm.get_name() if sm else 'None'}")
                        if not sm:
                            mesh = unreal.load_asset("/Game/LevelPrototyping/Meshes/SM_Cylinder")
                            ct.set_static_mesh(mesh)
                            unreal.log(f"    Assigned SM_Cylinder")
        
        unreal.EditorAssetLibrary.save_asset("/Game/Blueprints/Actor/BP_OAPendulumSwing")
        unreal.log("BP saved")
    else:
        unreal.log_error("Failed to create BP")
    
    # Delete the temp actor
    actor_subsys.destroy_actor(spawn_result)
    unreal.log("Temp actor deleted")
else:
    unreal.log_error("Could not spawn OAPendulumSwing actor")
