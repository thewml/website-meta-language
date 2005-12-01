
require "TEST.pl";
&TEST::init;

print "1..4\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::std::toc style=pre
<h1>Chapter 1</h1>
<h2>Section 1</h2>
<h2>Section 2</h2>
<h1>Chapter 2</h1>
<h2>Section 1</h2>
<h2>Section 2</h2>
<toc>
EOT_IN
<h1><a name="ToC1">Chapter 1</a></h1>
<h2><a name="ToC2">Section 1</a></h2>
<h2><a name="ToC3">Section 2</a></h2>
<h1><a name="ToC4">Chapter 2</a></h1>
<h2><a name="ToC5">Section 1</a></h2>
<h2><a name="ToC6">Section 2</a></h2>
<pre>
<a href="#ToC1"><strong>Chapter 1</strong></a>
    <a href="#ToC2"><strong>Section 1</strong></a>
    <a href="#ToC3"><strong>Section 2</strong></a>
<a href="#ToC4"><strong>Chapter 2</strong></a>
    <a href="#ToC5"><strong>Section 1</strong></a>
    <a href="#ToC6"><strong>Section 2</strong></a>
</pre>
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::std::toc style=ol type=A1a
<h1>Chapter 1</h1>
<h2>Section 1</h2>
<h3>Subsection 1</h3>
<h3>Subsection 2</h3>
<h2>Section 2</h2>
<toc>
EOT_IN
<h1><a name="ToC1">Chapter 1</a></h1>
<h2><a name="ToC2">Section 1</a></h2>
<h3><a name="ToC3">Subsection 1</a></h3>
<h3><a name="ToC4">Subsection 2</a></h3>
<h2><a name="ToC5">Section 2</a></h2>
<ol type="A"><li><a href="#ToC1"><strong>Chapter 1</strong></a>
<ol type="1"><li><a href="#ToC2"><strong>Section 1</strong></a>
<ol type="a"><li><a href="#ToC3"><strong>Subsection 1</strong></a>
<li><a href="#ToC4"><strong>Subsection 2</strong></a>
</ol><li><a href="#ToC5"><strong>Section 2</strong></a>
</ol></ol>
EOT_OUT

&TEST::cleanup;

