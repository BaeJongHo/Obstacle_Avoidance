# @Description: Check C++ class properties and try to set via editor property system
import unreal

bp_path = '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter'
bp = unreal.load_asset(bp_path)

if bp:
    cdo = unreal.get_default_object(bp.generated_class())
    
    # Try using set_editor_property which uses UE reflection system
    # First check what properties exist
    try:
        val = cdo.get_editor_property('DashAction')
        unreal.log(f"DashAction (editor_property) = {val}")
    except Exception as e:
        unreal.log(f"DashAction not found via editor_property: {e}")
    
    try:
        val = cdo.get_editor_property('dash_action')
        unreal.log(f"dash_action (editor_property) = {val}")
    except Exception as e:
        unreal.log(f"dash_action not found via editor_property: {e}")

    # Try the parent C++ class CDO directly
    cpp_class = unreal.find_class('Obstacle_AvoidanceCharacter')
    if cpp_class:
        unreal.log(f"Found C++ class: {cpp_class}")
    else:
        unreal.log("C++ class not found via find_class")
    
    # Try loading the C++ class directly
    cpp_class2 = unreal.find_object(None, '/Script/Obstacle_Avoidance.Obstacle_AvoidanceCharacter')
    if cpp_class2:
        unreal.log(f"Found via find_object: {cpp_class2}")
        cpp_cdo = unreal.get_default_object(cpp_class2)
        try:
            val = cpp_cdo.get_editor_property('DashAction')
            unreal.log(f"C++ CDO DashAction = {val}")
        except Exception as e:
            unreal.log(f"C++ CDO DashAction error: {e}")
    else:
        unreal.log("C++ class not found via find_object either")
