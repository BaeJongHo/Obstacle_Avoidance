# @UnrealClaude Script
# @Description: Assign SM_Plane mesh to BP_OAMovingPlatform

import unreal

# Load the Blueprint
bp_path = "/Game/Blueprints/Actor/BP_OAMovingPlatform"
bp = unreal.load_asset(bp_path)
if not bp:
    raise Exception("BP not found: " + bp_path)

# Load the static mesh
mesh_path = "/Game/LevelPrototyping/Meshes/SM_Plane"
mesh = unreal.load_asset(mesh_path)
if not mesh:
    raise Exception("Mesh not found: " + mesh_path)

print("BP: {}".format(bp.get_name()))
print("Mesh: {}".format(mesh.get_name()))

# Get the generated class CDO
gen_class = bp.generated_class()
if not gen_class:
    raise Exception("No generated class")

cdo = unreal.get_default_object(gen_class)
if not cdo:
    raise Exception("No CDO")

print("CDO: {}".format(cdo.get_name()))

# Find the StaticMeshComponent on the CDO
components = cdo.get_components_by_class(unreal.StaticMeshComponent)
print("StaticMeshComponents found: {}".format(len(components)))

if len(components) > 0:
    smc = components[0]
    smc.set_static_mesh(mesh)
    print("Mesh assigned to: {}".format(smc.get_name()))
else:
    print("No StaticMeshComponent found on CDO, trying root component")
    root = cdo.root_component
    if root and isinstance(root, unreal.StaticMeshComponent):
        root.set_static_mesh(mesh)
        print("Mesh assigned to root component")
    else:
        print("Root is not StaticMeshComponent, type: {}".format(type(root)))

# Save the package
pkg_path = "/Game/Blueprints/Actor/BP_OAMovingPlatform"
pkg = unreal.find_package(pkg_path)
if pkg:
    unreal.EditorLoadingAndSavingUtils.save_packages([pkg], only_dirty=False)
    print("Package saved!")
else:
    print("Could not find package to save")
