
# @UnrealClaude Script
# @Description: Assign properties and SAVE BP to disk

import unreal

bp_path = '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter'
bp = unreal.load_asset(bp_path)

if bp is None:
    unreal.log_error("BP not found!")
else:
    cdo = unreal.get_default_object(bp.generated_class())
    
    # Load assets
    dash_action = unreal.load_asset('/Game/Input/Actions/IA_Dash')
    slide_action = unreal.load_asset('/Game/Input/Actions/IA_Slide')
    dash_montage = unreal.load_asset('/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash')
    slide_montage = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide')
    
    unreal.log(f"dash_action loaded: {dash_action is not None}")
    unreal.log(f"slide_action loaded: {slide_action is not None}")
    unreal.log(f"dash_montage loaded: {dash_montage is not None}")
    unreal.log(f"slide_montage loaded: {slide_montage is not None}")
    
    # Set properties
    if dash_action:
        cdo.set_editor_property('DashAction', dash_action)
    if slide_action:
        cdo.set_editor_property('SlideAction', slide_action)
    if dash_montage:
        cdo.set_editor_property('DashMontage', dash_montage)
    if slide_montage:
        cdo.set_editor_property('SlideMontage', slide_montage)
    
    # Verify
    unreal.log(f"After set - DashAction: {cdo.get_editor_property('DashAction')}")
    unreal.log(f"After set - SlideAction: {cdo.get_editor_property('SlideAction')}")
    unreal.log(f"After set - DashMontage: {cdo.get_editor_property('DashMontage')}")
    unreal.log(f"After set - SlideMontage: {cdo.get_editor_property('SlideMontage')}")
    
    # Save BP
    saved = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    unreal.log(f"BP saved: {saved}")
