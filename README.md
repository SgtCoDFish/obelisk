# Obelisk
A tower-defence card game - theme "combine two incompatible genres"

## Devlog
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

#### Day 1 Progress
I didn't really get any gameplay in, which was sort of my goal. I anticipate I should have some actual game mechanics
done by about halfway tomorrow, giving me about a day to polish and about half a day to prepare for distribution.

For now, sleep.
