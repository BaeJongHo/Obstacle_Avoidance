/** @UnrealClaude Script
 * @Description: Assign DashAction, SlideAction, DashMontage, SlideMontage to BP_ThirdPersonCharacter
 */

import unreal

bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"

# Load the blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("Cannot load blueprint")

# Get generated class and CDO
gen_class = bp.generated_class()
cdo = gen_class.get_default_object()

# Load input actions
ia_dash = unreal.load_asset("/Game/Input/Actions/IA_Dash")
ia_slide = unreal.load_asset("/Game/Input/Actions/IA_Slide")

# Load montages
am_dash = unreal.load_asset("/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash")
am_slide = unreal.load_asset("/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide")

unreal.log("Loaded assets:")
unreal.log(f"  IA_Dash: {ia_dash}")
unreal.log(f"  IA_Slide: {ia_slide}")
unreal.log(f"  AM_Dash: {am_dash}")
unreal.log(f"  AM_Slide: {am_slide}")

# Set properties on CDO
try:
    cdo.set_editor_property("DashAction", ia_dash)
    unreal.log("Set DashAction OK")
except Exception as e:
    unreal.log_error(f"DashAction failed: {e}")

try:
    cdo.set_editor_property("SlideAction", ia_slide)
    unreal.log("Set SlideAction OK")
except Exception as e:
    unreal.log_error(f"SlideAction failed: {e}")

try:
    cdo.set_editor_property("DashMontage", am_dash)
    unreal.log("Set DashMontage OK")
except Exception as e:
    unreal.log_error(f"DashMontage failed: {e}")

try:
    cdo.set_editor_property("SlideMontage", am_slide)
    unreal.log("Set SlideMontage OK")
except Exception as e:
    unreal.log_error(f"SlideMontage failed: {e}")

# Save
unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
unreal.log("Blueprint saved successfully!")
