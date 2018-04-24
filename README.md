# Obelisk
A tower-defence card game for Ludum Dare 41 ("combine two incompatible genres")

Attempt to survive the hordes of enemies for 5 minutes to win. Let 3 through and you lose. It's absolutely possible to win, although victory can often be quite close!

Be careful when playing upgrades (green cards) - towers can't attack while upgrading, but you'll succumb to the later waves if you don't upgrade enough!

## Controls
- Left click to pick up cards, draw cards and play cards on tower blocks. Note that you cannot drag and drop cards.
- Right click to drop a card without playing.
- If your hand is full, you can play a card on the trashcan to delete it and draw another card

## About
Obelisk is a combination of tower defence and a card game based upon the theme of "combining two incompatible genres"

Draw cards, and play them to power up your defence. Manage your resources and defeat the attackers for as long as you can!

All code was written for Ludum Dare 41.

Obelisk was built using my own C++ game library APG and my ECS framework AshleyCPP, which were both created before Ludum Dare.

The art is from Kenney.nl and was not created for the jam.

## Devlog
### Day 3: 23-04-2018
Both of my previous devlogs had an early segfault, and today wasn't much different. This time, it was a use-after-free in my ECS library, [AshleyCPP](https://github.com/SgtCoDFish/AshleyCPP/commit/40499bb878a647dac35ef38bf3b19052c8229927). I'm amazed that didn't blow up in a really obvious way a long time ago.

Today was a mixture of a lot of everything and, as so often happens on the final day, a bit of a blur. I did consider distribution on native platforms, but in the end decided that HTML5/WebAssembly was good enough, and that's what I stuck with. Not having to deal with Visual Studio is a win in my books, too!

As far as gameplay goes, I'm actually pleasently surprised with myself at how it turned out; whether that's more by luck or judgement I'm not sure. In the games I played and won, it was usually a nail-biter of a finish. That could potentially mean that the game's slightly too hard given I've got the advantage of having made the game, but I guess we'll see!

Another big change today was "crushing" the game world so it would fit in a small itch.io window. I think making it smaller actually improved the feel of the game, too in a non-quantifiable way.

I'm glad I participated, but after this and my 7DRL game, [ARDOR](https://github.com/SgtCoDFish/ardor) I definitely need a break from game dev for a while!

### Day 2: 22-04-2018
The best way to start the morning was, I decided, to test the build on my macOS machine using clang, since I'd spent
the first day entirely on Debian Stretch with gcc. This immediately highlighted a missing include, and then more urgently
highlighted a segfault which doesn't occur under Linux. Running valgrind didn't help, as it immediately died complaining
of an illegal processor instruction, which seems to be an upstream bug.

The fix was easy enough once I'd found a sane way to debug it; turns out Linux is more forgiving when it comes to accessing OpenGL
functions before a context has been created. This kind of bug is common in APG, since it tries to do the heavy lifting
for you and initialise all of SDL2 and OpenGL. In a future refactor I think I'll have to think of sacrificing a little of
the veil of magic and forcing the user to call a provided function to initialise SDL/GL. This will also remove a good chunk
of the `unique_ptr` "hell" that's all over the APG codebase, limiting use of regular objects.

I cracked on again, and by the end of the day had a much more smooth, game-like application, without actually having any real
gameplay once again. That said, I feel like the gameplay will flow pretty quickly on Day 3 - I've done a lot of prep work.

At the end of the day I did a little testing of emscripten, which worked fantastically. I'm very hopeful that distributing
the game in the browser will work, making native build gravy rather than a requirement.


### Day 1: 21-04-2018
I started Ludum Dare as I often start a game jam; throwing together a build system I can live with, spending slightly
too long on it and then getting started properly after that. It seems to be a constant theme for me that my code and
build scripts are too engineered for game-jams.

That said, given that I approach most game jams either as a learning experience or the chance to give my
game engines / frameworks a test, it's not the worst thing in the world to spend a little extra time on the quality.

Pretty quickly, I ran into a segfault which I'd managed to introduce during my work on [APG](https://github.com/SgtCoDFish/APG)
earlier in the week, while preparing for Ludum Dare. It wasn't too bad to fix, especially with CLion's valgrind
integration.

Another obvious choice was that since I was working alone, I'd be using whatever I could find for free in terms of
sprites, fonts, graphics and music. [Kenney](http://kenney.nl/) was an excellent source of art, as ever.

I didn't really get any gameplay in, which was sort of my goal. I anticipate I should have some actual game mechanics
done by about halfway tomorrow, giving me about a day to polish and about half a day to prepare for distribution.

For now, sleep.
