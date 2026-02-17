# @UnrealClaude Script
# @Description: Recompile BP_ThirdPersonCharacter and assign DashAction, SlideAction, DashMontage, SlideMontage

import unreal

bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"

# Load the blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("Cannot load blueprint")

# Force recompile the blueprint
unreal.KismetEditorUtilities.compile_blueprint(bp)
unreal.log("Blueprint recompiled!")

# Get generated class and CDO
gen_class = bp.generated_class()
cdo = gen_class.get_default_object()

# List available properties to verify
props = ["DashAction", "SlideAction", "DashMontage", "SlideMontage", "MoveAction", "JumpAction", "LookAction"]
for p in props:
    try:
        val = cdo.get_editor_property(p)
        unreal.log(f"  {p} = {val}")
    except Exception as e:
        unreal.log_error(f"  {p} NOT FOUND: {e}")
