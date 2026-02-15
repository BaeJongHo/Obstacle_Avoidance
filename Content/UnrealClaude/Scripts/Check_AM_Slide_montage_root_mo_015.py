
# @UnrealClaude Script
# @Description: Check AM_Slide montage root motion settings
import unreal

slide_montage = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/AM_Slide')
if slide_montage:
    print(f"AM_Slide found: {slide_montage}")
    print(f"  enable_root_motion_translation: {slide_montage.get_editor_property('enable_root_motion_translation')}")
    print(f"  enable_root_motion_rotation: {slide_montage.get_editor_property('enable_root_motion_rotation')}")
    print(f"  force_root_lock: {slide_montage.get_editor_property('force_root_lock')}")
    
    # Also check the source animation
    anim = unreal.load_asset('/Game/FreeAnimationLibrary/Animations/Slide/anim_Slide_L')
    if anim:
        print(f"\nanim_Slide_L found: {anim}")
        print(f"  enable_root_motion: {anim.get_editor_property('enable_root_motion')}")
        print(f"  force_root_lock: {anim.get_editor_property('force_root_lock')}")
    
    # Check dash montage for comparison
    dash_montage = unreal.load_asset('/Game/Characters/Mannequins/Anims/Unarmed/Jump/AM_Dash')
    if dash_montage:
        print(f"\nAM_Dash found: {dash_montage}")
        print(f"  enable_root_motion_translation: {dash_montage.get_editor_property('enable_root_motion_translation')}")
        print(f"  enable_root_motion_rotation: {dash_montage.get_editor_property('enable_root_motion_rotation')}")
        print(f"  force_root_lock: {dash_montage.get_editor_property('force_root_lock')}")
        
        dash_anim = unreal.load_asset('/Game/Characters/Mannequins/Anims/Unarmed/Jump/MM_Dash')
        if dash_anim:
            print(f"\nMM_Dash found: {dash_anim}")
            print(f"  enable_root_motion: {dash_anim.get_editor_property('enable_root_motion')}")
            print(f"  force_root_lock: {dash_anim.get_editor_property('force_root_lock')}")
else:
    print("AM_Slide not found!")
