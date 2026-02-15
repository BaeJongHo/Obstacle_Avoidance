
# @UnrealClaude Script
# @Description: Set force_root_lock on slide animation to prevent snapback
import unreal

anim = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/anim_Slide_L')
if anim:
    anim.set_editor_property('force_root_lock', True)
    anim.set_editor_property('root_motion_root_lock', unreal.RootMotionRootLock.ANIM_FIRST_FRAME)
    unreal.EditorAssetLibrary.save_asset('/Game/FreeAnimationLibrary/Animations/Slide/anim_Slide_L')
    print(f"force_root_lock set to: {anim.get_editor_property('force_root_lock')}")
    print(f"root_motion_root_lock set to: {anim.get_editor_property('root_motion_root_lock')}")
    print("Saved successfully!")
else:
    print("Animation not found!")
