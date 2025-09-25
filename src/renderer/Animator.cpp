#include "renderer/Animator.h"

#include <cmath>
#include <iostream>
#include <cassert>

#include "utils/Timing.h"

namespace une
{
	Animation::Animation(const std::vector<Texture*>& animationTextures, const std::vector<int>& animationDelays)
	{
		assert(animationTextures.size() == animationDelays.size() && "Failed to create animation! Number of frames and delays do not match!");

		textures = animationTextures;
		delays = animationDelays;
		length = animationDelays.size();
	}

	//Update every entity with relevant components
	void AnimationSystem::Update()
	{
		//For each entity that has the required components
		for (ecs::Entity entity : entities)
		{
			//Get the relevant components from entity
			Animator& animator = ecs::GetComponent<Animator>(entity);

			//If the entity is currently playing an animation
			if (animator.playingAnimation)
			{
				animator.animationTimer += deltaTime * 1000 * animator.playbackSpeed;

				//If enough time (defined by animation) has passed advance the animation frame
				if (animator.animationTimer >= animator.animations[animator.currentAnimation].delays[animator.animationFrame])
				{
					AdvanceFrame(entity);
				}
			}
		}
	}

	///Advance to the next animation frame of current animation
	void AnimationSystem::AdvanceFrame(ecs::Entity entity)
	{
		//Get the relevant components from entity
		Animator& animator = ecs::GetComponent<Animator>(entity);
		SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);

		//Change Sprites texture
		sprite.texture = animator.animations[animator.currentAnimation].textures[animator.animationFrame];

		animator.animationFrame++;
		animator.animationTimer = 0;

		//If end of animation has been reached go to start or end animation
		if (animator.animationFrame >= animator.animations[animator.currentAnimation].length)
		{
			animator.animationFrame = 0;
			animator.animationTimer = 0;

			//End the animation if it is not set to repeat
			if (!animator.repeatAnimation)
			{
				animator.playingAnimation = false;
				animator.currentAnimation = "";
				//Call callback if applicable
				if (animator.onAnimationEnd)
					animator.onAnimationEnd(entity);
				return;
			}
		}
	}

	///Add animations to entity, they will be accessible by given names
	void AnimationSystem::AddAnimations(ecs::Entity entity, std::vector<Animation> animations, std::vector<std::string> names)
	{
		if (animations.size() > names.size())
			throw("Not enough names given for each animation!");

		Animator& animator = ecs::GetComponent<Animator>(entity);

		//For each animation to add
		for (size_t i = 0; i < animations.size(); i++)
		{
			animator.animations.insert({ names[i], animations[i] });
		}
	}

	///Add an animation to entity, it will be accessibl by given name
	void AnimationSystem::AddAnimation(ecs::Entity entity, Animation animation, std::string name)
	{
		Animator& animator = ecs::GetComponent<Animator>(entity);

		//Add the animation indexed by given name
		animator.animations.insert({ name, animation });
	}

	///Play an animation, optionally set it to repeat, if the animation is currently playing don't do anything
	void AnimationSystem::PlayAnimation(ecs::Entity entity, std::string animation, bool repeat)
	{
		Animator& animator = ecs::GetComponent<Animator>(entity);

		if (animator.animations.find(animation) == animator.animations.end())
		{
			std::cout << "Warning: No animation named \"" << animation << "\" was found in this entity." << std::endl;
			return;
		}

		if (animator.currentAnimation == animation)
			return;

		animator.currentAnimation = animation;
		animator.animationFrame = 0;
		animator.repeatAnimation = repeat;
		animator.playingAnimation = true;
		animator.animationTimer = 0;

		//Immediately advance to 1st frame of animation
		AdvanceFrame(entity);
	}

	///Stop an animation, optionally provide the specific animation to stop
	void AnimationSystem::StopAnimation(ecs::Entity entity, std::string animation)
	{
		Animator& animator = ecs::GetComponent<Animator>(entity);

		//If trying to stop animation that is not playing, return without doing anything
		if (animation != "")
			if (animator.currentAnimation != animation)
				return;

		animator.currentAnimation = "";
		animator.animationFrame = 0;
		animator.animationTimer = 0;
		animator.playingAnimation = false;
	}

	//Slice spritesheet image to multiple textures.
	//spritesWide is how many sprites wide the spritesheet is and spritesHigh is how many sprites tall the spritesheet is
	std::vector<Texture*> SliceSpritesheet(const char* path, int spritesWide, int spritesHigh)
	{
		std::vector<Texture*> slicedTextures;

		//Load the spritesheet form path
		Image spritesheet = Image(path);

		//Get the size of each sprite
		const int width = std::floor((spritesheet.width - 1) / spritesWide);
		const int height = std::floor((spritesheet.height - 1) / spritesHigh);

		//Warn if the spritesheet is weirdly sized
		if (spritesheet.width % spritesWide != 0 || spritesheet.height % spritesHigh != 0)
			std::cout << "Spritesheet is not divisible by sprite count. Clipping may occur!";

		//For each sprite to slice out
		int paddingY = 0;
		for (size_t row = 0; row < spritesHigh; row++)
		{
			int paddingX = 0;
			for (size_t col = 0; col < spritesWide; col++)
			{
				//Get the slice from the spritesheet
				Image slice = spritesheet.Slice(col * width + paddingX, row * height + paddingY, col * width + width + paddingX, row * height + height + paddingY);
				slicedTextures.push_back(new Texture(slice));
				paddingX++;
			}
			paddingY++;
		}

		return slicedTextures;
	}

	//Creates animations from a spritesheet.
	//Each row of sprites on the spritesheet becomes one animation.
	//You must provide a vector of delays for each frame going from top-left to bottom-right
	std::vector<Animation> AnimationsFromSpritesheet(const char* path, int spritesWide, int spritesHigh, std::vector<int> delays)
	{
		std::vector<Texture*> allFrames = SliceSpritesheet(path, spritesWide, spritesHigh);

		std::vector<Animation> animations;

		assert(allFrames.size() == delays.size() && "You must provide a delay for after each animation frame!");

		//For each animation (row in the spritesheet)
		for (size_t i = 0; i < spritesHigh; i++)
		{
			//Slice the proper textures to a new vector
			std::vector<Texture*> frames(spritesWide);
			copy(allFrames.begin() + i * spritesWide, allFrames.begin() + (i + 1) * spritesWide, frames.begin());

			//Slice the proper timings to a new vector
			std::vector<int> slicedDelays(spritesWide);
			copy(delays.begin() + i * spritesWide, delays.begin() + (i + 1) * spritesWide, slicedDelays.begin());

			//Create a new animation out of the sliced data
			animations.push_back(Animation(frames, slicedDelays));
		}

		return animations;
	}
}