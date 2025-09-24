#include <UnEngine.h>

namespace engine
{
	void EngineInit()
	{
		//Get the engine systems
		timerSystem = ecs::GetSystem<TimerSystem>();
		timerSystem->Init();
		collisionSystem = ecs::GetSystem<CollisionSystem>();
		physicsSystem = ecs::GetSystem<PhysicsSystem>();
		modelRenderSystem = ecs::GetSystem<ModelRenderSystem>();
		modelRenderSystem->Init();
		animationSystem = ecs::GetSystem<AnimationSystem>();
		spriteRenderSystem = ecs::GetSystem<SpriteRenderSystem>();
		spriteRenderSystem->Init();
		textRenderSystem = ecs::GetSystem<TextRenderSystem>();
		textRenderSystem->Init();
		primitiveRenderSystem = ecs::GetSystem<PrimitiveRenderSystem>();
		primitiveRenderSystem->Init();
		soundSystem = ecs::GetSystem<SoundSystem>();
		transformSystem = ecs::GetSystem<TransformSystem>();
		ecs::SetComponentDestructor<Transform>(TransformSystem::OnTransformRemoved);
	}

	//Updates all default engine systems, returns delta time
	double Update(Camera* cam)
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update engine systems
		//Physics must be before collision
		if (enablePhysics)
			physicsSystem->Update();
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update();
		if (enableRendering)
		{
			//ModelRenderer must be before SpriteRenderer
			modelRenderSystem->Update(cam);
			spriteRenderSystem->Update(cam);
			modelRenderSystem->DrawUIElements(cam); //This is a bandaid patch for UI models
			textRenderSystem->Update(cam);
			primitiveRenderSystem->Update(cam);
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