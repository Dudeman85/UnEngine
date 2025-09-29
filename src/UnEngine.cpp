#include "UnEngine.h"

namespace une
{
	void EngineInit()
	{
		//Get the engine systems
		timerSystem = ecs::GetSystem<TimerSystem>();
		timerSystem->Init();
		transformSystem = ecs::GetSystem<TransformSystem>();
		ecs::SetComponentDestructor<Transform>(TransformSystem::OnTransformRemoved);
		collisionSystem = ecs::GetSystem<CollisionSystem>();
		physicsSystem = ecs::GetSystem<PhysicsSystem>();
		soundSystem = ecs::GetSystem<SoundSystem>();
		animationSystem = ecs::GetSystem<AnimationSystem>();
		primitiveRenderSystem = ecs::GetSystem<renderer::PrimitiveRenderSystem>();
		spriteRenderSystem = ecs::GetSystem<renderer::SpriteRenderSystem>();
		modelRenderSystem = ecs::GetSystem<renderer::ModelRenderSystem>();
		textRenderSystem = ecs::GetSystem<renderer::TextRenderSystem>();
		renderer::Init();
	}

	//Updates all default engine systems, returns delta time
	double Update(Camera* cam)
	{
		renderer::UnifiedRenderPrepass();
		//Update engine systems
		//Physics must be before collision
		if (enablePhysics)
			physicsSystem->Update();
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update();
		if (enableRendering)
		{
			renderer::UnifiedRenderPass(cam);
		}
		soundSystem->Update();
		//Collision system should be after rendering
		if (enablePhysics)
			collisionSystem->Update(cam);
		//Transform must be after physics, collision and rendering
		transformSystem->Update();
		//Timer must be last
		timerSystem->Update(enablePhysics);

		return deltaTime;
	}
}