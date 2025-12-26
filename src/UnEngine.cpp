#include "UnEngine.h"

#include "debug/Logging.h"
#include "debug/Timers.h"
#include "debug/GUI.h"
#include "debug/Primitives.h"
#include "utils/ResourceManagement.h"

namespace une
{
	std::string frameTimerString;

	void EngineInit()
	{
		assert(mainWindow && "Make sure to create the main window before initializing UnEngine.");

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
		debug::gui::Init();
	}

	//Setup a new frame. Should be called at the very beginning of a frame
	void BeginFrame()
	{
		debug::StartTimer("FrameStart");
		glfwPollEvents();
		renderer::Clear();
	}

	//Updates all default engine systems
	void Update()
	{
		debug::StartTimer("EngineUpdate");
		frameTimerString = "Completed engine update for frame " + std::to_string(frameCount)
			+ ". Game took " + std::to_string(debug::EndTimer("FrameStart")) + "ms,";

		resources::Update();
		//Update engine systems
		debug::StartTimer("SystemsTimer");
		//Physics must be before collision
		if (enablePhysics)
		{
			physicsSystem->Update();
			frameTimerString += " Physics took " + std::to_string(debug::ResetTimer("SystemsTimer")) + "ms,";
		}
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update();
		if (enableRendering)
		{
			renderer::UnifiedRenderPrepass();
			cameraSystem->Update();
			frameTimerString += " Rendering took " + std::to_string(debug::EndTimer("SystemsTimer")) + "ms,";
		}
		soundSystem->Update();
		//Collision system should be after rendering
		if (enablePhysics)
			collisionSystem->Update();
		//Transform must be after physics, collision and rendering
		transformSystem->Update();

		//Debug stuff
		debug::gui::Update();
	}

	//Runs everything that should happen at the very end of a frame, returns deltaTime
	double EndFrame()
	{
		ecs::Update();
		glfwSwapBuffers(mainWindow->glWindow);
		timerSystem->Update(enablePhysics);

		debug::LogSpam(frameTimerString + " Total: " + std::to_string(debug::EndTimer("EngineUpdate")) + "ms");

		return deltaTime;
	}

	void UnInit()
	{
		debug::gui::UnInit();
		delete mainWindow;
	}
}
