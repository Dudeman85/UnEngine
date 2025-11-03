#include "UnEngine.h"

#include "Debug.h"

namespace une
{
	void EngineInit()
	{
		assert(mainWindow && "Make sure to create the main window before initializing the engine.");

		//Get the engine systems
		timerSystem = ecs::GetSystem<TimerSystem>();
		timerSystem->Init();
		transformSystem = ecs::GetSystem<TransformSystem>();
		ecs::SetComponentDestructor<Transform>(TransformSystem::OnTransformRemoved);
		collisionSystem = ecs::GetSystem<CollisionSystem>();
		physicsSystem = ecs::GetSystem<PhysicsSystem>();
		soundSystem = ecs::GetSystem<SoundSystem>();
		animationSystem = ecs::GetSystem<AnimationSystem>();
		cameraSystem = ecs::GetSystem<CameraSystem>();
		primitiveRenderSystem = ecs::GetSystem<renderer::PrimitiveRenderSystem>();
		spriteRenderSystem = ecs::GetSystem<renderer::SpriteRenderSystem>();
		modelRenderSystem = ecs::GetSystem<renderer::ModelRenderSystem>();
		textRenderSystem = ecs::GetSystem<renderer::TextRenderSystem>();
		tilemapRenderSystem = ecs::GetSystem<renderer::TilemapRenderSystem>();
		renderer::Init();
	}

	//Updates all default engine systems, returns delta time
	double Update()
	{
		debug::StartTimer("EngineUpdate");
		std::string debugString = "Completed engine update for frame " + std::to_string(frameCount) + ".";

		//Update engine systems
		debug::StartTimer("SystemsTimer");
		//Physics must be before collision
		if (enablePhysics)
		{
			physicsSystem->Update();
			debugString += " Physics took " + std::to_string(debug::ResetTimer("SystemsTimer")) + "ms,";
		}
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update();
		if (enableRendering)
		{
			renderer::UnifiedRenderPrepass();
			cameraSystem->Update();
			debugString += " Rendering took " + std::to_string(debug::EndTimer("SystemsTimer")) + "ms,";
		}
		soundSystem->Update();
		//Collision system should be after rendering
		if (enablePhysics)
			collisionSystem->Update();
		//Transform must be after physics, collision and rendering
		transformSystem->Update();
		//Timer must be last
		timerSystem->Update(enablePhysics);

		ecs::Update();

		glfwSwapBuffers(mainWindow->glWindow);
		glfwPollEvents();

		debug::LogSpam(debugString + " Total: " + std::to_string(debug::EndTimer("EngineUpdate")) + "ms");
		return deltaTime;
	}
}
