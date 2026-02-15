# @Description: Check BP_ThirdPersonCharacter property values
import unreal

bp_path = '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter'
bp = unreal.load_asset(bp_path)

if bp:
    cdo = unreal.get_default_object(bp.generated_class())
    
    # Check all relevant properties
    props_to_check = ['DashAction', 'SlideAction', 'DashMontage', 'SlideMontage', 
                       'JumpAction', 'MoveAction', 'LookAction']
    
    for prop_name in props_to_check:
        try:
            val = getattr(cdo, prop_name, 'ATTR_NOT_FOUND')
            unreal.log(f"{prop_name} = {val}")
        except Exception as e:
            unreal.log(f"{prop_name} error: {e}")
    
    # Also check the class hierarchy
    unreal.log(f"Class: {bp.generated_class().get_name()}")
    unreal.log(f"Parent: {bp.generated_class().get_super_class().get_name()}")
else:
    unreal.log_error("BP not found!")
