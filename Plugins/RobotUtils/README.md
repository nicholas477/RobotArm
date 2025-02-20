# RobotUtils
Unreal wrapper library around [orocos KDL](https://github.com/orocos/orocos_kinematics_dynamics). Used for my game [https://github.com/nicholas477/RobotArm](https://github.com/nicholas477/RobotArm).

Provides a component [RobotJointComponent](Source/RobotUtils/Public/RobotJointComponent.h) for specifying robot joints. From a robot joint component, you can call `MakeChain` to make a kinematic chain, and then pass that to `SolveIK` to generate and apply an IK solution.

## Notes on compiling KDL from source
You don't have to do this, as this library comes with my kdl dll/lib for Windows, but if you wish to compile KDL from source then you will have to compile and link against [my fork of the library](https://github.com/nicholas477/orocos_kinematics_dynamics). This fork makes some changes fixing some ABI incompatibility issues with Unreal. It also changes kdl to compile as a DLL to satisfy the LGPL[^1] linking restrictions that the Unreal EULA[^2] specifies.

[^1]: Orocos KDL is licensed under LGPL: https://orocos.org/content/orocos-licenses.html
[^2]: https://www.unrealengine.com/en-US/eula/unreal
