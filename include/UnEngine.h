#pragma once
#define WIN32_LEAN_AND_MEAN

#include <memory>

#include "ECS.h"
#include "renderer/UnifiedRenderer.h"
#include "renderer/Sprite.h"
#include "renderer/ModelSystem.h"
#include "renderer/gl/Window.h"
#include "renderer/TextRender.h"
#include "renderer/Primitive.h"
#include "renderer/Animator.h"
#include "audio/SoundComponent.h"
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

	//Engine system pointers (for peak performance)
	inline std::shared_ptr<TimerSystem> timerSystem;
	inline std::shared_ptr<CollisionSystem> collisionSystem;
	inline std::shared_ptr<PhysicsSystem> physicsSystem;
	inline std::shared_ptr<AnimationSystem> animationSystem;
	inline std::shared_ptr<ModelRenderSystem> modelRenderSystem;
	inline std::shared_ptr<renderer::SpriteRenderSystem> spriteRenderSystem;
	inline std::shared_ptr<TextRenderSystem> textRenderSystem;
	inline std::shared_ptr<renderer::PrimitiveRenderSystem> primitiveRenderSystem;
	inline std::shared_ptr<TransformSystem> transformSystem;
	inline std::shared_ptr<SoundSystem> soundSystem;

	void EngineInit();

	//Updates all default engine systems, returns delta time
	double Update(Camera* cam);
}