
require "TEST.pl";
&TEST::init;

print "1..10\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::typography
<headline>This is a Headline</headline>

<pi>
This paragraph has an indented first line.
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 

<p>
<subheadline>This is a Sub-Headline</subheadline>

Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
EOT_IN
<font face="Arial,Helvetica" size="+4"><b>This is a Headline</b></font>
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="5" align="bottom" border="0"><br>
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="40" height="16" align="bottom" border="0">
This paragraph has an indented first line.
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
<p>
<font face="Arial,Helvetica" size="+2"><b>This is a Sub-Headline</b></font>
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="2" align="bottom" border="0"><br>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::typography
<p>
<spaced interchar=1>This is spaced text</spaced><br>
<p>
<spaced interchar=2>This is more spaced text</spaced>
EOT_IN
<p>
T&nbsp;h&nbsp;i&nbsp;s&nbsp;&nbsp; i&nbsp;s&nbsp;&nbsp; s&nbsp;p&nbsp;a&nbsp;c&nbsp;e&nbsp;d&nbsp;&nbsp; t&nbsp;e&nbsp;x&nbsp;t&nbsp;
<br>
<p>
T&nbsp;&nbsp;h&nbsp;&nbsp;i&nbsp;&nbsp;s&nbsp;&nbsp;&nbsp;&nbsp; i&nbsp;&nbsp;s&nbsp;&nbsp;&nbsp;&nbsp; m&nbsp;&nbsp;o&nbsp;&nbsp;r&nbsp;&nbsp;e&nbsp;&nbsp;&nbsp;&nbsp; s&nbsp;&nbsp;p&nbsp;&nbsp;a&nbsp;&nbsp;c&nbsp;&nbsp;e&nbsp;&nbsp;d&nbsp;&nbsp;&nbsp;&nbsp; t&nbsp;&nbsp;e&nbsp;&nbsp;x&nbsp;&nbsp;t&nbsp;&nbsp;
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::typography
<p>
<spaced interline=1>
This is spaced text, including interline spacing.
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
This is spaced text, including interline spacing.
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
This is spaced text, including interline spacing.
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
This is spaced text, including interline spacing.
</spaced>
EOT_IN
<p>
This<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> is spaced text, including interline<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> spacing.
Foo bar quux Foo<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> bar quux Foo bar quux<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> Foo bar quux Foo bar<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> quux
This is spaced text,<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> including interline spacing.
Foo bar<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> quux Foo bar quux Foo<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> bar quux Foo bar quux<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> Foo bar quux
This is<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> spaced text, including interline spacing.<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt="">
Foo bar quux Foo bar<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> quux Foo bar quux Foo<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> bar quux Foo bar quux<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt="">
This is spaced text, including<img src="imgdot-1x1-transp-ffffff.gif" width="1" height="15" alt=""> interline spacing.
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::typography
<p>
<sc>This is Small Caps text</sc><br>
<p>
<sc complete>This is Complete Small Caps text</sc>
EOT_IN
<p>
T<font size="-1">HIS</font> <font size="-1">IS</font> S<font size="-1">MALL</font> C<font size="-1">APS</font> <font size="-1">TEXT</font><br>
<p>
T<font size="-1">HIS</font> I<font size="-1">S</font> C<font size="-1">OMPLETE</font> S<font size="-1">MALL</font> C<font size="-1">APS</font> T<font size="-1">EXT</font>
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::typography
<p>
This is a typographically more strong list environment
<tul>
   <tli>Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
   <tli pcolor="#ff3333">Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
        Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
	<tul>
	   <tli>Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
	   <tli>Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
			Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
	   <tli>Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
			Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
	</tul>
   <tli>Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
        Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux 
</tul>
EOT_IN
<p>
This is a typographically more strong list environment
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
<table cellspacing="0" cellpadding="0" border="0" summary="">
<tr>
<td valign="top"><img src="imgdot-1x1-transp-ffffff.gif" alt="" width="30" height="1" align="bottom" border="0"></td><td valign="top"><img src="imgdot-1x1-transp-ffffff.gif" alt="" width="30" height="1" align="bottom" border="0"><br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-cccccc.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-ff3333.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
        Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
<table cellspacing="0" cellpadding="0" border="0" summary="">
<tr>
<td valign="top"><img src="imgdot-1x1-transp-ffffff.gif" alt="" width="30" height="1" align="bottom" border="0"></td><td valign="top"><img src="imgdot-1x1-transp-ffffff.gif" alt="" width="30" height="1" align="bottom" border="0"><br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-cccccc.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-cccccc.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
			Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-cccccc.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
			Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux</td>
</tr>
</table>
<br>
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0"><br>
</td></tr>
<tr><td valign="top" align="right"><img src="imgdot-1x1-cccccc.gif" alt="" width="10" height="10" align="bottom" border="0">&nbsp;&nbsp;</td><td>
Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux
        Foo bar quux Foo bar quux Foo bar quux Foo bar quux Foo bar quux</td>
</tr>
</table>
EOT_OUT

push(@TEST::TMPFILES, "imgdot-1x1-cccccc.gif");
push(@TEST::TMPFILES, "imgdot-1x1-ff3333.gif");
push(@TEST::TMPFILES, "imgdot-1x1-transp-ffffff.gif");
&TEST::cleanup;

