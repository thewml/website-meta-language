
require "TEST.pl";
&TEST::init;

print "1..2\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::std::href
<href url="THE://URL">
<href url="THE://URL" name="THE_NAME">
<href url="THE://URL" image="THE_IMG.EXT">
<href url="THE://URL" name="THE_NAME" image="THE_IMG.EXT">
<href url="THE://URL" hint="THE_HINT">
<href url="THE://URL" name="THE_NAME" image="THE_IMG.EXT" hint="THE_HINT">
EOT_IN
<a href="THE://URL"><tt>THE://URL</tt></a>
<a href="THE://URL">THE_NAME</a>
<a href="THE://URL"><img src="THE_IMG.EXT" alt="THE://URL" border="0"></a>
<a href="THE://URL"><img src="THE_IMG.EXT" alt="THE_NAME" border="0"></a>
<a href="THE://URL" onmouseover="self.status='THE_HINT';return true" onmouseout="self.status='';return true" onfocus="self.status='THE_HINT';return true" onblur="self.status='';return true"><tt>THE://URL</tt></a>
<a href="THE://URL" onmouseover="self.status='THE_HINT';return true" onmouseout="self.status='';return true" onfocus="self.status='THE_HINT';return true" onblur="self.status='';return true"><img src="THE_IMG.EXT" alt="THE_NAME" border="0"></a>
EOT_OUT

&TEST::cleanup;

