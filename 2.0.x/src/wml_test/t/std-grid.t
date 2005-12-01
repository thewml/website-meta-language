
require "TEST.pl";
&TEST::init;

print "1..2\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::std::grid
<grid layout=2x3 align=lr valign=tbm>
  <cell>This is Cell #1 of Grid #1</cell>
  <cell>This is Cell #2 of Grid #2:
    <grid layout=2x2>
      <cell>A</cell>
      <cell>B</cell>
      <cell>C</cell>
      <cell>D</cell>
    </grid>
  </cell>
  <cell>This is Cell #3 of Grid #1</cell>
  <cell>This is Cell #4 of Grid #1</cell>
  <cell>This is Cell #5 of Grid #1</cell>
  <cell>This is Cell #6 of Grid #1</cell>
</grid>
EOT_IN
<table border="0" cellspacing="0" cellpadding="0" summary="">
  <tr>
    <td align="left" valign="top">This is Cell #1 of Grid #1</td>
    <td align="right" valign="top">This is Cell #2 of Grid #2:
    <table border="0" cellspacing="0" cellpadding="0" summary="">
      <tr>
        <td align="left" valign="top">A</td>
        <td align="left" valign="top">B</td>
      </tr>
      <tr>
        <td align="left" valign="top">C</td>
        <td align="left" valign="top">D</td>
      </tr>
    </table>
  </td>
  </tr>
  <tr>
    <td align="left" valign="bottom">This is Cell #3 of Grid #1</td>
    <td align="right" valign="bottom">This is Cell #4 of Grid #1</td>
  </tr>
  <tr>
    <td align="left" valign="middle">This is Cell #5 of Grid #1</td>
    <td align="right" valign="middle">This is Cell #6 of Grid #1</td>
  </tr>
</table>
EOT_OUT

&TEST::cleanup;

