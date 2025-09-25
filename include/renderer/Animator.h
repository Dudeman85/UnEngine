#pragma once

#include <vector>
#include <map>

#include "ECS.h"
#include "renderer/gl/Texture.h"
#include "renderer/Sprite.h"

namespace une
{
    //Animation struct. Not a component
    struct Animation
    {
        Animation() {};
        Animation(const std::vector<Texture*>& animationTextures, const std::vector<int>& animationDelays);

        std::vector<Texture*> textures;
        std::vector<int> delays;
        unsigned int length = 0;
    };

    //Animator component
    ECS_REGISTER_COMPONENT(Animator)
    struct Animator
    {
        std::map<std::string, Animation> animations;

        float playbackSpeed = 1;

        std::string currentAnimation;
        int animationFrame = 0;
        bool repeatAnimation = false;
        bool playingAnimation = false;

        float animationTimer = 0;

        std::function<void(ecs::Entity)> onAnimationEnd;
    };

	///Animator system, Requires Animator and SpriteRenderer
	ECS_REGISTER_SYSTEM(AnimationSystem, Animator, SpriteRenderer)
	class AnimationSystem : public ecs::System
	{
	public:
		//Update every entity with relevant components
		void Update();

		//Advance to the next animation frame of current animation
		static void AdvanceFrame(ecs::Entity entity);

		//Add animations to entity, they will be accessible by given names
		static void AddAnimations(ecs::Entity entity, std::vector<Animation> animations, std::vector<std::string> names);

		//Add an animation to entity, it will be accessibl by given name
		static void AddAnimation(ecs::Entity entity, Animation animation, std::string name);

		//Play an animation, optionally set it to repeat, if the animation is currently playing don't do anything
		static void PlayAnimation(ecs::Entity entity, std::string animation, bool repeat = false);

		//Stop an animation, optionally provide the specific animation to stop
		static void StopAnimation(ecs::Entity entity, std::string animation = "");
	};

    //Slice spritesheet image to multiple textures.
    //spritesWide is how many sprites wide the spritesheet is and spritesHigh is how many sprites tall the spritesheet is
    std::vector<Texture*> SliceSpritesheet(const char* path, int spritesWide, int spritesHigh);

    //Creates animations from a spritesheet.
    //Each row of sprites on the spritesheet becomes one animation.
    //You must provide a vector of delays for each frame going from top-left to bottom-right
    std::vector<Animation> AnimationsFromSpritesheet(const char* path, int spritesWide, int spritesHigh, std::vector<int> delays);
}