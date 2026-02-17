# @Description: Create BP_OAPendulumSwing and assign SM_Obstacle_Light_Post mesh
import unreal

# 1. Find the AOAPendulumSwing C++ class
parent_class = unreal.EditorAssetLibrary.load_asset("/Script/Obstacle_Avoidance.OAPendulumSwing")
if parent_class is None:
    raise Exception("AOAPendulumSwing class not found! LiveCoding may not have registered it.")

print(f"Parent class found: {parent_class}")

# 2. Create the Blueprint
factory = unreal.BlueprintFactory()
factory.set_editor_property("parent_class", parent_class)

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
bp = asset_tools.create_asset("BP_OAPendulumSwing", "/Game/Blueprints/Actor", unreal.Blueprint, factory)

if bp is None:
    raise Exception("Failed to create BP_OAPendulumSwing")

print(f"BP created: {bp.get_path_name()}")

# 3. Load the mesh
mesh = unreal.load_asset("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Light_Post.SM_Obstacle_Light_Post")
if mesh is None:
    raise Exception("SM_Obstacle_Light_Post mesh not found!")

print(f"Mesh loaded: {mesh.get_name()}")

# 4. Find MeshComp in the SCS and assign the mesh
scs = bp.get_editor_property("simple_construction_script")
if scs:
    nodes = scs.get_all_nodes()
    for node in nodes:
        template = node.get_editor_property("component_template")
        if template and template.get_name() == "MeshComp":
            template.set_editor_property("static_mesh", mesh)
            print(f"Mesh assigned to MeshComp!")
            break
    else:
        print("MeshComp not found in SCS nodes. Listing available nodes:")
        for node in nodes:
            template = node.get_editor_property("component_template")
            if template:
                print(f"  - {template.get_name()} ({type(template).__name__})")

# 5. Save the asset
unreal.EditorAssetLibrary.save_asset(bp.get_path_name())
print("BP saved successfully!")
