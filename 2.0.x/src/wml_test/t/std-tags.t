
require "TEST.pl";
&TEST::init;

print "1..2\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::std::tags
<define-tag nesting endtag=required whitespace=delete>
<perl>
    <perl:assign:sq $body>%body</perl:assign:sq>
    <perl:print: $body />
</perl>
</define-tag>
<nesting><nesting>ABC</nesting></nesting>
<nesting>D<nesting>E</nesting>F</nesting>
EOT_IN
ABC
DEF
EOT_OUT

&TEST::cleanup;

