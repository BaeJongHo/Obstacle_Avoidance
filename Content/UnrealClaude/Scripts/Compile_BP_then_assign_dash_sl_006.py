# @UnrealClaude Script
# @Description: Compile BP_ThirdPersonCharacter then assign dash/slide properties

import unreal

bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
bp = unreal.load_asset(bp_path)

# Try to compile via EditorSubsystem or direct method
try:
    # Try compile_blueprint on the asset directly
    bp.compile()
    unreal.log("Compiled via bp.compile()")
except:
    pass

# Try using the subsystem
try:
    subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    unreal.log(f"Subsystem: {subsystem}")
except:
    pass

# Try FKismetEditorUtilities via EditorScripting
try:
    unreal.EditorAssetLibrary.checkout_asset(bp_path)
    unreal.log("Checked out asset")
except:
    pass

# Reload the blueprint to force refresh
bp2 = unreal.load_asset(bp_path)
gen_class = bp2.generated_class()
cdo = gen_class.get_default_object()

# Check what properties exist
unreal.log(f"CDO class: {cdo.get_class().get_name()}")
unreal.log(f"CDO parent class: {cdo.get_class().get_super_class().get_name()}")

# Try listing all properties
for prop_name in ["DashAction", "SlideAction", "DashMontage", "SlideMontage", "MoveAction", "JumpAction"]:
    try:
        val = cdo.get_editor_property(prop_name)
        unreal.log(f"  FOUND: {prop_name} = {val}")
    except:
        unreal.log_warning(f"  MISSING: {prop_name}")

# Check parent class CDO
parent_class = cdo.get_class().get_super_class()
parent_cdo = parent_class.get_default_object()
unreal.log(f"Parent CDO class: {parent_cdo.get_class().get_name()}")
for prop_name in ["DashAction", "SlideAction", "DashMontage", "SlideMontage"]:
    try:
        val = parent_cdo.get_editor_property(prop_name)
        unreal.log(f"  Parent FOUND: {prop_name} = {val}")
    except:
        unreal.log_warning(f"  Parent MISSING: {prop_name}")
