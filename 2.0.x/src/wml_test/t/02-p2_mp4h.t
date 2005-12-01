
require "TEST.pl";
&TEST::init;

print "1..6\n";

#
#   TEST 1-2: throughput
#

$pass = 2;

#    Arrays
&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<set-var array = "value-zero
value-one
value-two">
<set-var i=0>
<while <get-var array[<get-var i>]>>;;;
      The value of array[<get-var i>] is `<get-var array[<get-var i>]>'.
<increment i>;;;
</while>
<set-var array[]="0\n1\n2\n3\n4\n5\n6\n7\n8\n9">
<foreach x array start=1 step=2> <get-var x>,</foreach>
<set-var array[]  = ""
         array[0] = 1
         array[1] = 2
         array[3] = 3
         array[4] = 4
         array[5] = 20>
<sort array><foreach x array> <get-var x></foreach>
<sort array numeric=true><foreach x array> <get-var x></foreach>
EOT_IN


The value of array[0] is `value-zero'.
The value of array[1] is `value-one'.
The value of array[2] is `value-two'.


 1, 3, 5, 7, 9,

  1 2 20 3 4
  1 2 3 4 20
EOT_OUT

#    Arithmetic operators
&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<set-var i=10><decrement i by=4><get-var i>
<multiply 2.3 8 3>
<divide -4.0 10>
EOT_IN
6
55.200000
-0.400000
EOT_OUT

#    String operators
&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<capitalize "This is a list">
<match "foobar" ".*">
<match "foobar" "foo">
<match "foobar" "foo" action=extract>
<match "foobar" "oob" action=delete>
<match "foobar" "oob" action=startpos>
<match "foobar" "oob" action=endpos>
<match "foobar" "oob" action=length>
<match "foobar" "[0-9]+">
<string-eq "foo" "FOO" caseless=true>
<subst-in-string "abc" "([a-z])" "\1 ">
EOT_IN
This Is A List
true
true
foo
far
1
4
3

true
a b c 
EOT_OUT

&TEST::cleanup;

