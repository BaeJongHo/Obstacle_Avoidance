"""
@UnrealClaude Script
@Description: Check BP_ThirdPersonCharacter CDO for input action assignments
"""
import unreal

asset = unreal.EditorAssetLibrary.load_asset('/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter')
if asset:
    gen_class = asset.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            props = ['JumpAction', 'MoveAction', 'LookAction', 'MouseLookAction', 'DashAction', 'SlideAction', 'DashMontage', 'SlideMontage']
            for p in props:
                try:
                    val = cdo.get_editor_property(p)
                    unreal.log_warning(f"{p}: {val if val else 'None/NULL'}")
                except:
                    unreal.log_warning(f"{p}: [property not found]")
        else:
            unreal.log_error("No CDO")
    else:
        unreal.log_error("No generated class")
else:
    unreal.log_error("Asset not found")
