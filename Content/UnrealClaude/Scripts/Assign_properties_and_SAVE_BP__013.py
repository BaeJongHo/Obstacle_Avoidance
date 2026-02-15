# @Description: Assign and SAVE all 4 properties on BP_ThirdPersonCharacter
import unreal

bp_path = '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter'
bp = unreal.load_asset(bp_path)
cdo = unreal.get_default_object(bp.generated_class())

# Load assets
dash_action = unreal.load_asset('/Game/Input/Actions/IA_Dash')
slide_action = unreal.load_asset('/Game/Input/Actions/IA_Slide')
dash_montage = unreal.load_asset('/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash')
slide_montage = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide')

# Set properties using editor_property (UE reflection)
cdo.set_editor_property('DashAction', dash_action)
cdo.set_editor_property('SlideAction', slide_action)
cdo.set_editor_property('DashMontage', dash_montage)
cdo.set_editor_property('SlideMontage', slide_montage)

# Verify
unreal.log(f"DashAction = {cdo.get_editor_property('DashAction')}")
unreal.log(f"SlideAction = {cdo.get_editor_property('SlideAction')}")
unreal.log(f"DashMontage = {cdo.get_editor_property('DashMontage')}")
unreal.log(f"SlideMontage = {cdo.get_editor_property('SlideMontage')}")

# SAVE the Blueprint asset
result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
unreal.log(f"Save result: {result}")
