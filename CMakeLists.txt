cmake_minimum_required(VERSION 3.5.1)

project(virtueror LANGUAGES CXX VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set platform version for preprocessor checks
add_compile_definitions($<$<PLATFORM_ID:Linux>:LINUX=1>
                        $<$<PLATFORM_ID:Windows>:WINDOWS=1>
                        $<$<CONFIG:Debug>:DEBUG=1>
                        $<$<CONFIG:Release>:RELEASE=1>
                        $<$<CONFIG:RelWithDebInfo>:RELEASE=1>)

# include internal libraries
add_subdirectory(lib)

# main executable
add_executable(
    game
    src/main.cpp
    src/CameraMapController.h
    src/CameraMapController.cpp
    src/Cell2D.h
    src/ControlMap.h
    src/ControlMap.cpp
    src/Game.h
    src/Game.cpp
    src/GameConstants.h
    src/GameConstants.cpp
    src/GameData.h
    src/GameData.cpp
    src/GameMap.h
    src/GameMap.cpp
    src/GameMapCell.cpp
    src/GameMapCell.h
    src/GameTestData.h
    src/GameTestData.cpp
    src/IsoLayer.h
    src/IsoLayer.cpp
    src/IsoMap.h
    src/IsoMap.cpp
    src/IsoObject.h
    src/IsoObject.cpp
    src/MapLoader.h
    src/MapLoader.cpp
    src/MapsRegistry.h
    src/MapsRegistry.cpp
    src/Player.h
    src/Player.cpp
    src/StatValue.h
    src/StatValue.cpp
    src/Version.h
    src/AI/ActionAI.h
    src/AI/ConquerPath.h
    src/AI/ConquerPath.cpp
    src/AI/ObjectPath.h
    src/AI/ObjectPath.cpp
    src/AI/PlayerAI.h
    src/AI/PlayerAI.cpp
    src/AI/WallBuildPath.h
    src/AI/WallBuildPath.cpp
    src/GameObjects/Base.h
    src/GameObjects/Base.cpp
    src/GameObjects/Blobs.h
    src/GameObjects/Blobs.cpp
    src/GameObjects/BlobsGenerator.h
    src/GameObjects/BlobsGenerator.cpp
    src/GameObjects/Collectable.h
    src/GameObjects/Collectable.cpp
    src/GameObjects/CollectableGenerator.h
    src/GameObjects/CollectableGenerator.cpp
    src/GameObjects/DefensiveTower.h
    src/GameObjects/DefensiveTower.cpp
    src/GameObjects/Diamonds.h
    src/GameObjects/Diamonds.cpp
    src/GameObjects/DiamondsGenerator.h
    src/GameObjects/DiamondsGenerator.cpp
    src/GameObjects/GameObject.h
    src/GameObjects/GameObject.cpp
    src/GameObjects/GameObjectAction.h
    src/GameObjects/GameObjectTypes.h
    src/GameObjects/LootBox.h
    src/GameObjects/LootBox.cpp
    src/GameObjects/ObjectData.h
    src/GameObjects/ObjectData.cpp
    src/GameObjects/ObjectsDataRegistry.h
    src/GameObjects/ObjectsDataRegistry.cpp
    src/GameObjects/PracticeTarget.h
    src/GameObjects/PracticeTarget.cpp
    src/GameObjects/RadarStation.h
    src/GameObjects/RadarStation.cpp
    src/GameObjects/RadarTower.h
    src/GameObjects/RadarTower.cpp
    src/GameObjects/ResourceGenerator.h
    src/GameObjects/ResourceGenerator.cpp
    src/GameObjects/ResourceStorage.h
    src/GameObjects/ResourceStorage.cpp
    src/GameObjects/SceneObject.h
    src/GameObjects/SceneObject.cpp
    src/GameObjects/Structure.h
    src/GameObjects/Structure.cpp
    src/GameObjects/Temple.h
    src/GameObjects/Temple.cpp
    src/GameObjects/Trees.h
    src/GameObjects/Trees.cpp
    src/GameObjects/Unit.h
    src/GameObjects/Unit.cpp
    src/GameObjects/Wall.h
    src/GameObjects/Wall.cpp
    src/GameObjects/WallGate.h
    src/GameObjects/WallGate.cpp
    src/Indicators/AttackRangeIndicator.h
    src/Indicators/AttackRangeIndicator.cpp
    src/Indicators/ConquestIndicator.h
    src/Indicators/ConquestIndicator.cpp
    src/Indicators/HealingRangeIndicator.h
    src/Indicators/HealingRangeIndicator.cpp
    src/Indicators/MoveIndicator.h
    src/Indicators/MoveIndicator.cpp
    src/Indicators/StructureIndicator.h
    src/Indicators/StructureIndicator.cpp
    src/Indicators/WallIndicator.h
    src/Indicators/WallIndicator.cpp
    src/Particles/DataParticleDamage.h
    src/Particles/DataParticleHealing.h
    src/Particles/DataParticleLootboxPrize.h
    src/Particles/DataParticleSingleLaser.h
    src/Particles/ParticleDamage.h
    src/Particles/ParticleDamage.cpp
    src/Particles/ParticleHealing.h
    src/Particles/ParticleHealing.cpp
    src/Particles/ParticleLootboxPrize.h
    src/Particles/ParticleLootboxPrize.cpp
    src/Particles/ParticleSingleLaser.h
    src/Particles/ParticleSingleLaser.cpp
    src/Particles/UpdaterDamage.h
    src/Particles/UpdaterDamage.cpp
    src/Particles/UpdaterHealing.h
    src/Particles/UpdaterHealing.cpp
    src/Particles/UpdaterLootboxPrize.h
    src/Particles/UpdaterLootboxPrize.cpp
    src/Particles/UpdaterSingleLaser.h
    src/Particles/UpdaterSingleLaser.cpp
    src/Screens/Screen.h
    src/Screens/Screen.cpp
    src/Screens/ScreenFactionSelection.h
    src/Screens/ScreenFactionSelection.cpp
    src/Screens/ScreenGame.h
    src/Screens/ScreenGame.cpp
    src/Screens/ScreenInit.h
    src/Screens/ScreenInit.cpp
    src/Screens/ScreenMainMenu.h
    src/Screens/ScreenMainMenu.cpp
    src/Screens/ScreenNewGame.h
    src/Screens/ScreenNewGame.cpp
    src/Screens/ScreenSettings.h
    src/Screens/ScreenSettings.cpp
    src/Screens/ScreenPlanetMap.h
    src/Screens/ScreenPlanetMap.cpp
    src/Screens/ScreenTest.h
    src/Screens/ScreenTest.cpp
    src/Screens/SharedScreenListener.h
    src/Screens/SharedScreenListener.cpp
    src/States/BaseGameState.h
    src/States/BaseGameState.cpp
    src/States/StateFactionSelection.h
    src/States/StateFactionSelection.cpp
    src/States/StateGame.h
    src/States/StateGame.cpp
    src/States/StateInit.h
    src/States/StateInit.cpp
    src/States/StateMainMenu.h
    src/States/StateMainMenu.cpp
    src/States/StateNewGame.h
    src/States/StateNewGame.cpp
    src/States/StatePlanetMap.h
    src/States/StatePlanetMap.cpp
    src/States/StateSettings.h
    src/States/StateSettings.cpp
    src/States/StatesIds.h
    src/States/StateTest.h
    src/States/StateTest.cpp
    src/Widgets/ButtonDialogBack.h
    src/Widgets/ButtonDialogBack.cpp
    src/Widgets/ButtonDialogContinue.h
    src/Widgets/ButtonDialogContinue.cpp
    src/Widgets/ButtonDialogSelect.h
    src/Widgets/ButtonDialogSelect.cpp
    src/Widgets/ButtonMainMenu.h
    src/Widgets/ButtonMainMenu.cpp
    src/Widgets/ButtonMainMenuSocial.h
    src/Widgets/ButtonMainMenuSocial.cpp
    src/Widgets/ButtonMainMenuWishlist.h
    src/Widgets/ButtonMainMenuWishlist.cpp
    src/Widgets/ButtonMinimap.h
    src/Widgets/ButtonMinimap.cpp
    src/Widgets/ButtonPanelSelectedObject.h
    src/Widgets/ButtonPanelSelectedObject.cpp
    src/Widgets/ButtonPlanetMap.h
    src/Widgets/ButtonPlanetMap.cpp
    src/Widgets/ButtonQuickUnitSelection.h
    src/Widgets/ButtonQuickUnitSelection.cpp
    src/Widgets/ButtonUnitsSelector.h
    src/Widgets/ButtonUnitsSelector.cpp
    src/Widgets/ButtonPanelTab.h
    src/Widgets/ButtonPanelTab.cpp
    src/Widgets/ButtonsPanel.h
    src/Widgets/ButtonsPanel.cpp
    src/Widgets/CountdownLabel.h
    src/Widgets/CountdownLabel.cpp
    src/Widgets/DialogChangelog.h
    src/Widgets/DialogChangelog.cpp
    src/Widgets/DialogEndMission.h
    src/Widgets/DialogEndMission.cpp
    src/Widgets/DialogExit.h
    src/Widgets/DialogExit.cpp
    src/Widgets/DialogExploreTemple.h
    src/Widgets/DialogExploreTemple.cpp
    src/Widgets/DialogNewElement.h
    src/Widgets/DialogNewElement.cpp
    src/Widgets/DialogSettings.h
    src/Widgets/DialogSettings.cpp
    src/Widgets/DigitsDisplay.h
    src/Widgets/DigitsDisplay.cpp
    src/Widgets/GameButton.h
    src/Widgets/GameButton.cpp
    src/Widgets/GameHUD.h
    src/Widgets/GameHUD.cpp
    src/Widgets/GameMapProgressBar.h
    src/Widgets/GameMapProgressBar.cpp
    src/Widgets/GameProgressBar.h
    src/Widgets/GameProgressBar.cpp
    src/Widgets/GameSimpleTooltip.h
    src/Widgets/GameSimpleTooltip.cpp
    src/Widgets/GameSliderH.h
    src/Widgets/GameSliderH.cpp
    src/Widgets/GameUIData.h
    src/Widgets/GameUIData.cpp
    src/Widgets/MiniMap.h
    src/Widgets/MiniMap.cpp
    src/Widgets/ObjectActionButton.h
    src/Widgets/ObjectActionButton.cpp
    src/Widgets/PanelPlanetActionConquer.h
    src/Widgets/PanelPlanetActionConquer.cpp
    src/Widgets/PanelPlanetActionConquerAI.h
    src/Widgets/PanelPlanetActionConquerAI.cpp
    src/Widgets/PanelPlanetActionExplore.h
    src/Widgets/PanelPlanetActionExplore.cpp
    src/Widgets/PanelObjectActions.h
    src/Widgets/PanelObjectActions.cpp
    src/Widgets/PanelPlanetActions.h
    src/Widgets/PanelPlanetActions.cpp
    src/Widgets/PanelPlanetInfo.h
    src/Widgets/PanelPlanetInfo.cpp
    src/Widgets/PanelPlanetResources.h
    src/Widgets/PanelPlanetResources.cpp
    src/Widgets/PanelResources.h
    src/Widgets/PanelResources.cpp
    src/Widgets/PanelSelectedObject.h
    src/Widgets/PanelSelectedObject.cpp
    src/Widgets/PlanetMap.h
    src/Widgets/PlanetMap.cpp
    src/Widgets/ProgressBarResources.h
    src/Widgets/ProgressBarResources.cpp
    src/Widgets/ResourceDisplay.h
    src/Widgets/ResourceDisplay.cpp
    src/Widgets/SimpleResourceDisplay.h
    src/Widgets/SimpleResourceDisplay.cpp
    src/Widgets/WidgetsConstants.h
    src/Widgets/UnitsSelector.h
    src/Widgets/UnitsSelector.cpp
    src/Widgets/Test/TestComboBox.h
    src/Widgets/Test/TestComboBox.cpp
    src/Widgets/Test/TestSliderH.h
    src/Widgets/Test/TestSliderH.cpp
)

target_include_directories(
    game
    PRIVATE src/
)

target_link_libraries(
    game
    PRIVATE ai
    PRIVATE core
    PRIVATE graphic
    PRIVATE media
    PRIVATE sgui
    PRIVATE utilities
)

# copy data to build directory when CMake is executed
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/)
file(COPY ${CMAKE_SOURCE_DIR}/data/ DESTINATION ${CMAKE_BINARY_DIR}/data/)

# GENERATE Version.h FILE
set(VERSION_FILE ${PROJECT_SOURCE_DIR}/src/Version.h)
# collect data
execute_process(COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_BRANCH)
execute_process(COMMAND git log -1 --format=%H WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_HASH)
execute_process(COMMAND git log -1 --format=%h WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHASH)
execute_process(COMMAND git rev-list HEAD --count WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_NUM)
# strip data of new lines
string(STRIP ${GIT_BRANCH} GIT_BRANCH)
string(STRIP ${GIT_HASH} GIT_HASH)
string(STRIP ${GIT_SHASH} GIT_SHASH)
string(STRIP ${GIT_NUM} GIT_NUM)
# write file
file(WRITE ${VERSION_FILE} "// DO NOT EDIT THIS FILE - it's generated by cmake and any change will be lost\n")
file(APPEND ${VERSION_FILE} "#define VERSION \"${CMAKE_PROJECT_VERSION}\"\n")
file(APPEND ${VERSION_FILE} "#define VERSION_BRANCH \"${GIT_BRANCH}\"\n")
file(APPEND ${VERSION_FILE} "#define VERSION_HASH \"${GIT_HASH}\"\n")
file(APPEND ${VERSION_FILE} "#define VERSION_NUM \"${GIT_NUM}\"\n")
file(APPEND ${VERSION_FILE} "#define VERSION_SHORT_HASH \"${GIT_SHASH}\"\n")
