LIGHT BAR
=========

This project is a collection of files for interfacing with an arduino with an
addressable light strip (I have the LPD8806).  If your model varies, you may
need to alter the ino code.

Goal
----
The goal of the project was to turn the light
strip into a music visualizer.  Each LED in the strip corresponds a frequency
band in the range of a piano.  When passed audio data, the frequencies that
are present light up on the bar.  The intensity that they light up at is
determined by the loudness of that frequency band relative to the loudest thing
the program has "heard" so far.  The light-bar is really better seen than
explained, so please view the videos below if you are interested.
Unfortunately, they are kind of quiet, so be sure to turn the volume up.

- [Radiohead - 15 Steps](http://www.youtube.com/watch?v=so0d3kgLeVM)
- [Sakamoto - Bolerish](http://www.youtube.com/watch?v=Mk16R0fIXag)

Important Files
---------------
Here is a list of the important files in the repo, what they do, and how they
do it.

- led_bar.ino: This program runs on the Arduino itself, and listens for
    commands sent over USB.  A command is an enum, followed by some data for
    that enum.  It supports
    * Setting the bar to a solid color
    * Setting the bar to act as a progress bar
    * Setting the bar to display random colors
    * Setting the bar by individually addressing each LED
- light_bar_controller.py: This is the library that supports sending data to
    the Arduino in the format specified by led_bar.ino
- freq_vu.py: This is the tour de force of the project.  First, freq_vu.py
    takes raw PCM data over stdin.  Then, it performs a Fourier transformation
    on the data to get the frequency information.  The result of this
    transformation must then be linearized because humans perceive exponential
    gaps in frequencies as linear (220Hz is an octave below 440Hz is an octave
    below 880Hz).  Bands of this linearized data are then averaged so that it
    fits into 64 slots, one for each LED.  Lastly, the brightness of the LEDs
    is normalized to the loudest sound the program has "heard" so far.  This
    program also supports colors, but I found that varying the brightness of a
    white light from 0 to 255 was ultimately more informative and visually
    appealing.  The program can be customized by editing the following global
    variables:
      * LEVEL_FILTER: This sets a minimum volume for a frequency to be
        displayed on the bar.  This can help to eliminate the presence of
        harmonics and other notes that humans don't distinguish from appearing
        on the bar.
      * SAMPLE_FREQUENCY: The sample rate of the input audio
      * MAX_FREQUENCY: Highest frequency to display on the bar.  Set to the
        highest note on a piano by default
      * MIN_FREQUENCY: Lowest frequency to display on the bar.  Set to the
        lowest note on a piano by default
      * FUDGE_RANGE: When the Fourier transformed data is linearized, there is
        a chance that we don't have enough information to fill each frequency
        band on the low end of the spectrum.  Thus, setting this to true has
        the program guess what those values might look like based off the
        values we do know.  If you set this to false, there is a chance that
        some LEDs may never light up on the low end of the bar.
- prec.py: This program takes raw PCM data over stdin and displays a progress
    bar illustrating the current average peak volume out of the highest average
    peak volume seen so far.  It looks like a standard VU meter.

Future Ideas
------------
Since my goal for this project was accomplished, development has slowed down a
bit.  However, this is what I may implement in the future
- Offloading Fourier transformation onto the GPU using open_cl
- Taking command line arguments rather than relying on global variables
- Rewriting in C/C++, which seems to have better support for Fast Fourier
    Transformations as well as open_cl libraries.

Pull Requests
-------------
If you write something cool that uses the light_bar_controller, please submit
a pull request and I'll take a look at it!  The bar can do much more than music
of course, and many of my own scripts have nothing to do with music.
