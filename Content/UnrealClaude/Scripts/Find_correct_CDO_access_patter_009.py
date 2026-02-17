# @UnrealClaude Script
# @Description: Find correct CDO via load_object and assign dash/slide properties

import unreal

# Try to directly load the CDO object
cdo_full_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.Default__BP_ThirdPersonCharacter_C"
cdo = unreal.load_object(None, cdo_full_path)
unreal.log(f"CDO via load_object: {cdo}, type: {type(cdo)}")

if cdo is None:
    # Try the native C++ CDO
    native_cdo_path = "/Script/Obstacle_Avoidance.Default__Obstacle_AvoidanceCharacter"
    cdo = unreal.load_object(None, native_cdo_path)
    unreal.log(f"Native CDO via load_object: {cdo}, type: {type(cdo)}")

if cdo:
    unreal.log(f"CDO class: {cdo.get_class().get_name()}")
    for prop_name in ["DashAction", "SlideAction", "DashMontage", "SlideMontage", "MoveAction", "JumpAction"]:
        try:
            val = cdo.get_editor_property(prop_name)
            unreal.log(f"  {prop_name} = {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop_name} NOT FOUND")
else:
    unreal.log_error("Cannot find CDO at all")
    # Last resort - try to find any actor in level
    actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsys.get_all_level_actors()
    for a in actors:
        unreal.log(f"  Level actor: {a.get_name()} ({a.get_class().get_name()})")
