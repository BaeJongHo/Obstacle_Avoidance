# @Description: List all properties on BP_ThirdPersonCharacter CDO
import unreal

bp_path = '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter'
bp = unreal.load_asset(bp_path)

if bp:
    cdo = unreal.get_default_object(bp.generated_class())
    
    # List all properties that contain relevant keywords
    for prop in dir(cdo):
        prop_lower = prop.lower()
        if any(kw in prop_lower for kw in ['dash', 'slide', 'action', 'montage', 'jump', 'move', 'look', 'input']):
            try:
                val = getattr(cdo, prop)
                if not callable(val):
                    unreal.log(f"  {prop} = {val}")
            except:
                pass
else:
    unreal.log_error("BP not found!")
