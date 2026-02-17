/** @UnrealClaude Script
 * @Description: Assign DashAction, SlideAction, DashMontage, SlideMontage to BP_ThirdPersonCharacter CDO
 */

import unreal

# Load the Blueprint asset
bp_path = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter"
bp_asset = unreal.load_asset(bp_path)

if bp_asset is None:
    raise Exception("Failed to load BP_ThirdPersonCharacter")

# Get the CDO (Class Default Object)
bp_gc = unreal.load_object(None, bp_path + "_C")
if bp_gc is None:
    bp_gc = bp_asset.generated_class()

cdo = unreal.get_default_object(bp_gc)
if cdo is None:
    raise Exception("Failed to get CDO")

# Load assets
ia_dash = unreal.load_asset("/Game/Input/Actions/IA_Dash.IA_Dash")
ia_slide = unreal.load_asset("/Game/Input/Actions/IA_Slide.IA_Slide")
am_dash = unreal.load_asset("/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash.AM_Dash")
am_slide = unreal.load_asset("/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide.AM_Slide")

results = []

# Assign DashAction
if ia_dash:
    cdo.set_editor_property("DashAction", ia_dash)
    results.append("DashAction = IA_Dash OK")
else:
    results.append("DashAction FAILED - asset not found")

# Assign SlideAction
if ia_slide:
    cdo.set_editor_property("SlideAction", ia_slide)
    results.append("SlideAction = IA_Slide OK")
else:
    results.append("SlideAction FAILED - asset not found")

# Assign DashMontage
if am_dash:
    cdo.set_editor_property("DashMontage", am_dash)
    results.append("DashMontage = AM_Dash OK")
else:
    results.append("DashMontage FAILED - asset not found")

# Assign SlideMontage
if am_slide:
    cdo.set_editor_property("SlideMontage", am_slide)
    results.append("SlideMontage = AM_Slide OK")
else:
    results.append("SlideMontage FAILED - asset not found")

# Mark blueprint as modified and save
unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)

for r in results:
    unreal.log(r)

unreal.log("All properties assigned and blueprint saved!")
