Why Bugel?
==========

Bugel is a tool for marking timings in a song, then turning those timings into something meaningful.

I wrote Bugel because I was making a game and my in-engine timeline editor was spectacularly bad. I couldn't find a tool that did what I wanted (although I didn't look very hard). I am making it publicly available because I think it might work for other peoples' projects.

If you're making a "real" rhythm game like DDR or Guitar Hero, you are probably better off with a specialized editor. There are only 4-5 possible "events" in those games, so an editor doesn't have to be very complicated.

But what if you have many types of events? What if those each of those events can have several parameters?

You may be able to easily find the timing you want, but you'll probably need to do a lot of iterations to turn it into fun gameplay. Events in one "layer" commonly depend on one another; tweaking one "stat" might require changing 20 events. With Bugel's post-processing, you just tweak one line of your script and instantly update 20 events.


How It Works
============

* A **timeline** holds information like backing track, BPM of backing track, and multiple timeline **layer**s.
* A **layer* has a name, post-processing script, and list of events.

* Use Bugel's GUI to add **placeholder event**s to a layer in time with music.
  - This is done by pressing the 1-9 keys on your keyboard with a layer selected as the song plays.
  - You can also add actual events directly into the timeline. (NOT DONE YET)
* Write a **post-processing script** in Lua to convert **placeholder event**s into actual events that your target application.
  - This allows you to easily keep event timing, but change what your events do.
  - You can also create additional events, as well as adjust timing. For example, you might want to add a "start windup animation" event 0.5 seconds before your attack event.
* Export your post-processed timeline as JSON. XML possibly coming later.


TODO
====

* BPM Snapping
* Undo/redo
* Add tooltip properties display to TimelineEventsWidget
* Event type creator GUI
* Remove type and time arguments from addEvent, make it just one table
* A way to pass arguments from GUI to script
