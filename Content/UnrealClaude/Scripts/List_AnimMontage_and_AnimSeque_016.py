
# @UnrealClaude Script
# @Description: List AnimMontage and AnimSequence properties for root motion
import unreal

slide_montage = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide')
if slide_montage:
    # Find root motion related properties
    for prop_name in dir(slide_montage):
        if 'root' in prop_name.lower() or 'motion' in prop_name.lower() or 'lock' in prop_name.lower():
            print(f"Montage prop: {prop_name}")

anim = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/anim_Slide_L')
if anim:
    print("")
    for prop_name in dir(anim):
        if 'root' in prop_name.lower() or 'motion' in prop_name.lower() or 'lock' in prop_name.lower():
            print(f"AnimSeq prop: {prop_name}")
    
    # Check enable_root_motion on AnimSequence
    try:
        val = anim.get_editor_property('enable_root_motion')
        print(f"\nanim enable_root_motion = {val}")
    except:
        pass
    try:
        val = anim.get_editor_property('force_root_lock')
        print(f"anim force_root_lock = {val}")
    except:
        pass
    try:
        val = anim.get_editor_property('root_motion_root_lock')
        print(f"anim root_motion_root_lock = {val}")
    except:
        pass
