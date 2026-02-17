"""
@UnrealClaude Script
@Description: Check BP_ThirdPersonCharacter CDO for DashAction/SlideAction/DashMontage/SlideMontage values
"""
import unreal

# Load the BP class
bp_class = unreal.load_object(None, '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C')
if bp_class:
    cdo = unreal.get_default_object(bp_class)
    if cdo:
        # Check input actions
        dash_action = cdo.get_editor_property('DashAction')
        slide_action = cdo.get_editor_property('SlideAction')
        dash_montage = cdo.get_editor_property('DashMontage')
        slide_montage = cdo.get_editor_property('SlideMontage')
        jump_action = cdo.get_editor_property('JumpAction')
        move_action = cdo.get_editor_property('MoveAction')
        
        unreal.log_warning(f"=== BP_ThirdPersonCharacter CDO Properties ===")
        unreal.log_warning(f"JumpAction: {jump_action}")
        unreal.log_warning(f"MoveAction: {move_action}")
        unreal.log_warning(f"DashAction: {dash_action}")
        unreal.log_warning(f"SlideAction: {slide_action}")
        unreal.log_warning(f"DashMontage: {dash_montage}")
        unreal.log_warning(f"SlideMontage: {slide_montage}")
    else:
        unreal.log_error("Could not get CDO")
else:
    unreal.log_error("Could not load BP class")
