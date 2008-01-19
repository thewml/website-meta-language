 Term::ReadKey 2.13 - Change terminal modes, and perform non-blocking reads.

 Copyright (C) 1994-1999 Kenneth Albanowski. Unlimited distribution and/or
 modification is allowed as long as this copyright notice remains intact.

This module, ReadKey, provides ioctl control for terminals and Win32
consoles so the input modes can be changed (thus allowing reads of a single
character at a time), and also provides non-blocking reads of stdin, as well
as several other terminal related features, including retrieval/modification
of the screen size, and retrieval/modification of the control characters.
Installation requires MakeMaker 3.5 or higher (MakeMaker 3.7 is included
with perl 5.001, so now is a good time to upgrade if you haven't already.)

To install, unpack somewhere, type "perl Makefile.PL", and then "make test".
If the compilation and the tests are successful, then change to root and run
"make install".

New in version 2.10 is support for Win32. This has only been tested with one
particular binary distribution and compiler, so it may need a bit of
exercise. There is are also some limitations, with the ReadLine call being
unavailable, and ReadKey possibly generating bad results if you are reading
from multiple consoles, and key repeat is used.

VERY IMPORTANT: In 2.00, the ReadKey/ReadLine arguments changed. Now, if
you want a call that is non-blocking and returns immediately if no
character is waiting, please call it with -1, instead of 1. Positive
arguments now indicate a timeout, so 1 would wait a second before timing
out.

As older versions will accept -1, it is reccomended to change all code 
that uses ReadMode.


The terminal mode function is controlled by the "ReadMode" function, which
takes a single numeric argument, and an optional filehandle. This argument
should be one of the following:

	0: (Reset) Restore original settings.

	1: (Cooked) Change to what is commonly the default mode, echo on,
           buffered, signals enabled, Xon/Xoff possibly enabled, and 8-bit mode 
	   possibly disabled.

	2: (Cooked-Invisible) Same as 1, just with echo off. Nice for reading 
           passwords.

	3: (CBreak) Echo off, unbuffered, signals enabled, Xon/Xoff possibly 
           enabled, and 8-bit mode possibly enabled.

	4: (Raw) Echo off, unbuffered, signals disabled, Xon/Xoff disabled, 
           and 8-bit mode possibly disabled.

	5: (Really-Raw) Echo off, unbuffered, signals disabled, Xon/Xoff 
           disabled, 8-bit mode enabled if parity permits, and CR to CR/LF 
           translation turned off. 

If you just need to read a key at a time, then modes 3 or 4 are probably
sufficient. Mode 4 is a tad more flexible, but needs a bit more work to
control. If you use ReadMode 3, then you should install a SIGINT or END
handler to reset the terminal (via ReadMode 0) if the user aborts the
program via ^C. (For any mode, an END handler consisting of "ReadMode 0" is
actually a good idea.)

Non-blocking support is provided via the ReadKey and ReadLine functions. If
they are passed no argument, or an argument of zero, they will act like a
normal getc(STDIN) or scalar(<STDIN>). If they are passed a negative
argument, then they will immediatly return undef if no input is present. If
passed a positive argument, then they will wait until that time in seconds
has passed before returning undef. In most situations, you will probably
want to use "ReadKey -1".

Note that a non-blocking ReadLine probably won't do what you expect,
although it is perfectly predictable, and that the ReadMode will have to be
1 or 0 for it to make sense at all.

A routine is also provided to get the current terminal size,
"GetTerminalSize". This will either return a four value array containing the
width and height of the screen in characters and then in pixels, or nothing
( if the OS can't return that info). SetTerminalSize allows the stored
settings to be modified. Note that this does _not_ change the physical size
of the screen, it will only change the size reported by GetTerminalSize, and
other programs that check the terminal size in the same manner.

GetControlChars returns a hash containing all of the valid control
characters, such as ("INTERRUPT" => "\x3", etc.). SetControlChars takes an
array (or a hash) as a parameter that should consist of similar name/value
pairs and will modify the control character settings.

Note that it is entirely possible that there are portability problems with
the routines in ReadKey.xs. If you find any problems, including compilation
failures, or control characters not supported by Set/GetControlChars,
_please_ tell me about them, by mailing me at kjahds@kjahds.com, or
CIS:70705,126, or lastly contacting perl5-porters@nicoh.com. Any problems
will get fixed if at all possible, but that's not going to happen if I don't
know about them.

Oh, you may also be interested in the Configure.pm module. It provides tools
to make porting stuff easier -- calling the compiler, finding headers, etc.
It contains documentation inside it, and you are welcome to use it in your
own modules. If you make use of it, I'd be grateful for a message sent to
the above address.
