#!/bin/bash
mkdir butler
rm -rfv butler/RobotArm
cp -r Windows/ butler/RobotArm
rm -rfv ./butler/RobotArm/RobotArm/Saved
rm -rfv ./butler/RobotArm/FileOpenOrder
rm -rfv ./butler/RobotArm/Manifest*.txt
./butler.exe push --ignore '*.pdb' butler/ epicgameguy/robot-arm:windows-beta
read -p "Press any key to close window..."

./butler.exe push ../PDF/Manual.pdf epicgameguy/robot-arm:manual