#pragma once
#define WIN32_LEAN_AND_MEAN

#include <memory>

#include "ECS.h"
#include "renderer/gl/Window.h"
#include "renderer/UnifiedRenderer.h"
#include "renderer/TilemapRenderer.h"
#include "renderer/SpriteRenderer.h"
#include "renderer/ModelRenderer.h"
#include "renderer/TextRenderer.h"
#include "renderer/PrimitiveRenderer.h"
#include "renderer/Animator.h"
#include "audio/SoundSystem.h"
#include "Transform.h"
#include "Physics.h"
#include "Collision.h"
#include "utils/Timing.h"

namespace une
{
	//If true updates physics and collision systems
	inline bool enablePhysics = true;
	//If true updates animation system
	inline bool enableAnimation = true;
	//If true enables sprite, model, text, and primitive rendering systems
	inline bool enableRendering = true;

	//Engine system pointers
	inline std::shared_ptr<TimerSystem> timerSystem;
	inline std::shared_ptr<TransformSystem> transformSystem;
	inline std::shared_ptr<CollisionSystem> collisionSystem;
	inline std::shared_ptr<PhysicsSystem> physicsSystem;
	inline std::shared_ptr<SoundSystem> soundSystem;
	inline std::shared_ptr<AnimationSystem> animationSystem;
	inline std::shared_ptr<CameraSystem> cameraSystem;
	inline std::shared_ptr<renderer::PrimitiveRenderSystem> primitiveRenderSystem;
	inline std::shared_ptr<renderer::SpriteRenderSystem> spriteRenderSystem;
	inline std::shared_ptr<renderer::ModelRenderSystem> modelRenderSystem;
	inline std::shared_ptr<renderer::TextRenderSystem> textRenderSystem;
	inline std::shared_ptr<renderer::TilemapRenderSystem> tilemapRenderSystem;

	void EngineInit();

	//Updates all default engine systems, returns delta time
	double Update();
}