// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RobotUtils.h"

THIRD_PARTY_INCLUDES_START
#undef  PI
#include <kdl/chain.hpp> 
#include <kdl/frames_io.hpp>
#include <kdl/chainiksolverpos_nr.hpp> 
#include <kdl/chainiksolvervel_pinv.hpp> 
#include <kdl/chainiksolverpos_nr_jl.hpp>
#include <kdl/chainiksolverpos_lma.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/chainiksolvervel_pinv.hpp>
#include <kdl/frames.hpp>
#define PI UE_PI
THIRD_PARTY_INCLUDES_END
