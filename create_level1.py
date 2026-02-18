# Lvl_ObstacleLevel1 - Level Generation Script
# Auto-run obstacle avoidance level (25,000cm, ~50sec at 500cm/s).
# 7 sections with progressive difficulty, 28 obstacles, 8 obstacle types.
#
# Run in UE Editor Output Log (Python):
#   with open(r'C:\path\to\create_level1.py') as f: exec(f.read())

import unreal

# ============================================================
# Constants
# ============================================================

SAVE_PATH = '/Game/Maps/Lvl_ObstacleLevel1'
BP_BASE = '/Game/Blueprints/Obstacle'
CUBE_MESH_PATH = '/Engine/BasicShapes/Cube'

# Blueprint class paths (append _C for generated class)
BP_PATHS = {
    'Conveyor':  f'{BP_BASE}/BP_OAConveyorBelt.BP_OAConveyorBelt_C',
    'Pillar':    f'{BP_BASE}/BP_OARotatingPillar.BP_OARotatingPillar_C',
    'TrapFloor': f'{BP_BASE}/BP_OATrapFloor.BP_OATrapFloor_C',
    'Laser':     f'{BP_BASE}/BP_OALaserBeam.BP_OALaserBeam_C',
    'Cannon':    f'{BP_BASE}/BP_OACannon.BP_OACannon_C',
    'JumpPad':   f'{BP_BASE}/BP_OAJumpPad.BP_OAJumpPad_C',
    'Platform':  f'{BP_BASE}/BP_OAMovingPlatform.BP_OAMovingPlatform_C',
    'Ice':       f'{BP_BASE}/BP_OAIceSurface.BP_OAIceSurface_C',
}

# ============================================================
# Helper Functions
# ============================================================

_class_cache = {}

def load_bp_class(key):
    """Load and cache a Blueprint generated class."""
    if key in _class_cache:
        return _class_cache[key]
    path = BP_PATHS[key]
    cls = unreal.load_class(None, path)
    if cls is None:
        unreal.log_error(f'[Level1] Failed to load BP class: {path}')
        return None
    _class_cache[key] = cls
    return cls


def spawn_obstacle(key, x, y, z, pitch=0.0, yaw=0.0, roll=0.0, label='', folder='', props=None):
    """
    Spawn an obstacle Blueprint actor.

    Args:
        key: BP_PATHS key (e.g. 'Conveyor', 'Laser')
        x, y, z: World location
        pitch, yaw, roll: Rotation in degrees
        label: Actor label for the Outliner
        folder: Outliner folder path
        props: Dict of {property_name: value} to set via set_editor_property
    Returns:
        The spawned actor, or None on failure.
    """
    cls = load_bp_class(key)
    if cls is None:
        return None

    loc = unreal.Vector(x, y, z)
    rot = unreal.Rotator(pitch, yaw, roll)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(cls, loc, rot)

    if actor is None:
        unreal.log_error(f'[Level1] Failed to spawn {key} at ({x}, {y}, {z})')
        return None

    if label:
        actor.set_actor_label(label)
    if folder:
        actor.set_folder_path(folder)
    if props:
        for prop_name, value in props.items():
            try:
                actor.set_editor_property(prop_name, value)
            except Exception as e:
                unreal.log_warning(f'[Level1] Could not set {prop_name} on {label}: {e}')

    return actor


def spawn_floor_tile(center_x, length, label='', folder='Floor'):
    """
    Spawn a floor tile (StaticMeshActor with Cube mesh).
    Tile surface sits at Z=0. Width=500cm (Y), thickness=20cm (Z).

    Args:
        center_x: X center position
        length: Total length in X (cm)
        label: Actor label
        folder: Outliner folder
    """
    scale_x = length / 100.0   # Cube is 100cm, so scale = length/100
    scale_y = 5.0               # 500cm width
    scale_z = 0.2               # 20cm thickness

    loc = unreal.Vector(center_x, 0.0, -10.0)  # Surface at Z=0
    rot = unreal.Rotator(0.0, 0.0, 0.0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor, loc, rot
    )
    if actor is None:
        unreal.log_error(f'[Level1] Failed to spawn floor tile at X={center_x}')
        return None

    # Set cube mesh
    cube_mesh = unreal.load_asset(CUBE_MESH_PATH)
    if cube_mesh:
        actor.static_mesh_component.set_static_mesh(cube_mesh)

    actor.set_actor_scale3d(unreal.Vector(scale_x, scale_y, scale_z))

    if label:
        actor.set_actor_label(label)
    if folder:
        actor.set_folder_path(folder)

    return actor


# ============================================================
# 1. Create New Level
# ============================================================

unreal.log('============================================')
unreal.log('[Level1] Creating Lvl_ObstacleLevel1...')
unreal.log('============================================')

# Try LevelEditorSubsystem first, then fallback
level_created = False
try:
    level_sub = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    level_created = level_sub.new_level(SAVE_PATH)
    if level_created:
        unreal.log(f'[Level1] New level created at {SAVE_PATH}')
except Exception as e:
    unreal.log_warning(f'[Level1] LevelEditorSubsystem.new_level failed: {e}')

if not level_created:
    try:
        level_created = unreal.EditorLevelLibrary.new_level('/Engine/Maps/Templates/Template_Default')
        unreal.log('[Level1] Created level from default template (save manually to Maps/Lvl_ObstacleLevel1)')
    except Exception as e2:
        unreal.log_error(f'[Level1] Could not create level: {e2}')
        unreal.log_error('[Level1] Please create a blank level manually, then re-run this script.')

# ============================================================
# 2. World Settings - GameMode Override
# ============================================================

unreal.log('[Level1] Setting GameMode Override...')

try:
    world = unreal.EditorLevelLibrary.get_editor_world()
    world_settings = world.get_world_settings()
    gm_class = unreal.load_class(None, '/Game/ThirdPerson/BP_ThirdPersonGameMode.BP_ThirdPersonGameMode_C')
    if gm_class:
        world_settings.set_editor_property('DefaultGameMode', gm_class)
        unreal.log('[Level1]   GameMode set to BP_ThirdPersonGameMode')
    else:
        unreal.log_warning('[Level1]   BP_ThirdPersonGameMode not found - set manually in World Settings')
except Exception as e:
    unreal.log_warning(f'[Level1]   GameMode override failed: {e}')
    unreal.log('[Level1]   Set manually: World Settings > GameMode Override > BP_ThirdPersonGameMode')

# ============================================================
# 3. Environment Actors
# ============================================================

unreal.log('[Level1] Placing environment actors...')

# Directional Light (sun)
try:
    dir_light_cls = unreal.load_class(None, '/Script/Engine.DirectionalLight')
    if dir_light_cls:
        dl = unreal.EditorLevelLibrary.spawn_actor_from_class(
            dir_light_cls,
            unreal.Vector(12500.0, 0.0, 5000.0),
            unreal.Rotator(-50.0, -30.0, 0.0)
        )
        if dl:
            dl.set_actor_label('DirectionalLight_Sun')
            dl.set_folder_path('Environment')
            unreal.log('[Level1]   DirectionalLight placed')
except Exception as e:
    unreal.log_warning(f'[Level1]   DirectionalLight failed: {e}')

# Sky Atmosphere
try:
    sky_atm_cls = unreal.load_class(None, '/Script/Engine.SkyAtmosphere')
    if sky_atm_cls:
        sa = unreal.EditorLevelLibrary.spawn_actor_from_class(
            sky_atm_cls,
            unreal.Vector(0.0, 0.0, 0.0),
            unreal.Rotator(0.0, 0.0, 0.0)
        )
        if sa:
            sa.set_actor_label('SkyAtmosphere')
            sa.set_folder_path('Environment')
            unreal.log('[Level1]   SkyAtmosphere placed')
except Exception as e:
    unreal.log_warning(f'[Level1]   SkyAtmosphere failed: {e}')

# Sky Light
try:
    sky_light_cls = unreal.load_class(None, '/Script/Engine.SkyLight')
    if sky_light_cls:
        sl = unreal.EditorLevelLibrary.spawn_actor_from_class(
            sky_light_cls,
            unreal.Vector(0.0, 0.0, 1000.0),
            unreal.Rotator(0.0, 0.0, 0.0)
        )
        if sl:
            sl.set_actor_label('SkyLight')
            sl.set_folder_path('Environment')
            unreal.log('[Level1]   SkyLight placed')
except Exception as e:
    unreal.log_warning(f'[Level1]   SkyLight failed: {e}')

# Exponential Height Fog
try:
    fog_cls = unreal.load_class(None, '/Script/Engine.ExponentialHeightFog')
    if fog_cls:
        fog = unreal.EditorLevelLibrary.spawn_actor_from_class(
            fog_cls,
            unreal.Vector(0.0, 0.0, 200.0),
            unreal.Rotator(0.0, 0.0, 0.0)
        )
        if fog:
            fog.set_actor_label('ExponentialHeightFog')
            fog.set_folder_path('Environment')
            unreal.log('[Level1]   ExponentialHeightFog placed')
except Exception as e:
    unreal.log_warning(f'[Level1]   ExponentialHeightFog failed: {e}')

# ============================================================
# 3. Floor Tiles
# ============================================================
# Gaps at: TrapFloor O3 (5000-5300), Section3 jump gap (11500-12500),
#          TrapFloor O15 (17450-17750), TrapFloors O24-O25 (22450-23050),
#          Platforming gap (23500-24700)

unreal.log('[Level1] Placing floor tiles...')

floor_segments = [
    # (center_x, length, label)
    (2500.0,  5000.0,  'Floor_00_Start'),         # 0 ~ 5000
    (8400.0,  6200.0,  'Floor_01_TutorialTiming'), # 5300 ~ 11500
    (14975.0, 4950.0,  'Floor_02_MoveCombine'),    # 12500 ~ 17450
    (20100.0, 4700.0,  'Floor_03_GauntletMid'),    # 17750 ~ 22450
    (23275.0, 450.0,   'Floor_04_PlatformLanding'), # 23050 ~ 23500
    (25100.0, 800.0,   'Floor_05_Goal'),           # 24700 ~ 25500
]

for cx, length, label in floor_segments:
    tile = spawn_floor_tile(cx, length, label=label)
    if tile:
        unreal.log(f'[Level1]   {label}: center={cx}, length={length}')

# ============================================================
# 4. PlayerStart
# ============================================================

unreal.log('[Level1] Placing PlayerStart...')

try:
    ps_cls = unreal.load_class(None, '/Script/Engine.PlayerStart')
    if ps_cls:
        ps = unreal.EditorLevelLibrary.spawn_actor_from_class(
            ps_cls,
            unreal.Vector(200.0, 0.0, 100.0),
            unreal.Rotator(0.0, 0.0, 0.0)
        )
        if ps:
            ps.set_actor_label('PlayerStart')
            ps.set_folder_path('Gameplay')
            unreal.log('[Level1]   PlayerStart at (200, 0, 100)')
except Exception as e:
    unreal.log_warning(f'[Level1]   PlayerStart failed: {e}')

# ============================================================
# 5. Obstacles - Section by Section
# ============================================================

unreal.log('[Level1] Placing obstacles...')
obstacle_count = 0

# ----------------------------------------------------------
# Section 1: Tutorial (2000-6000) - Single obstacles
# ----------------------------------------------------------
SEC1 = 'Sec1_Tutorial'

# O1: Reverse Conveyor Belt - slows player down
o1 = spawn_obstacle('Conveyor', 2500, 0, 0,
    label='O01_ConveyorBelt_Reverse', folder=SEC1,
    props={'bReverseDirection': True, 'BeltSpeed': 300.0})
if o1: obstacle_count += 1

# O2: Rotating Pillar - basic knockback hazard
o2 = spawn_obstacle('Pillar', 3800, 0, 0,
    label='O02_RotatingPillar', folder=SEC1)
if o2: obstacle_count += 1

# O3: Trap Floor - floor collapses after stepping
o3 = spawn_obstacle('TrapFloor', 5150, 0, 0,
    label='O03_TrapFloor', folder=SEC1)
if o3: obstacle_count += 1

# ----------------------------------------------------------
# Section 2: Timing (6000-10000) - Periodic hazards
# ----------------------------------------------------------
SEC2 = 'Sec2_Timing'

# O4: Laser Beam - 3s cycle (1.5s on, 1.5s off)
o4 = spawn_obstacle('Laser', 6800, 0, 0,
    label='O04_LaserBeam', folder=SEC2,
    props={'BeamCycle': 3.0})
if o4: obstacle_count += 1

# O5: Cannon (left side) - fires across the path
o5 = spawn_obstacle('Cannon', 8200, -350, 0, yaw=90.0,
    label='O05_Cannon_Left', folder=SEC2)
if o5: obstacle_count += 1

# O6: Cannon (right side) - fires across the path
o6 = spawn_obstacle('Cannon', 9200, 350, 0, yaw=-90.0,
    label='O06_Cannon_Right', folder=SEC2)
if o6: obstacle_count += 1

# ----------------------------------------------------------
# Section 3: Movement Skills (10000-14000)
# ----------------------------------------------------------
SEC3 = 'Sec3_Movement'

# O7: Jump Pad - launches player over 1000cm gap
o7 = spawn_obstacle('JumpPad', 11400, 0, 0,
    label='O07_JumpPad', folder=SEC3,
    props={'LaunchForce': 1500.0})
if o7: obstacle_count += 1

# O8: Moving Platform - aerial safety net in the gap
o8 = spawn_obstacle('Platform', 12000, 0, 200,
    label='O08_MovingPlatform_Gap', folder=SEC3,
    props={'bMoveLeftRight': True, 'MoveDistance': 200.0, 'MoveSpeed': 150.0})
if o8: obstacle_count += 1

# O9: Ice Surface - slippery landing after gap
o9 = spawn_obstacle('Ice', 13500, 0, 0,
    label='O09_IceSurface', folder=SEC3)
if o9: obstacle_count += 1

# ----------------------------------------------------------
# Section 4: Combinations (14000-18000)
# ----------------------------------------------------------
SEC4 = 'Sec4_Combination'

# O10: Reverse Conveyor + O11: Rotating Pillar combo
o10 = spawn_obstacle('Conveyor', 14500, 0, 0,
    label='O10_ConveyorBelt_Reverse', folder=SEC4,
    props={'bReverseDirection': True, 'BeltSpeed': 350.0})
if o10: obstacle_count += 1

o11 = spawn_obstacle('Pillar', 15200, 0, 0,
    label='O11_RotatingPillar', folder=SEC4,
    props={'RotationSpeed': 110.0})
if o11: obstacle_count += 1

# O12 + O13: Double Laser Beams
o12 = spawn_obstacle('Laser', 16200, 0, 0,
    label='O12_LaserBeam_Double1', folder=SEC4,
    props={'BeamCycle': 3.0})
if o12: obstacle_count += 1

o13 = spawn_obstacle('Laser', 16700, 0, 0,
    label='O13_LaserBeam_Double2', folder=SEC4,
    props={'BeamCycle': 3.0})
if o13: obstacle_count += 1

# O14: Cannon + O15: Trap Floor combo
o14 = spawn_obstacle('Cannon', 17200, -350, 0, yaw=90.0,
    label='O14_Cannon_Left', folder=SEC4)
if o14: obstacle_count += 1

o15 = spawn_obstacle('TrapFloor', 17600, 0, 0,
    label='O15_TrapFloor', folder=SEC4)
if o15: obstacle_count += 1

# ----------------------------------------------------------
# Section 5: Gauntlet (18000-22000) - Hard combinations
# ----------------------------------------------------------
SEC5 = 'Sec5_Gauntlet'

# O16: Ice Surface + O17: Low Rotating Pillar on ice
o16 = spawn_obstacle('Ice', 18700, 0, 0,
    label='O16_IceSurface', folder=SEC5)
if o16: obstacle_count += 1

o17 = spawn_obstacle('Pillar', 18900, 0, 0,
    label='O17_RotatingPillar_Low', folder=SEC5,
    props={'PillarHeight': 80.0, 'RotationSpeed': 100.0})
if o17: obstacle_count += 1

# O18: Rotating Pillar (offset Y)
o18 = spawn_obstacle('Pillar', 19400, -100, 0,
    label='O18_RotatingPillar_Offset', folder=SEC5,
    props={'RotationSpeed': 120.0})
if o18: obstacle_count += 1

# O19: Acceleration Conveyor (forward, fast) + O20: Fast Laser
o19 = spawn_obstacle('Conveyor', 19800, 0, 0,
    label='O19_ConveyorBelt_Accel', folder=SEC5,
    props={'bReverseDirection': False, 'BeltSpeed': 500.0})
if o19: obstacle_count += 1

o20 = spawn_obstacle('Laser', 20200, 0, 0,
    label='O20_LaserBeam_Fast', folder=SEC5,
    props={'BeamCycle': 2.0})
if o20: obstacle_count += 1

# O21 + O22: Cross-fire Cannons
o21 = spawn_obstacle('Cannon', 20800, -350, 0, yaw=90.0,
    label='O21_Cannon_Cross_Left', folder=SEC5,
    props={'FireInterval': 1.5})
if o21: obstacle_count += 1

o22 = spawn_obstacle('Cannon', 20800, 350, 0, yaw=-90.0,
    label='O22_Cannon_Cross_Right', folder=SEC5,
    props={'FireInterval': 1.8})
if o22: obstacle_count += 1

# O23: Reverse Conveyor (deceleration zone before section end)
o23 = spawn_obstacle('Conveyor', 21200, 0, 0,
    label='O23_ConveyorBelt_Decel', folder=SEC5,
    props={'bReverseDirection': True, 'BeltSpeed': 400.0})
if o23: obstacle_count += 1

# ----------------------------------------------------------
# Section 6: Platforming (22000-24700) - Gaps & chains
# ----------------------------------------------------------
SEC6 = 'Sec6_Platforming'

# O24 + O25: Consecutive Trap Floors
o24 = spawn_obstacle('TrapFloor', 22600, 0, 0,
    label='O24_TrapFloor_Chain1', folder=SEC6,
    props={'FallDelay': 1.0})
if o24: obstacle_count += 1

o25 = spawn_obstacle('TrapFloor', 22900, 0, 0,
    label='O25_TrapFloor_Chain2', folder=SEC6,
    props={'FallDelay': 1.0})
if o25: obstacle_count += 1

# O26: Jump Pad - launches to moving platform chain
o26 = spawn_obstacle('JumpPad', 23400, 0, 0,
    label='O26_JumpPad_Final', folder=SEC6,
    props={'LaunchForce': 1800.0})
if o26: obstacle_count += 1

# O27 + O28: Moving Platform chain (aerial bridge to goal)
o27 = spawn_obstacle('Platform', 23800, 0, 200,
    label='O27_MovingPlatform_Chain1', folder=SEC6,
    props={'bMoveLeftRight': True, 'MoveDistance': 150.0, 'MoveSpeed': 180.0})
if o27: obstacle_count += 1

o28 = spawn_obstacle('Platform', 24300, 0, 200,
    label='O28_MovingPlatform_Chain2', folder=SEC6,
    props={'bMoveLeftRight': True, 'MoveDistance': 150.0, 'MoveSpeed': 200.0})
if o28: obstacle_count += 1

# ============================================================
# 6. Save Level
# ============================================================

unreal.log(f'[Level1] Placed {obstacle_count}/28 obstacles')

try:
    level_sub = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    level_sub.save_current_level()
    unreal.log(f'[Level1] Level saved.')
except Exception as e:
    unreal.log_warning(f'[Level1] Auto-save failed: {e}')
    unreal.log('[Level1] Please save manually: File > Save Current Level As > Maps/Lvl_ObstacleLevel1')

# ============================================================
# Done
# ============================================================

unreal.log('============================================')
unreal.log('[Level1] Lvl_ObstacleLevel1 generation complete!')
unreal.log('[Level1] Next steps:')
unreal.log('[Level1]   1. Set World Settings > GameMode Override to BP_ThirdPersonGameMode (AOAGameMode)')
unreal.log('[Level1]   2. Play In Editor to test auto-run + obstacles')
unreal.log('[Level1]   3. Adjust obstacle properties as needed')
unreal.log('============================================')
