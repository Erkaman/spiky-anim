# Spiky Animation Source Code

The source code of some silly spiky animation I made. This program
creates a bunch of SVGs(they are the frames of the animation),
converts them all to png, and then makes a mp4 video file from them.

![Animated](images/spiky.gif)

## Making the Animation

You can build using make:

```
make
```

You can now generate all frames by doing

```
./spiky
```

this will generate all frames into `temp/`, and put a video file in
`./out.mp4`. But this will only work if you have the programs
`rsvg-convert` and `ffmpeg` installed! And it will probably only work on
Linux or OS X. You can now convert `out.mp4` to a nice gif by using for
instance [gifgen](https://github.com/lukechilds/gifgen):

```
gifgen -f 30 -o spiky.gif out.mp4
```
