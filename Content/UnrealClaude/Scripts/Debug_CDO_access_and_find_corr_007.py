# @UnrealClaude Script
# @Description: Debug CDO access and find correct way to set properties

import unreal

bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
bp = unreal.load_asset(bp_path)

gen_class = bp.generated_class()
unreal.log(f"Generated class: {gen_class}")
unreal.log(f"Generated class type: {type(gen_class)}")

# CDO should be accessed differently
cdo = unreal.SystemLibrary.get_class_default_object(gen_class)
unreal.log(f"CDO: {cdo}")
unreal.log(f"CDO type: {type(cdo)}")
unreal.log(f"CDO class name: {cdo.get_class().get_name()}")

# Now check properties
for prop_name in ["DashAction", "SlideAction", "DashMontage", "SlideMontage", "MoveAction", "JumpAction"]:
    try:
        val = cdo.get_editor_property(prop_name)
        unreal.log(f"  FOUND: {prop_name} = {val}")
    except Exception as e:
        unreal.log_warning(f"  MISSING: {prop_name} - {e}")
