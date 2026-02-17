# @UnrealClaude Script
# @Description: Use find_object for CDO, compile blueprint, set dash/slide properties

import unreal

bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
bp = unreal.load_asset(bp_path)

gen_class = bp.generated_class()

# Use find_object to get CDO - the CDO path is ClassName_C:Default__ClassName_C
cdo_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"
cdo = unreal.find_object(None, cdo_path + ":Default__BP_ThirdPersonCharacter_C")
if cdo is None:
    # Try another way - iterate world actors or use find
    cdo_name = f"Default__{gen_class.get_name()}"
    unreal.log(f"CDO name would be: {cdo_name}")
    # Try using the class directly
    cdo = gen_class.get_default_object()
    unreal.log(f"get_default_object returned: {cdo}, type: {type(cdo)}, class: {cdo.get_class().get_name()}")
else:
    unreal.log(f"Found CDO via find_object: {cdo}")

# The real issue: maybe the C++ class wasn't hot-reloaded properly
# Let's check the native parent class CDO
native_class_path = "/Script/Obstacle_Avoidance.Obstacle_AvoidanceCharacter"
native_class = unreal.find_object(None, native_class_path)
unreal.log(f"Native C++ class: {native_class}")

if native_class:
    native_cdo = native_class.get_default_object()
    unreal.log(f"Native CDO: {native_cdo}, type: {type(native_cdo)}")
    for prop_name in ["DashAction", "SlideAction", "DashMontage", "SlideMontage", "MoveAction"]:
        try:
            val = native_cdo.get_editor_property(prop_name)
            unreal.log(f"  Native CDO {prop_name} = {val}")
        except Exception as e:
            unreal.log_warning(f"  Native CDO {prop_name} MISSING: {e}")
