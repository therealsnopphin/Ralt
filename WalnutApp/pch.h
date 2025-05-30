#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>

#include <fmod.hpp>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Layer.h"
#include "Walnut/Random.h"

#include "misc\cpp\imgui_stdlib.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <omp.h> 
#include <thread>

#include "../stb_image/stb_image.h"

#include "monsters/view and logic/sprite.h"
#include "monsters/view and logic/camera.h"
#include "monsters/level/world.h"
#include "monsters/level/item.h"
#include "monsters/level/inventory.h"
#include "monsters/layers/menulayer.h"
#include "monsters/network/SimpleUDPSocket.h"
#include "monsters/level/player.h"
#include "monsters/view and logic/renderer.h"
#include "monsters/level/enemy.h"
#include "monsters/layers/playlayer.h"